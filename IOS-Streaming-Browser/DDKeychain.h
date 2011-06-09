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
    NSMutableDictionary        *keychainData;
    NSMutableDictionary        *genericPasswordQuery;
}

@property (nonatomic, retain) NSMutableDictionary *keychainData;
@property (nonatomic, retain) NSMutableDictionary *genericPasswordQuery;



- (void)mySetObject:(id)inObject forKey:(id)key;
- (id)myObjectForKey:(id)key;
/**
 
**/
- (void)resetKeychainItem;
//- (NSArray *)SSLIdentityAndCertificates

@end
