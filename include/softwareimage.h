#ifndef SOFTWARE_DOWNLOAD_SOFTWARE_IMAGE_H
#define SOFTWARE_DOWNLOAD_SOFTWARE_IMAGE_H

#include <QByteArray>
#include <QObject>
#include <QString>

class SoftwareImage {
  public:
    /**
     */
    SoftwareImage(uint32_t pageSize);

    /**
       * @brief readFile read the whole file into a Byte array
       * @param file the file to read
       * @return the Byte array or an empty one if failed.
       */
    bool openFile(const QString &filename);

    /**
     * @brief getDataSegment return the data for the segement given by the start address 
     * and length
     * @param address the start of the data segment
     * @param length the requested length
     * @return a byte array contianing the request data. may contain less then the requested d
     */
    QByteArray getDataSegment(uint32_t address, uint32_t length);

    /**
     * Get the reason for failure. if open returns false;
     */
    QString getFailureReason();

    uint32_t getImageSize();

  private:
    uint32_t      _pageSize;
    QByteArray    _data;
    QString       _failureReason;


};

#endif
