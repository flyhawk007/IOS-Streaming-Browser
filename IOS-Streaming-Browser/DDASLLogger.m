#import "DDASLLogger.h"

// Includes a group of functions for atomic reading and updating of values.
#import <libkern/OSAtomic.h>


@implementation DDASLLogger



static DDASLLogger *sharedInstance;

/**
    @brief Initialize the DDASLLogger
    The runtime sends initialize to each class in a program exactly one time just before the class, or any class that inherits from it, is sent its first message from within the program. (Thus the method may never be invoked if the class is not used.) The runtime sends the initialize message to classes in a thread-safe manner. Superclasses receive this message before their subclasses.
 
    This method may also be called directly (assumably by accident), hence the safety mechanism.
    @return void
**/
+ (void)initialize
{
    // Whether the DDASLLogger is initialized
	static BOOL initialized = NO;
	if (!initialized)
	{
		initialized = YES;
		
		sharedInstance = [[DDASLLogger alloc] init];
	}
}

/**
    Class method
    @return DDASLLogger
**/
+ (DDASLLogger *)sharedInstance
{
	return sharedInstance;
}


/**
    @brief Initializes the DDASLLogger
    @return id
**/
- (id)init
{
	if (sharedInstance != nil)
	{
		[self release];
		return nil;
	}
	
	if ((self = [super init]))
	{
		// A default asl client is provided for the main thread,
		// but background threads need to create their own client.
		
		client = asl_open(NULL, "com.apple.console", 0);
	}
	return self;
}

/**
    @brief Set the log message
    @param DDLogMessage
    @return void
**/
- (void)logMessage:(DDLogMessage *)logMessage
{
	NSString *logMsg = logMessage->logMsg;
	
    // If there is a log formatter
	if (formatter)
	{
		logMsg = [formatter formatLogMessage:logMessage];
	}
	
    // If there is a logMessage passed into this method
	if (logMsg)
	{
        // Create a constant read only local attribute
		const char *msg = [logMsg UTF8String];
		
		int aslLogLevel;
        
        // Switch based on the logging level
		switch (logMessage->logLevel)
		{
			// Note: By default ASL will filter anything above level 5 (Notice).
			// So our mappings shouldn't go above that level.
			
			case 1  : aslLogLevel = ASL_LEVEL_CRIT;    break;
			case 2  : aslLogLevel = ASL_LEVEL_ERR;     break;
			case 3  : aslLogLevel = ASL_LEVEL_WARNING; break;
			default : aslLogLevel = ASL_LEVEL_NOTICE;  break;
		}
		
		asl_log(client, NULL, aslLogLevel, "%s", msg);
	}
}

/**
    @brief Gets the logger name
    @return NSString
**/
- (NSString *)loggerName
{
	return @"cocoa.lumberjack.aslLogger";
}

@end
