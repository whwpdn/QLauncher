////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
// Copyright (C) 2000  Thierry Tremblay
//
// http://frogengine.net-connect.net/
//
////////////////////////////////////////////////////////////////////////////////////////

//#include "stdafx.h"
#include "socket.h"
#include "internet.h"
#include <cassert>


////////////////////////////////////////////////////////////////////////////////////////
//
// Automatically link the proper library
//
////////////////////////////////////////////////////////////////////////////////////////

#ifdef _MSC_VER

#if WINSOCK_VERSION == 1
#pragma comment( lib, "wsock32" )
#elif WINSOCK_VERSION == 2
#pragma comment( lib, "ws2_32" );
#endif

#endif



////////////////////////////////////////////////////////////////////////////////////////
//
// NetworkAddress - This is an interface to a socket address
//
////////////////////////////////////////////////////////////////////////////////////////

bool NetworkAddress::operator==( const NetworkAddress& other ) const
{
   int size = GetSize();
   
   if (size != other.GetSize())
      return false;

   return memcmp( *this, other, size ) == 0;
}



bool NetworkAddress::operator!=( const NetworkAddress& other ) const
{
   int size = GetSize();
   
   if (size != other.GetSize())
      return true;

   return memcmp(*this, other, size) != 0;
}



////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
//
////////////////////////////////////////////////////////////////////////////////////////

bool Socket::s_bInitialized = false;



Socket::Socket( int iAddressFamily, int type, int protocol ) :
   m_socket(INVALID_SOCKET),
   m_lastError(0),
   m_addressFamily(iAddressFamily),
   m_recvTimeout(-1),
   m_sendTimeout(-1),
   m_pLocalAddress(0),
   m_pRemoteAddress(0)//,
   //m_pIStream(0),
   //m_pOStream(0),
   //m_pIOStream(0)
{
   if (!s_bInitialized)
   {
      WSADATA data;
      WORD version;

#if WINSOCK_VERSION == 1
      version = MAKEWORD(1,1);
#elif WINSOCK_VERSION == 2
      version = MAKEWORD(2,2);
#endif

      m_lastError = WSAStartup( version, &data );
      if (m_lastError == 0)
         s_bInitialized = true;
   }

   if (m_lastError == 0)
   {
      m_socket = socket( iAddressFamily, type, protocol );
      if (m_socket == INVALID_SOCKET)
      {
         m_lastError = WSAGetLastError();
      }
   } 
}



Socket::Socket( SOCKET socket )
:  m_socket(socket),
   m_lastError(0),
   m_addressFamily(AF_UNSPEC),
   m_recvTimeout(-1),
   m_sendTimeout(-1),
   m_pLocalAddress(0),
   m_pRemoteAddress(0)
{
   // Discover address family
   BYTE address[1024];
   int lenAddress = sizeof(address);

   if (getsockname( m_socket, (sockaddr*)address, &lenAddress ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
   }
   else
   {
      m_addressFamily = ((SOCKADDR*)address)->sa_family;
   }
}



Socket::~Socket()
{
   if (m_socket != INVALID_SOCKET)
      Close();

   if (m_pLocalAddress)
      delete m_pLocalAddress;

   if (m_pRemoteAddress)
      delete m_pRemoteAddress;
}



bool Socket::Accept( Socket** ppSocket, NetworkAddress* pAddress ) const
{
   SOCKET socket;

   if (pAddress)
   {
      assert( pAddress->GetFamily() == m_addressFamily );
      
      int size = pAddress->GetSize();
      socket = accept( m_socket, *pAddress, &size );
   }
   else
   {
      socket = accept( m_socket, 0, 0 );
   }
   
   if (socket == INVALID_SOCKET)
   {
      *ppSocket = 0;
      m_lastError = WSAGetLastError();
      return false;
   }

   *ppSocket = new Socket( socket );
   return true;
}



bool Socket::Bind( const NetworkAddress& address ) const
{ 
   if (bind( m_socket, address, address.GetSize() ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }
   return true;
}



bool Socket::Close()
{
   if (closesocket(m_socket) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }
   m_socket = INVALID_SOCKET;
   return true;
}

bool Socket::Connect( const NetworkAddress& address ) const
{
   if (connect( m_socket, address, address.GetSize() ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }
   return true;
}

NetworkAddress* Socket::CreateAddress() const
{
   if (m_addressFamily == AF_INET)
      return new InternetAddress();

   // Unsupported address family
   assert(0);

   return 0;
}



/*
std::istream& Socket::GetInputStream()
{
   if (m_pIStream==0)
   {
      m_pIStream = new ISocketStream( *this );
      if (m_pOStream)
         m_pIStream->tie( m_pOStream );
   }

   return *m_pIStream;
}



std::ostream& Socket::GetOutputStream()
{
   if (m_pOStream==0)
   {
      m_pOStream = new OSocketStream( *this );
      if (m_pIStream)
         m_pIStream->tie( m_pOStream );
   }

   return *m_pOStream;
}



std::iostream& Socket::GetIOStream()
{
   if (m_pIOStream==0)
   {
      m_pIOStream = new IOSocketStream( *this );
   }

   return *m_pIOStream;
}
*/


const NetworkAddress& Socket::GetLocalAddress() const
{
   if (m_pLocalAddress==0)
      m_pLocalAddress = CreateAddress();

   int lenAddress = m_pLocalAddress->GetSize();
   if (getsockname( m_socket, m_pLocalAddress->GetSockAddr(), &lenAddress ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
   }

   return *m_pLocalAddress;
}



const NetworkAddress& Socket::GetRemoteAddress() const
{
   if (m_pRemoteAddress==0)
      m_pRemoteAddress = CreateAddress();

   int lenAddress = m_pRemoteAddress->GetSize();
   if (getpeername( m_socket, m_pRemoteAddress->GetSockAddr(), &lenAddress ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
   }

   return *m_pRemoteAddress;
}



bool Socket::IsExceptionPending( int sec, int usec ) const
{
   fd_set fds;

   FD_ZERO( &fds );
   FD_SET( m_socket, &fds );

   timeval tv;
   tv.tv_sec  = sec;
   tv.tv_usec = usec;

   int result = select( m_socket+1, 0, 0, &fds, &tv );
   
   if (result == 0 || result == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }

   return true;
}

long
Socket::GetReadyBytes()
{
    unsigned long   len = 0;
    int     r = ::ioctlsocket(m_socket, FIONREAD, & len);
    if (r != 0) {
        //nvThrow nvIOException("socketAvailableBytes");
        m_lastError = WSAGetLastError();
        return -1;
    }

    return len;
}

bool Socket::IsReadReady( int sec, int usec ) const
{
    /*
    unsigned long     len;
    int     r = ioctlsocket(m_socket, FIONREAD, & len);
    if (r < 0) {
        // ERROR
    }
    return len > 0;
    */

    
   fd_set fds;

   FD_ZERO( &fds );
   FD_SET( m_socket, &fds );

   timeval tv;
   tv.tv_sec  = sec;
   tv.tv_usec = usec;

   int result = select( m_socket+1, &fds, 0, 0, &tv );
   
   if (result == 0 || result == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }

   return true;

}



bool Socket::IsWriteReady( int sec, int usec ) const
{
   fd_set fds;

   FD_ZERO( &fds );
   FD_SET( m_socket, &fds );

   timeval tv;
   tv.tv_sec  = sec;
   tv.tv_usec = usec;

   int result = select( m_socket+1, 0, &fds, 0, &tv );
   
   if (result == 0 || result == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }

   return true;
}



bool Socket::Listen( int queueSize ) const
{
   if (listen( m_socket, queueSize ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }
   return true;
}



int Socket::Receive( char* pBuffer, int lenBuffer, int flags ) const
{
   if (m_recvTimeout >= 0 && !IsReadReady( m_recvTimeout ))
      return 0;

   int nbRecv = recv( m_socket, pBuffer, lenBuffer, flags );
   if (nbRecv == SOCKET_ERROR)
   {
      int error = WSAGetLastError();
      
      if (error == WSAEMSGSIZE)
         return lenBuffer;

      m_lastError = error;
      return 0;
   }
   return nbRecv;
}



int Socket::ReceiveFrom( char* pBuffer, int lenBuffer, NetworkAddress& address, int flags ) const
{
   if (m_recvTimeout >= 0 && !IsReadReady( m_recvTimeout ))
      return 0;

   assert( address.GetFamily() == m_addressFamily );

   int addressSize = address.GetSize();
   int nbRecv = recvfrom( m_socket, pBuffer, lenBuffer, flags, address, &addressSize );
   if (nbRecv == SOCKET_ERROR)
   {
      int error = WSAGetLastError();
      
      if (error == WSAEMSGSIZE)
         return lenBuffer;

      m_lastError = error;
      return 0;
   }
   return nbRecv;
}

int Socket::ReceiveFrom( char* pBuffer, int lenBuffer, int flags ) const
{
   if (m_recvTimeout >= 0 && !IsReadReady( m_recvTimeout ))
      return 0;

   //assert( address.GetFamily() == m_addressFamily );

   //int addressSize = address.GetSize();
   int nbRecv = recvfrom( m_socket, pBuffer, lenBuffer, flags, NULL, NULL);
   if (nbRecv == SOCKET_ERROR)
   {
      int error = WSAGetLastError();
      
      if (error == WSAEMSGSIZE)
         return lenBuffer;

      m_lastError = error;
      return 0;
   }
   return nbRecv;
}





int Socket::Send( const char* pBuffer, int lenBuffer, int flags ) const
{
   if (m_sendTimeout >= 0 && !IsWriteReady( m_sendTimeout ))
      return 0;
    
   int nbSent = send( m_socket, pBuffer, lenBuffer, flags );
   
   if (nbSent == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return 0;
   }
   return nbSent;
}



int Socket::SendTo( const char* pBuffer, int lenBuffer, const NetworkAddress& address, int flags ) const
{
   if (m_sendTimeout >= 0 && !IsWriteReady( m_sendTimeout ))
      return 0;

   int nbSent = sendto( m_socket, pBuffer, lenBuffer, flags, address, address.GetSize() );
   if (nbSent == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return 0;
   }
   return nbSent;
}



bool Socket::SetBlocking( bool bBlocking )
{
   ULONG param = bBlocking ? 0 : 1;

   if (ioctlsocket( m_socket, FIONBIO, &param ) == SOCKET_ERROR)
   {
      m_lastError = WSAGetLastError();
      return false;
   }
   return true;
}


/*
////////////////////////////////////////////////////////////////////////////////////////
//
// Socket::StreamBuffer
//
////////////////////////////////////////////////////////////////////////////////////////

Socket::StreamBuffer::StreamBuffer( Socket& socket )
:  m_socket( socket )
{
   char* pBeginInput = m_inputBuffer;
   char* pEndInput   = m_inputBuffer + sizeof(m_inputBuffer);

   setg( pBeginInput, pEndInput, pEndInput );
   setp( m_outputBuffer, m_outputBuffer + sizeof(m_outputBuffer) );
}



Socket::StreamBuffer::~StreamBuffer()
{
   FlushOutput();
}



int Socket::StreamBuffer::FlushOutput()
{
   // Return 0 if nothing to flush or success
   // Return EOF if couldnt flush

   if (pptr() < pbase())
      return 0;

   if (!m_socket.Send( pbase(), pptr() - pbase() ))
      return EOF;

   setp( m_outputBuffer, m_outputBuffer + sizeof(m_outputBuffer) );

   return 0;
}



int Socket::StreamBuffer::overflow( int c )
{
   if (c == EOF)
      return FlushOutput();

   *pptr() = c;
   pbump(1);

   if (c == '\n' || pptr() >= epptr())
   {
      if (FlushOutput() == EOF)
         return EOF;
   }

   return c;
}



int Socket::StreamBuffer::sync()
{
   return FlushOutput();
}



int Socket::StreamBuffer::underflow()
{
   if (gptr() < egptr())
      return *(unsigned char*)gptr();

   int nbRead = m_socket.Receive( m_inputBuffer, sizeof(m_inputBuffer) );
   
   if (nbRead == 0)
      return EOF;

   setg( eback(), eback(), eback() + nbRead );

   return *(unsigned char*)gptr();
}
*/

int Socket::SetOption(int level, int optname, void *optval, int optlen)
{
    return setsockopt(m_socket, level, optname, (const char FAR*) optval, optlen);
}
