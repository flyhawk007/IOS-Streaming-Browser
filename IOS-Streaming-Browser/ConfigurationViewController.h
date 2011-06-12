//
//  ConfigurationViewController.h
//  IOS-Streaming-Browser
//
//  Created by Will Rubel on 6/9/11.
//  Copyright 2011 Will Rubel. All rights reserved.
//

#import <UIKit/UIKit.h>

@interface ConfigurationViewController : UIViewController

{
    UISegmentedControl *fpsSegmentedControl;
    UISegmentedControl *scaleSegmentedControl;
    UISwitch *rotateValue;
    
}

@property (nonatomic,retain) IBOutlet UISegmentedControl *fpsSegmentedControl;
@property (nonatomic,retain) IBOutlet UISegmentedControl *scaleSegmentedControl;
@property (nonatomic,retain) IBOutlet UISwitch *rotateValue;

-(IBAction) selectFramePerSecond:(id)sender;

-(IBAction) selectImageScale:(id)sender;

-(IBAction) selectRotate:(id)sender;


@end
