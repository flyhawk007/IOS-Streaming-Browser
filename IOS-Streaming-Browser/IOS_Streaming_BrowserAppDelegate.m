//
//  IOS_Streaming_BrowserAppDelegate.m
//  IOS-Streaming-Browser
//
//  Created by Will Rubel on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "IOS_Streaming_BrowserAppDelegate.h"
#import "IOS_Streaming_BrowserViewController.h"
#import "HTTPConnection.h"
#import "DDLog.h"
#import "DDTTYLogger.h"
#import "DDFileLogger.h"


/** 
    Log levels: off, error, warn, info, verbose
**/
static const int ddLogLevel = LOG_LEVEL_VERBOSE;




@implementation IOS_Streaming_BrowserAppDelegate


@synthesize window;
@synthesize viewController;
@synthesize navigationController;


/**
    @brief Whether the application did finith launching
    @param UIApplication
    @param NSDictionary
    @return BOOL
**/
- (BOOL)application:(UIApplication *)application didFinishLaunchingWithOptions:(NSDictionary *)launchOptions
{
 
    /////////////////////////////////////////////
    // Set default values
    ////////////////////////////////////////////
    
    // initialize defaults
    NSString *dateKey    = @"dateKey";
    NSDate *lastRead    = (NSDate *)[[NSUserDefaults standardUserDefaults] objectForKey:dateKey];
    
    if (lastRead == nil)     // App first run: set up user defaults.
    {
        
         NSDictionary *appDefaults  = [NSDictionary dictionaryWithObjectsAndKeys:[NSDate date], dateKey, nil];
        
        // do any other initialization you want to do here - e.g. the starting default values.    
        [[NSUserDefaults standardUserDefaults] setBool:FALSE forKey:@"rotate"];
        [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:1] forKey:@"frames"];
        [[NSUserDefaults standardUserDefaults] setObject:[NSNumber numberWithInt:1] forKey:@"scale"];
        
        // sync the defaults to disk
        [[NSUserDefaults standardUserDefaults] registerDefaults:appDefaults];
        [[NSUserDefaults standardUserDefaults] synchronize];
        
    }
    [[NSUserDefaults standardUserDefaults] setObject:[NSDate date] forKey:dateKey];
    
    
    
    /////////////////////////////////////////
    // End of user defaults initialization
    /////////////////////////////////////////
    
    
    
    
    // Configure our logging framework.
	// To keep things simple and fast, we're just going to log to the Xcode console.
    DDFileLogger *fileLogger = [[DDFileLogger alloc] init];
    fileLogger.rollingFrequency = 60 * 60 * 24; // 24 hour rolling
    fileLogger.logFileManager.maximumNumberOfLogFiles = 7;
    [DDLog addLogger:fileLogger];
    
	[DDLog addLogger:[DDTTYLogger sharedInstance]];
    
    
    // Add the view controller's view to the window and display.
    //[window addSubview:viewController.view];
    [window addSubview: navigationController.view];
    [window makeKeyAndVisible];
    
    return YES;
}

/**
    @brief Standard Deconstructor
    @return void
**/ 
- (void)dealloc
{  
	[viewController release];
    [navigationController release];
	[window release];
	[super dealloc];
}


@end
