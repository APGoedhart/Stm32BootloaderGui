#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <SerialSource.h>
#include <commands/commandSequence.h>
#include <softwareimage.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    static constexpr uint32_t PageSize = 128;
    static constexpr uint32_t FlashStart = 0x08000000;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_tbLoad_clicked();
    void on_btnDownload_clicked();
    void updateProgress(QString command, int percent);
    void downloadComplete();
    void downloadFailed();

private:
    void createDownloadSequence(CommandSequence *seq, SoftwareImage *image);

    Ui::MainWindow *ui;
    SerialSource *_serial;
    CommandSequence *_softwareDownloadCmds;
};
#endif // MAINWINDOW_H
