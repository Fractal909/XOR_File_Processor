#ifndef FILEPROCESSOR_H
#define FILEPROCESSOR_H

#include <QObject>
#include <QString>
#include <QByteArray>

class FileProcessor : public QObject {
    Q_OBJECT
public:
    explicit FileProcessor(QObject* parent = nullptr) : QObject(parent) {}
public slots:
    void ProcessFile(const QString& input_file_path, const QString& output_file_path, const QByteArray& operation_value, bool delete_file);
signals:
    void progress(int percent);
    void finished();
    void error(const QString& message);

private:
    bool m_cancel = false;
};

#endif // FILEPROCESSOR_H
