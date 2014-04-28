#ifndef NETCONFIGMANAGER_H
#define NETCONFIGMANAGER_H

#include "internet.h"

#include <map>
#include <string>

using std::map;
using std::string;

class NetConfigElement
{
public:
    string  m_Name;
    string  m_IP;
    int     m_iPort;
};

class NetConfigManager
{
public:
    enum
    {
        GPS_DAYTIME = 0, 
        GPS_INFRARED, 
        CPS_DAYTIME,
        CPS_INFRARED, 
        client_num_visual_5, 
        client_num_visual_6,
        client_num_visual_7, 
        client_num_visual_8, 
        MISSION_FUNCTION,
		IFUT,
		IFUB,

        IOS, 
        client_num_sound, 
        client_num_motion, 
        client_num_icomm,
        client_num_ifub, 
        client_num_ifut,
        client_num_server, 
        client_num_utm,
        client_num_evaluation,
        client_num_ifu_emulator,
        client_num_test1,
        HOST,
        MULTICAST,
    };
public:
    static NetConfigManager&  GetInstance()
    {
        return sm_NetConfigManager;
    }

    void Init();

    int GetFullAddress(int iType, InternetAddress& aAddr); 

    const char* GetAddress(int iType);

    int GetPort(int iType);

private:
    NetConfigManager();
    ~NetConfigManager();

private:
    map<int, NetConfigElement*> m_NetConfigElemMaps;

    static NetConfigManager     sm_NetConfigManager;
};

#define TheNetConfigManager NetConfigManager::GetInstance()

#endif // NETCONFIGMANAGER_H