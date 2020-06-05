#ifndef RXPACKET_H
#define RXPACKET_H

#include <QString>

class RxPacket {
   public:
     RxPacket();
     explicit RxPacket(QByteArray &data);

     ~RxPacket = default;
     static RxPacket createVarPacket(QByteArray &data);
     static RxPacket createFixedPacket(QByteArray &data, int length);

     bool isValid();

     QByteArray &data();

private:
     QByteArray _data;
     bool       _isValid;

};

#endif // RXPACKET_H
