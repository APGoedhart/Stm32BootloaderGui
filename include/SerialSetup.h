#ifndef SERIALSETUP_H
#define SERIALSETUP_H
#include <QStringList>

class SerialSetup
{
  public:
    /**
     * @brief availablePorts Return a list of the names of the  com ports currently available on this computer
     * @return
     */
    static QStringList availablePorts();
    /**
     * @brief availableBaudRates Return a list of the baud rates the first com port can be set to.
     * @return
     */
    static QStringList availableBaudRates();
};

#endif // SERIALSETUP_H
