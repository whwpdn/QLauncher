#include "LauncherRole.h"
#include "Socket.h"
#include "internet.h"
#include "message.h"
#include "NetMessageManager.h"
#include "NetMessageParser.h"
#include "AddProgramDlg.h"

#include <QtGui>
#include <QtNetwork/QHostInfo>
#include "LauncherClient.h"
#include "networkInterface.h"
#include <tinyxml.h>

#define TCPPORT 309
#define UDPPORT 418

#define SERVER_PORT		33334
#define SUBSERVER_PORT	44445
#define CLIENT_PORT		55555
#define CLIENTTOMAIN_PORT 22223

enum LAUNCHER_COMMAND
{
    PROGRAM_OFF = 0,
    PROGRAM_ON,
};
enum CLIENT_INFO
{
    CLIENT_CONNECTIONLESS = 0,
    CLIENT_CONNECTED,
};
ServerRole			ServerRole::sm_Instance;
ClientRole			ClientRole::sm_Instance;

#define MAGICPPORT 40000

#include <IPHlpApi.h>
#pragma comment( lib, "iphlpapi.lib" )
void GetMacAddress(const char* ip, char* address)
{
    if(!address || !ip) return;

    UCHAR bVictimMac[ 6 ] = { 0, };
    ULONG uPhyAddrLen = 6;
    char macBuffer[ 15 ] = { 0, };
    unsigned int vicimIP32 = inet_addr(ip);

    if( SendARP( vicimIP32, 0, (ULONG*)bVictimMac, &uPhyAddrLen ) != NO_ERROR )
    {
    }

    sprintf( macBuffer, "%02X%02X%02X%02X%02X%02X", 
        bVictimMac[ 0 ], 
        bVictimMac[ 1 ], 
        bVictimMac[ 2 ], 
        bVictimMac[ 3 ], 
        bVictimMac[ 4 ], 
        bVictimMac[ 5 ] );

    strcpy(address, macBuffer);
}
//--------------------------------------------------------------------------------------
unsigned int HexStrToInt(QString hexStr)
{
    char *stop="";
	char num[3]={0,};
    UINT res = 0;

    //Nothing happens if return 0, just server will not wakeup
    if(hexStr.length() > 2)
    {
        return 0;		//or perhaps exit;
    }

    memset(num,'\0',3);

    //In Unicode it's: wcscpy
    strcpy(num, hexStr.toStdString().c_str());
    //In Unicode it's: wcstol
    res = strtol(num,&stop,16);

    if(res==LONG_MAX || res==LONG_MIN || res==0)
    {
        return 0;		//or perhaps exit;
    }
    return res;
}
//--------------------------------------------------------------------------------------
int createMagicP(QString macAddr, unsigned char* magicp)
{
    if(macAddr.length() != 12) return 0;

    for(int i = 0; i < 6; i++)
        magicp[i] = 0xff;

    for(int i = 0; i < 6; i++)
        magicp[i+6] = HexStrToInt(macAddr.mid(i*2,2));

    for(int i = 0; i < 15; i++)
        memcpy(&magicp[(i+2)*6],&magicp[6],6);
    return DATA_SIZE_MAGICP;
}

//-------------------------------------------------------------------------------------------
int GetMessageSize(unsigned char ucCode)
{
    switch (ucCode)
    {
        // 데이타 사이즈 1
    case PACKET_CLIENT_INFO:		return HEADER_SIZE + CLIENTINFO_SIZE;
    case PACKET_COMMAND_INFO:		return HEADER_SIZE + COMMANDINFO_SIZE;
    case PACKET_PROGRAM_INFO:		return HEADER_SIZE + PROGRAMINFO_SIZE;
    case PACKET_SERVERSTATE:		return HEADER_SIZE + SERVERSTATE_SIZE;
    case PACKET_CONNECTION:			return HEADER_SIZE;
    case PACKET_REBOOT:				return HEADER_SIZE;
	case PACKET_SHUTDOWN:			return HEADER_SIZE;
	case PACKET_SUBCLIENT_INFO:		return HEADER_SIZE + SUBCLIENTINFO_SIZE;
	case PACKET_SUBPROGRAM_INFO:	return HEADER_SIZE + SUBPROGRAMINFO_SIZE;
	case PACKET_CHECKBOX_INFO:		return	HEADER_SIZE + CHECKBOXUPDATE_SIZE;
    default:						return 0;        
    }
}
//-------------------------------------------------------------------------------------------
NetMessage* CreateMessage(unsigned char ucCode)
{
	switch (ucCode)
	{
	case PACKET_CLIENT_INFO:        return new ClientInfoPacket;
	case PACKET_COMMAND_INFO:       return new CommandInfoPacket;
	case PACKET_PROGRAM_INFO:		return new ProgramInfoPacket;
	case PACKET_CONNECTION:			return new NetMessage;
	case PACKET_REBOOT:				return new NetMessage;
	case PACKET_SHUTDOWN:			return new NetMessage;
	case PACKET_SERVERSTATE:        return new ServerStatePacket;
	case PACKET_SUBCLIENT_INFO:		return new SubClientInfoPacket;
	case PACKET_SUBPROGRAM_INFO:	return new SubProgramInfoPacket;
	case PACKET_CHECKBOX_INFO:		return new CheckBoxUpdatePacket;
	default:                        return 0;
	}
}
//------------------------------------------------------------------------------------------------
void DeleteMessage(NetMessage* pMessage)
{
    if (pMessage)
        delete pMessage;
}

LauncherRole::LauncherRole() : QObject(), m_pUI(0), m_bModified(false),m_LocalIP("")
{
    m_pTCPSocket = new Socket;
}

LauncherRole::~LauncherRole()
{
    if (m_pTCPSocket)
    {
        m_pTCPSocket->Close();
        delete m_pTCPSocket;
    }
}

void LauncherRole::setUI(Ui::QLauncherClass* pUI)
{
    m_pUI = pUI;
}

/*------------------------------------------------------------------------*/
ServerRole::ServerRole() : LauncherRole(), m_pActiveLauncherClient(0), m_iOldTime(0)
{
    m_Time = QTime::currentTime();
}

ServerRole::~ServerRole()
{
}

ServerRole& ServerRole::GetInstance()
{
    return sm_Instance;
}
void ServerRole::setLocalIP(QString strLocalIP)
{
	m_LocalIP = strLocalIP;

}
//------------------------------------------------------------------------------------------------
void ServerRole::setServerIP(QString ip,QString port)
{
    m_ServerIP = ip;
	m_ServerPort = port.toInt();
}
void ServerRole::initUI()
{
    m_pUI->btnAdd->hide();
	
    m_pUI->btnDelete->hide();
	m_pUI->tableMac->hide();

	SetWolTable();
	ResetServerListTable();
	
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());

    connect(pModel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(treeItemChangedSlot(QStandardItem*)));
    connect(m_pUI->btnOn, SIGNAL(clicked(bool)), this, SLOT(onButtonSlot(bool)));
    connect(m_pUI->btnOff, SIGNAL(clicked(bool)), this, SLOT(offButtonSlot(bool)));
    connect(m_pUI->btnPower, SIGNAL(clicked(bool)), this, SLOT(powerButtonSlot(bool)));
	connect(m_pUI->btnPowerOff, SIGNAL(clicked(bool)), this, SLOT(powerOffButtonSlot(bool)));

    connect(m_pUI->btnOnAll, SIGNAL(clicked(bool)), this, SLOT(OnAllButtonSlot(bool)));
    connect(m_pUI->btnOffAll, SIGNAL(clicked(bool)), this, SLOT(OffAllButtonSlot(bool)));
    connect(m_pUI->treeConnectionList, SIGNAL(clicked(const QModelIndex&)), this, SLOT(treeSelectSlot(const QModelIndex&)));
    connect(m_pUI->btnReboot, SIGNAL(clicked(bool)), this, SLOT(rebootSlot(bool)));
    //connect(m_pUI->btnDelete, SIGNAL(clicked(bool)), this, SLOT(delButtonSlot(bool)));
	connect(m_pUI->btnSaveConf, SIGNAL(clicked(bool)), this, SLOT(saveIpConfig(bool)));

    //QObject::connect(&m_ConnectionCheckTimer, SIGNAL(timeout()), this, SLOT(ConnectionCheckSlot()));
	connect(m_pUI->checkMacList,SIGNAL(stateChanged(int)),this,SLOT(changeCheckMacListSlot(int)));
	connect(m_pUI->checkServerList,SIGNAL(stateChanged(int)),this,SLOT(changeCheckServerListSlot(int)));
	connect(m_pUI->btnServerConn,SIGNAL(clicked(bool)), this,SLOT(btnServerConnSlot(bool)));
	addServerInfo();
}
void ServerRole::SetWolTable()
{
	// wol tablewidget
	m_pUI->tableMac->insertColumn(0);
	m_pUI->tableMac->insertColumn(1);
	m_pUI->tableMac->insertColumn(2);
	//m_pUI->tableMac->horizontalHeader()->setVisible(false);
	//m_pUI->tableMac->horizontalHeaderItem(0)->setText("ip");
	//m_pUI->tableMac->horizontalHeaderItem(1)->setText("Mac");
	m_pUI->tableMac->verticalHeader()->setVisible(false);
	m_pUI->tableMac->setHorizontalHeaderItem(0,new QTableWidgetItem("Ip"));
	m_pUI->tableMac->setHorizontalHeaderItem(1,new QTableWidgetItem("Mac"));

}
void ServerRole::ResetServerListTable()
{
	// server list
	m_pUI->tableServerList->insertColumn(0);
	m_pUI->tableServerList->insertColumn(1);
	m_pUI->tableServerList->insertColumn(2);
	m_pUI->tableServerList->insertColumn(3);
	m_pUI->tableServerList->insertColumn(4);
	m_pUI->tableServerList->verticalHeader()->setVisible(false);
	m_pUI->tableServerList->setHorizontalHeaderItem(0,new QTableWidgetItem("Name"));
	m_pUI->tableServerList->setHorizontalHeaderItem(1,new QTableWidgetItem("Ip"));
	m_pUI->tableServerList->setHorizontalHeaderItem(2,new QTableWidgetItem("Group"));
	m_pUI->tableServerList->setHorizontalHeaderItem(3,new QTableWidgetItem("check"));
	m_pUI->tableServerList->setHorizontalHeaderItem(4,new QTableWidgetItem("status"));
}
void ServerRole::btnServerConnSlot(bool clicked)
{
	OnConnection();
}
void ServerRole::changeCheckMacListSlot(int state)
{
	if(state ==Qt::Checked)
	{
		m_pUI->tableMac->show();
		SetWOLDataToList();
	}
	else
	{
		m_pUI->tableMac->hide();
		m_pUI->tableMac->setRowCount(0);
	}
}
void ServerRole::changeCheckServerListSlot(int state)
{
	if(state ==Qt::Checked)
	{
		m_pUI->tableServerList->show();
		SetServerListToTable();
	}
	else
	{
		m_pUI->tableServerList->clear();
		m_pUI->tableServerList->hide();
		m_pUI->tableServerList->setRowCount(0);
		
	}
}
//------------------------------------------------------------------------------------------------
void ServerRole::checkServerState()
{
	QMap<QString,ServerSocketInfo*>::iterator it,end;
	for(it=m_SocketMap.begin(),end=m_SocketMap.end();it!=end;++it)
	{
		ServerSocketInfo * pSSI = it.value();
		if(!pSSI)	continue;
		if(!pSSI->m_bState) continue;
		int curTime = m_Time.elapsed();

		if(curTime - pSSI->m_iReceiveServerTime > 3000)
		{
			char acBuff[1]={0,};
			if(pSSI->m_pSocket->Receive(acBuff,1,0)!=0) return;

			pSSI->m_bState = false;
			if(pSSI->m_pSocket)
			{
				pSSI->m_pSocket->Close();
				delete pSSI->m_pSocket;
			}

			LauncherClient* pClient = findClient(it.key());
			if(!pClient) continue;
			//QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
			//그룹을 찾는다.
			QStandardItem* pGroupItem = FindGroupItem(m_SocketMap[it.key()]->m_strGroupName);
			if(!pGroupItem) continue;

			//	QStandardItem* pClientItem = FindClientItem(pClient);

			// 그룹안에 있는 클라이언트 찾는거.
			QStandardItem* pClientItem=NULL;

			for(int i=0,n=pGroupItem->rowCount();i<n;i++)
			{
				pClientItem = pGroupItem->child(i);
				if(pClientItem)
				{
					if(pClient == pClientItem->data().value<LauncherClient*>())
						break;
				}
			}
			//QStandardItem* pItem = pModel->item(pClientItem->row());
			//if(!pItem) return;

			for(int i = 0, kn = pClientItem->rowCount(); i < kn; ++i)
			{
				delete pClientItem->child(i, 3);
				delete pClientItem->child(i, 2);
				delete pClientItem->child(i, 1);
				delete pClientItem->child(i, 0);
			}
			pGroupItem->removeRows(pClientItem->row(),1);
			Release(it.key());
			UpdateServerListTable();
			pSSI->m_pSocket = new Socket();
		}
        //InternetAddress aAddr;
        //aAddr.SetAddress(m_LocalIP.toAscii(),0);
        //aAddr.SetPortByPortNum(CLIENT_PORT);
        //m_pTCPSocket->Bind(aAddr);

        //emit connectionTimerRestart();
    }
}
void ServerRole::initComm()
{
	LoadWOLData();
}
void ServerRole::Release(QString strHost)
{
	QMap<QString, NetMessageManager*>::iterator it0 = m_NetMessageManagerMap.find(strHost);
	if(it0 != m_NetMessageManagerMap.end())
	{
		delete it0.value();
		m_NetMessageManagerMap.remove(it0.key());
	}

    QMap<QString, LauncherClient*>::iterator it1 = m_mapClient.find(strHost);
	if(it1 != m_mapClient.end())
	{
		delete it1.value();
		m_mapClient.remove(it1.key());
	}
    
	//QMap<QString, ServerSocketInfo*>::iterator it2 = m_SocketMap.find(strHost);
	//if(it2 != m_SocketMap.end())
	//{
	//	delete it2.value();
	//	m_SocketMap.remove(it2.key());
	//}
    
}
void ServerRole::Release()
{
    QMap<QString, NetMessageManager*>::iterator it0, end0;
    for (it0 = m_NetMessageManagerMap.begin(), end0 = m_NetMessageManagerMap.end(); it0 != end0; ++it0)
    {
        delete it0.value();
    }
	QMap<QString, LauncherClient*>::iterator it1, end1;
    for (it1 = m_mapClient.begin(), end1 = m_mapClient.end(); it1 != end1; ++it1)
    {
        delete it1.value();
    }
	QMap<QString, ServerSocketInfo*>::iterator it2,end2;
	for (it2 = m_SocketMap.begin(), end2 = m_SocketMap.end(); it2 != end2; ++it2)
    {
        delete it2.value();
    }
    WriteWOLData();
}
//------------------------------------------------------------------------------------------------
void ServerRole::OnConnection()
{	
	for(int i =0;i<m_pUI->tableServerList->rowCount();i++)
	{
		QTableWidgetItem* pItem = m_pUI->tableServerList->item(i,1);
		QString strHost = pItem->text();
		QTableWidgetItem* pCheck = m_pUI->tableServerList->item(i,3);
	
		if(pCheck && pCheck->checkState() != Qt::Checked)
			continue;
		if(m_SocketMap[strHost]->m_bState) continue;

		InternetAddress aAddr;
		aAddr.SetAddress(m_LocalIP.toStdString().c_str(),0);
		aAddr.SetPortByPortNum(CLIENT_PORT+i);
		bool bBind = m_SocketMap[strHost]->m_pSocket->Bind(aAddr);
		
		InternetAddress aTargetAddr;
		aTargetAddr.SetAddress(strHost.toStdString().c_str(),0);
		aTargetAddr.SetPortByPortNum(SUBSERVER_PORT);

		if (m_SocketMap[strHost]->m_pSocket->Connect(aTargetAddr))
		{
			
			NetMessageManager* pNetMessageManager = new NetMessageManager(NetMessageManager::TCP);
			pNetMessageManager->SetName(strHost.toLocal8Bit().data());

			pNetMessageManager->SetSocket(m_SocketMap[strHost]->m_pSocket);
			NetMessageParser* pParser = pNetMessageManager->GetParser();
			pParser->SetGetMessageSizeFunc(&GetMessageSize);
			pParser->SetCreateMessageFunc(&CreateMessage);
			pParser->SetDeleteMessageFunc(&DeleteMessage);

			QMap<QString, NetMessageManager*>::iterator it = m_NetMessageManagerMap.find(strHost);

			if (m_NetMessageManagerMap.end() != it)
			{
				delete it.value();
				//m_NetMessageManagerMap.erase(it);
				m_NetMessageManagerMap.remove(it.key());
			}
			m_NetMessageManagerMap.insert(strHost, pNetMessageManager);
			pNetMessageManager->StartReceiving();
			m_SocketMap[strHost]->m_bState = true;
			//m_bReseted = false;
			m_SocketMap[strHost]->m_iReceiveServerTime = m_Time.elapsed();
		}
		//else
		//{
		//	m_SocketMap[strHost]->m_bState = false;
		//}
	}
	emit connectionTimerFin();
	UpdateServerListTable();
}
//------------------------------------------------------------------------------------------------
void ServerRole::Update()
{
    NetMessageManager* pNetMessageManager = 0;
    QMap<QString, NetMessageManager*>::iterator it, end;
    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        NetMessageParser* pParser = pNetMessageManager->GetParser();
        pParser->UpdateEvent();
    }
	ProcessMessage();
	checkServerState();
    
    //sendState();
}
//------------------------------------------------------------------------------------------------
//void ServerRole::sendState()
//{
//    int curTime = m_Time.elapsed();
//    if(curTime - m_iOldTime < 2000) return;
//
//    m_iOldTime = curTime;
//	char buff[1000]={0,};
//	memset(buff, 0,sizeof(buff));
//
//    ServerStatePacket msg;
//    msg.m_ucState = 1;
//    int iSize = msg.MakeDataBuff(buff);
//
//    NetMessageManager* pNetMessageManager = 0;
//    QMap<QString, NetMessageManager*>::iterator it, end;
//    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
//    {
//        pNetMessageManager = it.value();
//        pNetMessageManager->Send(buff, iSize);
//    }
//}
//------------------------------------------------------------------------------------------------
void ServerRole::ProcessMessage()
{
    list<NetMessage*> alist;

    NetMessageManager* pNetMessageManager = 0;
    QMap<QString, NetMessageManager*>::iterator it, end;
    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        NetMessageParser* pParser = pNetMessageManager->GetParser();

        alist = pParser->GetMessages(1);
        list<NetMessage*>::iterator it0, end0;
        for (it0 = alist.begin(), end0 = alist.end(); it0 != end0; ++it0)
        {
            switch ((*it0)->m_ucID)
            {
            case PACKET_CLIENT_INFO:
                {
                    ClientInfoPacket* pPacket = (ClientInfoPacket*)(*it0);
                    if(pPacket->m_ucClientOn)
                        addClient(pPacket, (*it));
                    else
					{
                        removeClient(pPacket);
						return;
					}
                }
                break;
			
            case PACKET_PROGRAM_INFO:
                {
                    ProgramInfoPacket* pPacket = (ProgramInfoPacket*)(*it0);
                    ProcessProgramInfoServer(pPacket);
                }
                break;
                /*case PACKET_COMMAND_INFO:
                commandProcess((CommandInfoPacket*)(*it));
                break;*/
                //case PACKET_CONNECTION:
                //	{
                //		NetMessage* pMessage = (NetMessage*)(*it0);
                //		m_mapClientConnection[pMessage->m_acIP] = GetTickCount();
                //	}
                //	break;
			case PACKET_SERVERSTATE:
				{
					m_SocketMap[QString::fromUtf8(pNetMessageManager->GetName().c_str())]->m_iReceiveServerTime = m_Time.elapsed();
				}
				break;

            }
        }
        pParser->DeleteMessages();
    }
}

//------------------------------------------------------------------------------------------------
void ServerRole::sendPacket(NetMessage* pMsg, QString logMsg)
{
	char buff[1000]={0,};
    memset(buff, 0, sizeof(buff));

    NetMessageManager* pNetMessageManager = 0;
    QMap<QString, NetMessageManager*>::iterator it, end;
    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        int iSize = pMsg->MakeDataBuff(buff);
        pNetMessageManager->Send(buff, iSize);
    }
}
//------------------------------------------------------------------------------------------------
void ServerRole::addClient(ClientInfoPacket* pPacket, NetMessageManager* pMessageManager)
{
    if(!pPacket) return;
    static int id = 0;
    QString msgLog = "";
    if(pPacket->m_ucType == REQ_PACKET)
    {
        LauncherClient* pClient = findClient(pPacket->m_acIP);
        if(!pClient)
        {
            pClient = new LauncherClient;
            m_mapClient[pPacket->m_acIP] = pClient;
            //m_mapClientConnection[pPacket->m_acIP] = GetTickCount();
            id++;
            pClient->SetID(id);
            pClient->setClientIP(pPacket->m_acIP);
            pClient->setClientName(pPacket->m_acClientName);
            pClient->SetMessageManager(pMessageManager);
			pClient->setClientType(pPacket->m_acClientType);

			// wol 하기위한 맥어드레스 추가
			char buff[100]={0,};
            GetMacAddress(pPacket->m_acIP, buff);
            QString aMacAddress = buff;
          //  if (! m_MacAddresses.contains(aMacAddress))
			if (! m_MacAddresses.contains(pPacket->m_acIP))
			{
				m_MacAddresses[pPacket->m_acIP] = aMacAddress;
                //m_MacAddresses.push_back(buff);
				//WriteWOLData();
			}
            addConnectionItem(pClient);

            msgLog = QString("[%1] %2").arg(pPacket->m_acIP).arg(QString::fromLocal8Bit("추가 패킷 수신"));
            emit writeLogSignal(msgLog);
        }
    }
    else
    {
    }
}
LauncherClient* ServerRole::findClient(QString ip)
{
    if(!m_mapClient.contains(ip))
        return NULL;
    return m_mapClient[ip];
}
//------------------------------------------------------------------------------------------------
void ServerRole::removeClient(QString strIP)
{
	//////
	//그룹을 찾는다.
	QStandardItem* pGroupItem = FindGroupItem(m_SocketMap[strIP]->m_strGroupName);
	if(!pGroupItem) return;
	// 삭제할 아이템 ..
	LauncherClient* pClient = m_mapClient[strIP];
	
	//if (pClient)
	//{
	//	m_mapClient.remove(strIP);
	//	//m_mapClientConnection.remove(strIP);
	//	delete pClient;
	//}
	Release(strIP);
	for(int i = 0, n = pGroupItem->rowCount(); i < n; ++i)
	{
		QStandardItem* pClientItem = pGroupItem->child(i);
		if(!pClientItem) continue;

		if(strIP.compare(pClientItem->text(), Qt::CaseInsensitive) == 0)
		{
			delete pGroupItem->child(i, 3);
			delete pGroupItem->child(i, 2);
			delete pGroupItem->child(i, 1);
			delete pGroupItem->child(i, 0);
			pGroupItem->removeRow(i);
			break;
		}
	}
	m_SocketMap[strIP]->m_bState = false;
	m_SocketMap[strIP]->m_pSocket->Close();
	delete m_SocketMap[strIP]->m_pSocket;
	m_SocketMap[strIP]->m_pSocket = new Socket;
	
	UpdateServerListTable();
	//Release(strIP);
	//////
	/*
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pModel) return;

    LauncherClient* pClient = m_mapClient[strIP];
	
    if (pClient)
    {
        m_mapClient.remove(strIP);
        //m_mapClientConnection.remove(strIP);
        delete pClient;
    }

    for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pModel->item(i);
        if(!pItem) continue;

        if(strIP.compare(pItem->text(), Qt::CaseInsensitive) == 0)
        {
			delete pModel->item(i, 3);
            delete pModel->item(i, 2);
            delete pModel->item(i, 1);
            delete pModel->item(i, 0);
            pModel->removeRow(i);
            break;
        }
    }
	*/
}
//------------------------------------------------------------------------------------------------
void ServerRole::removeClient(ClientInfoPacket* pPacket)
{
    if(!pPacket) return;

    QString strIP = pPacket->m_acIP;
    removeClient(strIP);

}
//--------------------------------------------------------------------------------------------------
void ServerRole::saveIpConfig(bool clicked)
{
	TiXmlDocument Doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	TiXmlElement* pConfigElem = new TiXmlElement("config");
	Doc.LinkEndChild(decl);
	//Doc.LinkEndChild(pConfigElem);
	//m_ServerIP = m_pUI->edTargetIp->text();
	//m_ServerPort = m_pUI->edTargetPort->text().toInt();
	//char tmpPort[6] ="";
	//sprintf_s(tmpPort,"%d",m_ServerPort);
	pConfigElem->SetAttribute("apptype","server");

	pConfigElem->SetAttribute("localIP",m_LocalIP.toStdString().c_str());
	pConfigElem->SetAttribute("targetIP","127.0.0.1");
	pConfigElem->SetAttribute("targetPort","9999");
	Doc.LinkEndChild(pConfigElem);

	Doc.SaveFile("init.config");
}
//------------------------------------------------------------------------------------------------
void ServerRole::onButtonSlot(bool clicked)
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    QStandardItem* pCurrentItem = pModel->itemFromIndex(m_pUI->treeConnectionList->currentIndex());

    if(!pCurrentItem) return;

    QStandardItem* pParent = pCurrentItem->parent();


    QString strIP = pParent->text();
    LauncherClient* pClient = findClient(strIP);
    if(!pClient) return;

    CommandInfoPacket packet;
    sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());

    QStandardItem* pProgramItem = pParent->child(pCurrentItem->row());

    packet.m_ProgramId = pProgramItem->data().value<unsigned char>();

    packet.m_ucCommand = PROGRAM_ON;
    packet.m_ucType = REQ_PACKET;

    NetMessageManager* pMessageManager = pClient->GetMessageManager();
	char buff[1000]={0,};
    int iSize = packet.MakeDataBuff(buff);
    pMessageManager->Send(buff, iSize);

    emit updateProgramStateSignal(packet.m_ProgramId, true, pClient,pParent->parent()->text());
}
//------------------------------------------------------------------------------------------------
void ServerRole::offButtonSlot(bool clicked)
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    QStandardItem* pCurrentItem = pModel->itemFromIndex(m_pUI->treeConnectionList->currentIndex());

    if(!pCurrentItem) return;

    QStandardItem* pParent = pCurrentItem->parent();

    QString strIP = pParent->text();
    LauncherClient* pClient = findClient(strIP);
    if(!pClient) return;

    CommandInfoPacket packet;
    sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());

    QStandardItem* pProgramItem = pParent->child(pCurrentItem->row());

    packet.m_ProgramId = pProgramItem->data().value<unsigned char>();

    packet.m_ucCommand = PROGRAM_OFF;
    packet.m_ucType = REQ_PACKET;

    NetMessageManager* pMessageManager = pClient->GetMessageManager();
	char buff[1000]={0,};
    int iSize = packet.MakeDataBuff(buff);
    pMessageManager->Send(buff, iSize);

    emit updateProgramStateSignal(packet.m_ProgramId, false, pClient,pParent->parent()->text());
}
//------------------------------------------------------------------------------------------------
void ServerRole::powerButtonSlot(bool clicked)
{
    //QList<QString>::iterator it, end;
	QMap<QString,QString>::iterator it, end;

    unsigned char* magicp = new unsigned char[DATA_SIZE_MAGICP];

    Socket* pSocket = new Socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    bool bOptVal = true;
    pSocket->SetOption(SOL_SOCKET, SO_BROADCAST,(char*)&bOptVal,sizeof(BOOL));

    InternetAddress m_ServerUDP;
    m_ServerUDP.SetAddress("255.255.255.255",0);
    m_ServerUDP.SetPortByPortNum(MAGICPPORT);

    for (it = m_MacAddresses.begin(), end = m_MacAddresses.end(); it != end; ++it)
    {
        //int size = createMagicP(*it, magicp);
		int size = createMagicP(it.value(), magicp);

        pSocket->SendTo((char*)magicp, size, m_ServerUDP);
    }
	delete magicp;
    delete pSocket;
}
//------------------------------------------------------------------------------------------------
void ServerRole::powerOffButtonSlot(bool clicked)
{
	LauncherClient* pLauncherClient =0;
	QMap<QString, LauncherClient*>::iterator it,end;
	//QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());

	for(it=m_mapClient.begin(),end=m_mapClient.end();it !=end;++it)
	{
		pLauncherClient = it.value();
		// 그룹을 찾는다
		QStandardItem* pGroupItem = FindGroupItem(m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName);
		QStandardItem* pClientItem =NULL;
		for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
		{
			pClientItem = pGroupItem->child(i);
			if(pClientItem->data().value<LauncherClient*>() == pLauncherClient)
				break;
		}
		QStandardItem* pCheck = pGroupItem->child(pClientItem->row(),2);
		if(pCheck && pCheck->checkState()!=Qt::Checked)
			continue;
		char buff[50]={0,};
		NetMessage aMsg(PACKET_SHUTDOWN);
		NetMessageManager* pMessageManager = pLauncherClient->GetMessageManager();
		int iSize = aMsg.MakeDataBuff(buff);
		pMessageManager->Send(buff, iSize);

	}	
}

//------------------------------------------------------------------------------------------------
void ServerRole::OnAllButtonSlot(bool clicked)
{
    CommandInfoPacket packet;
    sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
    packet.m_ucCommand = PROGRAM_ON;
    packet.m_ucType = REQ_PACKET;

	char buff[1000]={0,};
    LauncherClient* pLauncherClient = 0;
    QMap<QString, LauncherClient*>::iterator it, end;
    //QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
	
    for (it = m_mapClient.begin(), end = m_mapClient.end(); it != end; ++it)
    {
        pLauncherClient = it.value();
		// 그룹을 찾는다
		QString strGroupName = m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName;
		QStandardItem* pGroupItem = FindGroupItem(strGroupName);
		//
		QStandardItem* pClientItem =NULL;
		for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
		{
			//클라이언트를 찾는다
			pClientItem = pGroupItem->child(i);
			if(pLauncherClient == pClientItem->data().value<LauncherClient*>())
				break;
		}
		QMap<unsigned char, LauncherProgramInfo*>& aInfoMap = pLauncherClient->getProgramMap();

		QMap<unsigned char, LauncherProgramInfo*>::iterator it0, end0;

		LauncherProgramInfo* pLauncherProgramInfo = 0;
		int iProgramIdx =0;
		for (it0 = aInfoMap.begin(), end0 = aInfoMap.end(); it0 != end0; ++it0)
		{
			pLauncherProgramInfo = it0.value();
			////////////
			if(!pClientItem) continue;

			QStandardItem* pProgCheck =pClientItem->child(iProgramIdx,2);
			if(pProgCheck->checkState() !=Qt::Checked)
			{
				iProgramIdx++;
				continue;
			}
			packet.m_ProgramId = pLauncherProgramInfo->m_ucID;
			pLauncherProgramInfo->m_ucState =! pLauncherProgramInfo->m_ucState;

			NetMessageManager* pMessageManager = pLauncherClient->GetMessageManager();

			int iSize = packet.MakeDataBuff(buff);

			pMessageManager->Send(buff, iSize);
			iProgramIdx++;
			emit updateProgramStateSignal(pLauncherProgramInfo->m_ucID, true, pLauncherClient,strGroupName);
		}
	}
}
//------------------------------------------------------------------------------------------------
void ServerRole::OffAllButtonSlot(bool clicked)
{
	CommandInfoPacket packet;
	sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
	packet.m_ucCommand = PROGRAM_OFF;
	packet.m_ucType = REQ_PACKET;
	///////////
	char buff[1000]={0,};
	LauncherClient* pLauncherClient = 0;
	QMap<QString, LauncherClient*>::iterator it, end;
	//QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
	//////	
	for (it = m_mapClient.begin(), end = m_mapClient.end(); it != end; ++it)
	{
		pLauncherClient = it.value();
		QString strGroupName = m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName;
		QStandardItem* pGroupItem = FindGroupItem(strGroupName);
		//
		QStandardItem* pClientItem =NULL;

		for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
		{
			// 클라이언트를 찾는다
			pClientItem = pGroupItem->child(i);
			if(pLauncherClient == pClientItem->data().value<LauncherClient*>())
				break;
		}

		QMap<unsigned char, LauncherProgramInfo*>& aInfoMap = pLauncherClient->getProgramMap();
		QMap<unsigned char, LauncherProgramInfo*>::iterator it0, end0;
		LauncherProgramInfo* pLauncherProgramInfo = 0;

		int iProgramIdx =0;
		for (it0 = aInfoMap.begin(), end0 = aInfoMap.end(); it0 != end0; ++it0)
		{
			pLauncherProgramInfo = it0.value();
			////////////
			if(!pClientItem) continue;

			QStandardItem* pProgCheck =pClientItem->child(iProgramIdx,2);
			if(pProgCheck->checkState() !=Qt::Checked)
			{
				iProgramIdx++;
				continue;
			}

			packet.m_ProgramId = pLauncherProgramInfo->m_ucID;
			pLauncherProgramInfo->m_ucState =! pLauncherProgramInfo->m_ucState;

			NetMessageManager* pMessageManager = pLauncherClient->GetMessageManager();

			int iSize = packet.MakeDataBuff(buff);

			pMessageManager->Send(buff, iSize);
			iProgramIdx++;
			emit updateProgramStateSignal(pLauncherProgramInfo->m_ucID, false, pLauncherClient,strGroupName);
			
		}
	}
}
//------------------------------------------------------------------------------------------------
void ServerRole::treeItemChangedSlot(QStandardItem* item)
{
	//////////////////

	if(!item->isCheckable()) return;

	QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
	QModelIndex index = pModel->indexFromItem(item);

	QStandardItem* pParent = item->parent();
	if(!pParent)// 그룹 체크박스 선택일경우
	{
		QStandardItem* pFirstItem = pModel->item(index.row());
		for(int i=0,n=pFirstItem->rowCount();i<n;++i)
		{
			QStandardItem* pChild = pFirstItem->child(i,2);
			if(pChild && pChild->isCheckable())
				pChild->setCheckState(item->checkState());
		}
		return;	
	}

	QStandardItem* pGParent = pParent->parent();
	if(!pGParent)	// client의 체크박스 선택일경우
	{	
		QStandardItem* pChild = pParent->child(index.row());
		for(int i=0,n=pChild->rowCount();i<n;i++)
		{
			QStandardItem* pProgItem = pChild->child(i,2);
			if(pProgItem&&pProgItem->isCheckable())
			{
				pProgItem->setCheckState(item->checkState());
			}
		}
		return;
	}
	else
	{	//subclient 의 프로그램 체크박스 선택일경우
		bool bcheck = item->checkState()==Qt::Checked ? true : false;
		QString strClientIP = pParent->text();
		QString strServerIP = pGParent->text();
		QStandardItem* pProgramItem = pParent->child(item->row());
		unsigned char ucProgId = pProgramItem->data().value<unsigned char>();
		//SendChangedCheckBox(strServerIP, strClientIP, ucProgId, bcheck);
	}
	///////////////////////////////////////////////
	/*
    if(!item->isCheckable()) return;

    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());

    QModelIndex index = pModel->indexFromItem(item);

    QStandardItem* pParent = item->parent();
    QStandardItem* pFirstItem = pModel->item(index.row());
    if(!pParent && pFirstItem->hasChildren())
    {
        for(int i = 0, n = pFirstItem->rowCount(); i < n; ++i)
        {
            QStandardItem* pChild = pFirstItem->child(i, 2);
            if(pChild && pChild->isCheckable())
                pChild->setCheckState(item->checkState());
        }
    }
	*/
}

//------------------------------------------------------------------------------------------------
void ServerRole::ProcessProgramInfoServer(ProgramInfoPacket* pPacket)
{   
    LauncherClient* pClient = findClient(pPacket->m_acIP);

    if (pClient)
    {
        QDir dir(pPacket->m_acPath);

        switch (pPacket->m_Action)
        {
        case ProgramInfoPacket::ADD:
            {
                LauncherProgramInfo* pInfo = pClient->addProgramInfo(pPacket->m_ProgramId, false, dir.dirName(), pPacket->m_acPath);

                QString msgLog = "";
                msgLog = QString("[%1] %2 %3").arg(pPacket->m_acIP).arg(dir.dirName()).arg(QString::fromLocal8Bit("추가"));
                emit writeLogSignal(msgLog);

                //if (pClient == m_pActiveLauncherClient)

                AddProgramServer(pClient, pInfo);
            }
            break;
        case ProgramInfoPacket::REMOVE:
            {
                LauncherProgramInfo* pInfo = pClient->getProgramInfo(pPacket->m_ProgramId);

                //if (pClient == m_pActiveLauncherClient)
                RemoveProgramServer(pClient, pInfo);

                QString strIP = pPacket->m_acIP;

            }
            break;
        }
    }
}
//------------------------------------------------------------------------------------------------
void ServerRole::addConnectionItem(LauncherClient* pLauncherClient)
{
	//트리에 서버 추가 하는부분.
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pModel) return;

	// 그룹 추가
	QString strGroupName = m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName;
	QStandardItem* pGroup = FindGroupItem(strGroupName);
	if(!pGroup)
	{
		QList<QStandardItem*> lstGroupItem;
		QStandardItem *pType,*pCheck;
		pGroup = new QStandardItem(strGroupName);
		pGroup->setData(QVariant::fromValue<QString>(strGroupName));
		pType = new QStandardItem(QString::fromUtf8("Group"));
		pCheck = new QStandardItem;
		pCheck->setCheckable(true);
		pCheck->setCheckState(Qt::Checked);
		lstGroupItem<<pGroup<<pType<<pCheck;
		pModel->insertRow(pModel->rowCount(), lstGroupItem);

	}
	
	// 서버 추가
	QStandardItem* pCol1, *pCol2, *pCol3,*pCol4;
    //QList<QStandardItem*> lstItem;

	pCol1 = new QStandardItem(pLauncherClient->getClientIP());
    pCol1->setData(QVariant::fromValue<LauncherClient*>(pLauncherClient));

    pCol2 = new QStandardItem(pLauncherClient->getClientName());
    pCol3 = new QStandardItem;
    pCol3->setCheckable(true);
    pCol3->setCheckState(Qt::Checked);
	pCol4 = new QStandardItem(pLauncherClient->getClientType());

	int row = pGroup->rowCount();
	pGroup->setChild(row, 0, pCol1);
	pGroup->setChild(row, 1, pCol2);
	pGroup->setChild(row, 2, pCol3);
	pGroup->setChild(row, 3, pCol4);

    m_ConnectionListSel = pModel->indexFromItem(pCol1);

	/*
	////////
	QStandardItem* pClientItem = FindClientItem(pLauncherClient);
	if(!pClientItem) return;

	QStandardItem* pCol1, *pCol2, *pCol3, *pCol4;
	QList<QStandardItem*> list;
	pCol1 = new QStandardItem(pInfo->m_ip);
	pCol1->setData(QVariant::fromValue<unsigned char>(pInfo->m_ucID));

	pCol2 = new QStandardItem(pInfo->m_strName);
	//pCol2->setData(QVariant::fromValue<LauncherProgramInfo*>(pInfo));

	pCol3 = new QStandardItem;
	pCol3->setCheckable(true);
	pCol3->setCheckState(Qt::Checked);
	pCol4 = new QStandardItem("subclient");
	//list<<pCol1<<pCol2;
	//pModel->appendRow(list);
	int row = pClientItem->rowCount();
	pClientItem->setChild(row, 0, pCol1);
	pClientItem->setChild(row, 1, pCol2);
	pClientItem->setChild(row, 2, pCol3);
	pClientItem->setChild(row, 3, pCol4);


	//////////
    QStandardItem* pCol1, *pCol2, *pCol3,*pCol4;
    QList<QStandardItem*> lstItem;
    pCol1 = new QStandardItem(pLauncherClient->getClientIP());
    pCol1->setData(QVariant::fromValue<LauncherClient*>(pLauncherClient));

    pCol2 = new QStandardItem(pLauncherClient->getClientName());
    pCol3 = new QStandardItem;
    pCol3->setCheckable(true);
    pCol3->setCheckState(Qt::Checked);
	pCol4 = new QStandardItem(pLauncherClient->getClientType());
    lstItem<<pCol1<<pCol2<<pCol3<<pCol4;
    pModel->insertRow(pModel->rowCount(), lstItem);

    //m_pUI->treeConnectionList->setIndexWidget(pModel->index(row, 2), new QCheckBox);
    m_ConnectionListSel = pModel->indexFromItem(pCol1);
	*/
}
//------------------------------------------------------------------------------------------------
void ServerRole::storeMacAddressFromIP(QString reqIP)
{
    char addr[256] = {0,};
    GetMacAddress(reqIP.toStdString().c_str(), addr);
    LauncherClient* pClient = findClient(reqIP);
    if(pClient)
    {
        if(strcmp(addr, "000000000000") == 0) return;
        QString str = QString::fromLocal8Bit("[%1] %2 MacAddress 확인").arg(reqIP).arg(addr);
        emit writeLogSignal(str);
        pClient->setMacAddress(addr);
    }
}

void ServerRole::SetServerListToTable()
{
	//table에 server ip주소 추가
	m_pUI->tableServerList->setRowCount(0);
	QMap<QString, ServerSocketInfo*>::iterator it2,end2;
	int i=0;
	for (it2 = m_SocketMap.begin(), end2 = m_SocketMap.end(); it2 != end2; ++it2)
    {
        ServerSocketInfo* pServerSocketInfo = it2.value();
		QTableWidgetItem* pCol1,*pCol2,*pCol3,*pCol4,*pCol5;
		QList<QTableWidgetItem*> lstItem;

		pCol1 = new QTableWidgetItem(pServerSocketInfo->m_strName);
		pCol2 = new QTableWidgetItem(pServerSocketInfo->m_strHost);
		pCol3 = new QTableWidgetItem(pServerSocketInfo->m_strGroupName);
		pCol4 = new QTableWidgetItem;
		if(pServerSocketInfo->m_bState)
		{
			pCol4->setCheckState(Qt::Unchecked);
			//pCol4->setFlags((Qt::ItemIsUserCheckable));
			pCol5 = new QTableWidgetItem("connected") ;
		}
		else
		{
			pCol4->setCheckState(Qt::Checked);
			//pCol3->setEnabled(true);
			//pCol3->setFlags(Qt::ItemIsUserCheckable);
			pCol5 = new QTableWidgetItem("disconnected");
		}
		m_pUI->tableServerList->insertRow(i);
		int j = m_pUI->tableMac->columnCount();
		m_pUI->tableServerList->setItem(i,0,pCol1);
		m_pUI->tableServerList->setItem(i,1,pCol2);
		m_pUI->tableServerList->setItem(i,2,pCol3);
		m_pUI->tableServerList->setItem(i,3,pCol4);
		m_pUI->tableServerList->setItem(i,4,pCol5);
		i++;
	}
}
void ServerRole::UpdateServerListTable()
{
	//table에 server ip주소 추가
	//m_pUI->tableServerList->setRowCount(0);
	QMap<QString, ServerSocketInfo*>::iterator it2,end2;
	int i=0;
	for (it2 = m_SocketMap.begin(), end2 = m_SocketMap.end(); it2 != end2; ++it2)
	{
		ServerSocketInfo* pServerSocketInfo = it2.value();
		QTableWidgetItem* pCol4,*pCol5;
		//QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->tableServerList->model());

		pCol4 = m_pUI->tableServerList->item(i,3);
		pCol5 = m_pUI->tableServerList->item(i,4);
		if(pServerSocketInfo->m_bState)
		{
			pCol4->setCheckState(Qt::Unchecked);
			//pCol4->setFlags((Qt::ItemIsUserCheckable));
			pCol5->setText("connected");
		}
		else
		{
			pCol4->setCheckState(Qt::Checked);
			//pCol4->setFlags((Qt::ItemIsUserCheckable));
			//pCol3->setFlags(Qt::ItemIsUserCheckable);
			pCol5->setText("disconnected");
		}
		i++;
	}
	m_pUI->tableServerList->viewport()->update();
}
void ServerRole::SetWOLDataToList()
{
	//리스트에 mac주소 추가
//	QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->listMac->model());
//	if(!pModel) return;

	QStandardItemModel *model = new QStandardItemModel();
	QMap<QString,QString>::iterator it, end;

	int i=0;
	for (it = m_MacAddresses.begin(), end = m_MacAddresses.end(); it != end; ++it)
	{
	//	QStandardItem* pCol1, *pCol2, *pCol3;
		QTableWidgetItem* pCol1,*pCol2,*pCol3;
		QList<QTableWidgetItem*> lstItem;
	
		//pCol1 = new QStandardItem(it.key());
		//pCol2 = new QStandardItem(it.value());
		pCol1 = new QTableWidgetItem(it.key());
		pCol2 = new QTableWidgetItem(it.value());
		pCol3 = new QTableWidgetItem;
		pCol3->setCheckState(Qt::Checked);
		
		//pCol3 = new QStandardItem;
		//pCol3->setCheckable(true);
		//pCol3->setCheckState(Qt::Checked);
	//	lstItem<<pCol1<<pCol2<<pCol3;
//		model->insertRow(0,lstItem);
		m_pUI->tableMac->insertRow(i);
		int j = m_pUI->tableMac->columnCount();
		m_pUI->tableMac->setItem(i,0,pCol1);
		m_pUI->tableMac->setItem(i,1,pCol2);
		m_pUI->tableMac->setItem(i,2,pCol3);
		//m_pUI->tableMac->setItem(m_pUI->tableMac->currentRow(),2,pCol3);
		++i;
	}
		//m_pUI->listMac->setModelColumn(3);
	//m_pUI->tableMac->setModel(model);
	//pModel->insertRow(pModel->rowCount(), lstItem);
	//m_pUI->treeConnectionList->setIndexWidget(pModel->index(row, 2), new QCheckBox);
	//m_ConnectionListSel = pModel->indexFromItem(pCol1);
}
//--------------------------------------------------------------------------------------
void ServerRole::LoadWOLData()
{
    TiXmlDocument aDoc;    
    QString strAppType;

    if (! aDoc.LoadFile("WOL.dat")) return;

    TiXmlElement* pRootElem = aDoc.RootElement();

    TiXmlElement* pMacAddressElem = 0;

    for (pMacAddressElem = pRootElem->FirstChildElement("MacAddress");
        pMacAddressElem != NULL; pMacAddressElem = pMacAddressElem->NextSiblingElement("MacAddress"))
    {
        //QString aMacAddress = pMacAddressElem->Attribute("name");
        //m_MacAddresses.push_back(aMacAddress);
		QString aMacAddress = pMacAddressElem->Attribute("name");
		QString aWolIp = pMacAddressElem->Attribute("ip");
		m_MacAddresses[aWolIp] = aMacAddress;
    }
	//SetWOLDataToList();
}
//--------------------------------------------------------------------------------------
void ServerRole::WriteWOLData()
{
    //if(m_MacAddresses.count() == 0) return;

    TiXmlDocument Doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement* pRootElem = new TiXmlElement("MacAddresses");

    Doc.LinkEndChild(decl);
    Doc.LinkEndChild(pRootElem);

    //QList<QString>::iterator it, end;
	QMap<QString,QString>::iterator it,end;
    for (it = m_MacAddresses.begin(), end = m_MacAddresses.end(); it != end; ++it)
    {
        TiXmlElement* pAddressElem = new TiXmlElement("MacAddress");
        pAddressElem->SetAttribute("name", it.value().toStdString().c_str());
		pAddressElem->SetAttribute("ip",it.key().toStdString().c_str());
        pRootElem->LinkEndChild(pAddressElem);
    }

    Doc.SaveFile("WOL.dat");
}
//------------------------------------------------------------------------------------------------
void ServerRole::AddProgramServer(LauncherClient* pLauncherClient, LauncherProgramInfo* pInfo)
{
	if (!pInfo || !pLauncherClient) return;
	//그룹을 찾는다.
	QStandardItem* pGroupItem = FindGroupItem(m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName);
 	if(!pGroupItem) return;
	// 추가할 프로그램 아이템 ..
	QStandardItem* pCol1, *pCol2, *pCol3,*pCol4;
	QList<QStandardItem*> list;
	pCol1 = new QStandardItem(pInfo->m_strName);
	pCol1->setData(QVariant::fromValue<unsigned char>(pInfo->m_ucID));
	pCol2 = new QStandardItem(pInfo->m_ucState == 0 ? "OFF" : "ON");
	pCol2->setData(QVariant::fromValue<LauncherProgramInfo*>(pInfo));
	pCol3 = new QStandardItem;
	pCol3->setCheckable(true);
	pCol3->setCheckState(Qt::Checked);
	pCol4 = new QStandardItem("program");

	// 그룹에 있는 서버를 찾는다..
	// 거기에 프로그램아이템을 추가한다.
	//QStandardItem* pClientItem = FindClientItem(pLauncherClient);

	//LauncherClient* clientID = pClient->m_ucID;

	for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
	{
		QStandardItem* pGroupChild = pGroupItem->child(i);
		LauncherClient* pClientTree  = pGroupChild->data().value<LauncherClient*>();

		if (pClientTree  == pLauncherClient)
		{
			int row = pGroupChild->rowCount();
			pGroupChild->setChild(row, 0, pCol1);
			pGroupChild->setChild(row, 1, pCol2);
			pGroupChild->setChild(row, 2, pCol3);
			pGroupChild->setChild(row, 3, pCol4);
			break;
		}
	}

	/*
    if (!pInfo || !pLauncherClient) return;

    QStandardItem* pClientItem = FindClientItem(pLauncherClient);
    if(!pClientItem) return;

    QStandardItem* pCol1, *pCol2, *pCol3,*pCol4;
    QList<QStandardItem*> list;
    pCol1 = new QStandardItem(pInfo->m_strName);
    pCol1->setData(QVariant::fromValue<unsigned char>(pInfo->m_ucID));

    pCol2 = new QStandardItem(pInfo->m_ucState == 0 ? "OFF" : "ON");
    pCol2->setData(QVariant::fromValue<LauncherProgramInfo*>(pInfo));

    pCol3 = new QStandardItem;
    pCol3->setCheckable(true);
    pCol3->setCheckState(Qt::Checked);

	pCol4 = new QStandardItem("program");

    //list<<pCol1<<pCol2;
    //pModel->appendRow(list);
    int row = pClientItem->rowCount();
    pClientItem->setChild(row, 0, pCol1);
    pClientItem->setChild(row, 1, pCol2);
    pClientItem->setChild(row, 2, pCol3);
	pClientItem->setChild(row, 3, pCol4);
    //m_pUI->treeConnectionList->setIndexWidget(pModel->index(pCol1->row(), 2), new QCheckBox);
	*/
}
//------------------------------------------------------------------------------------------------
QStandardItem* ServerRole::FindClientItem(LauncherClient* pLauncherClient)
{
    if(!pLauncherClient) return NULL;

    QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pProgramModel) return NULL;

    QStandardItem* pClientItem = NULL;
    for(int i = 0, n = pProgramModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pProgramModel->item(i);
        if(pItem)
        {
            if(pLauncherClient == pItem->data().value<LauncherClient*>())
                return pItem;
        }
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------
QStandardItem* ServerRole::FindGroupItem(QString strGroupName)
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pModel) return NULL;

    for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pModel->item(i);
        if(pItem)
        {
            if(strGroupName== pItem->data().value<QString>())
                return pItem;
        }
    }
    return NULL;
}

//------------------------------------------------------------------------------------------------
void ServerRole::RemoveProgramServer(LauncherClient* pLauncherClient, LauncherProgramInfo* pLauncherProgramInfo)
{
    if (! pLauncherProgramInfo) return;

    QStandardItem* pClientItem = FindClientItem(pLauncherClient);
    if(!pClientItem) return;

    for (int i = 0, n = pClientItem->rowCount(); i < n; ++i)
    {
        QStandardItem* pCol = pClientItem->child(i);
        unsigned char id = pCol->data().value<unsigned char>();

        if (id == pLauncherProgramInfo->m_ucID)
        {
            QStandardItem* pDel = pClientItem->child(i, 0);
            delete pDel;
            pDel = pClientItem->child(i, 1);
            delete pDel;
            pDel = pClientItem->child(i, 2);
            delete pDel;
			pDel = pClientItem->child(i, 3);
			delete pDel;

            pClientItem->removeRow(i);
            return;
        }
    }
}
//------------------------------------------------------------------------------------------------
QStandardItem* ServerRole::FindProgramItem(LauncherClient* pLauncherClient, unsigned char id)
{
    if(!pLauncherClient) return NULL;

    QStandardItem* pClientItem = FindClientItem(pLauncherClient);
    if(!pClientItem) return NULL;

    for(int i = 0, n = pClientItem->rowCount(); i < n; ++i)
    {
        QStandardItem* pChild = pClientItem->child(i);
        if(id == pChild->data().value<unsigned char>())
            return pChild;
    }
    return NULL;
}
//------------------------------------------------------------------------------------------------
void ServerRole::updateProgramList(QString strIP)
{
    QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pProgramModel) return;

    if(strIP.isNull() || strIP.isEmpty())
    {
        pProgramModel->clear();
        return;
    }
    LauncherClient* pClient = findClient(strIP);
    if(!pClient) return;

    pProgramModel->removeRows(0, pProgramModel->rowCount());
    m_pUI->treeConnectionList->reset();

    QMap<unsigned char, LauncherProgramInfo*>& programMap = pClient->getProgramMap();
    QList<QStandardItem*> list;

    QMap<unsigned char, LauncherProgramInfo*>::iterator it, end;
    for (it = programMap.begin(), end = programMap.end(); it != end; ++it)
    {
        LauncherProgramInfo* pInfo = it.value();

        QStandardItem* pCol1, *pCol2;
        pCol1 = new QStandardItem(pInfo->m_strName);
        pCol1->setData(QVariant::fromValue<unsigned char>(pInfo->m_ucID));
        pCol2 = new QStandardItem(pInfo->m_ucState == 0 ? "OFF" : "ON");
        pCol2->setData(QVariant::fromValue<unsigned char>(pInfo->m_ucID));
        list<<pCol1<<pCol2;

        pProgramModel->appendRow(list);
        list.clear();
    }
}
//------------------------------------------------------------------------------------------------
void ServerRole::updateProgramList(int row)
{
    if (row < 0)
    {
        updateProgramList("");
        return;
    }

    QStandardItemModel* pClientModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pClientModel) return;

    QStandardItem* pItem = pClientModel->item(row);
    if (! pItem) return;

    QVariant aVar = pItem->data();
    LauncherClient* pClient = aVar.value<LauncherClient*>();

    updateProgramList(pClient->getClientIP());
}
//void ServerRole::ConnectionCheckSlot()
//{
//	int iCurTime = GetTickCount();
//	QMap<QString, int>::iterator it, end;
//	for (it = m_mapClientConnection.begin(), end = m_mapClientConnection.end(); it != end; ++it)
//	{
//		if (iCurTime - it.value() > 2000)
//			removeClient(it.key());
//	}
//}
//------------------------------------------------------------------------------------------------
void ServerRole::treeSelectSlot(const QModelIndex& index)
{
    //QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    //QStandardItem* pCol = pModel->item(index.row(), 0);
    //QVariant aVar = pCol->data();
    //m_pActiveLauncherClient = aVar.value<LauncherClient*>();

    //updateProgramList(m_pActiveLauncherClient->getClientIP());
}
//------------------------------------------------------------------------------------------------
void ServerRole::rebootSlot(bool clicked)
{

	LauncherClient* pLauncherClient =0;
	QMap<QString, LauncherClient*>::iterator it,end;
	//QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());

	for(it=m_mapClient.begin(),end=m_mapClient.end();it !=end;++it)
	{
		pLauncherClient = it.value();
		// 그룹을 찾는다
		QStandardItem* pGroupItem = FindGroupItem(m_SocketMap[pLauncherClient->getClientIP()]->m_strGroupName);
		QStandardItem* pClientItem =NULL;
		for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
		{
			pClientItem = pGroupItem->child(i);
			if(pClientItem->data().value<LauncherClient*>() == pLauncherClient)
				break;
		}
		QStandardItem* pCheck = pGroupItem->child(pClientItem->row(),2);
		if(pCheck && pCheck->checkState()!=Qt::Checked)
			continue;
		char buff[50]={0,};
		NetMessage aMsg(PACKET_REBOOT);
		NetMessageManager* pMessageManager = pLauncherClient->GetMessageManager();
		int iSize = aMsg.MakeDataBuff(buff);
		pMessageManager->Send(buff, iSize);
	}	
    
}
//------------------------------------------------------------------------------------------------
/*
void ServerRole::delButtonSlot(bool clicked)
{
    int row = m_pUI->treeConnectionList->currentIndex().row();
    if(row < 0) return;

    QStandardItemModel* pConModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pConModel) return;

    QStandardItem* pConItem = pConModel->item(m_pUI->treeConnectionList->currentIndex().row(), 0);

    LauncherClient* pSelClient = pConItem->data().value<LauncherClient*>();

    QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pProgramModel) return;

    QStandardItem* pCol1 = pProgramModel->item(row, 0);
    QStandardItem* pCol2 = pProgramModel->item(row, 1);

    QVariant aVar = pCol1->data();
    unsigned char id = aVar.value<unsigned char>();

    pProgramModel->removeRows(row, 1);
    QString removePath = pSelClient->getProgramInfo(id)->m_strPath;
    pSelClient->removeProgramInfo(id);

    ProgramInfoPacket aPacket;
    //QString strHomeName = QHostInfo::localHostName();
    sprintf_s(aPacket.m_acIP , 16, "%s", m_LocalIP.toStdString().c_str());

    aPacket.m_Action    = ProgramInfoPacket::REMOVE;
    aPacket.m_ProgramId = id;

    sprintf_s(aPacket.m_acPath, 260, "%s", removePath.toLocal8Bit().data());    

    QString strLog = QString("%1 %2").arg(removePath).arg(QString::fromLocal8Bit("삭제 패킷 전송"));    

    sendPacket(&aPacket, strLog);
}
*/
bool ServerRole::addServerInfo()
{
	TiXmlDocument aDoc;    
	QString strAppType;

	if (! aDoc.LoadFile("serverlist.config")) return false;

    TiXmlElement* pRootElem = aDoc.RootElement();
    const char* pStr;

    TiXmlElement* pTargetIpElem = 0;

    const char* strIp;
	const char* strName;
	const char* strGroup;
    for (pTargetIpElem = pRootElem->FirstChildElement("target"); pTargetIpElem != NULL;
        pTargetIpElem = pTargetIpElem->NextSiblingElement("target"))
    {
        strIp = pTargetIpElem->Attribute("targetIp");
        /*if (! strIp)
            continue;*/
		strName = pTargetIpElem->Attribute("targetName");
		strGroup = pTargetIpElem->Attribute("group");
//        m_strTargetIp.push_back(QString(strIp));
		Socket* pSocket = new Socket;
		ServerSocketInfo* pSocketInfo = new ServerSocketInfo(pSocket,false,QString(strName),QString(strIp),QString(strGroup));
		m_SocketMap[strIp] = pSocketInfo;
		strName ="";
		strGroup ="";
		strIp = "";
    }
	SetServerListToTable();
	return true;
}
/*------------------------------------------서버 함수 끝-----------------------------------------*/

/*------------------------------------------------------------------------*/
ClientRole::ClientRole() : LauncherRole(),m_bReseted(false),m_iOldTime(0)
{
    m_pTCPSocket = new Socket;
    m_Time = QTime::currentTime();
	m_ucProgramId =0;
}
ClientRole::~ClientRole()
{
}
void ClientRole::setLocalIP(QString strLocalIP)
{
	m_LocalIP = strLocalIP;
}
ClientRole& ClientRole::GetInstance()
{
    return sm_Instance;
}

void ClientRole::initUI()
{
    connect(m_pUI->btnAdd, SIGNAL(clicked(bool)), this, SLOT(addButtonSlot(bool)));
    connect(m_pUI->btnDelete, SIGNAL(clicked(bool)), this, SLOT(delButtonSlot(bool)));
	connect(m_pUI->btnSaveConf, SIGNAL(clicked(bool)), this, SLOT(saveIpConfig(bool)));

    //m_pUI->treeConnectionList->setVisible(false);
    m_pUI->btnOn->hide();
    m_pUI->btnOff->hide();
    m_pUI->btnPower->hide();
    m_pUI->btnOnAll->hide();
    m_pUI->btnOffAll->hide();
    m_pUI->btnReboot->hide();
	
	
	m_pUI->tableMac->hide();
	m_pUI->checkMacList->hide();
	m_pUI->btnPowerOff->hide();
	m_pUI->checkServerList->hide();
	m_pUI->tableServerList->hide();
	m_pUI->btnServerConn->hide();
//	m_pUI->btnConnMain->hide();
	QMap<unsigned char,QString>::iterator it,end;
	for (it=m_mapPrograms.begin(),end=m_mapPrograms.end();it!=end;++it)
	{
		AddProgramClient(it.value());
	}
}
//------------------------------------------------------------------------------------------------
void ClientRole::initComm()
{
	bool bBind = false;
	do
	{
		//서버 기능
		InternetAddress aAddr;
		aAddr.SetAddress(m_LocalIP.toStdString().c_str(),0);
		aAddr.SetPortByPortNum(SUBSERVER_PORT);
		bBind =m_pTCPSocket->Bind(aAddr);
	}while(!bBind);

    m_pTCPSocket->Listen(5);
    m_pTCPSocket->SetBlocking(false);
	m_ConnectionCheckTimer.start(2000);
	//bool bBind = false;
	//do{

	//	InternetAddress aAddr;
	//	aAddr.SetAddress(m_LocalIP.toStdString().c_str(),0);
	//	aAddr.SetPortByPortNum(CLIENT_PORT);
	//	bBind = m_pTCPSocket->Bind(aAddr);
	//}while(!bBind);

    loadProgramList();
}
//------------------------------------------------------------------------------------------------
void ClientRole::Release()
{
	ClientInfoPacket packet;
	QString strHomeName = QHostInfo::localHostName();
	sprintf_s(packet.m_acClientName, 24, "%s", strHomeName.isEmpty() ? "NoName" : strHomeName.toStdString().c_str());
	sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
	sprintf_s(packet.m_acClientType,7,"%s","client");
	packet.m_uiProgramCount = 1;
	packet.m_ucClientOn = CLIENT_CONNECTIONLESS;
	sendPacket(&packet, QString::fromLocal8Bit("클라이언트 연결 패킷 전송"));

	QMap<QString, NetMessageManager*>::iterator it, end;
	NetMessageManager* pNetMessageManager = 0;
	for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
	{
		pNetMessageManager = it.value();
		delete pNetMessageManager;
		pNetMessageManager =0;
	}
   // if(m_bModified)
   //     saveProgramList();
  //  if (m_pNetMessageManager)
  //  {
  //      ClientInfoPacket packet;
  //      QString strHomeName = QHostInfo::localHostName();
  //      sprintf_s(packet.m_acClientName, 24, "%s", strHomeName.isEmpty() ? "NoName" : strHomeName.toStdString().c_str());
  //      sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
  //      packet.m_uiProgramCount = 1;
  //      packet.m_ucClientOn = CLIENT_CONNECTIONLESS;

		//char buff[1000]={0,};
  //      int iSize = packet.MakeDataBuff(buff);

  //      m_pNetMessageManager->Send(buff, iSize);

  //      delete m_pNetMessageManager;
  //      m_pNetMessageManager = 0;
  //  }
}
//------------------------------------------------------------------------------------------------
void ClientRole::Update()
{
	NetMessageManager* pNetMessageManager = 0;
	QMap<QString, NetMessageManager*>::iterator it, end;
	for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
	{
		pNetMessageManager = it.value();
		NetMessageParser* pParser = pNetMessageManager->GetParser();
		pParser->UpdateEvent();
		ProcessMessage();
		//checkServerState();
	}
	sendState();
    /*if(!m_pNetMessageManager) return;
    NetMessageParser* pParser = m_pNetMessageManager->GetParser();

    pParser->UpdateEvent();

    ProcessMessage();

    NetMessage aMsg(PACKET_CONNECTION);

	char buff[100]={0,};
    int iSize = aMsg.MakeDataBuff(buff);

    m_pNetMessageManager->Send(buff, iSize);
	*/ // serverRole에 추가..
    //checkServerState();
}
//------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------
void ClientRole::ProcessMessage()
{
	list<NetMessage*> alist;

    NetMessageManager* pNetMessageManager = 0;
    QMap<QString, NetMessageManager*>::iterator it, end;
	for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        NetMessageParser* pParser = pNetMessageManager->GetParser();

        alist = pParser->GetMessages(1);
        list<NetMessage*>::iterator it0, end0;
        for (it0 = alist.begin(), end0 = alist.end(); it0 != end0; ++it0)
        {
			switch ((*it0)->m_ucID)
			{
			case PACKET_REBOOT:
				system("shutdown /r /f /t 0");
				break;
			case PACKET_SHUTDOWN:
				system("shutdown /s /t 0");
				break;
			case PACKET_COMMAND_INFO:
				commandProcess((CommandInfoPacket*)(*it0));
				break;
			case PACKET_PROGRAM_INFO:
				{
					ProgramInfoPacket* pPacket = (ProgramInfoPacket*)(*it0);
					ProcessProgramInfo(pPacket);
				}
				break;
			/*case PACKET_SERVERSTATE:
				{
					m_iReceiveServerTime = m_Time.elapsed();
				}
				break;*/
			}
		}
		pParser->DeleteMessages();
	}

  //  if (! m_pNetMessageManager) return;

  //  NetMessageParser* pParser = m_pNetMessageManager->GetParser();

  //  list<NetMessage*> alist;
  //  alist = pParser->GetMessages(1);
  //  list<NetMessage*>::iterator it, end;
  //  for (it = alist.begin(), end = alist.end(); it != end; ++it)
  //  {
  //      switch ((*it)->m_ucID)
  //      {
  //          //case PACKET_CLIENT_INFO:
  //          //	{
  //          //		ClientInfoPacket* pPacket = (ClientInfoPacket*)(*it);
  //          //		if(pPacket->m_ucClientOn)
  //          //			addClient(pPacket, 0);
  //          //		else
  //          //			removeClient(pPacket);            
  //          //	}
  //          //	break;
  //      case PACKET_REBOOT:
  //          system("shutdown /r /f /t 0");
  //          break;
		//case PACKET_SHUTDOWN:
		//	system("shutdown /s /t 0");
		//	break;
  //      case PACKET_COMMAND_INFO:
  //          commandProcess((CommandInfoPacket*)(*it));
  //          break;
  //      case PACKET_PROGRAM_INFO:
  //          {
  //              ProgramInfoPacket* pPacket = (ProgramInfoPacket*)(*it);
  //              ProcessProgramInfo(pPacket);
  //          }
  //          break;
  //      case PACKET_SERVERSTATE:
  //          {
  //              m_iReceiveServerTime = m_Time.elapsed();
  //          }
  //          break;
  //      }
  //  }
  //  pParser->DeleteMessages();
}
//------------------------------------------------------------------------------------------------
void ClientRole::sendState()
{
    int curTime = m_Time.elapsed();
    if(curTime - m_iOldTime < 2000) return;

    m_iOldTime = curTime;
	char buff[1000]={0,};
	memset(buff, 0,sizeof(buff));

    ServerStatePacket msg;
    msg.m_ucState = 1;
    int iSize = msg.MakeDataBuff(buff);

    NetMessageManager* pNetMessageManager = 0;
    QMap<QString, NetMessageManager*>::iterator it, end;
    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        pNetMessageManager->Send(buff, iSize);
    }
}
//------------------------------------------------------------------------------------------------

//void ClientRole::checkServerState()
//{
//    if(m_bReseted) return;
//
//    int curTime = m_Time.elapsed();
//    if(curTime - m_iReceiveServerTime > 7000)
//    {
//        m_bReseted = true;
//        Release();
//        m_pTCPSocket->Close();
//        delete m_pTCPSocket;
//
//        QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
//
//        for(int i = 0, n = pModel->rowCount(); i < n; ++i)
//        {
//            QStandardItem* pItem = pModel->item(i);
//            if(!pItem) continue;
//            for(int k = 0, kn = pItem->rowCount(); k < kn; ++k)
//            {
//                delete pItem->child(i, 0);
//                delete pItem->child(i, 1);
//                delete pItem->child(i, 2);
//            }
//            delete pModel->item(i, 0);
//            delete pModel->item(i, 1);
//            delete pModel->item(i, 2);
//        }
//        pModel->clear();
//
//        m_pTCPSocket = new Socket;
//
//        InternetAddress aAddr;
//        aAddr.SetAddress(m_LocalIP.toAscii(),0);
//        aAddr.SetPortByPortNum(CLIENT_PORT);
//        m_pTCPSocket->Bind(aAddr);
//
//        emit connectionTimerRestart();
//    }
//}
//------------------------------------------------------------------------------------------------
//void ClientRole::ConnectionSendSlot()
//{
//	NetMessage aMsg(PACKET_CONNECTION);
//
//	char buff[100];
//	int iSize = aMsg.MakeDataBuff(buff);
//
//	m_pNetMessageManager->Send(buff, iSize);
//}
void ClientRole::loadProgramList()
{
    TiXmlDocument aDoc;    
    QString strAppType;

    if (! aDoc.LoadFile("programlist.config")) return;

    TiXmlElement* pRootElem = aDoc.RootElement();
    const char* pStr;

    TiXmlElement* pProgramElem = 0;

    const char* strPath;
    for (pProgramElem = pRootElem->FirstChildElement("program"); pProgramElem != NULL;
        pProgramElem = pProgramElem->NextSiblingElement("program"))
    {
        strPath = pProgramElem->Attribute("path");

        if (! strPath)
            continue;
        //m_strAddProgramPath.push_back(QString(strPath));
		//m_mapPrograms[m_ucProgramId] = strPath;
		AddProgramClient(QString(strPath));
    }
	
}
//------------------------------------------------------------------------------------------------
void ClientRole::saveProgramList()
{
    TiXmlDocument Doc;
    TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "", "");
    TiXmlElement* pProgramsElem = new TiXmlElement("programs");

    Doc.LinkEndChild(decl);
    Doc.LinkEndChild(pProgramsElem);

    QMap<unsigned char, QString>::iterator it, end;

    for (it = m_mapPrograms.begin(), end = m_mapPrograms.end(); it != end; ++it)
    {   
        TiXmlElement* pProgramElem = new TiXmlElement("program");
        pProgramElem->SetAttribute("path", it.value().toStdString().c_str());
        pProgramsElem->LinkEndChild(pProgramElem);
    }
    Doc.SaveFile("programlist.config");
}
//------------------------------------------------------------------------------------------------
void ClientRole::saveIpConfig(bool clicked)
{
	TiXmlDocument Doc;
	TiXmlDeclaration* decl = new TiXmlDeclaration("1.0", "utf-8", "");
	TiXmlElement* pConfigElem = new TiXmlElement("config");

	Doc.LinkEndChild(decl);
	//Doc.LinkEndChild(pConfigElem);
	m_ServerIP = m_pUI->edTargetIp->text();
	m_ServerPort = m_pUI->edTargetPort->text().toInt();
	char tmpPort[6] ="";
	sprintf_s(tmpPort,"%d",m_ServerPort);
	pConfigElem->SetAttribute("apptype","client");
	pConfigElem->SetAttribute("localIP",m_LocalIP.toStdString().c_str());
	pConfigElem->SetAttribute("targetIP",m_ServerIP.toStdString().c_str());
	pConfigElem->SetAttribute("targetPort",m_ServerPort);
	Doc.LinkEndChild(pConfigElem);

	Doc.SaveFile("init.config");
}
//--------------------------------------------------------------------------------------------------
void ClientRole::addButtonSlot(bool clicked)
{
    QString strProgram; 
    AddProgramDlg aDlg;
    if (QDialog::Accepted == aDlg.exec())
    {
        strProgram = aDlg.ui.ProgramLineEdit->text();

        AddProgramClient(strProgram);
        m_bModified = true;
		SendAddProgramPacket(strProgram, m_ucProgramId-1);
		saveProgramList();
    }
}
//--------------------------------------------------------------------------------------------------
void ClientRole::AddProgramClient(const QString& aPath)
{   
//    static unsigned char id = 0;
    m_mapPrograms[m_ucProgramId] = aPath;

    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    QList<QStandardItem*> list;
    QStandardItem* pCol1, *pCol2, *pCol3;

    QDir dir(aPath);
    QString aStr = dir.dirName();

    pCol1 = new QStandardItem(dir.dirName());
    pCol1->setData(QVariant::fromValue<unsigned char>(m_ucProgramId));

    pCol2 = new QStandardItem("OFF");
    pCol3 = new QStandardItem;
    pCol3->setCheckable(true);
    pCol3->setCheckState(Qt::Checked);

    list<<pCol1<<pCol2<<pCol3;
    pModel->appendRow(list);
    //SendAddProgramPacket(aPath, id);
	m_ucProgramId++;
}
//---------------------------------------------------------------------------------------------
void ClientRole::SendAddProgramPacketOne(const QString& path, unsigned char id, QString strHost)
{
    ProgramInfoPacket aPacket;
    //QString strHomeName = QHostInfo::localHostName();
    sprintf_s(aPacket.m_acIP , 16, "%s", m_LocalIP.toStdString().c_str());
    aPacket.m_Action    = ProgramInfoPacket::ADD;
    aPacket.m_ProgramId = id;
    sprintf_s(aPacket.m_acPath, 260, "%s", path.toLocal8Bit().data());    
    QString strLog = QString("%1 %2").arg(path).arg(QString::fromLocal8Bit("추가 패킷 전송"));
    sendPacketOne(&aPacket, strLog,strHost);
}
//---------------------------------------------------------------------------------------------
void ClientRole::SendAddProgramPacket(const QString& path, unsigned char id)
{
    ProgramInfoPacket aPacket;
    //QString strHomeName = QHostInfo::localHostName();
    sprintf_s(aPacket.m_acIP , 16, "%s", m_LocalIP.toStdString().c_str());
    aPacket.m_Action    = ProgramInfoPacket::ADD;
    aPacket.m_ProgramId = id;
    sprintf_s(aPacket.m_acPath, 260, "%s", path.toLocal8Bit().data());    
    QString strLog = QString("%1 %2").arg(path).arg(QString::fromLocal8Bit("추가 패킷 전송"));    
    sendPacket(&aPacket, strLog);
}
//------------------------------------------------------------------------------------------------
void ClientRole::ProcessProgramInfo(ProgramInfoPacket* pPacket)
{
    switch (pPacket->m_Action)
    {
    case ProgramInfoPacket::REMOVE:
        {
            QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
            if(!pProgramModel) return;

            for (int i = 0, n = pProgramModel->rowCount(); i < n; ++i)
            {
                QStandardItem* pCol = pProgramModel->item(i);
                QVariant aVar = pCol->data();
                unsigned char id = aVar.value<unsigned char>();

                if (id == pPacket->m_ProgramId)
                {
                    delete pProgramModel->item(i, 0);
                    delete pProgramModel->item(i, 1);
                    delete pProgramModel->item(i, 2);
                    pProgramModel->removeRow(i);
                    return;
                }
                if(m_mapPrograms.contains(pPacket->m_ProgramId))
                    m_mapPrograms.remove(pPacket->m_ProgramId);
            }
        }
        break;
    }
}
//------------------------------------------------------------------------------------------------
void ClientRole::commandProcess(CommandInfoPacket* pPacket)
{
    if(!pPacket) return;

    QProcess* pProcess = 0;
    if(pPacket->m_ucType == REQ_PACKET)
    {
        if(pPacket->m_ucCommand == PROGRAM_ON)
        {
            QMap<unsigned char, QString>::iterator it;
            it = m_mapPrograms.find(pPacket->m_ProgramId);

            if (it == m_mapPrograms.end())
                return;

            QMap<unsigned char, QProcess*>::iterator it0;
            it0 = m_mapProcesses.find(pPacket->m_ProgramId);
            if (it0 != m_mapProcesses.end())
                return;

            if(!isAllowedProgram(pPacket->m_ProgramId))
                return;

            pProcess = new QProcess;
            //pProcess->setReadChannel(QProcess::StandardOutput);
            //QObject::connect(pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(ProcessStdOutSlot()));

            QString strProgramPath = it.value();

            QStringList aStrList = strProgramPath.split(" ");
            QStringList aArgList;
            int size = 0;
            for (int i = aStrList.size()-1; i >= 0; --i)
            {
                if(aStrList[i].contains("."))
                    break;
                size += aStrList[i].size();
                aArgList.append(aStrList[i]);
            }

            QString temp = strProgramPath.mid(0, strProgramPath.size() - size);

            QFileInfo info(temp);
            //int idx = strProgram.lastIndexOf('\\', -1, Qt::CaseInsensitive);
            //if(idx == -1)
            //	idx = strProgram.lastIndexOf('/', -1, Qt::CaseInsensitive);
            //QString folder = strProgram.left(idx);

            bool ret = QDir::setCurrent(info.dir().path());
            //pProc->start(targetPath.mid(idx+1));
            //bool res = pProc->isOpen();


            pProcess->start(info.fileName(), aArgList);
            m_mapProcesses[pPacket->m_ProgramId] = pProcess;

            pProcess->waitForStarted();

            emit updateProgramStateSignal(pPacket->m_ProgramId, true, NULL,NULL);
        }
        else // PROGRAM_OFF
        {
            QMap<unsigned char, QProcess*>::iterator it0;
            it0 = m_mapProcesses.find(pPacket->m_ProgramId);
			if (it0 == m_mapProcesses.end())
			{
				QMap<unsigned char, QString>::iterator it2;
				it2 = m_mapPrograms.find(pPacket->m_ProgramId);

				if(it2!=m_mapPrograms.end())
				{
					QDir dir(it2.value());
					QString strName = dir.dirName();
					TerminateProcessor(strName.toStdString());
				}
                return;
			}

            QProcess* pProcess = it0.value();
            pProcess->kill();
            pProcess->waitForFinished();
            m_mapProcesses.remove(pPacket->m_ProgramId);
            //QObject::disconnect(pProcess, SIGNAL(readyReadStandardOutput()), this, SLOT(ProcessStdOutSlot()));

            delete pProcess;

            emit updateProgramStateSignal(pPacket->m_ProgramId, false, NULL,NULL);
        }
    }
    else
    {
    }
}
//------------------------------------------------------------------------------------------------
void ClientRole::OnConnection()
{
	//InternetAddress aTargetAddr;
	//aTargetAddr.SetAddress(m_ServerIP.toStdString().c_str(),0);
	//aTargetAddr.SetPortByPortNum(m_ServerPort);
	//if (m_pTCPSocket->Connect(aTargetAddr))
	//{
	//	emit connectionTimerFin();
	//	printf("Connected To Server..\n");

	//	m_pNetMessageManager = new NetMessageManager(NetMessageManager::TCP);

	//	m_pNetMessageManager->SetSocket(m_pTCPSocket);

	//	NetMessageParser* pParser = m_pNetMessageManager->GetParser();
	//	pParser->SetGetMessageSizeFunc(&GetMessageSize);
	//	pParser->SetCreateMessageFunc(&CreateMessage);
	//	pParser->SetDeleteMessageFunc(&DeleteMessage);

	//	m_pNetMessageManager->StartReceiving();

	//	ClientInfoPacket packet;
	//	QString strHomeName = QHostInfo::localHostName();
	//	sprintf_s(packet.m_acClientName, 24, "%s", strHomeName.isEmpty() ? "NoName" : strHomeName.toStdString().c_str());
	//	sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
	//	sprintf_s(packet.m_acClientType,7,"%s","client");
	//	packet.m_uiProgramCount = 0;
	//	packet.m_ucClientOn = CLIENT_CONNECTED;
	//	sendPacket(&packet, QString::fromLocal8Bit("클라이언트 연결 패킷 전송"));

	//	for (int i = 0, n = m_strAddProgramPath.size(); i < n; ++i)
	//	{
	//		AddProgramClient(m_strAddProgramPath.at(i));
	//	}
	//	m_bReseted = false;
	//	m_iReceiveServerTime = m_Time.elapsed();
	//}
	Socket* pSocket;
	InternetAddress aReceiveAddress;
	//aReceiveAddress.SetAddress(NULL, 0);
	if (m_pTCPSocket->Accept(&pSocket, &aReceiveAddress))
	{
		//m_Clients.push_back(pSocket);
		char buff[16] = {0, };
		//aReceiveAddress.GetHostName(buff, 100);
		aReceiveAddress.GetIP(buff,16);	// ip주소로 클라이언트 관리
		//char buff[100] = "user-pc";
		//sprintf(buff, "%s", "user_pc");
		printf("Client Connectd %s ..\n", buff);
		QString strHostName = buff;

		NetMessageManager* pNetMessageManager = new NetMessageManager(NetMessageManager::TCP);
		pNetMessageManager->SetName(buff);

		pNetMessageManager->SetSocket(pSocket);
		pNetMessageManager->SetName(strHostName.toLocal8Bit().data());
		NetMessageParser* pParser = pNetMessageManager->GetParser();
		pParser->SetGetMessageSizeFunc(&GetMessageSize);
		pParser->SetCreateMessageFunc(&CreateMessage);
		pParser->SetDeleteMessageFunc(&DeleteMessage);

		QMap<QString, NetMessageManager*>::iterator it = m_NetMessageManagerMap.find(strHostName);

		if (m_NetMessageManagerMap.end() != it)
		{
			delete it.value();
			m_NetMessageManagerMap.erase(it);
		}
		m_NetMessageManagerMap.insert(strHostName, pNetMessageManager);
		pNetMessageManager->StartReceiving();

		ClientInfoPacket packet;
		QString strHomeName = QHostInfo::localHostName();
		sprintf_s(packet.m_acClientName, 24, "%s", strHomeName.isEmpty() ? "NoName" : strHomeName.toStdString().c_str());
		sprintf_s(packet.m_acIP, 16, "%s", m_LocalIP.toStdString().c_str());
		sprintf_s(packet.m_acClientType,7,"%s","client");
		packet.m_uiProgramCount = 0;
		packet.m_ucClientOn = CLIENT_CONNECTED;
		sendPacketOne(&packet, QString::fromLocal8Bit("클라이언트 연결 패킷 전송"),strHostName);

		QMap<unsigned char,QString>::iterator programIt,programEnd;
		for (programIt = m_mapPrograms.begin(), programEnd = m_mapPrograms.end(); programIt != programEnd; ++programIt)
		{
			SendAddProgramPacketOne(programIt.value(),programIt.key(),strHostName);
		}
		//for (int i = 0, n = m_strAddProgramPath.size(); i < n; ++i)
		//{
		//	SendAddProgramPacket(m_strAddProgramPath.at(i),m_ucProgramId);
		//}
	}
}
//void ClientRole::ProcessStdOutSlot()
//{
//    QProcess* pProcess = (QProcess*)sender();
//    pProcess->readAllStandardOutput();
//}
////------------------------------------------------------------------------------------------------
//void ClientRole::sendPacket(NetMessage* pMsg, QString logMsg)
//{
//    if (! pMsg) return;
//
//	char buff[1000]={0,};
//    memset(buff, 0, sizeof(buff));
//    int iSize = pMsg->MakeDataBuff(buff);
//    m_pTCPSocket->Send(buff, iSize);
//
//    if(logMsg.isEmpty() || logMsg.isNull()) return;
//
//    m_pUI->listLog->insertItem(0, logMsg);
//}
//------------------------------------------------------------------------------------------------
void ClientRole::sendPacketOne(NetMessage* pMsg, QString logMsg, QString strHost)
{
	char buff[1000]={0,};
    memset(buff, 0, sizeof(buff));

    NetMessageManager* pNetMessageManager = 0;
	QMap<QString, NetMessageManager*>::iterator it = m_NetMessageManagerMap.find(strHost);
	pNetMessageManager = it.value();
	int iSize = pMsg->MakeDataBuff(buff);
	pNetMessageManager->Send(buff, iSize);
	if(logMsg.isEmpty() || logMsg.isNull()) return;
	m_pUI->listLog->insertItem(0, logMsg);
}
//------------------------------------------------------------------------------------------------
void ClientRole::sendPacket(NetMessage* pMsg, QString logMsg)
{
	char buff[1000]={0,};
    memset(buff, 0, sizeof(buff));

    NetMessageManager* pNetMessageManager = 0;

	QMap<QString, NetMessageManager*>::iterator it, end;
    for (it = m_NetMessageManagerMap.begin(), end = m_NetMessageManagerMap.end(); it != end; ++it)
    {
        pNetMessageManager = it.value();
        int iSize = pMsg->MakeDataBuff(buff);
        pNetMessageManager->Send(buff, iSize);
    }
	if(logMsg.isEmpty() || logMsg.isNull()) return;
	m_pUI->listLog->insertItem(0, logMsg);
}
//------------------------------------------------------------------------------------------------
void ClientRole::setServerIP(QString ip,QString port)
{
    m_ServerIP = ip;
	m_ServerPort = port.toInt();
}
//------------------------------------------------------------------------------------------------
void ClientRole::delButtonSlot(bool clicked)
{
    int row = m_pUI->treeConnectionList->currentIndex().row();
    if(row < 0) return;

    QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pProgramModel) return;

    QStandardItem* pCol1 = pProgramModel->item(row, 0);
    QStandardItem* pCol2 = pProgramModel->item(row, 1);

    QVariant aVar = pCol1->data();
    unsigned char id = aVar.value<unsigned char>();
    if(!m_mapPrograms.contains(id))
        return;

    QString removePath = m_mapPrograms[id];

    pProgramModel->removeRows(row, 1);
    m_mapPrograms.remove(id);

    ProgramInfoPacket aPacket;
    //QString strHomeName = QHostInfo::localHostName();
    sprintf_s(aPacket.m_acIP , 16, "%s", m_LocalIP.toStdString().c_str());
    aPacket.m_Action    = ProgramInfoPacket::REMOVE;
    aPacket.m_ProgramId = id;
    sprintf_s(aPacket.m_acPath, 260, "%s", removePath.toLocal8Bit().data());    
    QString strLog = QString("%1 %2").arg(removePath).arg(QString::fromLocal8Bit("삭제 패킷 전송"));    
    sendPacket(&aPacket, strLog);
    m_bModified = true;
	saveProgramList();
};
//------------------------------------------------------------------------------------------------
bool ClientRole::isAllowedProgram(unsigned char id)
{
    QStandardItemModel* pProgramModel = qobject_cast<QStandardItemModel*>(m_pUI->treeConnectionList->model());
    if(!pProgramModel) return false;

    for(int i = 0, n = pProgramModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pProgramModel->item(i);
        if(id == pItem->data().value<unsigned char>())
        {
            QStandardItem* pCheckItem = pProgramModel->item(i, 2);
            if(pCheckItem && pCheckItem->checkState() == Qt::Checked)
                return true;
            else
                return false;
        }
    }
    return false;
}
//////////////////////////////////////////////////////////////////////////
// 클라이언트 끝.
/*------------------------------------------------------------------------*/