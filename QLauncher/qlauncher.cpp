#include "qlauncher.h"

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
#include "LauncherRole.h"
//------------------------------------------------------------------------------------------------
QLauncher::QLauncher(QWidget *parent, Qt::WFlags flags)
: QDialog(parent, flags), m_pCurrentRole(0)
{
    ui.setupUi(this);
    ui.treeProgramList->hide();
    setWindowFlags(Qt::WindowMinimizeButtonHint);

    loadConfig();
}
//------------------------------------------------------------------------------------------------
QLauncher::~QLauncher()
{
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(ui.treeConnectionList->model());

    for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pModel->item(i);
        if(!pItem) continue;
        for(int k = 0, kn = pItem->rowCount(); k < kn; ++k)
        {
            delete pItem->child(i, 0);
            delete pItem->child(i, 1);
            delete pItem->child(i, 2);
        }
        delete pModel->item(i, 0);
        delete pModel->item(i, 1);
        delete pModel->item(i, 2);
    }
    pModel->clear();
    delete pModel;

    m_UpdateTimer.stop();
    m_ConnectionTimer.stop();
    if(m_pCurrentRole)
	{
        m_pCurrentRole->Release();
		
	}
}

//------------------------------------------------------------------------------------------------
bool QLauncher::loadConfig()
{ 
    TiXmlDocument aDoc;    
    QString strAppType;

    if (! aDoc.LoadFile("init.config")) return false;

    TiXmlElement* pRootElem = aDoc.RootElement();
    const char* pStr;

    pStr = pRootElem->Attribute("apptype");
    if (pStr)
        strAppType = pStr;
    else
        return false;

	pStr = pRootElem->Attribute("localIP");
	if(pStr)
		m_LocalIP = pStr;
	else
		return false;

//    pStr = pRootElem->Attribute("targetIP");
//    if (pStr)
//        m_ServerIP = pStr; 
//    else
//        return false;

//	pStr = pRootElem->Attribute("targetPort");
//	if(pStr)
//		m_ServerPort = pStr;
//	else
//		return false;

	if(strAppType.compare("server", Qt::CaseInsensitive) == 0)
	{
		m_pCurrentRole = &ServerRole::GetInstance();
		((ServerRole*)m_pCurrentRole)->setLocalIP(m_LocalIP);
		//((ServerRole*)m_pCurrentRole)->setServerIP(m_ServerIP,m_ServerPort);
		//((ServerRole*)m_pCurrentRole)->addServerIP();
		setWindowTitle("QLauncher : SERVER");
	}
	else if(strAppType.compare("client", Qt::CaseInsensitive) == 0)
	{
		m_pCurrentRole = &ClientRole::GetInstance();
		((ClientRole*)m_pCurrentRole)->setLocalIP(m_LocalIP);
		//((ClientRole*)m_pCurrentRole)->setServerIP(m_ServerIP,m_ServerPort);
		setWindowTitle("QLauncher : CLIENT");
		this->resize(500,300);
	}
	else
		return false;
	m_pCurrentRole->setUI(&ui);
	initUI();
	m_pCurrentRole->initComm();
	return true;
}
//------------------------------------------------------------------------------------------------
void QLauncher::initUI()
{
    if(!m_pCurrentRole) return;

    QObject::connect(&m_ConnectionTimer, SIGNAL(timeout()), m_pCurrentRole, SLOT(OnConnection()));
    QObject::connect(&m_UpdateTimer, SIGNAL(timeout()), this, SLOT(updateSlot()));
    m_ConnectionTimer.start(50);

    //connect(ui.btnDelete, SIGNAL(clicked(bool)), this, SLOT(delButtonSlot(bool)));

    QStandardItemModel* pModel = new QStandardItemModel;

    pModel->setColumnCount(4);
    pModel->setHeaderData(0,  Qt::Horizontal, QVariant("IP"), Qt::DisplayRole);
    pModel->setHeaderData(1,  Qt::Horizontal, QVariant(QString::fromLocal8Bit("이름")), Qt::DisplayRole);
    pModel->setHeaderData(2,  Qt::Horizontal, QVariant(true), Qt::CheckStateRole);
	pModel->setHeaderData(3,  Qt::Horizontal, QVariant("type"),Qt::DisplayRole);

    ui.treeConnectionList->setModel(pModel);
    ui.treeConnectionList->setRootIsDecorated(true);

    //pModel = new QStandardItemModel;
    //pModel->setColumnCount(3);
    //pModel->setHeaderData(0,  Qt::Horizontal, QVariant(QString::fromLocal8Bit("이름")), Qt::DisplayRole);
    //pModel->setHeaderData(1,  Qt::Horizontal, QVariant(QString::fromLocal8Bit("상태")), Qt::DisplayRole);
    //pModel->setHeaderData(2,  Qt::Horizontal, QVariant(QString::fromLocal8Bit("")), Qt::DisplayRole);

    //ui.treeProgramList->setModel(pModel);
    //ui.treeProgramList->setRootIsDecorated(true);
    m_UpdateTimer.start(50);

	ui.edTargetIp->setText(m_ServerIP);
	ui.edTargetPort->setText(m_ServerPort);

    m_pCurrentRole->initUI();
    connect(m_pCurrentRole, SIGNAL(updateProgramStateSignal(unsigned char, unsigned char, LauncherClient*,QString)), this, SLOT(updateProgramStateSlot(unsigned char, unsigned char, LauncherClient*,QString)));
    connect(m_pCurrentRole, SIGNAL(connectionTimerFin()), this, SLOT(connectionTimerFinSlot()));
    connect(m_pCurrentRole, SIGNAL(connectionTimerRestart()), this, SLOT(connectionTimerReStartSlot()));
    connect(m_pCurrentRole, SIGNAL(writeLogSignal(QString)), this, SLOT(writeLog(QString)));
}
//---------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------
void QLauncher::writeLog(QString msg)
{
    if(msg.isNull() || msg.isEmpty()) return;

    if(ui.listLog->count() > 100)
        ui.listLog->reset();
    ui.listLog->insertItem(0, msg);
}
//------------------------------------------------------------------------------------------------
void QLauncher::updateProgramStateSlot(unsigned char id, unsigned char ucState, LauncherClient* pClient,QString strGroupName)
{
    //QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(ui.treeConnectionList->model());
    //if(!pModel) return;
	// 그룹을 찾는다
	//QStandardItem* pGroupItem = FindGroupItem(strGroupName);
	QStandardItem* pGroupItem =0;
	QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(ui.treeConnectionList->model());
	if(!pModel) return;

	for(int i = 0, n = pModel->rowCount(); i < n; ++i)
	{
		pGroupItem = pModel->item(i);
		if(pGroupItem)
		{	// group 아이테을 찾는다
			if(strGroupName== pGroupItem->data().value<QString>())
				break;
		}

		// group 일경우 이름 ON, OFF 로 안바뀌도록
		if(pModel->item(i,1)->text().compare("Group",Qt::CaseInsensitive) ==0)
			continue;

		// Client 일 경우 프로그램 아이템의 ON,OFF 여부를 변경
		QStandardItem* pItem = pModel->item(i);
		QVariant aVar = pItem->data();
		if (id == aVar.value<unsigned char>())
		{
			QStandardItem* pFindItem = pModel->item(i, 1);
			if(pFindItem)
				pFindItem->setText(ucState == 0 ? "OFF" : "ON");
			return;
		}
	}

	for (int i = 0, n = pGroupItem->rowCount(); i < n; i++)
	{
		// server 일 경우 client의 프로그램 리스트의 ON,OFF 여부를 변경
		QStandardItem* pClientItem = pGroupItem->child(i);
		if(!pClientItem) continue;
		if(pClient != pClientItem->data().value<LauncherClient*>())
			continue;

		for(int k = 0, kn = pClientItem->rowCount(); k < kn; ++k)
		{
			QStandardItem* pProgItem = pClientItem->child(k);
			if(!pProgItem) continue;
			unsigned char ucid = pProgItem->data().value<unsigned char>();
			if (id == ucid)
			{
				QStandardItem* pFindItem = pClientItem->child(k, 1);
				if(pFindItem)
					pFindItem->setText(ucState == 0 ? "OFF" : "ON");
				return;
			}
		}		
	}
//////
    //for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    //{
    //    QStandardItem* pItem = pModel->item(i);
    //    if(!pItem) continue;

    //    if(pClient != pItem->data().value<LauncherClient*>())
    //        continue;

    //    for(int k = 0, kn = pItem->rowCount(); k < kn; ++k)
    //    {
    //        QStandardItem* pChild = pItem->child(k);
    //        if(!pChild) continue;
    //        unsigned char ucid = pChild->data().value<unsigned char>();
    //        if (id == ucid)
    //        {
    //            QStandardItem* pFindItem = pItem->child(k, 1);
    //            if(pFindItem)
    //                pFindItem->setText(ucState == 0 ? "OFF" : "ON");
    //            return;
    //        }
    //    }

		//QVariant aVar = pItem->data();
  //      if (id == aVar.value<unsigned char>())
  //      {
  //          QStandardItem* pFindItem = pModel->item(i, 1);
  //          if(pFindItem)
  //              pFindItem->setText(ucState == 0 ? "OFF" : "ON");
  //          return;
  //      }
	 // }

    //ui.treeConnectionList->setCurrentIndex(ui.treeConnectionList->currentIndex());
}
//------------------------------------------------------------------------------------------------
void QLauncher::updateSlot()
{
    if(m_pCurrentRole)
        m_pCurrentRole->Update();
}
void QLauncher::connectionTimerFinSlot()
{
    m_ConnectionTimer.stop();
}
void QLauncher::connectionTimerReStartSlot()
{
    if(!m_ConnectionTimer.isActive())
        m_ConnectionTimer.start();
}