
//������Ŷó��

#define SOCKET_OUT       3428
#define PROGRAM_GO      3429
#define PROGRAM_EXIT    3430

#define CHECK_PLZ_SERVER 3431
#define CHECK_OK_CLIENT  3432

typedef struct _pack
{
    char File[10][120];     //���ϸ�
    char FilePath[10][256]; //���ϰ��
    double FileSize;        //���Ͽ뷮

    char ServerSendFile[256]; //������ �����϶�� ����� ���ϸ�
    
    bool ProgramOn;         //���α׷��� �������ΰ�.

    int ServerToClientOrder;
    int ListCount;
}pack;