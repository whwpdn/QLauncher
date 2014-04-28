#include "QMsgUpdateThread.h"
#include "LauncherClient.h"

#include "QLauncher.h"
#include <QtGui>
#include <QtCore>
#include "networkinterface.h"
QMsgUpdateThread::QMsgUpdateThread(QObject *parent)
        : QThread(parent)
{
    stopped = false;
}

QMsgUpdateThread::~QMsgUpdateThread()
{
    stop();
    
    writeClientInfo();

    QMap<QString, LauncherClient*>::Iterator iter;
    for(iter = m_mapClient.begin(); iter != m_mapClient.end(); iter++)
    {
        LauncherClient* pClient = iter.value();
        if(pClient)
            delete pClient;
        pClient = 0;
    }
    m_mapClient.clear();
    
    QMap<QString, QProcess*>::iterator iterProc;
    for(iterProc = m_mapProcess.begin(); iterProc != m_mapProcess.end(); ++iterProc)
    {
        QProcess* pProc = iterProc.value();
        if(pProc)
        {
            pProc->disconnect();
            if(pProc->isOpen())
                pProc->kill();
	    else
		delete pProc;
            m_mapProcess[iterProc.key()] = pProc = 0;
        }
    }
    m_mapProcess.clear();
}

void QMsgUpdateThread::init()
{
    loadClientInfo();
}

void QMsgUpdateThread::run()
{
    while(!stopped)
    {
        if(TheNetChannel)
        {
            TheNetChannel->OnReceiveRCMMessage();
            analizePacket();
        }
    }
}

void QMsgUpdateThread::stop()
{
    stopped = true;
}

void QMsgUpdateThread::writeLog(QString msg)
{
    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
    if(!pLauncher) return;

    if(msg.isNull() || msg.isEmpty()) return;

    if(pLauncher->ui.listLog->count() > 100)
        pLauncher->ui.listLog->reset();
    pLauncher->ui.listLog->insertItem(0, msg);
}
void QMsgUpdateThread::analizePacket()
{
    if(!TheNetChannel)
        return;
    NetMessage* pMsg = TheNetChannel->GetNetMessage();
    
    if(!pMsg)
        return;

    switch(pMsg->m_ucID)
    {
    case PACKET_CLIENT_INFO:
        {
            ClientInfoPacket* pPacket = (ClientInfoPacket*)pMsg;
            if(pPacket->m_ucClientOn)
                addClient(pPacket);
            else
                removeClient(pPacket);            
        }
        break;
    case PACKET_COMMAND_INFO:
        commandProcess((CommandInfoPacket*)pMsg);
        break;
 //   case PACKET_MACADDRESS:
	//macProcess((MacAddressPacket*)pMsg);
	//break;
    }
    if(TheNetChannel)
        TheNetChannel->DeleteMsg(pMsg);
}

void QMsgUpdateThread::addClient(QString ip, QString mac, QString name)
{
    LauncherClient* pClient = findClient(ip);
    if(pClient) return;

    pClient = new LauncherClient;
    pClient->setClientIP(ip);
    pClient->setClientName(name);
    pClient->setMacAddress(mac);
    m_mapClient[ip] = pClient;

    //gui에 추가
    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());

    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(pLauncher->ui.treeConnectionList->model());
    if(!pModel) return;

    QStandardItem* pCol1, *pCol2;
    QList<QStandardItem*> lstItem;

    pCol1 = new QStandardItem(pClient->getClientIP());
    pCol2 = new QStandardItem(pClient->getClientName());

    lstItem<<pCol1<<pCol2;
    pModel->insertRow(pModel->rowCount(), lstItem);

    writeLog(QString("[%1] %2").arg(ip).arg(QString::fromLocal8Bit("파일로부터 클라이언트 정보 추가")));

    storeMacAddressFromIP(ip);
}

void QMsgUpdateThread::addConnectionItem(QString ip, QString name)
{
    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
    if(!pLauncher) return;
    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(pLauncher->ui.treeConnectionList->model());
    if(!pModel) return;

    QStandardItem* pCol1, *pCol2;
    QList<QStandardItem*> lstItem;

    pCol1 = new QStandardItem(ip);
    pCol2 = new QStandardItem(name);
    lstItem<<pCol1<<pCol2;
    pModel->insertRow(pModel->rowCount(), lstItem);
}

void QMsgUpdateThread::addClient(ClientInfoPacket* pPacket)
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
            id++;
            pClient->SetID(id);
            pClient->setClientIP(pPacket->m_acIP);
            pClient->setClientName(pPacket->m_acClientName);

	    addConnectionItem(pClient->getClientIP(), pClient->getClientName());

            msgLog = QString("[%1] %2").arg(pPacket->m_acIP).arg(QString::fromLocal8Bit("추가 패킷 수신"));
            writeLog(msgLog);
        }

        for(int i = 0, n = pPacket->m_vecProgramInfo.size(); i < n; ++i)
        {
            ProgramInfo* pInfo = pPacket->m_vecProgramInfo[i];
            if(pInfo)
            {
                pClient->addProgramInfo(pInfo->m_ucProgramID, pInfo->m_TurnOn, QString::fromLocal8Bit(pInfo->m_acName), QString::fromLocal8Bit(pInfo->m_acPath));

                msgLog = QString("[%1] %2 %3").arg(pPacket->m_acIP).arg(pInfo->m_acName).arg(QString::fromLocal8Bit("추가"));
                writeLog(msgLog);
            }
        }
        if(TheNetChannel)
        {
            TheNetChannel->SetServerAddr(pPacket->m_acIP, TCPPORT, UDPPORT);
	    QString tIP = pPacket->m_acIP;

            sprintf_s(pPacket->m_acIP, 16, NetworkInterface::getLocalIP().toStdString().c_str());
            pPacket->m_ucType = ACK_PACKET;
            pPacket->m_ucClientOn = CLIENT_CONNECTED;
            TheNetChannel->SendNetMessage(pPacket);

            msgLog = QString("[%1] %2 ").arg(pPacket->m_acIP).arg(QString::fromLocal8Bit("응답 패킷 전송"));
            writeLog(msgLog);

	    storeMacAddressFromIP(tIP);
        }
    }
    else
    {
        QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
        if(pLauncher)
        {
            QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(pLauncher->ui.treeProgramList->model());
            QList<QStandardItem*> list;
            QStandardItem* pCol1, *pCol2;
            for(int i = 0, n = pPacket->m_vecProgramInfo.size(); i < n; ++i)
            {
                ProgramInfo* pProInfo = pPacket->m_vecProgramInfo[i];
                
                pCol1 = new QStandardItem(pProInfo->m_acName);
                pCol1->setData(QVariant::fromValue<QString>(pProInfo->m_acPath));
                pCol2 = new QStandardItem(pProInfo->m_TurnOn == 0 ? "OFF" : "ON");
                pCol2->setData(QVariant::fromValue<unsigned char>(pProInfo->m_ucProgramID));
                list<<pCol1<<pCol2;
                pModel->insertRow(pModel->rowCount(), list);
            }
            msgLog = QString("%1").arg(QString::fromLocal8Bit("클라이언트 추가 응답 패킷 수신"));
            writeLog(msgLog);
        }
    }
}

void QMsgUpdateThread::removeClient(ClientInfoPacket* pPacket)
{
    if(!pPacket) return;

    QString strIP = pPacket->m_acIP;
    //LauncherClient* pClient = findClient(strIP);
    //
    //if(pClient)
    //    m_mapClient.remove(strIP);
    
    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
    if(!pLauncher) return;

    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(pLauncher->ui.treeConnectionList->model());
    if(!pModel) return;

    for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pModel->item(i);
        if(!pItem) continue;

        if(strIP.compare(pItem->text(), Qt::CaseInsensitive) == 0)
        {
            //pModel->removeRow(i);
            pLauncher->ui.treeProgramList->reset();
            break;
        }
    }
}

//void QMsgUpdateThread::macProcess(MacAddressPacket* pPacket)
//{
//    if(!pPacket) return;
//
//    if(pPacket->m_ucType == REQ_PACKET)
//    {
//	writeLog(QString::fromLocal8Bit("mac address 요청 패킷 수신"));
//	sendMacAddress(pPacket->m_acIP);
//    }
//    else
//    {
//	LauncherClient* pClient = findClient(pPacket->m_acIP);
//	if(pClient)
//	{
//	    writeLog(QString::fromLocal8Bit("mac address 수신"));
//	    pClient->setMacAddress(pPacket->m_address);
//	}
//    }
//}

void QMsgUpdateThread::commandProcess(CommandInfoPacket* pPacket)
{
    if(!pPacket) return;

    QString msgLog = "";
    QProcess* pProc = new QProcess(this);
    connect(pProc, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(processFinishSlot(int, QProcess::ExitStatus)));
    connect(pProc, SIGNAL(error(QProcess::ProcessError)), this, SLOT(processErrorSlot(QProcess::ProcessError)));
    if(pPacket->m_ucType == REQ_PACKET)
    {
        if(pPacket->m_ucCommand == PROGRAM_ON)
        {
            QString targetPath = QString::fromLocal8Bit(pPacket->m_acPath);
            QProcess* pOld = m_mapProcess[targetPath];
            if(pOld)
            {
                pProc->disconnect();
                delete pProc;
                pProc = 0;
                pProc = pOld;
            }

            msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("Program Start 수신")).arg(targetPath);
            writeLog(msgLog);
            if(TheNetChannel)
            {
                TheNetChannel->SetServerAddr(pPacket->m_acIP, TCPPORT, UDPPORT);
                pPacket->m_ucType = ACK_PACKET;
                TheNetChannel->SendNetMessage(pPacket);
                msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("start 응답 패킷 송신")).arg(targetPath);
                writeLog(msgLog);
            }
            
            int idx = targetPath.lastIndexOf('\\', -1, Qt::CaseInsensitive);
            if(idx == -1)
                idx = targetPath.lastIndexOf('/', -1, Qt::CaseInsensitive);
            QString folder = targetPath.left(idx);

	    QString curDir = QDir::currentPath();
            bool ret = QDir::setCurrent(folder);
            pProc->start(targetPath.mid(idx+1));
            bool res = pProc->isOpen();
            updateProgramState(targetPath, (unsigned char)res);

	    QDir::setCurrent(curDir);
            if(res)
                m_mapProcess[targetPath] = pProc;
            else
            {
                pProc->disconnect();
                delete pProc;
                pProc = 0;
                m_mapProcess.remove(targetPath);
            }

            if(TheNetChannel)
            {
                TheNetChannel->SetServerAddr(pPacket->m_acIP, TCPPORT, UDPPORT);
                pPacket->m_ucType = ACK_PACKET;
                pPacket->m_ucCommand = res;
                sprintf_s(pPacket->m_acIP, 16, "%s", NetworkInterface::getLocalIP().toStdString().c_str());
                TheNetChannel->SendNetMessage(pPacket);
                msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("start 응답 패킷 송신")).arg(targetPath);
                writeLog(msgLog);
            }
        }
        else // off
        {
            QString targetPath = QString::fromLocal8Bit(pPacket->m_acPath);
            QProcess* pOld = m_mapProcess[targetPath];
            if(pOld)
            {
                pOld->kill();
                m_mapProcess.remove(targetPath);
                delete pOld;
                pOld = 0;
            }
            msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("Program Close 수신")).arg(targetPath);
            writeLog(msgLog);
            if(TheNetChannel)
            {
                pPacket->m_ucType = ACK_PACKET;
                pPacket->m_ucCommand = 0;
                sprintf_s(pPacket->m_acIP, 16, "%s", NetworkInterface::getLocalIP().toStdString().c_str());
                TheNetChannel->SendNetMessage(pPacket);
                msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("close 응답 패킷 송신")).arg(QString::fromLocal8Bit(pPacket->m_acPath));
                writeLog(msgLog);
            }
            updateProgramState(targetPath, pPacket->m_ucCommand);
        }
    }
    else
    {
        LauncherClient* pClient = findClient(pPacket->m_acIP);
        if(pClient)
        {
            msgLog = QString("%1 %2").arg(QString::fromLocal8Bit("command 응답 패킷 수신")).arg(QString::fromLocal8Bit(pPacket->m_acPath));
            writeLog(msgLog);
            QList<LauncherClient::LauncherProgramInfo*>& list = pClient->getProgramList();

            for(int i = 0, n = list.size(); i < n; ++i)
            {
                QString path = QString::fromLocal8Bit(pPacket->m_acPath);
                if(list[i]->m_strPath.compare(path, Qt::CaseInsensitive) == 0)
                {
                    list[i]->m_ucState = pPacket->m_ucCommand;
                    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
                    if(pLauncher)
                        pLauncher->updateProgramList(pPacket->m_acIP);
                    break;
                }
            }
        }
    }
}

void QMsgUpdateThread::updateProgramState(QString strPath, unsigned char ucState)
{
    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
    if(!pLauncher) return;

    QStandardItemModel* pModel = qobject_cast<QStandardItemModel*>(pLauncher->ui.treeProgramList->model());
    if(!pModel) return;

    for(int i = 0, n = pModel->rowCount(); i < n; ++i)
    {
        QStandardItem* pItem = pModel->item(i);
        if(!pItem) continue;

        if(strPath.compare(pItem->data().toString(), Qt::CaseInsensitive) == 0)
        {
            pItem = pModel->item(i, 1);
            if(pItem)
                pItem->setText(ucState == 0 ? "OFF" : "ON");
        }
    }

    pLauncher->ui.treeConnectionList->setCurrentIndex(pLauncher->ui.treeConnectionList->currentIndex());
}

LauncherClient* QMsgUpdateThread::findClient(QString ip)
{
    if(!m_mapClient.contains(ip))
	return NULL;
    return m_mapClient[ip];
}

void QMsgUpdateThread::processFinishSlot(int exitCode, QProcess::ExitStatus exitStatus)
{
    QProcess* pProc = qobject_cast<QProcess*>(sender());
    if(!pProc) return;

    QMap<QString, QProcess*>::iterator iter;
    for(iter = m_mapProcess.begin(); iter != m_mapProcess.end(); ++iter)
    {
        if(iter.value() == pProc)
        {
            pProc->disconnect();
            delete pProc;
            pProc = 0;
            m_mapProcess.remove(iter.key());
            break;
        }
    }
}

void QMsgUpdateThread::processErrorSlot(QProcess::ProcessError error)
{
    QString err = "";
    switch(error)
    {
    case QProcess::Crashed: err = "Crashed"; break;
    case QProcess::FailedToStart: err = "Failed To Start"; break;
    case QProcess::ReadError: err = "Read Error"; break;
    case QProcess::Timedout: err = "Time Out"; break;
    case QProcess::UnknownError: err = "Unknown Error"; break;
    case QProcess::WriteError: err = "Write Error"; break;
    }

    QProcess* pProc = qobject_cast<QProcess*>(sender());

    QMap<QString, QProcess*>::iterator iter;
    if(!pProc) return;
    QString logStr = "";
    for(iter = m_mapProcess.begin(); iter != m_mapProcess.end(); ++iter)
    {
        if(iter.value() == pProc)
        {
            logStr.sprintf("[%s] %s", err.toStdString().c_str(),iter.key().toStdString().c_str());
            break;
        }
    }

    QLauncher* pLauncher = qobject_cast<QLauncher*>(parent());
    if(!pLauncher) return;

    pLauncher->ui.listLog->insertItem(0, logStr);
    pLauncher->ui.listLog->item(0)->setTextColor(QColor(Qt::red));
}

void QMsgUpdateThread::sendMagicP(QString ip)
{
    if(ip.isEmpty() || ip.isNull()) return;
    if(!TheNetChannel) return;

    LauncherClient* pClient = findClient(ip);

    if(!pClient) return;

    unsigned char* magicp = new unsigned char[DATA_SIZE_MAGICP];
    int size = createMagicP(pClient->getMacAddress(), magicp);
    TheNetChannel->SetServerAddr("255.255.255.255", TCPPORT, MAGICPPORT);
    TheNetChannel->Send((char*)magicp, size);
    writeLog(QString::fromLocal8Bit("%1 매직패킷 전송").arg(ip));
    //소켓수정
}

int QMsgUpdateThread::createMagicP(QString macAddr, unsigned char* magicp)
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

unsigned int QMsgUpdateThread::HexStrToInt(QString hexStr)
{
    char *stop;
    char num[3];
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

void QMsgUpdateThread::loadClientInfo()
{
    QFile file("WOL.dat");
    if(!file.open(QIODevice::ReadOnly)) return;

    QTextStream TextStream(&file);
    while(!TextStream.atEnd())
    {
	QStringList strFields = TextStream.readLine().split(':');

        addClient(strFields[0], strFields[2], strFields[1]);
    }

    file.close();
}
void QMsgUpdateThread::writeClientInfo()
{
    QFile file("WOL.dat");
    if(!file.open(QIODevice::WriteOnly)) return;

    QTextStream TextStream(&file);
    TextStream.seek(0);

    QMap<QString, LauncherClient*>::Iterator iter;
    for(iter = m_mapClient.begin(); iter != m_mapClient.end(); iter++)
    {
        LauncherClient* pClient = iter.value();
	TextStream<<pClient->getClientIP()<<":"<<pClient->getClientName()<<":"<<pClient->getMacAddress()<<"\n";
    }
    m_mapClient.clear();
    file.close();
}

void QMsgUpdateThread::storeMacAddressFromIP(QString reqIP)
{
    if(!TheNetChannel) return;

    //TheNetChannel->SetServerAddr(reqIP.toStdString().c_str(), TCPPORT, UDPPORT);
    //printf("%s %d %d send\n", reqIP.toStdString().c_str(), TCPPORT, UDPPORT);

    //MacAddressPacket macp;
    //macp.m_ucType = REQ_PACKET;
    //sprintf_s(macp.m_acIP, 16, "%s", NetworkInterface::getLocalIP().toStdString().c_str());
    //TheNetChannel->SendNetMessage(&macp);
    //writeLog(QString("[%1] %2 ").arg(reqIP).arg(QString::fromLocal8Bit("Mac주소 요청 패킷 전송")));

    char addr[256] = {0,};
    GetMacAddress(reqIP.toStdString().c_str(), addr);
    LauncherClient* pClient = findClient(reqIP);
    if(pClient)
    {
	if(strcmp(addr, "000000000000") == 0) return;
	QString str = QString::fromLocal8Bit("[%1] %2 MacAddress 확인").arg(reqIP).arg(addr);
	writeLog(str);
	pClient->setMacAddress(addr);
    }
}

//void QMsgUpdateThread::sendMacAddress(QString serverIP)
//{
//    if(!TheNetChannel) return;
//
//    TheNetChannel->SetServerAddr(serverIP.toStdString().c_str(), TCPPORT, UDPPORT);
//
//    MacAddressPacket macp;
//    macp.m_ucType = ACK_PACKET;
//    sprintf_s(macp.m_acIP, 16, "%s", NetworkInterface::getLocalIP().toStdString().c_str());
//    sprintf_s(macp.m_address, DATA_SIZE_MACADDRESS_INFO, NetworkInterface::getLocalMacAddress().toStdString().c_str());
//    TheNetChannel->SendNetMessage(&macp);
//    writeLog(QString("[%1] %2 ").arg(serverIP).arg(QString::fromLocal8Bit("Mac주소 응답 패킷 전송")));
//}