//
//  ComponentDownloader.h
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "DownloadLogger.h"
#include "iniparser/src/iniparser.h"

@protocol ComponentDownloaderDelegate<NSObject>
@optional
- (NSString *)getPackageFilenameForIndex:(int)idx;
- (NSString *)getPathForIndex:(int)idx;
- (uint64_t)componentFileSizeForIndex:(int)idx;
- (void)componentFinishedDownloading:(NSString *)path idx:(int)idx;
- (void)setComponentDownloadProgress:(float)progress idx:(int)idx;
- (void)componentDownloadFailed:(int)idx withError:(NSString *)error;
- (void)componentDownloadStarted:(int)idx;
@end


@interface ComponentDownloader : NSObject
@property (readonly) int idx;
@property (readonly) NSInteger fileSize;
@property (readonly) NSInteger receivedSize;
@property (readonly) NSURLDownload *urlDownload;

- (void)init:(id<ComponentDownloaderDelegate>)delegate ini:(dictionary *)ini logger:(DownloadLogger *)logger userAgent:(NSString *)userAgentString;
- (void)download:(int)idx;
- (void)downloadCurrent;
- (void)cancelCurrent;
- (BOOL)isPaused;
@end
