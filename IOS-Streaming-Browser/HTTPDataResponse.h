#import <Foundation/Foundation.h>
#import "HTTPResponse.h"

/**
    @brief Class for handling an HTTP Data Response
    Implements HTTPResponse protocol
**/
@interface HTTPDataResponse : NSObject <HTTPResponse>
{

    /**
        @brief the offset
    **/
	NSUInteger offset;
    
    /**
        @brief The data
    **/
	NSData *data;  
}

/**
    @brief Initializes the HTTPDataResponse with data
    @param NSDAta
    @return id
**/
- (id)initWithData:(NSData *)data;

@end
