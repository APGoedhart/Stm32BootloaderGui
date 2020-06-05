#ifndef LogView_H
#define LOG_VIEW_H

#include <QWidget>
#include <QStringListModel>
#include <QStringList>
#include <filelogger.h>

namespace Ui {
  class LogView;
  }

class LogView : public QWidget
{
    using Milliseconds = std::chrono::milliseconds;
    using StopwatchTime = std::chrono::time_point<std::chrono::steady_clock, std::chrono::nanoseconds>;
    Q_OBJECT

    static constexpr auto REMOVE_LINES = 50;
  public:
    explicit LogView(QWidget *parent = 0);
    ~LogView();

  public slots:
    /**
     * @brief logLine Log a single line of text on a new Line
     * @param line
     */
    void logLine(QString line);

    /**
     * @brief log log text formated with CR LF
     * @param text the string to write out tho the window has embedded CR LF
     */
    void log(QString text);

    /**
     * Update the time displayed on the left of the trace window
     * @brief updateTime
     */
    void updateTime();


  private slots:
    void on_logActiveCb_toggled(bool checked);

    void on_clearBtn_clicked();

    void on_logFileBtn_clicked();

    void on_pauseButton_clicked();

  private:
    QString formatLine(QString line);
    QStringList splitOnNewLine(QString text);
    void updateFirstRowWithExistingData(QStringList & newLines);
    void insertCombinedTraceRows(QStringList newLines);
    void updateWithTimestamp(QStringList &lines);

    Ui::LogView *ui;

    QStringListModel *_model;
    bool              _paused;
    FileLogger        _traceFile;
    int32_t          _maxLineLength;
    int32_t          _maxLines;
    StopwatchTime    _start;
    Milliseconds     _offset;
    bool             _timestampChanged;

};

#endif // OUTPUTWINDOW_H
