#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QFileDialog>
#include <QString>
#include <QTimer>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_btn_select_input_folder_clicked();
    void on_btn_select_output_folder_clicked();
    void on_btn_process_clicked();

    void on_rb_delete_files_clicked(bool checked);

    void on_rb_timer_clicked(bool checked);
    void HandleTimeout();
    void on_btn_stop_clicked();

private:
    QString GetOutputFilePath(const QString& file_name);
    void StartProcessing(const QString& input_file_path, const QString& output_file_path);
    void PrepareFiles();
    Ui::MainWindow *ui;

    QTimer* timer_;
    QTime init_time_;
};
#endif // MAINWINDOW_H
