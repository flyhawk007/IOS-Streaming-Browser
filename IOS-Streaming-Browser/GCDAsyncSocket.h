//  
//  GCDAsyncSocket.h
//  
//  This class is in the public domain.
//  Originally created by Robbie Hanson in Q3 2010.
//  Updated and maintained by Deusty LLC and the Mac development community.
//  
//  http://code.google.com/p/cocoaasyncsocket/
//

#import <Foundation/Foundation.h>
#import <Security/Security.h>
#import <dispatch/dispatch.h>

@class GCDAsyncReadPacket;
@class GCDAsyncWritePacket;

extern NSString *const GCDAsyncSocketException;
extern NSString *const GCDAsyncSocketErrorDomain;

#if !TARGET_OS_IPHONE
extern NSString *const GCDAsyncSocketSSLCipherSuites;
extern NSString *const GCDAsyncSocketSSLDiffieHellmanParameters;
#endif

/** 
 *
 * \enum GCDAsyncSocketError
 *
 * \brief Creates a data type consisting of a set of named values for holding the various socket errors 
**/
enum GCDAsyncSocketError
{
	GCDAsyncSocketNoError = 0,           ///< Never used
	GCDAsyncSocketBadConfigError,        ///< Invalid configuration
	GCDAsyncSocketBadParamError,         ///< Invalid parameter was passed
	GCDAsyncSocketConnectTimeoutError,   ///< A connect operation timed out
	GCDAsyncSocketReadTimeoutError,      ///< A read operation timed out
	GCDAsyncSocketWriteTimeoutError,     ///< A write operation timed out
	GCDAsyncSocketReadMaxedOutError,     ///< Reached set maxLength without completing
	GCDAsyncSocketClosedError,           ///< The remote peer closed the connection
	GCDAsyncSocketOtherError,            ///< Description provided in userInfo
};
typedef enum GCDAsyncSocketError GCDAsyncSocketError;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief A Grand Central Dispatch Asynchronous Socket
**/
@interface GCDAsyncSocket : NSObject
{
    /**
        @brief Flags for the asynchronous socket
    **/
	UInt16 flags;
    
    
    /**
        
    **/
	UInt16 config;
	
    /**
        @brief The delegate for the GCDAsyncSocket
    **/
	id delegate;
    
    /**
        @brief Dispatch queues are lightweight objects to which blocks may be submitted. The system manages a pool of threads which process dispatch queues and invoke blocks submitted to them.
    **/
	dispatch_queue_t delegateQueue;  //dispatch queue
	
    /**
        @brief IP version 4 socket file descriptor
    **/
	int socket4FD;  

	/**
        @brief IP version 6 socket file descriptor
    **/
    int socket6FD;  

    /**
        @brief Index of the connection
    **/
	int connectIndex; 
    
    /**
        @brief Object-oriented wrappers for byte buffers
        IP version 4 interface
    **/
	NSData * connectInterface4;  
    
    /**
        @brief IP version 6 interface
    **/
	NSData * connectInterface6;  
	
    /**
        @brief The dispatch queue upon which blocks are submitted
    **/
	dispatch_queue_t socketQueue;  
	
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
    **/
	dispatch_source_t accept4Source; 
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t accept6Source;
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t connectTimer;
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t readSource;
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t writeSource;
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t readTimer;
    
    /**
        @brief Dispatch sources are used to automatically submit event handler blocks to dispatch queues in response to external events.
     **/
	dispatch_source_t writeTimer;
	
    
    /**
        @brief The read queue
    **/
	NSMutableArray *readQueue;  
    
    /**
        @brief The write queue
    **/
	NSMutableArray *writeQueue;  
	
    
    /**
        @brief The read packet
    **/
	GCDAsyncReadPacket *currentRead; 
    
    /**
        @brief The write packet
    **/
	GCDAsyncWritePacket *currentWrite; 
	
    /**
        @brief Socket file descriptor bytes available
        Value is 0 to 2,147,483,647
    **/
	unsigned long socketFDBytesAvailable;  
	
    /**
        @brief A partial read buffer for buffering the host request
    **/
	NSMutableData *partialReadBuffer;  
		
#if TARGET_OS_IPHONE
    
    /**
        @brief A struct for holding client information
    **/
	CFStreamClientContext streamContext;
    
    /**
        @brief Is the interface for reading a byte stream either synchronously or asynchronously.
    **/
	CFReadStreamRef readStream;
    
    /**
        @brief Is the interface for writing a byte stream either synchronously or asynchronously
    **/
	CFWriteStreamRef writeStream;
    
#else
    
    /**
        @brief The SSL context reference
    **/
	SSLContextRef sslContext; 
    
    /**
        @brief The SSL read buffer for buffering the host response
    **/
	NSMutableData *sslReadBuffer; 
    
    /**
        @brief Size of the SSL write cache
    **/
	size_t sslWriteCachedLength; 
    
#endif
	
    /**
        @brief User data
    **/
	id userData; 
}

/**
    GCDAsyncSocket uses the standard delegate paradigm, but executes all delegate callbacks on a given delegate dispatch queue.
    This allows for maximum concurrency, while at the same time providing easy thread safety.
 
    You MUST set a delegate AND delegate dispatch queue before attempting to use the socket, or you will get an error.
 
    The socket queue is optional.
    If you pass NULL, GCDAsyncSocket will automatically create it's own socket queue.
 
    If you choose to provide a socket queue, the socket queue must not be a concurrent queue.
 
    The delegate queue and socket queue can optionally be the same.
    @return id
**/
- (id)init;

/**
    @brief Initialize the GCDAsyncSocket with a socket queue
    @param dispatch_queue_t
    @return id (self)
**/
- (id)initWithSocketQueue:(dispatch_queue_t)sq;

/**
    @brief Initialize the GCDAsyncSocket with a delegate and delegate queue
    @param dispatch_queue_t
    @return id (self)
**/
- (id)initWithDelegate:(id)aDelegate delegateQueue:(dispatch_queue_t)dq;


/**
    @brief Initialize the GCDAsyncSocket with a delegate, delegate queue, and socket queue
    @param id
    @param dispatch_queue_t
    @param dispatch_queue_t
    @return id
**/
- (id)initWithDelegate:(id)aDelegate delegateQueue:(dispatch_queue_t)dq socketQueue:(dispatch_queue_t)sq;

#pragma mark Configuration

/**
    @brief Gets the delegate
    @return id
**/
- (id)delegate;

/**
    @brief Sets the delegate
    @param id
    @return void
**/
- (void)setDelegate:(id)delegate;

/**
    @brief Synchronously sets the delegate
    @param id
    @return void
**/
- (void)synchronouslySetDelegate:(id)delegate;

/**
    @brief Get the delegate queue
    @return dispatch_queue_t
**/
- (dispatch_queue_t)delegateQueue;

/**
    @brief Set the delegate queue
    @param dispatch_queue_t
    @return void
**/
- (void)setDelegateQueue:(dispatch_queue_t)delegateQueue;

/**
    @param dispatch_queue_t
    @return void
**/
- (void)synchronouslySetDelegateQueue:(dispatch_queue_t)delegateQueue;

/**
    @brief Get delegate and delegate queue
    @param id
    @param dispatch_queue_t
    @return void
**/
- (void)getDelegate:(id *)delegatePtr delegateQueue:(dispatch_queue_t *)delegateQueuePtr;

/**
    @brief Set the delegate and delegate queue
    @param id
    @param dispatch_queue_t
    @return void
**/
- (void)setDelegate:(id)delegate delegateQueue:(dispatch_queue_t)delegateQueue;

/**
    @brief Synchronously set the delegate and delegate queue
    @param id
    @param dispatch_queue_t
    @return void
**/
- (void)synchronouslySetDelegate:(id)delegate delegateQueue:(dispatch_queue_t)delegateQueue;

/**
 * Traditionally sockets are not closed until the conversation is over.
 * However, it is technically possible for the remote endpoint to close its write stream.
 * Our socket would then be notified that there is no more data to be read,
 * but our socket would still be writeable and the remote endpoint could continue to receive our data.
 * 
 * The argument for this confusing functionality stems from the idea that a client could shut down its
 * write stream after sending a request to the server, thus notifying the server there are to be no further requests.
 * In practice, however, this technique did little to help server developers.
 * 
 * To make matters worse, from a TCP perspective there is no way to tell the difference from a read stream close
 * and a full socket close. They both result in the TCP stack receiving a FIN packet. The only way to tell
 * is by continuing to write to the socket. If it was only a read stream close, then writes will continue to work.
 * Otherwise an error will be occur shortly (when the remote end sends us a RST (reset) packet).
 * 
 * In addition to the technical challenges and confusion, many high level socket/stream API's provide
 * no support for dealing with the problem. If the read stream is closed, the API immediately declares the
 * socket to be closed, and shuts down the write stream as well. In fact, this is what Apple's CFStream API does.
 * It might sound like poor design at first, but in fact it simplifies development.
 * 
 * The vast majority of the time if the read stream is closed it's because the remote endpoint closed its socket.
 * Thus it actually makes sense to close the socket at this point.
 * And in fact this is what most networking developers want and expect to happen.
 * However, if you are writing a server that interacts with a plethora of clients,
 * you might encounter a client that uses the discouraged technique of shutting down its write stream.
 * If this is the case, you can set this property to NO,
 * and make use of the socketDidCloseReadStream delegate method.
 * 
 * The default value is YES.
**/

/**
    @brief Whether automatically disconnecting upon the closing of a read stream
    @return BOOL
**/
- (BOOL)autoDisconnectOnClosedReadStream;

/**
    @brief Sets the flag for whether automatically disconnecting upon the closing of a read stream
    @param BOOL
    @return void
**/
- (void)setAutoDisconnectOnClosedReadStream:(BOOL)flag;

/**
 * By default, both IPv4 and IPv6 are enabled.
 * 
 * For accepting incoming connections, this means GCDAsyncSocket automatically supports both protocols,
 * and can simulataneously accept incoming connections on either protocol.
 * 
 * For outgoing connections, this means GCDAsyncSocket can connect to remote hosts running either protocol.
 * If a DNS lookup returns only IPv4 results, GCDAsyncSocket will automatically use IPv4.
 * If a DNS lookup returns only IPv6 results, GCDAsyncSocket will automatically use IPv6.
 * If a DNS lookup returns both IPv4 and IPv6 results, the preferred protocol will be chosen.
 * By default, the preferred protocol is IPv4, but may be configured as desired.
**/

/**
    @brief Whether IP version 4 is enabled
    @return BOOL
**/
- (BOOL)isIPv4Enabled;

/**
    @brief Set the flag for whether IP version 4 protocol is enabled
    @param BOOL
    @return void
**/
- (void)setIPv4Enabled:(BOOL)flag;


/**
    @brief Whether IP version 6 protocol is enabled
    @return BOOL
**/
- (BOOL)isIPv6Enabled;


/**
    @brief Set the flag for whether IP version 6 protocol is enabled
    @param BOOL
    @return void
**/
- (void)setIPv6Enabled:(BOOL)flag;


/**
    @brief Whether IP version 4 protocol is preferred over IP version 6 protocol
    @return BOOL
**/
- (BOOL)isIPv4PreferredOverIPv6;


/**
    @brief Set the flag for whether IP version 4 protocol is preferred over IP version 6
    @param BOOL
    @return void
**/
- (void)setPreferIPv4OverIPv6:(BOOL)flag;

/**
    @brief User data allows you to associate arbitrary information with the socket.
    This data is not used internally by socket in any way.
    @return id
**/
- (id)userData;

/**
    @brief Set userData
    @param id
    @return void
**/
- (void)setUserData:(id)arbitraryUserData;

#pragma mark Accepting

/**
    @brief Tells the socket to begin listening and accepting connections on the given port.

    When a connection is accepted, a new instance of GCDAsyncSocket will be spawned to handle it, and the socket:didAcceptNewSocket: delegate method will be invoked.
 
    The socket will listen on all available interfaces (e.g. wifi, ethernet, etc)
    @param UInt16 port
    @param NSError
    @return BOOL
**/
- (BOOL)acceptOnPort:(UInt16)port error:(NSError **)errPtr;


/**
 * This method is the same as acceptOnPort:error: with the
 * additional option of specifying which interface to listen on.
 * 
 * For example, you could specify that the socket should only accept connections over ethernet,
 * and not other interfaces such as wifi.
 * 
 * The interface may be specified by name (e.g. "en1" or "lo0") or by IP address (e.g. "192.168.4.34").
 * You may also use the special strings "localhost" or "loopback" to specify that
 * the socket only accept connections from the local machine.
 * 
 * You can see the list of interfaces via the command line utility "ifconfig",
 * or programmatically via the getifaddrs() function.
 * 
 * To accept connections on any interface pass nil, or simply use the acceptOnPort:error: method.
    @param NSString
    @param UInt6
    @param NSError
    @return BOOL
**/
- (BOOL)acceptOnInterface:(NSString *)interface port:(UInt16)port error:(NSError **)errPtr;

#pragma mark Connecting

/**
    @brief Connects to the given host and port.
 
    This method invokes connectToHost:onPort:viaInterface:withTimeout:error: and uses the default interface, and no timeout.
    @param NSString
    @param UInt16
    @return BOOL
**/
- (BOOL)connectToHost:(NSString *)host onPort:(UInt16)port error:(NSError **)errPtr;

/**
    @brief Connects to the given host and port with an optional timeout.
 
    This method invokes connectToHost:onPort:viaInterface:withTimeout:error: and uses the default interface.
    @param NSString
    @param UInt16
    @param NSTimeInterval
    @param NSError
    @return BOOL
**/
- (BOOL)connectToHost:(NSString *)host
               onPort:(UInt16)port
          withTimeout:(NSTimeInterval)timeout
                error:(NSError **)errPtr;

/**
    @brief Connects to the given host & port, via the optional interface, with an optional timeout.
 * 
 * The host may be a domain name (e.g. "deusty.com") or an IP address string (e.g. "192.168.0.2").
 * The interface may be a name (e.g. "en1" or "lo0") or the corresponding IP address (e.g. "192.168.4.35").
 * The interface may also be used to specify the local port (see below).
 * 
 * To not time out use a negative time interval.
 * 
 * This method will return NO if an error is detected, and set the error pointer (if one was given).
 * Possible errors would be a nil host, invalid interface, or socket is already connected.
 * 
 * If no errors are detected, this method will start a background connect operation and immediately return YES.
 * The delegate callbacks are used to notify you when the socket connects, or if the host was unreachable.
 * 
 * Since this class supports queued reads and writes, you can immediately start reading and/or writing.
 * All read/write operations will be queued, and upon socket connection,
 * the operations will be dequeued and processed in order.
 * 
 * The interface may optionally contain a port number at the end of the string, separated by a colon.
 * This allows you to specify the local port that should be used for the outgoing connection. (read paragraph to end)
 * To specify both interface and local port: "en1:8082" or "192.168.4.35:2424".
 * To specify only local port: ":8082".
 * Please note this is an advanced feature, and is somewhat hidden on purpose.
 * You should understand that 99.999% of the time you should NOT specify the local port for an outgoing connection.
 * If you think you need to, there is a very good chance you have a fundamental misunderstanding somewhere.
 * Local ports do NOT need to match remote ports. In fact, they almost never do.
 * This feature is here for networking professionals using very advanced techniques.
    @param NSString
    @param UInt16
    @param NSString
    @param NSTimeInterval
    @param NSerror
    @return BOOL
**/
- (BOOL)connectToHost:(NSString *)host
               onPort:(UInt16)port
         viaInterface:(NSString *)interface
          withTimeout:(NSTimeInterval)timeout
                error:(NSError **)errPtr;

/**
    @brief Connects to the given address, specified as a sockaddr structure wrapped in a NSData object.
 
    For example, a NSData object returned from NSNetservice's addresses method.
 
    If you have an existing struct sockaddr you can convert it to a NSData object like so: 
 
        struct sockaddr sa  -> NSData *dsa = [NSData dataWithBytes:&remoteAddr length:remoteAddr.sa_len];
        struct sockaddr *sa -> NSData *dsa = [NSData dataWithBytes:remoteAddr length:remoteAddr->sa_len];
  
    This method invokes connectToAdd
 
    @param NSData
    @param NSError
    @return BOOL
**/
- (BOOL)connectToAddress:(NSData *)remoteAddr error:(NSError **)errPtr;

/**
    @brief This method is the same as connectToAddress:error: with an additional timeout option.
    To not time out use a negative time interval, or simply use the connectToAddress:error: method.
    @param NSData
    @param NSTimeInterval
    @param NSError
    @return BOOL
**/
- (BOOL)connectToAddress:(NSData *)remoteAddr withTimeout:(NSTimeInterval)timeout error:(NSError **)errPtr;

/**
    @brief Connects to the given address, using the specified interface and timeout.
 
    The address is specified as a sockaddr structure wrapped in a NSData object.
    For example, a NSData object returned from NSNetservice's addresses method.
 
    If you have an existing struct sockaddr you can convert it to a NSData object like so:
    struct sockaddr sa  -> NSData *dsa = [NSData dataWithBytes:&remoteAddr length:remoteAddr.sa_len];
    struct sockaddr *sa -> NSData *dsa = [NSData dataWithBytes:remoteAddr length:remoteAddr->sa_len];
 
    The interface may be a name (e.g. "en1" or "lo0") or the corresponding IP address (e.g. "192.168.4.35").
    The interface may also be used to specify the local port (see below).
  
    The timeout is optional. To not time out use a negative time interval.
  
    This method will return NO if an error is detected, and set the error pointer (if one was given).
    Possible errors would be a nil host, invalid interface, or socket is already connected.
  
    If no errors are detected, this method will start a background connect operation and immediately return YES.
    The delegate callbacks are used to notify you when the socket connects, or if the host was unreachable.
  
    Since this class supports queued reads and writes, you can immediately start reading and/or writing.
    All read/write operations will be queued, and upon socket connection, the operations will be dequeued and processed in order.
  
    The interface may optionally contain a port number at the end of the string, separated by a colon.
    This allows you to specify the local port that should be used for the outgoing connection. (read paragraph to end)
    To specify both interface and local port: "en1:8082" or "192.168.4.35:2424".
    To specify only local port: ":8082".
    Please note this is an advanced feature, and is somewhat hidden on purpose.
    You should understand that 99.999% of the time you should NOT specify the local port for an outgoing connection.
    If you think you need to, there is a very good chance you have a fundamental misunderstanding somewhere.
    Local ports do NOT need to match remote ports. In fact, they almost never do.
    This feature is here for networking professionals using very advanced techniques.
 
    @param NSData
    @param NSString
    @param NSTimeInterval
    @param NSError
    @return BOOL
**/
- (BOOL)connectToAddress:(NSData *)remoteAddr
            viaInterface:(NSString *)interface
             withTimeout:(NSTimeInterval)timeout
                   error:(NSError **)errPtr; // pointer to a pointer

#pragma mark Disconnecting

/**
    @brief Disconnects immediately (synchronously). Any pending reads or writes are dropped.
 * If the socket is not already disconnected, the socketDidDisconnect delegate method
 * will be called immediately, before this method returns.
 * 
 * Please note the recommended way of releasing an AsyncSocket instance (e.g. in a dealloc method)
 * [asyncSocket setDelegate:nil];
 * [asyncSocket disconnect];
 * [asyncSocket release];
    @return void
**/
- (void)disconnect;

/**
    @brief Disconnects after all pending reads have completed.

    After calling this, the read and write methods will do nothing.
    The socket will disconnect even if there are still pending writes.
    @return void
**/
- (void)disconnectAfterReading;

/**
    @brief Disconnects after all pending writes have completed.
    After calling this, the read and write methods will do nothing.
    The socket will disconnect even if there are still pending reads.
    @return void
**/
- (void)disconnectAfterWriting;

/**
    @brief Disconnects after all pending reads and writes have completed.
    After calling this, the read and write methods will do nothing.
    @return void
**/
- (void)disconnectAfterReadingAndWriting;

#pragma mark Diagnostics

/**
    @brief Returns whether the socket is disconnected or connected.
 
    A disconnected socket may be recycled.
    That is, it can used again for connecting or listening.
    
    If a socket is in the process of connecting, it may be neither disconnected nor connected.
    @return BOOL
**/
- (BOOL)isDisconnected;

/**
    @brief Whether is connected
    @return BOOL
**/
- (BOOL)isConnected;

/**
    @brief Returns the local or remote host and port to which this socket is connected, or nil and 0 if not connected.
    The host will be an IP address.
    @return NSString
**/
- (NSString *)connectedHost;

/**
    @brief Gets the connected port
    @return UInt16
**/
- (UInt16)connectedPort;

/**
    @brief Gets the local host
    @return NSString
**/
- (NSString *)localHost;

/**
    @brief Gets the local port
    @return UInt16
**/
- (UInt16)localPort;

/**
    @brief Returns the local or remote address to which this socket is connected, specified as a sockaddr structure wrapped in a NSData object.
 
    See also the connectedHost, connectedPort, localHost and localPort methods.
 
    @return NSData
**/
- (NSData *)connectedAddress;


/**
    @brief Gets the local address
    @return NSData
**/
- (NSData *)localAddress;

/**
    @brief Returns whether the socket is IPv4.
    An accepting socket may be both.
    @return BOOL
**/
- (BOOL)isIPv4;

/**
    @brief Wehter the socket is IP version 6
    @return BOOL
**/
- (BOOL)isIPv6;

#pragma mark Reading

// The readData and writeData methods won't block (they are asynchronous).
// 
// When a read is complete the socket:didReadData:withTag: delegate method is dispatched on the delegateQueue.
// When a write is complete the socket:didWriteDataWithTag: delegate method is dispatched on the delegateQueue.
// 
// You may optionally set a timeout for any read/write operation. (To not timeout, use a negative time interval.)
// If a read/write opertion times out, the corresponding "socket:shouldTimeout..." delegate method
// is called to optionally allow you to extend the timeout.
// Upon a timeout, the "socket:willDisconnectWithError:" method is called, followed by "socketDidDisconnect".
// 
// The tag is for your convenience.
// You can use it as an array index, step number, state id, pointer, etc.

/**
    @brief Reads the first available bytes that become available on the socket.
 
    If the timeout value is negative, the read operation will not use a timeout.
    @param NSTimeInterval
    @param long
    @return void
**/
- (void)readDataWithTimeout:(NSTimeInterval)timeout tag:(long)tag;

/**
    @brief Reads the first available bytes that become available on the socket.
 * The bytes will be appended to the given byte buffer starting at the given offset.
 * The given buffer will automatically be increased in size if needed.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * If the buffer if nil, the socket will create a buffer for you.
 * 
 * If the bufferOffset is greater than the length of the given buffer,
 * the method will do nothing, and the delegate will not be called.
 * 
 * If you pass a buffer, you must not alter it in any way while AsyncSocket is using it.
 * After completion, the data returned in socket:didReadData:withTag: will be a subset of the given buffer.
 * That is, it will reference the bytes that were appended to the given buffer.
    @param NSTimeInterval
    @param NSMutableData
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataWithTimeout:(NSTimeInterval)timeout
					 buffer:(NSMutableData *)buffer // read buffer
			   bufferOffset:(NSUInteger)offset
						tag:(long)tag;

/**
    @brief Reads the first available bytes that become available on the socket.
 * The bytes will be appended to the given byte buffer starting at the given offset.
 * The given buffer will automatically be increased in size if needed.
 * A maximum of length bytes will be read.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * If the buffer if nil, a buffer will automatically be created for you.
 * If maxLength is zero, no length restriction is enforced.
 * 
 * If the bufferOffset is greater than the length of the given buffer,
 * the method will do nothing, and the delegate will not be called.
 * 
 * If you pass a buffer, you must not alter it in any way while AsyncSocket is using it.
 * After completion, the data returned in socket:didReadData:withTag: will be a subset of the given buffer.
 * That is, it will reference the bytes that were appended to the given buffer.
    @param NSTimeInterval
    @param NSMutableData
    @param NSUInteger
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataWithTimeout:(NSTimeInterval)timeout
                     buffer:(NSMutableData *)buffer // read buffer
               bufferOffset:(NSUInteger)offset
                  maxLength:(NSUInteger)length
                        tag:(long)tag;

/**
    @brief Reads the given number of bytes.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * 
 * If the length is 0, this method does nothing and the delegate is not called.
    @param NSUInteger
    @param NSTimeInterval
    @param long
    @return void
**/
- (void)readDataToLength:(NSUInteger)length withTimeout:(NSTimeInterval)timeout tag:(long)tag;

/**
    @brief Reads the given number of bytes.
 * The bytes will be appended to the given byte buffer starting at the given offset.
 * The given buffer will automatically be increased in size if needed.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * If the buffer if nil, a buffer will automatically be created for you.
 * 
 * If the length is 0, this method does nothing and the delegate is not called.
 * If the bufferOffset is greater than the length of the given buffer,
 * the method will do nothing, and the delegate will not be called.
 * 
 * If you pass a buffer, you must not alter it in any way while AsyncSocket is using it.
 * After completion, the data returned in socket:didReadData:withTag: will be a subset of the given buffer.
 * That is, it will reference the bytes that were appended to the given buffer.
    @param NSUInteger
    @param NSTimeInterval
    @param NSMutableData
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataToLength:(NSUInteger)length
             withTimeout:(NSTimeInterval)timeout
                  buffer:(NSMutableData *)buffer
            bufferOffset:(NSUInteger)offset
                     tag:(long)tag;

/**
    @brief Reads bytes until (and including) the passed "data" parameter, which acts as a separator.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * 
 * If you pass nil or zero-length data as the "data" parameter,
 * the method will do nothing, and the delegate will not be called.
 * 
 * To read a line from the socket, use the line separator (e.g. CRLF for HTTP, see below) as the "data" parameter.
 * Note that this method is not character-set aware, so if a separator can occur naturally as part of the encoding for a character, the read will prematurely end.
    @param NSData
    @param NSTimeInterval
    @param long
    @return void
**/
- (void)readDataToData:(NSData *)data withTimeout:(NSTimeInterval)timeout tag:(long)tag;

/**
    @brief Reads bytes until (and including) the passed "data" parameter, which acts as a separator.
 * The bytes will be appended to the given byte buffer starting at the given offset.
 * The given buffer will automatically be increased in size if needed.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * If the buffer if nil, a buffer will automatically be created for you.
 * 
 * If the bufferOffset is greater than the length of the given buffer,
 * the method will do nothing, and the delegate will not be called.
 * 
 * If you pass a buffer, you must not alter it in any way while AsyncSocket is using it.
 * After completion, the data returned in socket:didReadData:withTag: will be a subset of the given buffer.
 * That is, it will reference the bytes that were appended to the given buffer.
 * 
 * To read a line from the socket, use the line separator (e.g. CRLF for HTTP, see below) as the "data" parameter.
 * Note that this method is not character-set aware, so if a separator can occur naturally as part of the encoding for
 * a character, the read will prematurely end.
    @param NSdata
    @param NSTimeInterval
    @param NSMutableData
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataToData:(NSData *)data
           withTimeout:(NSTimeInterval)timeout
                buffer:(NSMutableData *)buffer
          bufferOffset:(NSUInteger)offset
                   tag:(long)tag;

/**
    @brief Reads bytes until (and including) the passed "data" parameter, which acts as a separator.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * 
 * If maxLength is zero, no length restriction is enforced.
 * Otherwise if maxLength bytes are read without completing the read,
 * it is treated similarly to a timeout - the socket is closed with a GCDAsyncSocketReadMaxedOutError.
 * The read will complete successfully if exactly maxLength bytes are read and the given data is found at the end.
 * 
 * If you pass nil or zero-length data as the "data" parameter,
 * the method will do nothing, and the delegate will not be called.
 * If you pass a maxLength parameter that is less than the length of the data parameter,
 * the method will do nothing, and the delegate will not be called.
 * 
 * To read a line from the socket, use the line separator (e.g. CRLF for HTTP, see below) as the "data" parameter.
 * Note that this method is not character-set aware, so if a separator can occur naturally as part of the encoding for
 * a character, the read will prematurely end.
    @param NSdata
    @param NSTimeInterval
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataToData:(NSData *)data withTimeout:(NSTimeInterval)timeout maxLength:(NSUInteger)length tag:(long)tag;

/**
    @brief Reads bytes until (and including) the passed "data" parameter, which acts as a separator.
 * The bytes will be appended to the given byte buffer starting at the given offset.
 * The given buffer will automatically be increased in size if needed.
 * 
 * If the timeout value is negative, the read operation will not use a timeout.
 * If the buffer if nil, a buffer will automatically be created for you.
 * 
 * If maxLength is zero, no length restriction is enforced.
 * Otherwise if maxLength bytes are read without completing the read,
 * it is treated similarly to a timeout - the socket is closed with a GCDAsyncSocketReadMaxedOutError.
 * The read will complete successfully if exactly maxLength bytes are read and the given data is found at the end.
 * 
 * If you pass a maxLength parameter that is less than the length of the data parameter,
 * the method will do nothing, and the delegate will not be called.
 * If the bufferOffset is greater than the length of the given buffer,
 * the method will do nothing, and the delegate will not be called.
 * 
 * If you pass a buffer, you must not alter it in any way while AsyncSocket is using it.
 * After completion, the data returned in socket:didReadData:withTag: will be a subset of the given buffer.
 * That is, it will reference the bytes that were appended to the given buffer.
 * 
 * To read a line from the socket, use the line separator (e.g. CRLF for HTTP, see below) as the "data" parameter.
 * Note that this method is not character-set aware, so if a separator can occur naturally as part of the encoding for
 * a character, the read will prematurely end.
    @param NSData
    @param NSTimeInterval
    @param NSMutableData
    @param NSUInteger
    @param NSUInteger
    @param long
    @return void
**/
- (void)readDataToData:(NSData *)data
           withTimeout:(NSTimeInterval)timeout
                buffer:(NSMutableData *)buffer
          bufferOffset:(NSUInteger)offset
             maxLength:(NSUInteger)length
                   tag:(long)tag;

#pragma mark Writing

/**
    @brief Writes data to the socket, and calls the delegate when finished.
 * 
 * If you pass in nil or zero-length data, this method does nothing and the delegate will not be called.
 * If the timeout value is negative, the write operation will not use a timeout.
    @param NSData
    @param NSTimeInterval
    @param long
    @return void
**/
- (void)writeData:(NSData *)data withTimeout:(NSTimeInterval)timeout tag:(long)tag;

///////////////////////////////////////////////////////////////////
#pragma mark Security
///////////////////////////////////////////////////////////////////

/**
    @brief Secures the connection using SSL/TLS.
 
    This method may be called at any time, and the TLS handshake will occur after all pending reads and writes are finished. This allows one the option of sending a protocol dependent StartTLS message, and queuing the upgrade to TLS at the same time, without having to wait for the write to finish.
    Any reads or writes scheduled after this method is called will occur over the secured connection.
 
 * The possible keys and values for the TLS settings are well documented.
 * Some possible keys are:
 * - kCFStreamSSLLevel
 * - kCFStreamSSLAllowsExpiredCertificates
 * - kCFStreamSSLAllowsExpiredRoots
 * - kCFStreamSSLAllowsAnyRoot
 * - kCFStreamSSLValidatesCertificateChain
 * - kCFStreamSSLPeerName
 * - kCFStreamSSLCertificates
 * - kCFStreamSSLIsServer
 * 
 * Please refer to Apple's documentation for associated values, as well as other possible keys.
 * 
 * If you pass in nil or an empty dictionary, the default settings will be used.
 * 
 * The default settings will check to make sure the remote party's certificate is signed by a
 * trusted 3rd party certificate agency (e.g. verisign) and that the certificate is not expired.
 * However it will not verify the name on the certificate unless you
 * give it a name to verify against via the kCFStreamSSLPeerName key.
 * The security implications of this are important to understand.
 * Imagine you are attempting to create a secure connection to MySecureServer.com,
 * but your socket gets directed to MaliciousServer.com because of a hacked DNS server.
 * If you simply use the default settings, and MaliciousServer.com has a valid certificate,
 * the default settings will not detect any problems since the certificate is valid.
 * To properly secure your connection in this particular scenario you
 * should set the kCFStreamSSLPeerName property to "MySecureServer.com".
 * If you do not know the peer name of the remote host in advance (for example, you're not sure
 * if it will be "domain.com" or "www.domain.com"), then you can use the default settings to validate the
 * certificate, and then use the X509Certificate class to verify the issuer after the socket has been secured.
 * The X509Certificate class is part of the CocoaAsyncSocket open source project.
    @param NSDictionary
    @return void
**/
- (void)startTLS:(NSDictionary *)tlsSettings;

/////////////////////////////////////////////////////////////////////
#pragma mark Advanced
/////////////////////////////////////////////////////////////////////

/**
    @brief It's not thread-safe to access certain variables from outside the socket's internal queue.
 * 
 * For example, the socket file descriptor.
 * File descriptors are simply integers which reference an index in the per-process file table.
 * However, when one requests a new file descriptor (by opening a file or socket),
 * the file descriptor returned is guaranteed to be the lowest numbered unused descriptor.
 * So if we're not careful, the following could be possible:
 * 
 * - Thread A invokes a method which returns the socket's file descriptor.
 * - The socket is closed via the socket's internal queue on thread B.
 * - Thread C opens a file, and subsequently receives the file descriptor that was previously the socket's FD.
 * - Thread A is now accessing/altering the file instead of the socket.
 * 
 * In addition to this, other variables are not actually objects,
 * and thus cannot be retained/released or even autoreleased.
 * An example is the sslContext, of type SSLContextRef, which is actually a malloc'd struct.
 * 
 * Although there are internal variables that make it difficult to maintain thread-safety,
 * it is important to provide access to these variables
 * to ensure this class can be used in a wide array of environments.
 * This method helps to accomplish this by invoking the current block on the socket's internal queue.
 * The methods below can be invoked from within the block to access
 * those generally thread-unsafe internal variables in a thread-safe manner.
 * The given block will be invoked synchronously on the socket's internal queue.
 * 
 * If you save references to any protected variables and use them outside the block, you do so at your own peril.
    @param dispatch_block_t
    @return void
**/
- (void)performBlock:(dispatch_block_t)block;

/**
    @brief These methods are only available from within the context of a performBlock: invocation.
 * See the documentation for the performBlock: method above.
 * 
 * Provides access to the socket's file descriptor(s).
 * If the socket is a server socket (is accepting incoming connections),
 * it might actually have multiple internal socket file descriptors - one for IPv4 and one for IPv6.
    @return int
**/
- (int)socketFD;

/**
    @brief Gets the IP version 4 socket file descriptor
    @return int
**/
- (int)socket4FD;

/**
    @brief Gets the IP version 6 socket file descriptor
    @return int
**/
- (int)socket6FD;

#if TARGET_OS_IPHONE

/**
    @brief These methods are only available from within the context of a performBlock: invocation.
 * See the documentation for the performBlock: method above.
 * 
 * Provides access to the socket's internal read/write streams.
 * These streams are normally only created if startTLS has been invoked to start SSL/TLS (see note below),
 * but if these methods are invoked, the read/write streams will be created automatically so that you may use them.
 * 
 * See also: (BOOL)enableBackgroundingOnSocket
 * 
 * Note: Apple has decided to keep the SecureTransport framework private is iOS.
 * This means the only supplied way to do SSL/TLS is via CFStream or some other API layered on top of it.
 * Thus, in order to provide SSL/TLS support on iOS we are forced to rely on CFStream,
 * instead of the preferred and faster and more powerful SecureTransport.
    @return CFReadStreamRef
**/
- (CFReadStreamRef)readStream;

/**
    @brief Gets the write stream reference
    @return CFWriteStreamRef
**/
- (CFWriteStreamRef)writeStream;

/**
    @brief This method is only available from within the context of a performBlock: invocation.
 * See the documentation for the performBlock: method above.
 * 
 * Configures the socket to allow it to operate when the iOS application has been backgrounded.
 * In other words, this method creates a read & write stream, and invokes:
 * 
 * CFReadStreamSetProperty(readStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
 * CFWriteStreamSetProperty(writeStream, kCFStreamNetworkServiceType, kCFStreamNetworkServiceTypeVoIP);
 * 
 * Returns YES if successful, NO otherwise.
 * 
 * Note: Apple does not officially support backgrounding server sockets.
 * That is, if your socket is accepting incoming connections, Apple does not officially support
 * allowing iOS applications to accept incoming connections while an app is backgrounded.
 * 
 * Example usage:
 * 
 * - (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port
 * {
 *     [asyncSocket performBlock:^{
 *         [asyncSocket enableBackgroundingOnSocket];
 *     }];
 * }
    @return BOOL
**/
- (BOOL)enableBackgroundingOnSocket;

/**
    @brief This method is only available from within the context of a performBlock: invocation.
 * See the documentation for the performBlock: method above.
 * 
 * This method should be used in place of the usual enableBackgroundingOnSocket method if
 * you later plan on securing the socket with SSL/TLS via the startTLS method.
 * 
 * This is due to a bug in iOS. Description of the bug:
 * 
 * First of all, Apple has decided to keep the SecureTransport framework private in iOS.
 * This removes the preferred, faster, and more powerful way of doing SSL/TLS.
 * The only option they have given us on iOS is to use CFStream.
 * 
 * In addition to this, Apple does not allow us to enable SSL/TLS on a stream after it has been opened.
 * This effectively breaks many newer protocols which negotiate upgrades to TLS in-band (such as XMPP).
 * 
 * And on top of that, if we flag a socket for backgrounding, that flag doesn't take effect until
 * after we have opened the socket. And if we try to flag the socket for backgrounding after we've opened
 * the socket, the flagging fails.
 * 
 * So the order of operations matters, and the ONLY order that works is this:
 * 
 * - Create read and write stream
 * - Mark streams for backgrounding
 * - Setup SSL on streams
 * - Open streams
 * 
 * So the caveat is that this method will mark the socket for backgrounding,
 * but it will not open the read and write streams. (Because if it did, later attempts to start TLS would fail.)
 * Thus the socket will not actually support backgrounding until after the startTLS method has been called.
    @return BOOL
**/
- (BOOL)enableBackgroundingOnSocketWithCaveat;

#else

/**
    @brief This method is only available from within the context of a performBlock: invocation.
 * See the documentation for the performBlock: method above.
 * 
 * Provides access to the socket's SSLContext, if SSL/TLS has been started on the socket.
    @return SSLContextRef
**/
- (SSLContextRef)sslContext;

#endif

/////////////////////////////////////////////////////////////////////
#pragma mark Utilities
/////////////////////////////////////////////////////////////////////


/***************************************************************
 * Extracting host and port information from raw address data.
*****************************************************************/

/**
    Class method
    @brief Gets the host from an address
    @param NSData
    @return NSString
**/
+ (NSString *)hostFromAddress:(NSData *)address;

/**
    Class method
    @brief Get the port from an address
    @param NSData
    @return UInt16
**/
+ (UInt16)portFromAddress:(NSData *)address;

/**
    Class method
    @param NSString (pointer to a pointer)
    @param UInt16
    @param NSData
    @return BOOL
**/
+ (BOOL)getHost:(NSString **)hostPtr port:(UInt16 *)portPtr fromAddress:(NSData *)address;

/**
 * A few common line separators, for use with the readDataToData:... methods.
**/

/**
    Class method
    @return NSData
**/
+ (NSData *)CRLFData;   // 0x0D0A - Carriage return and line feed

/**
    Class method
    @return NSData
**/
+ (NSData *)CRData;     // 0x0D  - Carriage return

/**
    Class method
    @return NSData
**/
+ (NSData *)LFData;     // 0x0A - line feed

/**
    Class method
    @return NSData
**/
+ (NSData *)ZeroData;   // 0x00 - empty NSData object

@end

///////////////////////////////////////////////////////////////////////
#pragma mark -
///////////////////////////////////////////////////////////////////////

/**
    @brief Protocol for implementing a GCDAsyncSocket as a delegate
**/
@protocol GCDAsyncSocketDelegate
@optional

/**
    @brief This method is called immediately prior to socket:didAcceptNewSocket:.
 * It optionally allows a listening socket to specify the socketQueue for a new accepted socket.
 * If this method is not implemented, or returns NULL, the new accepted socket will create its own default queue.
 * 
 * Since you cannot autorelease a dispatch_queue,
 * this method uses the "new" prefix in its name to specify that the returned queue has been retained.
 * 
 * Thus you could do something like this in the implementation:
 * return dispatch_queue_create("MyQueue", NULL);
 * 
 * If you are placing multiple sockets on the same queue,
 * then care should be taken to increment the retain count each time this method is invoked.
 * 
 * For example, your implementation might look something like this:
 * dispatch_retain(myExistingQueue);
 * return myExistingQueue;
    @param NSData
    @param GCDAsyncSocket
    @return dispatch_queue_t
**/
- (dispatch_queue_t)newSocketQueueForConnectionFromAddress:(NSData *)address onSocket:(GCDAsyncSocket *)sock;

/**
    @brief Called when a socket accepts a connection.
 * Another socket is automatically spawned to handle it.
 * 
 * You must retain the newSocket if you wish to handle the connection.
 * Otherwise the newSocket instance will be released and the spawned connection will be closed.
 * 
 * By default the new socket will have the same delegate and delegateQueue.
 * You may, of course, change this at any time.
    @param GCDAsyncSocket
    @param GCDAsyncSocket
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket;

/**
    @brief Called when a socket connects and is ready for reading and writing.
 * The host parameter will be an IP address, not a DNS name.
    @param GCDAsyncSocket
    @param NSString
    @param UInt16
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didConnectToHost:(NSString *)host port:(UInt16)port;

/**
    @brief Called when a socket has completed reading the requested data into memory.
    Not called if there is an error.
    @param GCDAsyncSocket
    @param NSData
    @param long
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didReadData:(NSData *)data withTag:(long)tag;

/**
    @brief Called when a socket has read in data, but has not yet completed the read.
 * This would occur if using readToData: or readToLength: methods.
 * It may be used to for things such as updating progress bars.
    @param GCDAsyncSocket
    @param NSUInteger
    @param long
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didReadPartialDataOfLength:(NSUInteger)partialLength tag:(long)tag;

/**
    @brief Called when a socket has completed writing the requested data. Not called if there is an error.
    @param GCDAsyncSocket
    @param long
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didWriteDataWithTag:(long)tag;

/**
    @brief Called when a socket has written some data, but has not yet completed the entire write.
 * It may be used to for things such as updating progress bars.
    @param GCDAsyncSocket
    @param NSUInteger
    @param long
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didWritePartialDataOfLength:(NSUInteger)partialLength tag:(long)tag;

/**
    @brief Called if a read operation has reached its timeout without completing.
 * This method allows you to optionally extend the timeout.
 * If you return a positive time interval (> 0) the read's timeout will be extended by the given amount.
 * If you don't implement this method, or return a non-positive time interval (<= 0) the read will timeout as usual.
 * 
 * The elapsed parameter is the sum of the original timeout, plus any additions previously added via this method.
 * The length parameter is the number of bytes that have been read so far for the read operation.
 * 
 * Note that this method may be called multiple times for a single read if you return positive numbers.
    @param GCDAsyncSocket
    @param long
    @param NSTimeInterval
    @param NSUInteger
    @return NSTimeInterval
**/
- (NSTimeInterval)socket:(GCDAsyncSocket *)sock shouldTimeoutReadWithTag:(long)tag
            elapsed:(NSTimeInterval)elapsed
            bytesDone:(NSUInteger)length;

/**
    @brief Called if a write operation has reached its timeout without completing.
 
    This method allows you to optionally extend the timeout.
 
    If you return a positive time interval (> 0) the write's timeout will be extended by the given amount.
 
    If you don't implement this method, or return a non-positive time interval (<= 0) the write will timeout as usual.
 
    The elapsed parameter is the sum of the original timeout, plus any additions previously added via this method.
 
    The length parameter is the number of bytes that have been written so far for the write operation.

    Note that this method may be called multiple times for a single write if you return positive numbers.
 
    @param GCDAsyncSocket
    @param long
    @param NSTimeInterval
    @param NSUInteger
    @return NSTimeInterval
**/
- (NSTimeInterval)socket:(GCDAsyncSocket *)sock shouldTimeoutWriteWithTag:(long)tag
        elapsed:(NSTimeInterval)elapsed
        bytesDone:(NSUInteger)length;

/**
    @brief Conditionally called if the read stream closes, but the write stream may still be writeable.

    This delegate method is only called if autoDisconnectOnClosedReadStream has been set to NO.
    See the discussion on the autoDisconnectOnClosedReadStream method for more information.
    @param GCDAsyncSocket
    @return void
**/
- (void)socketDidCloseReadStream:(GCDAsyncSocket *)sock;

/**
    @brief Called when a socket disconnects with or without error.
 
    If you call the disconnect method, and the socket wasn't already disconnected, this delegate method will be called before the disconnect method returns.
    @param GCDAsyncSocket
    @param NSError
    @return void
**/
- (void)socketDidDisconnect:(GCDAsyncSocket *)sock withError:(NSError *)err;

/**
    @brief Called after the socket has successfully completed SSL/TLS negotiation.
 
    This method is not called unless you use the provided startTLS method.
 
    If a SSL/TLS negotiation fails (invalid certificate, etc) then the socket will immediately close, and the socketDidDisconnect:withError: delegate method will be called with the specific SSL error code.
    @param GCDAsyncSocket
    @return void
**/
- (void)socketDidSecure:(GCDAsyncSocket *)sock;

@end
