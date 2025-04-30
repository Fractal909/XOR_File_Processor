#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FileProcessor.h"

#include <QDir>
#include <QString>
#include <QStringList>
#include <QMessageBox>
#include <QThread>
#include <QProgressBar>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

QString MainWindow::GetOutputFilePath(const QString& file_name) {

    const QDir output_directory(ui->btn_select_output_folder->text());
    QString output_file_path;

    if (output_directory.exists(file_name)) {
        if (ui->comboBox->currentIndex() == 0) {
            output_file_path = output_directory.absoluteFilePath(file_name);
        } else {
            int iter = 0;
            int point_index = file_name.lastIndexOf('.');
            QString output_file_name;

            do {
                ++iter;
                output_file_name = file_name.left(point_index) + '_' + QString::number(iter) + file_name.right(file_name.size() - point_index);
            } while (output_directory.exists(output_file_name));

            output_file_path = output_directory.absoluteFilePath(output_file_name);
        }
    } else {
        output_file_path = output_directory.absoluteFilePath(file_name);
    }

    return output_file_path;
}

void MainWindow::StartProcessing(const QString& input_file_path, const QString& output_file_path) {

    auto qt_bar = new QProgressBar;
    auto qt_hor_lo = new QHBoxLayout();
    auto qt_lbl = new QLabel();

    qt_lbl->setText(output_file_path);
    qt_hor_lo->addWidget(qt_lbl);
    qt_hor_lo->addWidget(qt_bar);
    ui->vlo->addLayout(qt_hor_lo);


    QThread* thread = new QThread;
    FileProcessor* fp_worker = new FileProcessor;

    fp_worker->moveToThread(thread);

    connect(thread, &QThread::started, [=](){
        fp_worker->ProcessFile(input_file_path,
                               output_file_path,
                               QByteArray::fromHex(ui->le_operation_value->text().toLatin1()),
                               ui->rb_delete_files->isChecked());
    });

    connect(fp_worker, &FileProcessor::progress, qt_bar, &QProgressBar::setValue);

    connect(fp_worker, &FileProcessor::finished, qt_hor_lo, &QHBoxLayout::deleteLater);
    connect(fp_worker, &FileProcessor::finished, qt_bar, &QProgressBar::deleteLater);
    connect(fp_worker, &FileProcessor::finished, qt_lbl, &QLabel::deleteLater);

    connect(fp_worker, &FileProcessor::finished, thread, &QThread::quit);
    connect(fp_worker, &FileProcessor::finished, fp_worker, &QObject::deleteLater);
    connect(thread, &QThread::finished, thread, &QObject::deleteLater);

    thread->start();
}

void MainWindow::PrepareFiles() {

    const QDir input_directory(ui->btn_select_input_folder->text());
    const QStringList input_files = input_directory.entryList(QStringList() << ui->le_file_mask->text(), QDir::Files);

    if (input_files.empty()) {
        if (!ui->rb_timer->isChecked()) {
            QMessageBox messageBox;
            messageBox.information(0,"Information","No matching files in the folder!");
        }
        return;
    }

    for (const auto& file_name : input_files) {
        const QString input_file_path = input_directory.absoluteFilePath(file_name);
        const QString output_file_path = GetOutputFilePath(file_name);
        StartProcessing(input_file_path, output_file_path);
    }
}



void MainWindow::on_btn_select_input_folder_clicked()
{
    auto input_folder_name = QFileDialog::getExistingDirectory();
    ui->btn_select_input_folder->setText(input_folder_name);
}


void MainWindow::on_btn_select_output_folder_clicked()
{
    auto output_folder_name = QFileDialog::getExistingDirectory();
    ui->btn_select_output_folder->setText(output_folder_name);
}


void MainWindow::on_btn_process_clicked()
{
    if (ui->btn_select_input_folder->text().toStdString().empty()) {
        QMessageBox messageBox;
        messageBox.warning(0,"Error","Input folder not selected!");
        return;
    }

    if (ui->btn_select_output_folder->text().toStdString().empty()) {
        QMessageBox messageBox;
        messageBox.warning(0,"Error","Output folder not selected!");
        return;
    }

    if (ui->le_operation_value->text().size() < 2) {
        QMessageBox messageBox;
        messageBox.warning(0,"Error","Value must be at least 1 byte long");
        return;
    }

    if (ui->rb_timer->isChecked()) {
        timer_ = new QTimer(this);
        init_time_ = ui->te_timer->time();
        connect(timer_, &QTimer::timeout, this, &MainWindow::HandleTimeout);
        timer_->start(1000);

        ui->btn_stop->setDisabled(false);
        ui->btn_process->setDisabled(true);
        ui->te_timer->setDisabled(true);
    }

    PrepareFiles();
}


void MainWindow::on_rb_delete_files_clicked(bool checked)
{
    if (checked) {
        ui->rb_delete_files->setStyleSheet("QRadioButton {color: red;}");
    } else {
        ui->rb_delete_files->setStyleSheet("");
    }
}


void MainWindow::on_rb_timer_clicked(bool checked)
{
    ui->te_timer->setDisabled(!checked);
}

void MainWindow::HandleTimeout() {

    auto time = ui->te_timer->time().addSecs(-1);

    if (time > init_time_) {
        ui->te_timer->setTime(init_time_);
        PrepareFiles();
    } else {
        ui->te_timer->setTime(time);
    }
}


void MainWindow::on_btn_stop_clicked()
{
    timer_->stop();
    timer_->deleteLater();

    ui->te_timer->setTime(init_time_);

    ui->btn_stop->setDisabled(true);
    ui->btn_process->setDisabled(false);
    ui->te_timer->setDisabled(false);
}

