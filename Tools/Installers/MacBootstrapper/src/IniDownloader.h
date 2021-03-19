//
//  IniDownloader.h
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DownloadLogger.h"

@protocol IniDownloaderDelegate<NSObject>
@optional
- (void)iniFinishedDownloading:(NSString *)path;
- (void)iniFailedDownloading:(NSString *)error;
@end

@interface IniDownloader : NSObject
@property (readonly) NSString *ini_url_base;

- (IniDownloader *)initWithPath:(NSString *)downloadLocation;
- (void)run:(id<IniDownloaderDelegate>)delegate logger:(DownloadLogger*)logger userAgent:(NSString *)userAgentString;

@end
