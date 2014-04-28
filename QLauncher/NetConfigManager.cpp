#include "NetConfigManager.h"

#include <tinyxml.h>

NetConfigManager NetConfigManager::sm_NetConfigManager;
//---------------------------------------------------------------------------------
NetConfigManager::NetConfigManager()
{
}
//---------------------------------------------------------------------------------
NetConfigManager::~NetConfigManager()
{

}
//---------------------------------------------------------------------------------
void NetConfigManager::Init()
{
    TiXmlDocument aDoc;

    if (! aDoc.LoadFile("NetCfg.xml")) return;

    TiXmlElement* pRootElem = aDoc.RootElement();

    TiXmlElement* pElem = 0;

    NetConfigElement* pConfigElem;

    for (pElem = pRootElem->FirstChildElement("node"); pElem != NULL; pElem = pElem->NextSiblingElement("node"))
    {
        const char* strName     = pElem->Attribute("name");
        const char* strIP       = pElem->Attribute("ip");
        const char* strPort     = pElem->Attribute("port");        

        pConfigElem = new NetConfigElement;

        if (strName) 
            pConfigElem->m_Name = strName;

        if (strIP)
            pConfigElem->m_IP = strIP;

        if (strPort)
            sscanf(strPort, "%d", &(pConfigElem->m_iPort));

        if (pConfigElem->m_Name == "Host")
            m_NetConfigElemMaps[NetConfigManager::HOST] = pConfigElem;
        else if (pConfigElem->m_Name == "GPSDayTime")
            m_NetConfigElemMaps[NetConfigManager::GPS_DAYTIME] = pConfigElem;
        else if (pConfigElem->m_Name == "GPSInfraRed")
            m_NetConfigElemMaps[NetConfigManager::GPS_INFRARED] = pConfigElem;
        else if (pConfigElem->m_Name == "CPSDayTime")
            m_NetConfigElemMaps[NetConfigManager::GPS_DAYTIME] = pConfigElem;
        else if (pConfigElem->m_Name == "CPSInfrared")
            m_NetConfigElemMaps[NetConfigManager::CPS_INFRARED] = pConfigElem;
        else if (pConfigElem->m_Name == "IOS")
            m_NetConfigElemMaps[NetConfigManager::IOS] = pConfigElem;
        else if (pConfigElem->m_Name == "MissionFunction")
            m_NetConfigElemMaps[NetConfigManager::MISSION_FUNCTION] = pConfigElem;
		else if (pConfigElem->m_Name == "IFUT")
            m_NetConfigElemMaps[IFUT] = pConfigElem;
		else if (pConfigElem->m_Name == "IFUB")
            m_NetConfigElemMaps[IFUB] = pConfigElem;
     else if (pConfigElem->m_Name == "MULTICAST")
            m_NetConfigElemMaps[NetConfigManager::MULTICAST] = pConfigElem;

    }
}
//---------------------------------------------------------------------------------
int NetConfigManager::GetFullAddress(int iType, InternetAddress& aAddr)
{
    map<int, NetConfigElement*>::iterator it = m_NetConfigElemMaps.find(iType);

    if (m_NetConfigElemMaps.end() == it)
        return 0;
    else
    {
        NetConfigElement* pElem = it->second;
        aAddr.SetAddress(pElem->m_IP.c_str(),0);
        aAddr.SetPortByPortNum(pElem->m_iPort);
        return 1;
    }
}
//---------------------------------------------------------------------------------
const char* NetConfigManager::GetAddress(int iType)
{
    map<int, NetConfigElement*>::iterator it = m_NetConfigElemMaps.find(iType);

    if (m_NetConfigElemMaps.end() == it)
        return 0;
    else
    {
        NetConfigElement* pElem = it->second;
        return pElem->m_IP.c_str();
    }
}
//---------------------------------------------------------------------------------
int NetConfigManager::GetPort(int iType)
{
    map<int, NetConfigElement*>::iterator it = m_NetConfigElemMaps.find(iType);

    if (m_NetConfigElemMaps.end() == it)
        return 0;
    else
    {
        NetConfigElement* pElem = it->second;
        return pElem->m_iPort;
    }
}
//---------------------------------------------------------------------------------