#ifndef SOFTWAREIMAGE_CPP
#define SOFTWAREIMAGE_CPP

#include <QFile>

#include <softwareimage.h>


SoftwareImage::SoftwareImage(uint32_t pageSize):
  _pageSize(pageSize),
  _data(),
 _failureReason(""){
}

/**
 * LOad the data from the file returning an empty byte array if no file is found
 * @brief SoftwareImage::readFile
 * @param filename
 * @return
 */
bool SoftwareImage::openFile(const QString &filename){
  QFile file(filename);
  if( !file.exists()){
    _failureReason = "File does not exist";
    _data = QByteArray();
    return false;
  }

  if( file.open(QIODevice::ReadOnly)){
    _data = file.readAll();
    file.close();
  }

  if( _data.size() ==0){
    _failureReason = "Invalid Image File";
    return false;
  }
  _failureReason = "";
  return true;
}


/**
 * @brief getDataSegment return the data for the segement given by the start address
 * and length
 * @param address the start of the data segment
 * @param length the requested length
 * @return a byte array contianing the request data. may contain less then the requested d
 */
QByteArray SoftwareImage::getDataSegment(uint32_t address, uint32_t length){
  auto imageSize = getImageSize();
  if( address > imageSize ){
    return QByteArray();
  }

  // adjust for image overruns
  if( address + length > imageSize){
    length = imageSize - address;
  }

  // adjust for page boundaries
  if( (address / _pageSize) != ((address+length)/_pageSize)){
    uint32_t endOfPage = ((address+length)/_pageSize)*_pageSize;
    length = endOfPage - address;
  }
  return _data.mid(static_cast<int>(address), static_cast<int>(length));
}


QString SoftwareImage::getFailureReason(){
  return _failureReason;
}

uint32_t SoftwareImage::getImageSize(){
  return static_cast<uint32_t>(_data.size());
}


#endif // SOFTWAREIMAGE_CPP
