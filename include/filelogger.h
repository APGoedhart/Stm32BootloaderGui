#ifndef FILELOGGER_H
#define FILELOGGER_H

#include <memory>
#include <QFile>
#include <QTextStream>

class FileLogger{
  private:
    QFile _file;
    std::unique_ptr<QTextStream>  _outputStream;

  public:
    FileLogger();
    FileLogger(const FileLogger &other) = delete ;
    FileLogger operator=(const FileLogger &other) = delete ;

    void createLogFile(QString filename);
    void closeLogFile();

    bool isLogging();

    void logLine(QString line);
    void log(QString text);
};

#endif // FILELOGGER_H
