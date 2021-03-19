//
//  DownloadLogger.h
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 02/03/16.
//  Copyright (c) 2016 Unity Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface DownloadLogger : NSObject
- (void)log:(NSString *)format, ...;
@end
