#import <Foundation/Foundation.h>
#import "DDLog.h"

@class DDLogFileInfo;


// Default configuration and safety/sanity values.
// 
// maximumFileSize         -> DEFAULT_LOG_MAX_FILE_SIZE
// rollingFrequency        -> DEFAULT_LOG_ROLLING_FREQUENCY
// maximumNumberOfLogFiles -> DEFAULT_LOG_MAX_NUM_LOG_FILES
// 
// You should carefully consider the proper configuration values for your application.

#define DEFAULT_LOG_MAX_FILE_SIZE     (1024 * 1024)   //  1 MB
#define DEFAULT_LOG_ROLLING_FREQUENCY (60 * 60 * 24)  // 24 Hours
#define DEFAULT_LOG_MAX_NUM_LOG_FILES (5)             //  5 Files


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// The LogFileManager protocol is designed to allow you to control all aspects of your log files.
// 
// The primary purpose of this is to allow you to do something with the log files after they have been rolled.
// Perhaps you want to compress them to save disk space.
// Perhaps you want to upload them to an FTP server.
// Perhaps you want to run some analytics on the file.
// 
// A default LogFileManager is, of course, provided.
// The default LogFileManager simply deletes old log files according to the maximumNumberOfLogFiles property.
// 
// This protocol provides various methods to fetch the list of log files.
// 
// There are two variants: sorted and unsorted.
// If sorting is not necessary, the unsorted variant is obviously faster.
// The sorted variant will return an array sorted by when the log files were created,
// with the most recently created log file at index 0, and the oldest log file at the end of the array.
// 
// You can fetch only the log file paths (full path including name), log file names (name only),
// or an array of DDLogFileInfo objects.
// The DDLogFileInfo class is documented below, and provides a handy wrapper that
// gives you easy access to various file attributes such as the creation date or the file size.

@protocol DDLogFileManager <NSObject>
@required

// Public properties

@property (readwrite, assign) NSUInteger maximumNumberOfLogFiles;

// Public methods

/**
    @return NSString
**/
- (NSString *)logsDirectory;

/**
    @return NSArray
**/
- (NSArray *)unsortedLogFilePaths;

/**
    @brief Gets unsorted array with the log file names
    @return NSArray
**/
- (NSArray *)unsortedLogFileNames;

/**
    
    @return NSArray
**/
- (NSArray *)unsortedLogFileInfos;

/**
    @return NSArray
**/
- (NSArray *)sortedLogFilePaths;

/**
    @return NSArray
**/
- (NSArray *)sortedLogFileNames;

/**
    @return NSArray
**/
- (NSArray *)sortedLogFileInfos;

// Private methods (only to be used by DDFileLogger)

/**
    @return NSString
**/
- (NSString *)createNewLogFile;

@optional

//////////////////////////////////////
// Notifications from DDFileLogger
//////////////////////////////////////

/**
    @param NSString
    @return void
**/
- (void)didArchiveLogFile:(NSString *)logFilePath;

/**
    @param NSString
    @return void
**/
- (void)didRollAndArchiveLogFile:(NSString *)logFilePath;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Default log file manager.

    All log files are placed inside the logsDirectory.
    On Mac, this is in ~/Library/Application Support/<Application Name>/Logs.
    On iPhone, this is in ~/Documents/Logs.
 
    Log files are named "log-<uuid>.txt", where uuid is a 6 character hexadecimal consisting of the set [0123456789ABCDEF].

    Archived log files are automatically deleted according to the maximumNumberOfLogFiles property.
**/
@interface DDLogFileManagerDefault : NSObject <DDLogFileManager>
{
    /**
        @brief The maximum number of log files
    **/
	NSUInteger maximumNumberOfLogFiles;
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Most users will want file log messages to be prepended with the date and time.
    Rather than forcing the majority of users to write their own formatter, we will supply a logical default formatter.
    Users can easily replace this formatter with their own by invoking the setLogFormatter method.
    It can also be removed by calling setLogFormatter, and passing a nil parameter.
    
    In addition to the convenience of having a logical default formatter, it will also provide a template that makes it easy for developers to copy and change.
**/
@interface DDLogFileFormatterDefault : NSObject <DDLogFormatter>
{
    /**
        @brief The date formatter
    **/
	NSDateFormatter *dateFormatter;
}

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
    @brief Logs information to a file
**/
@interface DDFileLogger : DDAbstractLogger <DDLogger>
{
    /**
        @brief The log file manager
    **/
	id <DDLogFileManager> logFileManager;
	
    /**
        @brief The current log file information
    **/
	DDLogFileInfo *currentLogFileInfo;
    
    /**
        @brief The NSFileHandle class is an object-oriented wrapper for a file descriptor
    **/
	NSFileHandle *currentLogFileHandle;
	
    /**
     
    **/
	NSTimer *rollingTimer;
	
    /**
        @brief The maximum file size has to be between 0 and 9,223,372,036,854,775,807
    **/
	unsigned long long maximumFileSize;
    
    /**
     
    **/
	NSTimeInterval rollingFrequency;
}

/**
    @brief Initialize the DDFileLogger
    @return id
**/
- (id)init;

/**
    @brief Initialize the DDFileLogger with a file manager
    @param id <DDLogFileManager
    @return id
**/
- (id)initWithLogFileManager:(id <DDLogFileManager>)logFileManager;

// Configuration
// 
// maximumFileSize:
//   The approximate maximum size to allow log files to grow.
//   If a log file is larger than this value after a write,
//   then the log file is rolled.
// 
// rollingFrequency
//   How often to roll the log file.
//   The frequency is given as an NSTimeInterval, which is a double that specifies the interval in seconds.
//   Once the log file gets to be this old, it is rolled.
// 
// Both the maximumFileSize and the rollingFrequency are used to manage rolling.
// Whichever occurs first will cause the log file to be rolled.
// 
// For example:
// The rollingFrequency is 24 hours,
// but the log file surpasses the maximumFileSize after only 20 hours.
// The log file will be rolled at that 20 hour mark.
// A new log file will be created, and the 24 hour timer will be restarted.
// 
// logFileManager
//   Allows you to retrieve the list of log files,
//   and configure the maximum number of archived log files to keep.


// The maximum file size has to be between 0 and 9,223,372,036,854,775,807
@property (readwrite, assign) unsigned long long maximumFileSize;


@property (readwrite, assign) NSTimeInterval rollingFrequency;

@property (nonatomic, readonly) id <DDLogFileManager> logFileManager;


/**
    @brief You can optionally force the current log file to be rolled with this method.
    @return void
**/
- (void)rollLogFile;

// Inherited from DDAbstractLogger

// - (id <DDLogFormatter>)logFormatter;
// - (void)setLogFormatter:(id <DDLogFormatter>)formatter;

@end

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#pragma mark -
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// DDLogFileInfo is a simple class that provides access to various file attributes.
// It provides good performance as it only fetches the information if requested,
// and it caches the information to prevent duplicate fetches.
// 
// It was designed to provide quick snapshots of the current state of log files,
// and to help sort log files in an array.
// 
// This class does not monitor the files, or update it's cached attribute values if the file changes on disk.
// This is not what the class was designed for.
// 
// If you absolutely must get updated values,
// you can invoke the reset method which will clear the cache.

@interface DDLogFileInfo : NSObject
{
    /**
        @brief The file path of the log file
    **/
	NSString *filePath;
    
    /**
        @brief The log file name
    **/
	NSString *fileName;
	
    /**
        @brief NSDiction for the file attributes
    **/
	NSDictionary *fileAttributes;
	
    /**
        @brief The file creation date
    **/
	NSDate *creationDate;
    
    /**
        @brief The modification date
    **/
	NSDate *modificationDate;
	
    
    // The filesize can be between 0 and 9,223,372,036,854,775,807
	unsigned long long fileSize;
}

@property (nonatomic, readonly) NSString *filePath; // the file path
@property (nonatomic, readonly) NSString *fileName; // the file name

@property (nonatomic, readonly) NSDictionary *fileAttributes; // the file attributes

@property (nonatomic, readonly) NSDate *creationDate; // the file creation date
@property (nonatomic, readonly) NSDate *modificationDate; // the file modification date

@property (nonatomic, readonly) unsigned long long fileSize;  // the size of the file

@property (nonatomic, readonly) NSTimeInterval age;

@property (nonatomic, readwrite) BOOL isArchived;  // whether the file is archived


/**
    Class method
    @param NSString
    @return id
**/
+ (id)logFileWithPath:(NSString *)filePath;


/**
    @brief Initialize the DDLogFileInfo with a file path
    @param NSString
    @return id
**/
- (id)initWithFilePath:(NSString *)filePath;

/**
    @return void
**/
- (void)reset;

/**
    @brief Renames the file
    @param NSString
    @return void
**/
- (void)renameFile:(NSString *)newFileName;

#if TARGET_IPHONE_SIMULATOR

// So here's the situation.
// Extended attributes are perfect for what we're trying to do here (marking files as archived).
// This is exactly what extended attributes were designed for.
// 
// But Apple screws us over on the simulator.
// Everytime you build-and-go, they copy the application into a new folder on the hard drive,
// and as part of the process they strip extended attributes from our log files.
// Normally, a copy of a file preserves extended attributes.
// So obviously Apple has gone to great lengths to piss us off.
// 
// Thus we use a slightly different tactic for marking log files as archived in the simulator.
// That way it "just works" and there's no confusion when testing.
// 
// The difference in method names is indicative of the difference in functionality.
// On the simulator we add an attribute by appending a filename extension.
// 
// For example:
// log-ABC123.txt -> log-ABC123.archived.txt


/**
    @param NSString
    @return BOOL
**/
- (BOOL)hasExtensionAttributeWithName:(NSString *)attrName;

/**
    @param NSString
    @return void
**/
- (void)addExtensionAttributeWithName:(NSString *)attrName;

/**
    @param NSString
    @return void
**/
- (void)removeExtensionAttributeWithName:(NSString *)attrName;

#else

// Normal use of extended attributes used everywhere else,
// such as on Macs and on iPhone devices.

/**
    @param NSString
    @return BOOL
**/
- (BOOL)hasExtendedAttributeWithName:(NSString *)attrName;

/**
    @param NSString
    @return void
**/
- (void)addExtendedAttributeWithName:(NSString *)attrName;

/**
    @param NSString
    @return void
**/
- (void)removeExtendedAttributeWithName:(NSString *)attrName;

#endif

/**
    @param DDLogFileInfo
    @return NSComparisonResult
**/
- (NSComparisonResult)reverseCompareByCreationDate:(DDLogFileInfo *)another;

/**
    @param DDLogFileInfo
    @return NSComparisonResult
**/
- (NSComparisonResult)reverseCompareByModificationDate:(DDLogFileInfo *)another;

@end
