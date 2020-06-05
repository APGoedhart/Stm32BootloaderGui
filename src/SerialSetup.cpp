#include "SerialSetup.h"
#include <QString>
#include <QtSerialPort/QSerialPortInfo>


QStringList SerialSetup::availablePorts()
{
    auto serialPorts = QSerialPortInfo::availablePorts();
    QStringList devices;
#ifdef linux
    QString prepend = "/dev/";
#else
    QString prepend = "";
#endif
    for(auto port :serialPorts){
      devices.push_back(prepend+port.portName());
    }
    return devices;
}

QStringList SerialSetup::availableBaudRates()
{
    auto serialPorts = QSerialPortInfo::availablePorts();
    QStringList baudRates;
    baudRates.append(QString::number(460800));
    for(auto rate: serialPorts.first().standardBaudRates()){
      baudRates.append(QString::number(rate));
    }
    return baudRates;
}
