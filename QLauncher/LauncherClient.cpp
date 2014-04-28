#include "LauncherClient.h"

LauncherClient::LauncherClient(QObject *parent)
: QObject(parent), ID(0), m_strClientIP(""), m_strClientName("") 
{
	//m_vecProgramList.clear();
}

LauncherClient::~LauncherClient()
{
	//for(int i = 0, n = m_vecProgramList.size(); i < n; ++i)
	//{
	//    if(m_vecProgramList[i])
	//        delete m_vecProgramList[i];
	//    m_vecProgramList[i] = 0;
	//}
	//m_vecProgramList.clear();

	QMap<unsigned char, LauncherProgramInfo*>::iterator it, end;
	for (it = m_ProgramMap.begin(), end = m_ProgramMap.end(); it != end; ++it)
	{
		delete it.value();
	}
	QMap<QString, LauncherSubClientInfo*>::iterator it2, end2;
	for (it2 = m_ClientMap.begin(), end2 = m_ClientMap.end(); it2 != end2; ++it2)
	{
		delete it2.value();
	}
}
//---------------------------------------------------------------------------------------------------
LauncherProgramInfo* LauncherClient::addProgramInfo(unsigned char id, unsigned char state, QString name, QString path)
{
	LauncherProgramInfo* pInfo = new LauncherProgramInfo(id, state, name, path);
	m_ProgramMap[id] = pInfo;
	//m_ProgramMap[path] = pInfo;

	return pInfo;
}
//---------------------------------------------------------------------------------------------------

LauncherProgramInfo* LauncherClient::addProgramInfo(QString strIP,unsigned char id, unsigned char state, QString name, QString path)
{

	LauncherProgramInfo* pInfo = new LauncherProgramInfo(id, state, name, path);
	m_ClientMap[strIP]->ProgramMap[id] = pInfo;
	//m_ProgramMap[path] = pInfo;

	return pInfo;
}
//---------------------------------------------------------------------------------------------------
LauncherSubClientInfo* LauncherClient::addSubClientInfo(unsigned char id, QString name, QString ip)
{
	LauncherSubClientInfo* pInfo = new LauncherSubClientInfo(id, name, ip);
	m_ClientMap[ip] = pInfo;
	//m_ProgramMap[path] = pInfo;

	return pInfo;
}
//---------------------------------------------------------------------------------------------------
void LauncherClient::removeProgramInfo(char id)
{
	QMap<unsigned char, LauncherProgramInfo*>::iterator it = m_ProgramMap.find(id);

	if (it  != m_ProgramMap.end())
	{
		delete it.value();
		m_ProgramMap.erase(it);
	}
}
//---------------------------------------------------------------------------------------------------
void LauncherClient::removeSubClientInfo(QString strIP)
{
	QMap<QString, LauncherSubClientInfo*>::iterator it = m_ClientMap.find(strIP);

	if (it  != m_ClientMap.end())
	{
		delete it.value();
		m_ClientMap.erase(it);
	}
}
//---------------------------------------------------------------------------------------------------
LauncherProgramInfo* LauncherClient::getProgramInfo(unsigned char id)
{
	QMap<unsigned char, LauncherProgramInfo*>::iterator it = m_ProgramMap.find(id);
	if (it  != m_ProgramMap.end())
		return it.value();
	else 
		return 0;
}
//---------------------------------------------------------------------------------------------------
LauncherSubClientInfo* LauncherClient::getSubClientInfo(QString strIP)
{
	QMap<QString, LauncherSubClientInfo*>::iterator it = m_ClientMap.find(strIP);
	if (it  != m_ClientMap.end())
		return it.value();
	else 
		return 0;
}


void LauncherClient::removeProgramInfo(QString strIP,char id)
{
	QMap<unsigned char, LauncherProgramInfo*>::iterator it = m_ClientMap[strIP]->ProgramMap.find(id);

	if (it  != m_ClientMap[strIP]->ProgramMap.end())
	{
		delete it.value();
		m_ClientMap[strIP]->ProgramMap.erase(it);
	}

}
LauncherProgramInfo* LauncherClient::getProgramInfo(QString strIP,unsigned char id)
{
	QMap<unsigned char, LauncherProgramInfo*>::iterator it = m_ClientMap[strIP]->ProgramMap.find(id);
	if (it  != m_ClientMap[strIP]->ProgramMap.end())
		return it.value();
	else 
		return 0;
}