#ifndef NETWORKINTERFACE_H
#define NETWORKINTERFACE_H
#include <QtNetwork/QNetworkInterface>
#include <QtCore/QString>

#include <winsock.h>
//#include <winsock2.h>
//#include <NetChannel.h>

class NetworkInterface
{
public:
    static QString getLocalIP()
    {
        char address[256] = {0,};
        char Name[256] = {0,};
    
        gethostname(Name, sizeof(Name));
        PHOSTENT host = gethostbyname(Name);
        strcpy(address, inet_ntoa(*(struct in_addr*)*host->h_addr_list));

        return QString::fromAscii(address);
    }

    static QString getLocalMacAddress()
    {
        foreach(QNetworkInterface inter, QNetworkInterface::allInterfaces())
        {
            printf("mac interface name : %s\n", inter.humanReadableName().toStdString().c_str());
        }
        foreach(QNetworkInterface inter, QNetworkInterface::allInterfaces())
        {
            // Return only the first non-loopback MAC Address
            if (!(inter.flags() & QNetworkInterface::IsLoopBack))
            {
                QString address = inter.hardwareAddress();
                QString retAddr = "";

                int index = 0;
                int start = 0;
                while(1)
                {
                    retAddr += address.mid(index, 2);
                    index = address.indexOf(':', index, Qt::CaseInsensitive) + 1;

                    if(index == 0) break;
                }
                return retAddr;
            }
        }
        return QString();
    }
    //   static QString getLocalMacAddress()
    //   {
    //QString address = QNetworkInterface().hardwareAddress();

    //QString retAddr = "";

    //int index = 0;
    //int start = 0;
    //while(1)
    //{
    //    retAddr += address.mid(index, 2);
    //    index = address.indexOf(':', index, Qt::CaseInsensitive) + 1;

    //    if(index >= address.length() || index == -1) break;
    //}
    //return retAddr;
    //   }
};
#endif