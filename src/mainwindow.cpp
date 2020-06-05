#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QFileDialog>
#include <QtSerialPort>
#include <SerialSource.h>
#include "SerialSetup.h"
#include <softwareimage.h>
#include <commands/command.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
      _serial(new SerialSource(this)),
      _softwareDownloadCmds(new CommandSequence(*_serial, this)){
    ui->setupUi(this);
    ui->pbDownload->setValue(0);
    ui->cbBaudRate->addItems(SerialSetup::availableBaudRates());
    ui->cbBaudRate->setCurrentText("115200");
    ui->cbDevice->addItems(SerialSetup::availablePorts());
    ui->cbDevice->setCurrentText("/dev/ttyUSB0");
    ui->btnCancel->setEnabled(false);
    connect(_serial, &SerialSource::rxData, _softwareDownloadCmds, &CommandSequence::rxData);
    connect(_softwareDownloadCmds, &CommandSequence::updateProgress, this, &MainWindow::updateProgress);
    connect(_softwareDownloadCmds, &CommandSequence::downloadComplete, this, &MainWindow::downloadComplete);
    connect(_softwareDownloadCmds, &CommandSequence::downloadFailed, this, &MainWindow::downloadFailed);


}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_tbLoad_clicked(){
    QString binFile = QFileDialog::getOpenFileName(
       this, tr("Load software binary"), "./", tr("Bin Files(*.bin)"));
       ui->leSoftwareFile->setText(binFile);
}

void MainWindow::updateProgress(QString command, int percent){
    ui->logView->logLine(command);
    ui->pbDownload->setValue(percent);
}

void MainWindow::on_btnDownload_clicked()
{
  SoftwareImage image(PageSize);
  if( image.openFile(ui->leSoftwareFile->text())){
      _serial->closePort();
      auto port = ui->cbDevice->currentText();
      auto baud = ui->cbBaudRate->currentText().toInt();
      _serial->openPort(port, baud);
      if( _serial->isOpen()){
          ui->btnDownload->setEnabled(false);
          ui->btnCancel->setEnabled(true);
          createDownloadSequence(_softwareDownloadCmds, &image);
          _softwareDownloadCmds->start();
      }

  }
}

void MainWindow::createDownloadSequence(CommandSequence *seq, SoftwareImage *image){
  seq->reset();
  seq->addCommand(Command::resetIntoBootLoader(seq));
  seq->addCommand(Command::syncBaud());
  seq->addCommand(Command::eraseFlash());
  for( uint32_t addr = 0; addr< image->getImageSize(); addr+= PageSize){
      seq->addCommand(Command::writeData(addr+FlashStart, image->getDataSegment(addr, PageSize)));
  }
  for( uint32_t addr = 0; addr< image->getImageSize(); addr+= PageSize){
      seq->addCommand(Command::verifyData(addr+FlashStart, image->getDataSegment(addr, PageSize)));
  }
  seq->addCommand(Command::resetIntoRunMode(seq));
}

void MainWindow::downloadFailed(){
    ui->logView->logLine(" ## Download Failed ##");
    ui->btnDownload->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    _serial->closePort();
}

void MainWindow::downloadComplete(){
    ui->logView->logLine(" ## Download Complete ##");
    ui->btnDownload->setEnabled(true);
    ui->btnCancel->setEnabled(false);
    _serial->closePort();
}
