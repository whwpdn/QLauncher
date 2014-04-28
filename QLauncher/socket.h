////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
// Copyright (C) 2000  Thierry Tremblay
//
// http://frogengine.net-connect.net/
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef TT_SOCKET_H
#define TT_SOCKET_H

////////////////////////////////////////////////////////////////////////////////////////
//
// WINSOCK_VERSION: This control which version of winsock is used
//
////////////////////////////////////////////////////////////////////////////////////////

#ifndef WINSOCK_VERSION
#define WINSOCK_VERSION    1
#endif
//
#if !(WINSOCK_VERSION >=1 && WINSOCK_VERSION <= 2)
#error Winsock version not supported
#endif



////////////////////////////////////////////////////////////////////////////////////////
//
// Headers
//
////////////////////////////////////////////////////////////////////////////////////////

#if WINSOCK_VERSION == 1
#include <winsock.h>
#else
#define INCL_WINSOCK_API_TYPEDEFS 1
#include <winsock2.h>
#endif



////////////////////////////////////////////////////////////////////////////////////////
//
// NetworkAddress - This is an interface to a socket address
//
////////////////////////////////////////////////////////////////////////////////////////

class NetworkAddress
{
public:
   virtual ~NetworkAddress() {}
   
   bool  operator==( const NetworkAddress& other ) const;
   bool  operator!=( const NetworkAddress& other ) const;

   virtual  operator void*() const = 0;
   operator sockaddr*() const                   { return GetSockAddr(); }

   virtual NetworkAddress* Clone() const = 0;
   virtual int             GetFamily() const = 0;
   virtual int             GetSize() const = 0;
   virtual sockaddr*       GetSockAddr() const = 0;
};



////////////////////////////////////////////////////////////////////////////////////////
//
// Socket
//
////////////////////////////////////////////////////////////////////////////////////////

class Socket
{
public:
    int SetOption(int level, int optname, void* optval, int optlen);

   // Construction / destruction
   Socket( int iAddressFamily = AF_INET, int type = SOCK_STREAM, int protocol = IPPROTO_TCP );
   Socket( SOCKET socket );
   virtual ~Socket();
   
   // Connection handling
   bool     Accept( Socket** ppSocket, NetworkAddress* pAddress = 0 ) const;
   bool     Bind( const NetworkAddress& address ) const;
   bool     Close();
   bool     Connect( const NetworkAddress& address ) const;
   bool     Listen( int queueSize ) const;
   
   // Transmission
   int      Receive( char* pBuffer, int lenBuffer, int flags = 0 ) const;
   int      Send( const char* pBuffer, int lenBuffer, int flags = 0 ) const;
   int      ReceiveFrom( char* pBuffer, int lenBuffer, NetworkAddress& address, int flags = 0 ) const;
   int      ReceiveFrom( char* pBuffer, int lenBuffer, int flags = 0 ) const; 
   int      SendTo( const char* pBuffer, int lenBuffer, const NetworkAddress& address, int flags = 0 ) const;
 
   // Timeout management
   int      GetReceiveTimeout() const              { return m_recvTimeout; }
   int      GetSendTimeout() const                 { return m_sendTimeout; }
   void     SetReceiveTimeout( int seconds )       { m_recvTimeout = seconds; }
   void     SetSendTimeout( int seconds )          { m_sendTimeout = seconds; }

   // State management
   int      GetLastError() const                   { return m_lastError; }
   bool     IsExceptionPending( int timeout = -1, int usec = 0 ) const;
   bool     IsReadReady( int timeout = -1, int usec = 0 ) const;
   bool     IsWriteReady( int timeout = -1, int usec = 0 ) const;
   bool     SetBlocking( bool bBlocking );

   long     GetReadyBytes();

   // Address management
   NetworkAddress*         CreateAddress() const;
   const NetworkAddress&   GetLocalAddress() const;
   const NetworkAddress&   GetRemoteAddress() const;

   // Stream access
// std::istream&           GetInputStream();
// std::ostream&           GetOutputStream();
// std::iostream&          GetIOStream();


   SOCKET                   GetSocket() { return m_socket; }


protected:

   static bool             s_bInitialized;

   SOCKET                  m_socket;
   mutable int             m_lastError;
   int                     m_addressFamily;
   int                     m_sendTimeout;
   int                     m_recvTimeout;
   mutable NetworkAddress* m_pLocalAddress;
   mutable NetworkAddress* m_pRemoteAddress;

/*

   ////////////////////////////////////////////////////////////////////////////////////////
   // StreamBuffer
   ////////////////////////////////////////////////////////////////////////////////////////

   class StreamBuffer : public std::basic_streambuf<char>
   {
   public:
      StreamBuffer( Socket& socket );
      ~StreamBuffer();

   protected:
      virtual int overflow( int c = EOF );
      virtual int underflow();   
      virtual int sync();

   private:
      int         FlushOutput();
      
      Socket&     m_socket;
      char        m_inputBuffer[512];
      char        m_outputBuffer[512];
   };



   ////////////////////////////////////////////////////////////////////////////////////////
   // Socket Streams
   ////////////////////////////////////////////////////////////////////////////////////////

   class ISocketStream : public std::basic_istream<char>
   {
   public:
      ISocketStream( Socket& socket ) : std::basic_istream<char>( new StreamBuffer( socket ) ) {}
      ~ISocketStream() { delete rdbuf(); }
   };

   class OSocketStream : public std::basic_ostream<char>
   {
   public:
      OSocketStream( Socket& socket ) : std::basic_ostream<char>( new StreamBuffer( socket ) ) {}
      ~OSocketStream() { delete rdbuf(); }
   };

   class IOSocketStream : public std::basic_iostream<char>
   {
   public:
      IOSocketStream( Socket& socket ) : std::basic_iostream<char>( new StreamBuffer( socket ) ) {}
      ~IOSocketStream() { delete rdbuf(); }
   };



   ISocketStream*    m_pIStream;
   OSocketStream*    m_pOStream;
   IOSocketStream*   m_pIOStream;
*/
};


#endif