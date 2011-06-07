#import <Foundation/Foundation.h>
#import "HTTPResponse.h"

/**
    @brief Class for handling and HTTP redirect response
    Implements HTTPResponse protocol
**/
@interface HTTPRedirectResponse : NSObject <HTTPResponse>
{
    /**
        @brief The redirect path
    **/
	NSString *redirectPath;
}


/**
    @brief Initialize the HTTPRedirectResponse with a redirectPath
    @param NSString
    @return id
**/
- (id)initWithPath:(NSString *)redirectPath;

@end
