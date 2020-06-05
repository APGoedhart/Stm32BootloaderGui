#include <SerialSource.h>

SerialSource::SerialSource(QObject *parent) :
  QObject(parent),
  _serialPort(nullptr),
  _txQueue(std::make_unique< QQueue<QByteArray > >()),
  _isTransmitting(false),
  _retry(new QTimer(this))
{
  _serialPort = new QSerialPort(this);
  connect(_serialPort, &QSerialPort::readyRead, this, &SerialSource::readyRead );
  connect(_serialPort, &QSerialPort::bytesWritten, this, &SerialSource::bytesWritten);
  connect(_serialPort, &QSerialPort::errorOccurred, this, &SerialSource::errorOccured);
  connect(_retry, &QTimer::timeout, this, &SerialSource::poll);
  _retry->start(200);
}


SerialSource::~SerialSource(){
  if( _serialPort->isOpen()){
    closePort();
  }
}


bool SerialSource::isOpen(){
  return _serialPort->isOpen();
}

QString SerialSource::getPort(){
  return _serialPort->portName();
}

uint32_t SerialSource::getBaudRate(){
    return static_cast<uint32_t>(_serialPort->baudRate());
}

void SerialSource::dtr(bool isEnabled){
  _serialPort->setDataTerminalReady(isEnabled);
}

void SerialSource::rts(bool isEnabled){
  _serialPort->setRequestToSend(isEnabled);
}

void SerialSource::txData(const QByteArray &data){
  _txQueue->enqueue(data);
  sendIfTxComplete();
}

void SerialSource::openPort(const QString &portName, int baudRate ){
  if(_serialPort->isOpen()){
    clearQueues();
    closePort();
  }
  _serialPort->setPortName(portName);
  _serialPort->setBaudRate(baudRate);
  _serialPort->setDataBits(QSerialPort::Data8);
  _serialPort->setParity(QSerialPort::EvenParity);
  _serialPort->setFlowControl(QSerialPort::NoFlowControl);
  _serialPort->setStopBits(QSerialPort::OneStop);
  if( _serialPort->open(QIODevice::ReadWrite) ){
     dtr(false);
     rts(false);
    emit opened();
  } else {
    emit closed();
  }
}



void SerialSource::closePort(){
  if( _serialPort->isOpen()){
    _serialPort->close();
    emit closed();
  }
}

// called from serial port.
/**
 * @brief bytesWritten called by underlying serial port once the
 * previus message has been sent.
 * @param bytes the number of bytes sent.
 */
void SerialSource::bytesWritten(qint64){
  sendIfTxComplete();
}

void SerialSource::poll(){
  readyRead();
}

void SerialSource::readyRead(){
  if( _serialPort->bytesAvailable()){
    _serialPort->setDataTerminalReady(true);
    auto data = _serialPort->readAll();
    emit rxData(data);
  }
}

/**
 * Called when an error occurs on the port.
 * @brief errorOccured
 * @param error
 */
void SerialSource::errorOccured(QSerialPort::SerialPortError){
  closePort();
}

/**
 * @brief clearQueues clear out any packets curretnly in the queues.
 * read any buffered received data.
 */
void SerialSource::clearQueues(){
  if( _serialPort->isOpen()){
    while(!_txQueue->isEmpty()){
      (void)_txQueue->dequeue();
    }
    while(_serialPort->bytesAvailable()){
      _serialPort->readAll();
    }
  }
}

void SerialSource::sendIfTxComplete(){
  if( (_txQueue->size() > 0 ) && (_serialPort->bytesToWrite() == 0 )){
    auto data = _txQueue->dequeue();
    _serialPort->write(data);
  }
}





