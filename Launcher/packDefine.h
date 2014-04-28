
//통합패킷처리

#define SOCKET_OUT       3428
#define PROGRAM_GO      3429
#define PROGRAM_EXIT    3430

#define CHECK_PLZ_SERVER 3431
#define CHECK_OK_CLIENT  3432

typedef struct _pack
{
    char File[10][120];     //파일명
    char FilePath[10][256]; //파일경로
    double FileSize;        //파일용량

    char ServerSendFile[256]; //서버가 실행하라고 명령한 파일명
    
    bool ProgramOn;         //프로그램이 실행중인가.

    int ServerToClientOrder;
    int ListCount;
}pack;