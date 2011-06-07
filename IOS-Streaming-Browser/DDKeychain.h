//
//  DDKeychain.h
//  IOS-Streaming-Browser
//
//  Created by Will Rubel on 6/6/11.
//  Copyright 2011 Will Rubel. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Security/Security.h>

/**
 
**/
@interface DDKeychain : NSObject
{
    NSMutableDictionary *keychainData;		// The actual keychain item data backing store.
    NSMutableDictionary *genericPasswordQuery;	// A placeholder for the generic keychain item query used to locate the item.
}

@property (nonatomic, retain) NSMutableDictionary *keychainData;
@property (nonatomic, retain) NSMutableDictionary *genericPasswordQuery;

/**
    @param id
    @param id
    @return void
**/
- (void)mySetObject:(id)inObject forKey:(id)key;

/**
    @param id
    @return id
**/
- (id)myObjectForKey:(id)key;

/**
    @return void
**/
- (void)resetKeychainItem;

@end
