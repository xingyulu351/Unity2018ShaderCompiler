//
//  DownloadLogger.m
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 02/03/16.
//  Copyright (c) 2016 Unity Technologies. All rights reserved.
//

#import "DownloadLogger.h"
#include <stdarg.h>

@implementation DownloadLogger
{
    FILE *_logFile;
}

- (id)init
{
    self = [super init];
    NSArray *URLs = [[NSFileManager defaultManager] URLsForDirectory: NSLibraryDirectory
                     inDomains: NSUserDomainMask];
    NSURL *logsURL = [[URLs lastObject] URLByAppendingPathComponent: @"Logs"];
    NSString *logsPath = [[logsURL path] stringByAppendingString: @"/Unity"];

    [[NSFileManager defaultManager] createDirectoryAtPath: logsPath
     withIntermediateDirectories: YES
     attributes: nil
     error: nil];

    NSString *logFilePath = [logsPath stringByAppendingString: @"/DownloadAssistant.log"];


    // backup the previous log
    NSString *prevLogFilePath = [logsPath stringByAppendingString: @"/DownloadAssistant-prev.log"];

    rename([logFilePath UTF8String], [prevLogFilePath UTF8String]);

    _logFile = fopen([logFilePath UTF8String], "w+t");

    return self;
}

- (void)dealloc
{
    if (_logFile)
    {
        fclose(_logFile);
        _logFile = NULL;
    }
}

- (void)log:(NSString *)format, ...
{
    if (!_logFile)
    {
        return;
    }

    va_list args;
    va_start(args, format);
    NSString *formattedString = [[NSString alloc] initWithFormat: format
                                 arguments: args];
    va_end(args);

    time_t timer;
    char formattedTime[100];
    struct tm* tm_info;

    time(&timer);
    tm_info = localtime(&timer);

    strftime(formattedTime, sizeof(formattedTime), "[%F %T]", tm_info);
    (void)fprintf(_logFile, "%s %s\n", formattedTime, [formattedString UTF8String]);
    (void)fflush(_logFile);
}

@end
