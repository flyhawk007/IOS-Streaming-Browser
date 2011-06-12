//
//  ConfigurationViewControllerIpad.m
//  IOS-Streaming-Browser
//
//  Created by Will Rubel on 6/9/11.
//  Copyright 2011 Will Rubel. All rights reserved.
//

#import "ConfigurationViewController.h"
#import "DDLog.h"


// Log levels: off, error, warn, info, verbose
static const int ddLogLevel = LOG_LEVEL_VERBOSE;



@implementation ConfigurationViewController

@synthesize  fpsSegmentedControl;
@synthesize  scaleSegmentedControl;
@synthesize rotateValue;


- (id)initWithNibName:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    self = [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
    if (self) {
        // Custom initialization
    }
    return self;
}

- (void)didReceiveMemoryWarning
{
    // Releases the view if it doesn't have a superview.
    [super didReceiveMemoryWarning];
    
    // Release any cached data, images, etc that aren't in use.
}

#pragma mark - View lifecycle

- (void)viewDidLoad
{
    [super viewDidLoad];
    // Do any additional setup after loading the view from its nib.
    
    
    // Sets the switch in the view to the value from user defaults
    self.rotateValue.on=[[NSUserDefaults standardUserDefaults] boolForKey:@"rotate"];
    
    self.fpsSegmentedControl.selectedSegmentIndex=[[[NSUserDefaults standardUserDefaults] objectForKey:@"frames"]intValue];
    
    DDLogError(@"frames equals: %d",[[[NSUserDefaults standardUserDefaults] objectForKey:@"frames"]intValue]);
    
    self.scaleSegmentedControl.selectedSegmentIndex=[[[NSUserDefaults standardUserDefaults] objectForKey:@"scale"]intValue];
    
    DDLogError(@"scale equals: %d",[[[NSUserDefaults standardUserDefaults] objectForKey:@"scale"]intValue]);
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    // Release any retained subviews of the main view.
    // e.g. self.myOutlet = nil;
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    // Return YES for supported orientations
    
    return [[NSUserDefaults standardUserDefaults] boolForKey:@"rotate"];
    
}


-(IBAction) selectFramePerSecond:(id)sender
{
    NSInteger index = ((UISegmentedControl*)sender).selectedSegmentIndex;
    
    // Get the shared defaults object.
    NSUserDefaults *settings = [NSUserDefaults standardUserDefaults];

    // Save the index.
    [settings setObject:[NSNumber numberWithInt:index] forKey:@"frames"];
    
    // Write them to disk - this is optional here,
    // but should be done when the app exits.
    [settings synchronize];

}

-(IBAction) selectImageScale:(id)sender
{
    NSInteger index = ((UISegmentedControl*)sender).selectedSegmentIndex;
   
    // Get the shared defaults object.
    NSUserDefaults *settings = [NSUserDefaults standardUserDefaults];
    
    // Save the index.
    [settings setObject:[NSNumber numberWithInt:index] forKey:@"scale"];
            
    // Write them to disk - this is optional here,
    // but should be done when the app exits.
    [settings synchronize];
         
    
    
    
    
}

-(IBAction) selectRotate:(id)sender
{
     self.rotateValue = (UISwitch *) sender;
    
    if(rotateValue.on==1){
        DDLogError(@"rotateValue does equal 1");
        NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        [defaults setBool:TRUE forKey:@"rotate"];
        [defaults synchronize];
    }else{
        DDLogError(@"rotateValue does not equal 1");
        NSUserDefaults * defaults = [NSUserDefaults standardUserDefaults];
        [defaults setBool:FALSE forKey:@"rotate"];
        [defaults synchronize];
    }
}


@end
