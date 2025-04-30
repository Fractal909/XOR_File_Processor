#include "FileProcessor.h"

#include <QFile>


void FileProcessor::ProcessFile(const QString& input_file_path, const QString& output_file_path, const QByteArray& operation_value, bool delete_file) {

    QFile input_file(input_file_path);
    QFile output_file(output_file_path);

    if (!input_file.open(QIODevice::ReadOnly)) {
        emit error("Source open error: " + input_file.errorString());
        return;
    }

    if (!output_file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        emit error("Destination open error: " + output_file.errorString());
        input_file.close();
        return;
    }

    const int block_size = 65536;
    const qint64 total_bytes = input_file.size();
    const int op_value_size = operation_value.size();
    qint64 processed_bytes = 0;


    while (!input_file.atEnd() && !m_cancel) {

        QByteArray buffer = input_file.read(block_size);

        if (buffer.size() == 0) {
            emit error("Read error: " + input_file.errorString());
            break;
        }

        for (int i = 0; i < buffer.size(); ++i) {
            buffer[i] = buffer[i] ^ operation_value[i % op_value_size];
        }

        if (output_file.write(buffer) == -1) {
            emit error("Write error: " + output_file.errorString());
            break;
        }

        processed_bytes += buffer.size();
        emit progress(processed_bytes * 100 / total_bytes);
    }

    input_file.close();
    output_file.close();

    if (delete_file) {
        input_file.remove();
    }

    if (!m_cancel) {
        emit finished();
    }
}
