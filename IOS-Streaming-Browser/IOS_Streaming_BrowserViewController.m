//
//  IOS_Streaming_BrowserViewController.m
//  IOS-Streaming-Browser
//
//  Created by Will Rubel on 5/12/11.
//  Copyright 2011 __MyCompanyName__. All rights reserved.
//

#import "IOS_Streaming_BrowserViewController.h"
#import "HTTPServer.h"
#import "HTTPConnection.h"
#import "localhostAddresses.h"
#import "DDLog.h"
#import "DDTTYLogger.h"

// Log levels: off, error, warn, info, verbose
static const int ddLogLevel = LOG_LEVEL_VERBOSE;



@interface IOS_Streaming_BrowserViewController()

/**
    @brief Start recording
    @return void
**/
-(void) startRecording;

/**
    @brief Stop recording
    @return void
**/
-(void) stopRecording;

/**
    @brief Take a screen shot
    @return UIImage
**/
-(UIImage*) screenshot;

@end



@implementation IOS_Streaming_BrowserViewController


// Synthesize the getters and setters
@synthesize startStopButton;
@synthesize webView;
@synthesize addressBar;
@synthesize activityIndicator;



/**
    @brief Deallocate the object
    @return void
**/
- (void)dealloc
{

    [httpServer release];
    [clockTimer invalidate]; clockTimer = nil;
	[assetWriterTimer invalidate]; assetWriterTimer = nil;
	[assetWriter release]; assetWriter = nil;
	[assetWriterInput release]; assetWriterInput = nil;
	[assetWriterPixelBufferAdaptor release]; assetWriterPixelBufferAdaptor = nil;
    [super dealloc];
    
}

/**
    @brief Executed upon the receipt of a memory warning
    @return void
**/
- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}


///////////////////////////////////////////////////////////////
#pragma mark screenshot
///////////////////////////////////////////////////////////////


/**
    Copied from http://developer.apple.com/library/ios/#qa/qa1703/_index.html , with new imageScale to take Retina-to-320x480 scaling into account
    @return UIImage
**/
- (UIImage*)screenshot 
{
    
    
    // Create a graphics context with the target size
    // On iOS 4 and later, use UIGraphicsBeginImageContextWithOptions to take the scale into consideration
    // On iOS prior to 4, fall back to use UIGraphicsBeginImageContext
    
    
    // Gets the image size of the main screen
    CGSize imageSize = [[UIScreen mainScreen] bounds].size;
    
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPad)
	{  
        
        // Set the image scale
        CGFloat imageScale = imageSize.width / 760;
        if (NULL != UIGraphicsBeginImageContextWithOptions)
        {
            UIGraphicsBeginImageContextWithOptions(imageSize, NO, imageScale);
        }else{
            UIGraphicsBeginImageContext(imageSize);
        }
        
    }else
    {
        // Set the image scale
        CGFloat imageScale = imageSize.width / 200;
        if (NULL != UIGraphicsBeginImageContextWithOptions)
        {
            UIGraphicsBeginImageContextWithOptions(imageSize, NO, imageScale);
        }else{
            UIGraphicsBeginImageContext(imageSize);
        }
    }
    
    
    
    
    CGContextRef context = UIGraphicsGetCurrentContext();
	
    // Iterate over every window from back to front
    for (UIWindow *window in [[UIApplication sharedApplication] windows]) 
    {
        if (![window respondsToSelector:@selector(screen)] || [window screen] == [UIScreen mainScreen])
        {
            // -renderInContext: renders in the coordinate space of the layer,
            // so we must first apply the layer's geometry to the graphics context
            CGContextSaveGState(context);
            // Center the context around the window's anchor point
            CGContextTranslateCTM(context, [window center].x, [window center].y);
            // Apply the window's transform about the anchor point
            CGContextConcatCTM(context, [window transform]);
            // Offset by the portion of the bounds left of and above the anchor point
            CGContextTranslateCTM(context,
                                  -[window bounds].size.width * [[window layer] anchorPoint].x,
                                  -[window bounds].size.height * [[window layer] anchorPoint].y);
			
            // Render the layer hierarchy to the current context
            [[window layer] renderInContext:context];
			
            // Restore the context
            CGContextRestoreGState(context);
        }
    }
	
    // Retrieve the screenshot image
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
	
    
    UIGraphicsEndImageContext();
	
    
    return image;
}

///////////////////////////////////////////////////////////////
#pragma mark helpers
///////////////////////////////////////////////////////////////


/**
    @brief Gets the path to the document directory
    @return NSString
**/
-(NSString*) pathToDocumentsDirectory {
    
	NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    
	NSString *documentsDirectory = [paths objectAtIndex:0];
    
	return documentsDirectory;
}

/**
    @brief Write the image
    @param NSTimer
    @return void
**/
-(void) writeImage: (NSTimer*) _timer {
    
    
    NSString *imageName = [NSString stringWithFormat:@"1.png"];
    
	NSString *imagePath = [[self pathToDocumentsDirectory] stringByAppendingPathComponent:imageName];
    
    // DDLogError(@"Image Name is: %@",imagePath);
    
    // Check if a file already exists, and if so, remove it
	//if ([[NSFileManager defaultManager] fileExistsAtPath:imagePath]) {
	//	[[NSFileManager defaultManager] removeItemAtPath:imagePath error:nil];
	//}
	
    
    // get screenshot image!
    
    [UIImagePNGRepresentation([self screenshot]) writeToFile:imagePath atomically:YES];
    
    // DDLogError (@"made screenshot");
    
}


/**
    @brief Start the HTTP server
    @return void
**/
-(void) startHttpServer
{
    NSString *root = [NSSearchPathForDirectoriesInDomains(NSDocumentDirectory,NSUserDomainMask,YES) objectAtIndex:0];

    
    // Create server using our custom MyHTTPServer class
	httpServer = [[HTTPServer alloc] init];
	
	// Tell the server to broadcast its presence via Bonjour.
	// This allows browsers such as Safari to automatically discover our service.
	[httpServer setType:@"_http._udp."];
    // 6/8/11: Changed to udp for testing purposes
    // The old value was @"_http._tcp."

	
	// Normally there's no need to run our server on any specific port.
	// Technologies like Bonjour allow clients to dynamically discover the server's port at runtime.
	// However, for easy testing you may want force a certain port so you can just hit the refresh button.
	[httpServer setPort:12345];
	

    
    [httpServer setConnectionClass:[HTTPConnection class]];
    
    // Serve files from our embedded Web folder
	//NSString *webPath = [[[NSBundle mainBundle] resourcePath] stringByAppendingPathComponent:@"Web"];
    //[httpServer setDocumentRoot:webPath];
    
	
	[httpServer setDocumentRoot:[NSURL fileURLWithPath:root]];
	[httpServer setDocumentRoot:root];
    


	// Start the server (and check for problems)
    NSError *error;
    
    // Try and start the http server
    if(![httpServer start:&error])
    {
            DDLogError(@"Error starting HTTP Server: %@", error);
    }
    
    // Set the display information for the main browser view to nil because this will be updated with the actual IP address and port once it is obtained from the wireless router
    [self displayInfoUpdate:nil];
    
    
}

/**
    @brief Stop the HTTP server
    @return void
**/
-(void) stopHttpServer
{
    [httpServer stop];
    [self displayInfoUpdate:nil];
}



/**
    @brief Starts Recording images
    @return void
**/
-(void) startRecording 
{ 
    [self startHttpServer];
	// start writing images to it every tenth of a second
	[assetWriterTimer release];
	assetWriterTimer = [NSTimer scheduledTimerWithTimeInterval:2.0
														target:self
													  selector:@selector (writeImage:)
													  userInfo:nil
													   repeats:YES] ;
}

/**
    @brief Stop recording images
    @return void
**/
-(void) stopRecording {
    DDLogError(@"stopRecording");
    [httpServer stop];
	[assetWriterTimer invalidate]; // invalidate the timer
	assetWriterTimer = nil;
	[assetWriter finishWriting]; // Completes the writing of the output file.
	DDLogError (@"finished writing");
    
    // Clear images from directory
    // Check if a file already exists, and if so, remove it
    
    NSString *imageName = [NSString stringWithFormat:@"1.png"];
    NSString *imagePath = [[self pathToDocumentsDirectory] stringByAppendingPathComponent:imageName];
    
	
    //if ([[NSFileManager defaultManager] fileExistsAtPath:imagePath]) {
    //    [[NSFileManager defaultManager] removeItemAtPath:imagePath error:nil];
    //    // DDLogError(@"File removed");
    //}
}


///////////////////////////////////////////////////////////////
#pragma mark - View lifecycle
///////////////////////////////////////////////////////////////


/**
    @brief The view will appear
    @param BOOL
    @return void
**/
- (void) viewWillAppear:(BOOL)animated {
    DDLogError(@"viewWillAppear");
}

/**
    @brief Implement viewDidLoad to do additional setup after loading the view, typically from a nib.
    @return void
**/
- (void)viewDidLoad
{
    DDLogError(@"viewDidLoad");

    
    [super viewDidLoad];
    
    
    // Prevent cookies from being stored in the application
    [[NSHTTPCookieStorage sharedHTTPCookieStorage] setCookieAcceptPolicy:NSHTTPCookieAcceptPolicyAlways];    
    
    
    
    // Adds an observer to the local notification center to call the
    // displayInfoUpdate method once the local host is resolved
	[[NSNotificationCenter defaultCenter] addObserver:self selector:@selector(displayInfoUpdate:) name:@"LocalhostAdressesResolved" object:nil];
    
    
	[localhostAddresses performSelectorInBackground:@selector(list) withObject:nil];

    
    
    // Set the defaults web address to load by creating a 
    // string, then converting the string to a URL.  The URL is
    // then put inside a URL request
    NSString *urlAddress = @"http://www.google.com/webhp?safe=strict";
	NSURL *url = [NSURL URLWithString:urlAddress];
    NSMutableURLRequest *requestObj = [NSMutableURLRequest requestWithURL:url];
    
    webView.delegate = self;
	[webView loadRequest:requestObj];
	[addressBar setText:urlAddress];
    
}

/**
    @brief Executed upon the view unloading
    @return void
**/
- (void)viewDidUnload
{
    DDLogError(@"viewDidUnload");
  
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
    [self stopHttpServer];
    
}

/**
    @brief Whether the screen should autorotate
    @param UIInterfaceOrientation
    @return BOOL
**/
- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    
    return YES;
}


///////////////////////////////////////////////////////////////
#pragma mark event handlers
///////////////////////////////////////////////////////////////



/**
    @brief Upon the start/stop button being pressed
    @param id
    @return IBAction
**/
-(IBAction) handleStartStopTapped: (id) sender 
{
    DDLogError(@"handleStartStopTapped");
    
    
    // stop recording and deselect
	if (self.startStopButton.selected) { 
		[self stopRecording];

		self.startStopButton.selected = NO;
        
        [startStopButton setTitle:@"Start Broadcasting" forState:UIControlStateNormal];
	} else { // start recording and set the button as selected
		[self startRecording];

		self.startStopButton.selected = YES;
        
        [startStopButton setTitle:@"Stop Broadcasting" forState:UIControlStateSelected];
	}
}

/**
    @brief Gets the address from the address bar, and updates the webview with the requested URL
    @param id
    @return IBAction
**/
-(IBAction)gotoAddress:(id) sender 
{
    DDLogError(@"gotoAddress");
    
    // Gets the text from the address bar
	NSURL *url = [NSURL URLWithString:[addressBar text]];
        
    
    // Creates a request for the URL in the address bar
	NSMutableURLRequest *requestObj = [NSMutableURLRequest requestWithURL:url];
	
	//Load the request in the UIWebView.
	[webView loadRequest:requestObj];
    
    // Set the address bar as the first responder
	[addressBar resignFirstResponder];
}

/**
    @brief Upon the back button being pressed on the webview
    @param id
    @return IBAction
**/
-(IBAction) goBack:(id)sender 
{
    DDLogError(@"goBack");
	[webView goBack];
}

/**
    @brief Upon the configure button being pressed
    @param id
    @return IBAction
**/
-(IBAction) configureButton:(id)sender 
{
    DDLogError(@"configureButton");
    
	UIAlertView *alert = [[UIAlertView alloc] initWithTitle:@"Note"
                                                    message:@"Feature not available in free version"
                                                   delegate:nil
                                          cancelButtonTitle:@"OK"
                                          otherButtonTitles:nil];	
    [alert show];
    [alert release];
}


/**
    @brief Upon the forward button being pressed on the webview
    @param id
    @return IBAction
**/
-(IBAction) goForward:(id)sender 
{
    DDLogError(@"goForward");
    
	[webView goForward];
}

/**
    @brief Upon the reload image beinging pressed on the webview
    @param id
    @return IBAction
**/
-(IBAction) reloadPage:(id)sender 
{
    DDLogError(@"reloadPage");
    
	[webView reload];
}

/**
    @brief Upon the stop loading button beinging pressed on the webview
    @param id
    @return IBAction
**/
-(IBAction) stopLoading:(id)sender 
{
    DDLogError(@"stopLoading");
    
	[webView stopLoading];
}


/**
    @brief Upon the Home button being pressed on the webview
    @param id
    @return IBAction
**/
-(IBAction) goHome:(id)sender {
    DDLogError(@"goHome");
    
    
    // Set the defaults web address to load
    NSString *urlAddress = @"http://www.google.com/webhp?safe=strict";
	
	NSURL *url = [NSURL URLWithString:urlAddress];
	
    NSMutableURLRequest *requestObj = [NSMutableURLRequest requestWithURL:url];
	
	[webView loadRequest:requestObj];
	[addressBar setText:urlAddress];
}


/**
    @brief Whether should load the request in the UIWebView
    @param UIWebView
    @param NSMutableURLRequest
    @param UIWebViewNavigationType
    @return BOOL
**/
- (BOOL)webView:(UIWebView*)webView shouldStartLoadWithRequest:(NSMutableURLRequest*)request navigationType:(UIWebViewNavigationType)navigationType 
{
    
    DDLogError(@"webView shouldStartLoadWithRequest");
    
    // Loop through the cookies availabe for the HTTP request.
    // This is looking for cookie settings and ensuring strict filtering
    NSHTTPCookie *cookie;
    NSHTTPCookieStorage *cookieJar = [NSHTTPCookieStorage sharedHTTPCookieStorage];
    for (cookie in [cookieJar cookies]) {
        
        if (([[cookie name] isEqualToString:@"PREF"]) && ([[cookie domain] isEqualToString:@".youtube.com"]))
        {
        
            DDLogError(@"Cookie before delete: %@", cookie);
            [cookieJar deleteCookie:cookie];
            
            NSMutableDictionary *properties = [NSMutableDictionary dictionaryWithCapacity:4];
                                               
            [properties setObject:@"PREF" forKey:NSHTTPCookieName];
            [properties setObject:@"f1=50000000&f2=8000000&fms2=30000&fms1=30000" forKey:NSHTTPCookieValue];                                             
            [properties setObject:@".youtube.com" forKey:NSHTTPCookieDomain];
            [properties setObject:@"/" forKey:@"Path"];
            NSHTTPCookie *myCookie = [NSHTTPCookie cookieWithProperties:properties];
            [cookieJar setCookie:myCookie];
            DDLogError(@"%@", myCookie);

        }else if (([[cookie name] isEqualToString:@"PREF"]) && ([[cookie domain] isEqualToString:@".google.com"]))
        {
            
            DDLogError(@"%@", cookie);
            NSMutableDictionary *properties = [NSMutableDictionary dictionaryWithCapacity:4];
            
            [properties setObject:@"PREF" forKey:NSHTTPCookieName];
            NSString *myValue = [cookie value];
            NSString *myNewValue;
            myNewValue = [[myValue stringByReplacingOccurrencesOfString:@"FF=0" withString:@"FF=1"]stringByReplacingOccurrencesOfString:@"FF=4" withString:@"FF=1"];
            
            [properties setObject:myNewValue forKey:NSHTTPCookieValue];                                             
            [properties setObject:[cookie domain] forKey:NSHTTPCookieDomain];
            [properties setObject:[cookie path] forKey:@"Path"];
            NSHTTPCookie *myCookie = [NSHTTPCookie cookieWithProperties:properties];
                          
            [cookieJar deleteCookie:cookie]; // delete old cookie
            [cookieJar setCookie:myCookie]; // add new cookie
            DDLogError(@"%@", myCookie);
         
        }
    }

    
    
    if(navigationType == UIWebViewNavigationTypeOther){
        DDLogError(@"some other action occurred");
        NSURL *URL = [request URL];	
        DDLogError(@"URL is: %@",[URL absoluteString]);
        DDLogError(@"absoluteString:%@",[URL absoluteString]);
        DDLogError(@"absoluteURL:%@",[URL absoluteURL]);
        DDLogError(@"baseURL:%@",[URL baseURL]);
        DDLogError(@"fragment:%@",[URL fragment]);
        DDLogError(@"host:%@",[URL host]);
        DDLogError(@"lastPathComponent:%@",[URL lastPathComponent]);
        DDLogError(@"parameterString:%@",[URL parameterString]);
        DDLogError(@"password:%@",[URL password]);
        DDLogError(@"path:%@",[URL path]);
        DDLogError(@"pathComponents:%@",[URL pathComponents]);
        DDLogError(@"pathExtension:%@",[URL pathExtension]);
        DDLogError(@"port:%@",[URL port]);
        DDLogError(@"query:%@",[URL query]);
        DDLogError(@"relativePath:%@",[URL relativePath]);
        DDLogError(@"relativeString:%@",[URL relativeString]);
        DDLogError(@"resourceSpecifier:%@",[URL resourceSpecifier]);
        DDLogError(@"scheme:%@",[URL scheme]);
        DDLogError(@"standardizedURL:%@",[URL standardizedURL]);
        DDLogError(@"user:%@",[URL user]);
        
        // Request information
        DDLogError(@"cachePolicy: %@",[request cachePolicy]);
        DDLogError(@"mainDocumentURL:%@",[request mainDocumentURL]);
        DDLogError(@"timeoutInterval:%@",[request timeoutInterval]);
        DDLogError(@"URL: %@",[request URL]);

  
         
        
        return YES;
    }else if(navigationType == UIWebViewNavigationTypeFormSubmitted){
        DDLogError(@"user submitted form");
        NSURL *URL = [request URL];	
        DDLogError(@"URL is: %@",[URL absoluteString]);
        DDLogError(@"absoluteString:%@",[URL absoluteString]);
        DDLogError(@"absoluteURL:%@",[URL absoluteURL]);
        DDLogError(@"baseURL:%@",[URL baseURL]);
        DDLogError(@"fragment:%@",[URL fragment]);
        DDLogError(@"host:%@",[URL host]);
        DDLogError(@"lastPathComponent:%@",[URL lastPathComponent]);
        DDLogError(@"parameterString:%@",[URL parameterString]);
        DDLogError(@"password:%@",[URL password]);
        DDLogError(@"path:%@",[URL path]);
        DDLogError(@"pathComponents:%@",[URL pathComponents]);
        DDLogError(@"pathExtension:%@",[URL pathExtension]);
        DDLogError(@"port:%@",[URL port]);
        DDLogError(@"query:%@",[URL query]);
        DDLogError(@"relativePath:%@",[URL relativePath]);
        DDLogError(@"relativeString:%@",[URL relativeString]);
        DDLogError(@"resourceSpecifier:%@",[URL resourceSpecifier]);
        DDLogError(@"scheme:%@",[URL scheme]);
        DDLogError(@"standardizedURL:%@",[URL standardizedURL]);
        DDLogError(@"user:%@",[URL user]);


        
        
        
             
        
        return YES;
        
    }else if(navigationType == UIWebViewNavigationTypeFormResubmitted){
        DDLogError(@"user resubmitted a form");
        return YES;
        
    }else if(navigationType == UIWebViewNavigationTypeBackForward){
        DDLogError(@"user tapped the forward or back button");
        return YES;
        
    }else if(navigationType == UIWebViewNavigationTypeReload){
        DDLogError(@"user tapped the reload button");
         return YES;
    }
    
	//CAPTURE USER LINK-CLICK.
	else if (navigationType == UIWebViewNavigationTypeLinkClicked) 
    {
        DDLogError(@"is a link click");
		NSURL *URL = [request URL];	
        DDLogError(@"URL is: %@",[URL absoluteString]);
        DDLogError(@"absoluteString:%@",[URL absoluteString]);
        DDLogError(@"absoluteURL:%@",[URL absoluteURL]);
        DDLogError(@"baseURL:%@",[URL baseURL]);
        DDLogError(@"fragment:%@",[URL fragment]);
        DDLogError(@"host:%@",[URL host]);
        DDLogError(@"lastPathComponent:%@",[URL lastPathComponent]);
        DDLogError(@"parameterString:%@",[URL parameterString]);
        DDLogError(@"password:%@",[URL password]);
        DDLogError(@"path:%@",[URL path]);
        DDLogError(@"pathComponents:%@",[URL pathComponents]);
        DDLogError(@"pathExtension:%@",[URL pathExtension]);
        DDLogError(@"port:%@",[URL port]);
        DDLogError(@"query:%@",[URL query]);
        DDLogError(@"relativePath:%@",[URL relativePath]);
        DDLogError(@"relativeString:%@",[URL relativeString]);
        DDLogError(@"resourceSpecifier:%@",[URL resourceSpecifier]);
        DDLogError(@"scheme:%@",[URL scheme]);
        DDLogError(@"standardizedURL:%@",[URL standardizedURL]);
        DDLogError(@"user:%@",[URL user]);

		if (([[URL scheme] isEqualToString:@"http"])||([[URL scheme] isEqualToString:@"https"])) 
        {
            
                [addressBar setText:[URL absoluteString]];
                DDLogError(@"baseURL is: %@",[URL baseURL]);
                DDLogError(@"host: %@",[URL host]);
                [self gotoAddress:nil];
            
		}else // set the addressbar but dont' return anything to the user
        {	 
            [addressBar setText:[URL absoluteString]];
            return NO;
        }
    }
    // If the navigation type is not one of the standard navigation types
    // then don't load the page
    NSURL *URL = [request URL];	
    DDLogError(@"URL is: %@",[URL absoluteString]);
    DDLogError(@"absoluteString:%@",[URL absoluteString]);
    DDLogError(@"absoluteURL:%@",[URL absoluteURL]);
    DDLogError(@"baseURL:%@",[URL baseURL]);
    DDLogError(@"fragment:%@",[URL fragment]);
    DDLogError(@"host:%@",[URL host]);
    DDLogError(@"lastPathComponent:%@",[URL lastPathComponent]);
    DDLogError(@"parameterString:%@",[URL parameterString]);
    DDLogError(@"password:%@",[URL password]);
    DDLogError(@"path:%@",[URL path]);
    DDLogError(@"pathComponents:%@",[URL pathComponents]);
    DDLogError(@"pathExtension:%@",[URL pathExtension]);
    DDLogError(@"port:%@",[URL port]);
    DDLogError(@"query:%@",[URL query]);
    DDLogError(@"relativePath:%@",[URL relativePath]);
    DDLogError(@"relativeString:%@",[URL relativeString]);
    DDLogError(@"resourceSpecifier:%@",[URL resourceSpecifier]);
    DDLogError(@"scheme:%@",[URL scheme]);
    DDLogError(@"standardizedURL:%@",[URL standardizedURL]);
    DDLogError(@"user:%@",[URL user]);

    return NO;
}



/**
    @brief Start the activity indicator when the webview starts loading a webpage
    @param UIWebView
    @return void
**/
- (void)webViewDidStartLoad:(UIWebView *)webView 
{
    DDLogError(@"webViewDidStartLoad");
	[activityIndicator startAnimating];
}


/**
    @brief Stop the activity indicator when the webview finishes loading the webpage
    @param UIWebView
    @return void
**/
- (void)webViewDidFinishLoad:(UIWebView *)webView 
{
    
   DDLogError(@"webViewDidFinishLoad");
	[activityIndicator stopAnimating];
}

/**
    @brief The webView did load with an error
    @param UIWebView
    @param NSError
    @return void
**/
- (void)webView:(UIWebView *)webView didFailLoadWithError:(NSError *)error{
    DDLogError(@"webView didFailLoadWithError: %@",error);
    
}


/**
    @brief Updates the broadcast ip/port label on the view
    @param NSNotification
    @return void
**/
- (void)displayInfoUpdate:(NSNotification *) notification
{
	DDLogError(@"displayInfoUpdate:");
    
	if(notification)
	{
		[addresses release];
		addresses = [[notification object] copy];
		DDLogError(@"addresses: %@", addresses);
	}
    
    // Return if the notification doesn't contain an address
	if(addresses == nil)
	{
		return;
	}
	
    
	NSString *info;
	UInt16 port = [httpServer port]; // get http server port
	
	NSString *localIP = nil;
	
	localIP = [addresses objectForKey:@"en0"];
	
    
	if (!localIP)
	{
		localIP = [addresses objectForKey:@"en1"];
	}
    
	if (!localIP)
    {
		info = @"Wifi: No Connection!\n";
	}else{
		info = [NSString stringWithFormat:@"		http://%@:%d\n",localIP, port];
    }
    
    
	displayInfo.text = info;
}
@end
