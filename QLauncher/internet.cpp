////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
// Copyright (C) 2000  Thierry Tremblay
//
// http://frogengine.net-connect.net/
//
////////////////////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "internet.h"



////////////////////////////////////////////////////////////////////////////////////////
//
// InternetAddress
//
////////////////////////////////////////////////////////////////////////////////////////

InternetAddress::InternetAddress( unsigned addr, int port )
{
   sin_family      = AF_INET;
   sin_addr.s_addr = htonl( addr );
   sin_port        = htons( port );
}

InternetAddress::InternetAddress( const char* szHostName, const char* szService, const char* szProtocol )
{
   SetAddress( szHostName,0 );
   SetPort( szService, szProtocol );
}



InternetAddress::InternetAddress( const char* szHostName, int port )
{
   SetAddress( szHostName,0 );
   sin_port = htons( port );   
}



InternetAddress::InternetAddress( const sockaddr_in& sa )
{
   *(sockaddr_in*)this = sa;
}



bool InternetAddress::GetHostName( char* buffer, int lenBuffer ) const
{
   if (sin_addr.s_addr == INADDR_ANY)
   {
      if (gethostname( buffer, lenBuffer ) == SOCKET_ERROR)
      {
         buffer[0] = 0;
         return false;
      }
   }
   else
   {
      const HOSTENT* pHost = gethostbyaddr( (const char*)&sin_addr, sizeof(sin_addr), AF_INET);
      if (pHost == 0 || pHost->h_name == 0)
      {
         buffer[0] = 0;
         return false;
      }

      strncpy( buffer, pHost->h_name, lenBuffer );
   }

   return true;
}



void InternetAddress::SetAddress( const char* szHostName,int i )
{
    sin_family      = AF_INET;
    
    if (szHostName == NULL)
        sin_addr.s_addr = htonl(INADDR_ANY);
    else
        sin_addr.s_addr = inet_addr( szHostName );
    
    if (sin_addr.s_addr == INADDR_NONE)
    {
        const HOSTENT* pHost = gethostbyname( szHostName );
        if (pHost != 0)
        {
            sin_addr = *(IN_ADDR*)pHost->h_addr_list[i];
        }
    }
}

void InternetAddress::SetPort( const char* szService, const char* szProtocol )
{
   sin_port = 0;

   const SERVENT* pService = getservbyname( szService, szProtocol );
   
   if (pService != 0)
   {
      sin_port = pService->s_port;
   }
}

void InternetAddress::GetIP(char* buff,int lenBuff) const
{
	strncpy(buff,inet_ntoa(sin_addr),lenBuff);
}