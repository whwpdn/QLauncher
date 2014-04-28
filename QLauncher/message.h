#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#pragma pack(push, 1)

#define HEADER_SIZE         18
#define CLIENTINFO_SIZE     426
#define SUBCLIENTINFO_SIZE	435
#define PROGRAMINFO_SIZE    262
#define SUBPROGRAMINFO_SIZE	278
#define COMMANDINFO_SIZE    2
#define SERVERSTATE_SIZE    1
#define CHECKBOXUPDATE_SIZE	18

#define PACKET_CLIENT_INFO		1
#define PACKET_PROGRAM_INFO		2
#define PACKET_COMMAND_INFO		3
#define PACKET_CONNECTION		4
#define PACKET_REBOOT			5
#define PACKET_SERVERSTATE      6
#define PACKET_SUBCLIENT_INFO	7
#define PACKET_SUBPROGRAM_INFO	8
#define PACKET_SHUTDOWN			9
#define PACKET_CHECKBOX_INFO	10

#define REQ_PACKET              0
#define ACK_PACKET              1
#define HEADER_SIZE             18

#define DATA_SIZE_CLIENT_INFO   11
#define DATA_SIZE_PROGRAM_INFO  390
#define DATA_SIZE_MAGICP        102

#include <memory.h>
#include <vector>
#include <windows.h>

using std::vector;

//-------------------------------------------------------------------------------------------------
struct ProgramInfo
{
    unsigned char m_ucProgramID;
    unsigned char m_TurnOn;
    char m_acName[128];
    char m_acPath[260];    
    ProgramInfo()
    {
        m_ucProgramID = 0;
        m_TurnOn = 0;
        memset(m_acName, 0, sizeof(char)*128);
        memset(m_acPath, 0, sizeof(char)*260);;
    }
};
//-------------------------------------------------------------------------------------------------
class NetMessage 
{
public:
    NetMessage()
    {
        m_ucID      = 0;
        m_ucType    = 0;
        memset(m_acIP, 0, 16);
    }

    NetMessage(unsigned char id) : m_ucID(id)
    {
        m_ucType = 0;
        memset(m_acIP, 0, 16);
    }
    virtual ~NetMessage() {}

    virtual void FillData(char* buff, unsigned int size)
    {
        if(size < 0) return;

        int checkSize = size > HEADER_SIZE ? HEADER_SIZE : size;
        memcpy(m_Data, buff, checkSize);
        //return HEADER_SIZE;
    }

    virtual int MakeDataBuff(char* buff)
    {
        memcpy(buff, m_Data, HEADER_SIZE);

        return HEADER_SIZE;
    }

public:
    union
    {
        char                m_Data[HEADER_SIZE];
        struct
        {
            unsigned char       m_ucID;
            unsigned char       m_ucType;
            char                m_acIP[16];
        };
    };
};
//-------------------------------------------------------------------------------------------------
class ClientInfoPacket : public NetMessage
{
public:
    ClientInfoPacket() : NetMessage(PACKET_CLIENT_INFO), m_uiProgramCount(0), m_ucClientOn(0)
    {
        memset(m_Data, 0, CLIENTINFO_SIZE);
    }
    ~ClientInfoPacket()
    {
        Reset();
    }

public:
    void FillData(char* buff, unsigned int size)
    {
        Reset();
        NetMessage::FillData(buff, size);
        char* pBuff = buff + HEADER_SIZE;
        memcpy(m_Data, pBuff, CLIENTINFO_SIZE);
        //pBuff += CLIENTINFO_SIZE;

        //ProgramInfo* pInfo = NULL;
        //for(int i = 0; i < m_uiProgramCount; ++i)
        //{
        //    pInfo = new ProgramInfo;
        //    memcpy(pInfo, pBuff, DATA_SIZE_PROGRAM_INFO);
        //    pBuff+=DATA_SIZE_PROGRAM_INFO;
        //    m_vecProgramInfo.push_back(pInfo);
        //}
    }

    int MakeDataBuff(char buff[])
    {
        int iSize = NetMessage::MakeDataBuff(buff);
        memcpy(buff+iSize, m_Data, CLIENTINFO_SIZE);
        iSize += CLIENTINFO_SIZE;
        //for(int i = 0, n = m_vecProgramInfo.size(); i < n; ++i)
        //{
        //    memcpy(buff+iSize, m_vecProgramInfo[i], DATA_SIZE_PROGRAM_INFO);
        //    iSize += DATA_SIZE_PROGRAM_INFO;
        //}
        return iSize;
    }
    void Reset()
    {
        memset(m_Data, 0, CLIENTINFO_SIZE);
        //for(int i = 0, n = m_vecProgramInfo.size(); i < n; ++i)
        //{
        //    if(m_vecProgramInfo[i])
        //        delete m_vecProgramInfo[i];
        //    m_vecProgramInfo[i] = 0;
        //}
        //m_vecProgramInfo.clear();
    }
public:
    union
    {
        char m_Data[CLIENTINFO_SIZE];
        struct
        {
            unsigned char   m_ucClientOn;//426
            unsigned int    m_uiProgramCount;//425
            char            m_acClientName[24];//421
			char			m_acClientType[7]; //397
            ProgramInfo     m_ProgramInfo;//390

        };
    };
    //vector<ProgramInfo*> m_vecProgramInfo;
};
//-------------------------------------------------------------------------------------------------
class ProgramInfoPacket : public NetMessage
{
public:
    enum {
        ADD = 0,
        REMOVE
    };

public:
    ProgramInfoPacket() : NetMessage(PACKET_PROGRAM_INFO)
    {
        memset(m_Data, 0, PROGRAMINFO_SIZE);
    }

    ~ProgramInfoPacket()
    {
        Reset();
    }

public:
    void FillData(char* buff, unsigned int size)
    {
        Reset();
        NetMessage::FillData(buff, size);
        char* pBuff = buff + HEADER_SIZE;
        memcpy(m_Data, pBuff, PROGRAMINFO_SIZE);
    }

    int MakeDataBuff(char buff[])
    {
        int iSize = NetMessage::MakeDataBuff(buff);
        memcpy(buff+iSize, m_Data, PROGRAMINFO_SIZE);
        iSize += PROGRAMINFO_SIZE;
        return iSize;
    }

    void Reset()
    {
        memset(m_Data, 0, PROGRAMINFO_SIZE);
    }

public:
    union
    {
        char m_Data[PROGRAMINFO_SIZE];
        struct
        {  
           char             m_Action;
           unsigned char    m_ProgramId;
           char             m_acPath[260];    
        };
    };
};
//-------------------------------------------------------------------------------------------------
class CommandInfoPacket : public NetMessage
{
public:
    CommandInfoPacket() : NetMessage(PACKET_COMMAND_INFO)
    {
        memset(m_Data, 0, COMMANDINFO_SIZE);        
    }

    ~CommandInfoPacket()
    {
    }

public:
    void FillData(char* buff, unsigned int size)
    {
        NetMessage::FillData(buff, size);
        char* pBuff = buff + HEADER_SIZE;
        memcpy(m_Data, pBuff, COMMANDINFO_SIZE);

        //memcpy(&m_ucCommand, pBuff, sizeof(char));
        //pBuff += sizeof(char);
        //memcpy(m_acPath, pBuff, sizeof(char)* 260);
    }

    int MakeDataBuff(char buff[])
    {
        int iSize = NetMessage::MakeDataBuff(buff);

        memcpy(buff+iSize, m_Data, COMMANDINFO_SIZE);
        return iSize + COMMANDINFO_SIZE;

        //memcpy(buff+iSize, &m_ucCommand, sizeof(char));
        //iSize+=sizeof(char);
        //memcpy(buff+iSize, m_acPath, sizeof(char) * 260);
        //iSize+=260;
    }
public:
    union
    {
        char m_Data[COMMANDINFO_SIZE];
        struct {
            unsigned char m_ucCommand;
            unsigned char m_ProgramId;
            //char m_acPath[260];
        };
    };
};
//-------------------------------------------------------------------------------------------------
class ServerStatePacket : public NetMessage
{
public:
    ServerStatePacket() : NetMessage(PACKET_SERVERSTATE)
    {
        memset(m_Data, 0, SERVERSTATE_SIZE);        
    }

    ~ServerStatePacket()
    {
    }

public:
    void FillData(char* buff, unsigned int size)
    {
        NetMessage::FillData(buff, size);
        char* pBuff = buff + HEADER_SIZE;
        memcpy(m_Data, pBuff, SERVERSTATE_SIZE);
    }

    int MakeDataBuff(char buff[])
    {
        int iSize = NetMessage::MakeDataBuff(buff);

        memcpy(buff+iSize, m_Data, SERVERSTATE_SIZE);
        return iSize + SERVERSTATE_SIZE;
    }
public:
    union
    {
        char m_Data[SERVERSTATE_SIZE];
        struct
        {
            unsigned char m_ucState;
        };
    };
};
//-------------------------------------------------------------------------------------------------
class SubClientInfoPacket : public NetMessage
{
public:
	SubClientInfoPacket() : NetMessage(PACKET_SUBCLIENT_INFO), m_uiProgramCount(0), m_ucClientOn(0)
	{
		memset(m_Data, 0, SUBCLIENTINFO_SIZE);
	}
	~SubClientInfoPacket()
	{
		Reset();
	}

public:
	void FillData(char* buff, unsigned int size)
	{
		Reset();
		NetMessage::FillData(buff, size);
		char* pBuff = buff + HEADER_SIZE;
		memcpy(m_Data, pBuff, SUBCLIENTINFO_SIZE);
		//pBuff += CLIENTINFO_SIZE;

		//ProgramInfo* pInfo = NULL;
		//for(int i = 0; i < m_uiProgramCount; ++i)
		//{
		//    pInfo = new ProgramInfo;
		//    memcpy(pInfo, pBuff, DATA_SIZE_PROGRAM_INFO);
		//    pBuff+=DATA_SIZE_PROGRAM_INFO;
		//    m_vecProgramInfo.push_back(pInfo);
		//}
	}

	int MakeDataBuff(char buff[])
	{
		int iSize = NetMessage::MakeDataBuff(buff);
		memcpy(buff+iSize, m_Data, SUBCLIENTINFO_SIZE);
		iSize += SUBCLIENTINFO_SIZE;
		//for(int i = 0, n = m_vecProgramInfo.size(); i < n; ++i)
		//{
		//    memcpy(buff+iSize, m_vecProgramInfo[i], DATA_SIZE_PROGRAM_INFO);
		//    iSize += DATA_SIZE_PROGRAM_INFO;
		//}
		return iSize;
	}
	void Reset()
	{
		memset(m_Data, 0, SUBCLIENTINFO_SIZE);
		//for(int i = 0, n = m_vecProgramInfo.size(); i < n; ++i)
		//{
		//    if(m_vecProgramInfo[i])
		//        delete m_vecProgramInfo[i];
		//    m_vecProgramInfo[i] = 0;
		//}
		//m_vecProgramInfo.clear();
	}
public:
	union
	{
		char m_Data[SUBCLIENTINFO_SIZE];
		struct
		{
			char			m_acServerIP[16];
			unsigned char   m_ucClientOn;
			unsigned int    m_uiProgramCount;
			char            m_acClientName[24];
			ProgramInfo     m_ProgramInfo;
		};
	};
	//vector<ProgramInfo*> m_vecProgramInfo;
};
//-------------------------------------------------------------------------------------------------
class SubProgramInfoPacket : public NetMessage
{
public:
	SubProgramInfoPacket() : NetMessage(PACKET_SUBPROGRAM_INFO)
	{
		memset(m_Data, 0, SUBPROGRAMINFO_SIZE);
	}
	~SubProgramInfoPacket()
	{
		Reset();
	}

public:
	void FillData(char* buff, unsigned int size)
	{
		Reset();
		NetMessage::FillData(buff, size);
		char* pBuff = buff + HEADER_SIZE;
		memcpy(m_Data, pBuff, SUBPROGRAMINFO_SIZE);

	}

	int MakeDataBuff(char buff[])
	{
		int iSize = NetMessage::MakeDataBuff(buff);
		memcpy(buff+iSize, m_Data, SUBPROGRAMINFO_SIZE);
		iSize += SUBPROGRAMINFO_SIZE;
		return iSize;
	}
	void Reset()
	{
		memset(m_Data, 0, SUBPROGRAMINFO_SIZE);
	}
public:
	union
	{
		char m_Data[SUBPROGRAMINFO_SIZE];
		struct
		{
			char			m_acServerIP[16];
			char			m_Action;
			unsigned char	m_ProgramId;
			char			m_acPath[260];
		};
	};
	//vector<ProgramInfo*> m_vecProgramInfo;
};

//-------------------------------------------------------------------------------------------------
class CheckBoxUpdatePacket : public NetMessage
{
public:
    CheckBoxUpdatePacket() : NetMessage(PACKET_CHECKBOX_INFO)
    {
        memset(m_Data, 0, CHECKBOXUPDATE_SIZE);        
    }

    ~CheckBoxUpdatePacket()
    {
    }

public:
    void FillData(char* buff, unsigned int size)
    {
        NetMessage::FillData(buff, size);
        char* pBuff = buff + HEADER_SIZE;
        memcpy(m_Data, pBuff, CHECKBOXUPDATE_SIZE);

        //memcpy(&m_ucCommand, pBuff, sizeof(char));
        //pBuff += sizeof(char);
        //memcpy(m_acPath, pBuff, sizeof(char)* 260);
    }

    int MakeDataBuff(char buff[])
    {
        int iSize = NetMessage::MakeDataBuff(buff);

        memcpy(buff+iSize, m_Data, CHECKBOXUPDATE_SIZE);
        return iSize + CHECKBOXUPDATE_SIZE;

        //memcpy(buff+iSize, &m_ucCommand, sizeof(char));
        //iSize+=sizeof(char);
        //memcpy(buff+iSize, m_acPath, sizeof(char) * 260);
        //iSize+=260;
    }
public:
    union
    {
        char m_Data[CHECKBOXUPDATE_SIZE];
        struct {
			char m_ClientIp[16];	//16
            bool m_bCheck;			//1
            unsigned char m_ProgramId;	//1
            //char m_acPath[260];
        };
    };
};
#pragma pack(pop)


#endif