#import "HTTPServer.h"
#import "GCDAsyncSocket.h"
#import "HTTPConnection.h"
#import "WebSocket.h"

/**
    HTTPServer (PrivateAPI)
**/
@interface HTTPServer (PrivateAPI)

/**
    @brief Unpublish the bonjour from the list of published network services
    @return void
**/
- (void)unpublishBonjour;


/**
    @brief Publish as a list of network services
    @return void
**/
- (void)publishBonjour;


/**
    @brief Starts the bonjour thread if needed
    @return void
**/
+ (void)startBonjourThreadIfNeeded;

/**
    @brief Performs a block of code on the bonjour thread
    @param dispatch_block_t
    @param BOOL
    @return void
**/
+ (void)performBonjourBlock:(dispatch_block_t)block waitUntilDone:(BOOL)waitUntilDone;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    HTTP Server
**/
@implementation HTTPServer

/**
    @brief Standard Constructor.
 
    Instantiates an HTTP server, but does not start it.
    @return id
**/
- (id)init
{
	if ((self = [super init]))
	{
		
		// Initialize underlying dispatch queue and GCD based tcp socket
		serverQueue = dispatch_queue_create("HTTPServer", NULL);
        
        // create an asynchronous socket and initialize with the HTTPServer as the delegate, and the serverQueue as the delegate queue.  This means the HTTPServer will be handling all request for the socket, and the serverQueue will be the designated queue
		asyncSocket = [[GCDAsyncSocket alloc] initWithDelegate:self delegateQueue:serverQueue];
		
        
		// Use default connection class of HTTPConnection.  
		connectionQueue = dispatch_queue_create("HTTPConnection", NULL);

		// Set the connectionClass to self (i.e. HTTPServer)
        connectionClass = [HTTPConnection self];
		
		// By default bind on all available interfaces, en1, wifi etc
		interface = nil;
		
		// Use a default port of 0
		// This will allow the kernel to automatically pick an open port for us
		port = 0;
		
		// Configure default values for bonjour service
		
		// Bonjour domain. Use the local domain by default. We are setting the bonjour domain for peer-to-peer connections
		domain = @"local.";
		
		// If using an empty string ("") for the service name when registering,
		// the system will automatically use the "Computer Name".
		// Passing in an empty string will also handle name conflicts
		// by automatically appending a digit to the end of the name.
		name = @"";
		
		// Initialize arrays to hold all the normal connections
		connections = [[NSMutableArray alloc] init];
        
        // Initialize array to hold the web socket connections
		webSockets  = [[NSMutableArray alloc] init];
		
        
        // Initialize locks for the normal connections
		connectionsLock = [[NSLock alloc] init];
        
        // Initialize locks for websocket connections
		webSocketsLock  = [[NSLock alloc] init];
        
        
		// Register for notifications of closed connections.  This will notify the HTTPServer (i.e. self) when an http connection dies
		[[NSNotificationCenter defaultCenter] addObserver:self
                    selector:@selector(connectionDidDie:)
                    name:HTTPConnectionDidDieNotification
                    object:nil];
		
		// Register for notifications of closed websocket connections.  This will notify the HTTPServer (i.e. self) when a web socket dies
		[[NSNotificationCenter defaultCenter] addObserver:self
                    selector:@selector(webSocketDidDie:)
                    name:WebSocketDidDieNotification
                    object:nil];
        
        // Note: Just initialized the HTTPServer, have not started it yet
		isRunning = NO;
	}
	return self;
}

/**
    @brief Standard Deconstructor.
 
    Stops the server, and clients, and releases any resources connected with this instance.
    @return void
**/
- (void)dealloc
{
	
	// Remove notification observer
	[[NSNotificationCenter defaultCenter] removeObserver:self];
	
	// Stop the server if it's running
	[self stop];
	
	// Release all instance variables
	
	dispatch_release(serverQueue);
	dispatch_release(connectionQueue);
	
	[asyncSocket setDelegate:nil delegateQueue:NULL];
	[asyncSocket release];
	
	[documentRoot release];
	[interface release];
	
	[netService release];
	[domain release];
	[name release];
	[type release];
	[txtRecordDictionary release];
	
	[connections release];
	[webSockets release];
	[connectionsLock release];
	[webSocketsLock release];
	
	[super dealloc];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Server Configuration
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief The document root is filesystem root for the webserver.
 
    Thus requests for /index.html will be referencing the index.html file within the document root directory.
    All file requests are relative to this document root.
    @return NSString
**/
- (NSString *)documentRoot
{
    
	__block NSString *result;
	
    // Submits a block for synchronous execution on the serverQueue
    // This block increases the reference count for the documentRoot
	dispatch_sync(serverQueue, ^{
        
		result = [documentRoot retain];
        
	}); // END OF BLOCK
	
    // Returns the documentRoot and autoreleases it after returning to the caller
	return [result autorelease];
}

/**
    @brief Set the documents root
    @param NSString
    @return void
**/
- (void)setDocumentRoot:(NSString *)value
{
	
	// Document root used to be of type NSURL.
	// Add type checking for early warning to developers upgrading from older versions.
	if (value && ![value isKindOfClass:[NSString class]])
	{
		return;
	}
    
	// Creates a local attributes and makes a copy of the document root
	NSString *valueCopy = [value copy];
	
    
    // Submits a block for asynchronous execution on the serverQueue.  This block sets the servers document root
	dispatch_async(serverQueue, ^{
        
		[documentRoot release];
        
		documentRoot = [valueCopy retain];
        
	}); // END OF BLOCK
	
	[valueCopy release];
}

/**
    @brief The connection class is the class that will be used to handle connections.
 
    That is, when a new connection is created, an instance of this class will be intialized.
    The default connection class is HTTPConnection.
    If you use a different connection class, it is assumed that the class extends HTTPConnection
    @return Class
**/
- (Class)connectionClass
{
    // Creates a local attributed
	__block Class result;
	
    // Submits a block for synchronous execution on the serverQueue.  This block gets the connection class
	dispatch_sync(serverQueue, ^{
        
		result = connectionClass;
        
	}); // END OF BLOCK
	
	return result;
}

/**
    @brief Sets the connection class on the serverQueue
    @param Class
    @return void
**/
- (void)setConnectionClass:(Class)value
{
	// Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
        
		connectionClass = value;
        
	}); // END OF BLOCK
}

/**
    @brief What interface to bind the listening socket to.
    @return NSString
**/
- (NSString *)interface
{
	__block NSString *result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = [interface retain];
        
	}); // END OF BLOCK
	
	return [result autorelease];
}


/**
    @brief Set the server interface
    @param NSString
    @return void
**/
- (void)setInterface:(NSString *)value
{
    // copies the value into a local attribute
	NSString *valueCopy = [value copy];
	
    // Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
        
		[interface release];
		interface = [valueCopy retain];
        
	}); // END OF BLOCK
	
	[valueCopy release];
}

/**
    @brief The port to listen for connections on.
 
    By default this port is initially set to zero, which allows the kernel to pick an available port for us.
    After the HTTP server has started, the port being used may be obtained by this method.
    @return UInt16
**/
- (UInt16)port
{
    // Creates a local attribute
	__block UInt16 result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = port;
        
	}); // END OF BLOCK
	
    return result;
}

/**
    @brief Get the servers listening port by dispatching q block on the serverQueue
    @return UInt16
**/
- (UInt16)listeningPort
{
    // Creates a local attribute
	__block UInt16 result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		if (isRunning)  // check if server is running
        {
            // Get the sockets local port
			result = [asyncSocket localPort];
            
		}else{ // if server is not running
            
            
			result = 0;
        }
        
	}); // END OF BLOCK
	
	return result;
}

/**
    @brief Set the servers port
    @param UInt16
    @return void
**/
- (void)setPort:(UInt16)value
{
	// Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
        
		port = value;
        
	}); //END OF BLOCK
}

/**
    @brief Gets the domain on which to broadcast this service via Bonjour.
 
    The default domain is @"local".
    @return NSString
**/
- (NSString *)domain
{
    // Creates a local attribute
	__block NSString *result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = [domain retain];
        
	}); // END OF BLOCK
	
    return [domain autorelease];
}


/**
    @brief Set the domain on which to broadcast this service
    @param NSString
    @return void
**/
- (void)setDomain:(NSString *)value
{
	// Copies the value into a local attribute
	NSString *valueCopy = [value copy];
	
    // Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{

		[domain release];
		domain = [valueCopy retain];
        
	}); // END OF BLOCK
	
	[valueCopy release];
}

/**
    @brief Gets the name to use for this service via Bonjour.

    The default name is an empty string, which should result in the published name being the host name of the computer.
    @return NSString
**/
- (NSString *)name
{
    // Creates a local attribute
	__block NSString *result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = [name retain];
        
	}); //END OF BLOCK
	
	return [name autorelease];
}


/**
    @brief Gets the published name of the server
    @return NSString
**/
- (NSString *)publishedName
{
    // Creates a local attribute
	__block NSString *result;
	
    
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
		
        // If there is a network service
		if (netService == nil)
		{
			result = nil;
		}
		else // if there is not a network service 
		{
			
            // The prototype of blocks submitted to dispatch queues, which take no arguments and have no return value.
			dispatch_block_t bonjourBlock = ^{
                
				result = [[netService name] copy];
                
			}; // END OF BLOCK
			
            // Executes the bonjour block and waits for it to complete 
			[[self class] performBonjourBlock:bonjourBlock waitUntilDone:YES];
		}
	}); // END OF BLOCK
	
	return [result autorelease];
}


/**
    @brief Sets the published name of the server
    @param NSString
    @return void
**/
- (void)setName:(NSString *)value
{
    // Copies the value into a local attribute
	NSString *valueCopy = [value copy];
	
    // Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
        
		[name release];
		name = [valueCopy retain];
        
	}); // END OF BLOCK
	
	[valueCopy release];
}

/**
    @brief Gets the type of service to publish via Bonjour.
 
    No type is set by default, and one must be set in order for the service to be published.
    @return NSString
**/
- (NSString *)type
{
    // Creates a local attribute
	__block NSString *result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = [type retain];
        
	}); // END OF BLOCK
	
	return [result autorelease];
}

/**
    @brief Set the type of service to be published via Bonjour
    @param NSString
    @return void
**/
- (void)setType:(NSString *)value
{
    // Copies the value into a local attribute
	NSString *valueCopy = [value copy];
	
    // Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
        
		[type release];
		type = [valueCopy retain];
        
	}); // END OF BLOCK
	
	[valueCopy release];
}

/**
    @brief Gets the TXTRecordDictionary
 
    The extra data to use for this service via Bonjour.
    @return NSDictionary
**/
- (NSDictionary *)TXTRecordDictionary
{
    // Creates a local attribute
	__block NSDictionary *result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
        
		result = [txtRecordDictionary retain];
        
	}); // END OF BLOCK
	
	return [result autorelease];
}

/**
    @brief Sets the TXT record dictionary
    @param NSDictionary
    @return void
**/
- (void)setTXTRecordDictionary:(NSDictionary *)value
{
	// Copies the value into a local attribute
	NSDictionary *valueCopy = [value copy];
	
    
    // Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
	
		[txtRecordDictionary release];
		txtRecordDictionary = [valueCopy retain];
		
		// Update the txtRecord of the netService if it has already been published
		if (netService)
		{
            // Create a local attribute for the netService
			NSNetService *theNetService = netService;
			NSData *txtRecordData = nil;
            
            // If there is a textRecordDictionary
			if (txtRecordDictionary)
            {
                // Gets the data from the dictionary
				txtRecordData = [NSNetService dataFromTXTRecordDictionary:txtRecordDictionary];
			}
            
            // The prototype of blocks submitted to dispatch queues, which take no arguments and have no return value.
			dispatch_block_t bonjourBlock = ^{
                
                // Sets the network service text record data
				[theNetService setTXTRecordData:txtRecordData];
                
			}; // END OF BLOCK
			
            // Perform the bonjourBlock and don't wait for it to execute
			[[self class] performBonjourBlock:bonjourBlock waitUntilDone:NO];
		}
	}); // END OF BLOCK
	
	[valueCopy release];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Server Control
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


/**
    @brief Starts the server
    @param NSError
    @return BOOL
**/
- (BOOL)start:(NSError **)errPtr
{
	// Creates local attributes
	__block BOOL success = YES;
	__block NSError *err = nil;
	
    // Submits a block for synchronous execution on serverQueue
	dispatch_sync(serverQueue, ^{
        
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		
        // if the socket accepts on an interface and port
		success = [asyncSocket acceptOnInterface:interface port:port error:&err];
        
        // If the socket accepts on a specific interface and port
		if (success)
		{
            // flag that the server is running
			isRunning = YES;
            
            // publish the server as a network service
			[self publishBonjour];
		}
		else // if server does not accept connections on a specific interface and port then thow and error
		{
			[err retain];
		}
		
		[pool release];
	});  // END OF BLOCK
	
    // If there is an error pointer
	if (errPtr)
    {
		*errPtr = [err autorelease];
	}else{ // if there is not an error
		[err release];
	}
    
    // Return that the server is running
	return success;
}


/**
    @brief Stops the server
    @return BOOL
**/
- (BOOL)stop
{
	// Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
		NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
		
		// First stop publishing the service via bonjour
		[self unpublishBonjour];
		
		// Stop listening / accepting incoming connections
		[asyncSocket disconnect];
		isRunning = NO;
		
		// Now stop all HTTP connections the server owns
		[connectionsLock lock];
        
        // Enumerates through the HTTP connections and stops them
		for (HTTPConnection *connection in connections)
		{
            // Stops the connection
			[connection stop];
		}
        
        // Remove all the connections
		[connections removeAllObjects];
		[connectionsLock unlock];
		
		// Now stop all WebSocket connections the server owns
		[webSocketsLock lock];
        
        // Enumerate through the web sockets
		for (WebSocket *webSocket in webSockets)
		{
			[webSocket stop];
		}
        // remove all the web sockets
		[webSockets removeAllObjects];
		[webSocketsLock unlock];
		
		[pool release];
	}); // END OF BLOCK
	
	return YES;
}


/**
    @brief Whether the HTTP server is running
    @return BOOL
**/
- (BOOL)isRunning
{
    // Creates a local attribute
	__block BOOL result;
	
    // Submits a block for synchronous execution on the serverQueue
	dispatch_sync(serverQueue, ^{
		result = isRunning;
	}); // END OF BLOCK
	
	return result;
}


/**
    @brief Adds a web sockets to the webSockets array
    @param WebSocket
    @return void
**/
- (void)addWebSocket:(WebSocket *)ws
{
    // Locks the web socker
	[webSocketsLock lock];
    
    // Adds a web socket
	[webSockets addObject:ws];
    
    // Unlocks the web socket
	[webSocketsLock unlock];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Server Status
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Returns the number of http client connections that are currently connected to the server.
    @return NSUInteger
**/
- (NSUInteger)numberOfHTTPConnections
{
    
	NSUInteger result = 0;
	
    // Locks the connection
	[connectionsLock lock];
    
    // Gets the count of connections to the this server
	result = [connections count];
    
    // Unlocks the connection
	[connectionsLock unlock];
	
	return result;
}

/**
    @brief Returns the number of websocket client connections that are currently connected to the server.
    @return NSUInteger
**/
- (NSUInteger)numberOfWebSocketConnections
{
    // Local attribute
	NSUInteger result = 0;
	
    // Locks the web socket
	[webSocketsLock lock];
    
    // Gets the count of web socket connections
	result = [webSockets count];
    
    // Unlocks the web socket
	[webSocketsLock unlock];
	
	return result;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Incoming Connections
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Configures the server
    @return HTTPConfig
**/
- (HTTPConfig *)config
{
	// Override me if you want to provide a custom config to the new connection.
	// 
	// Generally this involves overriding the HTTPConfig class to include any custom settings,
	// and then having this method return an instance of 'MyHTTPConfig'.
	
	// Note: Think you can make the server faster by putting each connection on its own queue?
	// Then benchmark it before and after and discover for yourself the shocking truth!
	// 
	// Try the apache benchmark tool (already installed on your Mac):
	// $  ab -n 1000 -c 1 http://localhost:<port>/some_path.html
	
	return [[[HTTPConfig alloc] initWithServer:self documentRoot:documentRoot queue:connectionQueue] autorelease];
}


/**
    @brief When the server accepts a new socket
    @param GCDAsyncSocket
    @param GCDAsyncSocket
    @return void
**/
- (void)socket:(GCDAsyncSocket *)sock didAcceptNewSocket:(GCDAsyncSocket *)newSocket
{
    // Create a new HTTP connection
	HTTPConnection *newConnection = (HTTPConnection *)[[connectionClass alloc] initWithAsyncSocket:newSocket
                    configuration:[self config]];
    
    // Locks the connections
	[connectionsLock lock];
    
    // Adds a new connection
	[connections addObject:newConnection];
    
    // Unlocks the connection
	[connectionsLock unlock];
	
    // Start the new connection
	[newConnection start];
	[newConnection release];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Bonjour
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Publish the bonjour net service
    @return void
**/
- (void)publishBonjour
{
	// Test whether the current queue is the serverQueue
	NSAssert(dispatch_get_current_queue() == serverQueue, @"Invalid queue");
	
    // If there is a type
	if (type)
	{
        // Create a new net service with a domain, type, name and port
		netService = [[NSNetService alloc] initWithDomain:domain type:type name:name port:[asyncSocket localPort]];
        
        // set the net services' delegate as this instance of the HTTPServer
		[netService setDelegate:self];
		
        // Gets the netService
		NSNetService *theNetService = netService;
        
        
		NSData *txtRecordData = nil;
        
        // If there is a txtRecordDictionary
		if (txtRecordDictionary)
        {
            
            // Gets the text record dictionary from the network service
			txtRecordData = [NSNetService dataFromTXTRecordDictionary:txtRecordDictionary];
		}
        
        // The prototype of blocks submitted to dispatch queues, which take no arguments and have no return value.
		dispatch_block_t bonjourBlock = ^{
			
            // Removess the service to the specified run loop.
			[theNetService removeFromRunLoop:[NSRunLoop mainRunLoop] forMode:NSRunLoopCommonModes];
            
            // Adds the service to the specified run loop.
			[theNetService scheduleInRunLoop:[NSRunLoop currentRunLoop] forMode:NSRunLoopCommonModes];
            
            // Attempts to advertise the receiver’s on the network.
			[theNetService publish];
			
			// Do not set the txtRecordDictionary prior to publishing!!!
			// This will cause the OS to crash!!!
			if (txtRecordData)
			{
                
                // Sets the TXT record for the receiver, and returns a Boolean value that indicates whether the operation was successful.
				[theNetService setTXTRecordData:txtRecordData];
			}
		}; // END OF BLOCK
		
        // Start the thread and run the block
		[[self class] startBonjourThreadIfNeeded];
        
        // Executes the block
		[[self class] performBonjourBlock:bonjourBlock waitUntilDone:NO];
	}
}

/**
    @brief Unpublish the Bonjour service
    @return void
**/
- (void)unpublishBonjour
{
	// Test whether the current queue is the serverQueue
	NSAssert(dispatch_get_current_queue() == serverQueue, @"Invalid queue");
	
    // Test if there is a network service
	if (netService)
	{
		NSNetService *theNetService = netService;
		
        
        // The prototype of blocks submitted to dispatch queues, which take no arguments and have no return value.
		dispatch_block_t bonjourBlock = ^{
		
            // Halts a currently running attempt to publish or resolve a service.
			[theNetService stop];
            
            
			[theNetService release];
		}; // END OF BLOCK
		
        
        // Performs the bonjourBlock and do no wait for it to execute
		[[self class] performBonjourBlock:bonjourBlock waitUntilDone:NO];
		
		netService = nil;
	}
}

/**
    @brief Republishes the service via bonjour if the server is running.
 
    If the service was not previously published, this method will publish it (if the server is running).
    @return void
**/
- (void)republishBonjour
{
	// Submits a block for asynchronous execution on the serverQueue
	dispatch_async(serverQueue, ^{
		
        
		[self unpublishBonjour]; 
		[self publishBonjour];
	}); // END OF BLOCK
}

/**
    Called when our bonjour service has been successfully published.
 
    This method does nothing but output a log message telling us about the published service.
    @param NSNetService
    @return void
**/
- (void)netServiceDidPublish:(NSNetService *)ns
{
	// Override me to do something here...
	// 
	// Note: This method is invoked on our bonjour thread.
	
}

/**
    @brief Called if our bonjour service failed to publish itself.
 
    This method does nothing but output a log message telling us about the published service.
    @param NSNetService
    @param NSDictionary
    @return void
**/
- (void)netService:(NSNetService *)ns didNotPublish:(NSDictionary *)errorDict
{
	// Override me to do something here...
	// 
	// Note: This method in invoked on our bonjour thread.
	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Notifications
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief This method is automatically called when a notification of type HTTPConnectionDidDieNotification is posted.
 
    It allows us to remove the connection from our array.
    @param NSNotification
    @return void
**/
- (void)connectionDidDie:(NSNotification *)notification
{
	// Note: This method is called on the connection queue that posted the notification
	
    // Locks the http connection
	[connectionsLock lock];
	
    // removes the connections from the array of connections
	[connections removeObject:[notification object]];
	
    // unloack the connection
	[connectionsLock unlock];
}

/**
    @brief This method is automatically called when a notification of type WebSocketDidDieNotification is posted.
    It allows us to remove the websocket from our array.
    @param NSNotification
    @return void
**/
- (void)webSocketDidDie:(NSNotification *)notification
{
	// Note: This method is called on the connection queue that posted the notification
	
    // Locks the websocket
	[webSocketsLock lock];
	
    // Removes the websocket from the array of web sockets
	[webSockets removeObject:[notification object]];
	
    // Unlock the web socket
	[webSocketsLock unlock];
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark Bonjour Thread
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief NSNetService is runloop based, so it requires a thread with a runloop.
    This gives us two options:
    
    - Use the main thread
    - Setup our own dedicated thread
    
    Since we have various blocks of code that need to synchronously access the netservice objects, using the main thread becomes troublesome and a potential for deadlock.
**/
static NSThread *bonjourThread;


/**
    Class method
    @brief Start the bonjour thread
    @return void
**/
+ (void)startBonjourThreadIfNeeded
{
	//  A predicate for use with dispatch_once(). It must be initialized to zero.
	static dispatch_once_t predicate;
    
    // Execute a block once and only once
	dispatch_once(&predicate, ^{
		
        // Initialize and allocate a bonjour thread
		bonjourThread = [[NSThread alloc] initWithTarget:self
		                                        selector:@selector(bonjourThread)
		                                          object:nil];
        
        // Start the bonjour thread
		[bonjourThread start];
        
	}); // END OF BLOCK
}


/**
    Class method
    @return void
**/
+ (void)bonjourThread
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	
	
	// We can't run the run loop unless it has an associated input source or a timer.
	// So we'll just create a timer that will never fire - unless the server runs for 10,000 years.
	
	[NSTimer scheduledTimerWithTimeInterval:DBL_MAX target:self selector:@selector(ignore:) userInfo:nil repeats:YES];
	
    // Returns the NSRunLoop object for the current thread.
    // Puts the receiver into a permanent loop, during which time it processes data from all attached input sources.
	[[NSRunLoop currentRunLoop] run];
		
	[pool release];
}


/**
    Class method
    
    @brief Executes the block of code on the bonjour thread
    @param dispatch_block_t
    @return void
**/
+ (void)performBonjourBlock:(dispatch_block_t)block
{
	// Test whether the current thread is the bonjourThread
	NSAssert([NSThread currentThread] == bonjourThread, @"Executed on incorrect thread");
	
    // Executes the block passed in to this method
	block();
}


/**
    Class method
    @brief Executes a block on the bonjour thread
    @param dispatch_block_t
    @param BOOL
    @return void
**/
+ (void)performBonjourBlock:(dispatch_block_t)block waitUntilDone:(BOOL)waitUntilDone
{
	
    // The prototype of blocks submitted to dispatch queues, which take no arguments and have no return value.
	dispatch_block_t bonjourBlock = Block_copy(block);
	
    // Executes the block on the bonjour thread
	[self performSelector:@selector(performBonjourBlock:)
	             onThread:bonjourThread
	           withObject:bonjourBlock
	        waitUntilDone:waitUntilDone];
	
    
	Block_release(bonjourBlock);
}

@end
