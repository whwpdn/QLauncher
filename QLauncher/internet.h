////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
// Copyright (C) 2000  Thierry Tremblay
//
// http://frogengine.net-connect.net/
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef TT_INTERNET_H
#define TT_INTERNET_H

#include "socket.h"

////////////////////////////////////////////////////////////////////////////////////////
//
// InternetAddress
//
////////////////////////////////////////////////////////////////////////////////////////

class InternetAddress : public NetworkAddress, public sockaddr_in
{
public:
   InternetAddress( unsigned addr = INADDR_ANY, int port = 0 );
   InternetAddress( const char* szHostName, const char* szService, const char* szProtocol = "tcp" );
   InternetAddress( const char* szHostName, int port = 0 );
   InternetAddress( const sockaddr_in& sa );

   void GetIP(char* buff,int lenBuff) const;

   // Get methods
   bool            GetHostName( char* buffer, int lenBuffer ) const;
   int             GetPort() const        { return ntohs( sin_port ); }

   // Set methods
   void            SetAddress( const char* szHostName,int i );
   void            SetPort( const char* szService, const char* szProtocol = "tcp" );   
   void            SetPortByPortNum( int port )    { sin_port = htons( port ); }

   // NetworkAddress interface
   operator void*() const                 { return (sockaddr_in*) this; }
   
   NetworkAddress* Clone() const          { return new InternetAddress( *this ); }
   int             GetFamily() const      { return sin_family; }
   int             GetSize() const        { return sizeof(sockaddr_in); }
   sockaddr*       GetSockAddr() const    { return (sockaddr*)((sockaddr_in*)this); }
};



#endif