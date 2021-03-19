//
//  IniDownloader.m
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "IniDownloader.h"
#include "iniparser/src/iniparser.h"

@implementation IniDownloader
{
    id<IniDownloaderDelegate> _delegate;
    NSString *_downloadLocation;
    NSString *_ini_url;
    NSString *_ini_location;
    NSMutableArray *_ini_urls;
    int _ini_current_url;
    NSMutableArray *_ini_errors;
    DownloadLogger *_logger;
    NSString *_userAgentString;
}

- (IniDownloader *)initWithPath:(NSString *)downloadLocation
{
    self = [self init];
    _ini_errors = [[NSMutableArray alloc] init];
    _downloadLocation = downloadLocation;
    return self;
}

- (void)run:(id)delegate logger:(DownloadLogger *)logger userAgent:(NSString *)userAgentString
{
    _delegate = delegate;
    _logger = logger;
    _userAgentString = userAgentString;

    _ini_location = [NSString stringWithFormat: @"%@/install.ini", _downloadLocation];

    _ini_urls = [[NSMutableArray alloc] init];

    // get custom ini url, if passed on the cmdline
    NSArray *arguments = [[NSProcessInfo processInfo] arguments];

    NSString *iniarg = @"-ini=";
    [arguments enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        NSString *arg = obj;
        if ([arg length] > [iniarg length] && [[arg substringToIndex: [iniarg length]] isEqualToString: iniarg])
        {
            arg = [arg substringFromIndex: [iniarg length]];
            [_ini_urls addObject: arg];
        }
    }];
    if ([_ini_urls count])
    {
        _ini_current_url = 0;
        [self downloadCurrent];
        return;
    }

    // get ini urls from bundle's settings.ini
    NSBundle * myMainBundle = [NSBundle mainBundle];
    NSString * settingsFilePath = [myMainBundle pathForResource: @"settings" ofType: @"ini"];

    dictionary *settings = iniparser_load([settingsFilePath UTF8String]);

    if (!settings)
    {
        [_delegate iniFailedDownloading: @"Failed to load settings.ini"];
        return;
    }

    int mirrorcnt = iniparser_getsecnkeys(settings, "settings");
    char **mirrorkeys = iniparser_getseckeys(settings, "settings");
    for (int m = 0; m < mirrorcnt; m++)
    {
        if (strstr(mirrorkeys[m], "ini_url"))
        {
            char *url = iniparser_getstring(settings, mirrorkeys[m], NULL);
            if (url)
            {
                [_ini_urls addObject: [NSString stringWithUTF8String: url]];
            }
        }
    }
    iniparser_freedict(settings);

    if (![_ini_urls count])
    {
        [_delegate iniFailedDownloading: @"Failed to get ini_url from settings.ini"];
        return;
    }

    _ini_current_url = 0;
    [self downloadCurrent];
}

- (void)downloadCurrent
{
    _ini_url = _ini_urls[_ini_current_url];

    NSURL *nsurl = [NSURL URLWithString: _ini_url];

    [_logger log: @"Preparing to download installer ini file from the URL: %@", _ini_url];
    NSMutableURLRequest *req = [[NSMutableURLRequest alloc] initWithURL: nsurl];

    if (!req)
    {
        [_logger log: @"Failed to initialize url request for URL: %@", _ini_url];
        [_delegate iniFailedDownloading: @"Failed to initialize url request"];
        return;
    }

    [req setCachePolicy: NSURLRequestReloadIgnoringLocalCacheData];
    [req setTimeoutInterval: 15.0];
    [req addValue: _userAgentString forHTTPHeaderField: @"User-Agent"];

    NSURLDownload *dl = [[NSURLDownload alloc] initWithRequest: req delegate: (id<NSURLDownloadDelegate>)self];
    if (!dl)
    {
        [_logger log: @"Failed to initialize download request for URL: %@", _ini_url];
        [_delegate iniFailedDownloading: @"Failed to initialize download request"];
        return;
    }
    [dl setDestination: _ini_location allowOverwrite: YES];
}

// NSURLDownloadDelegate
- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
    if (_ini_current_url == 0)
    {
        [_ini_errors removeAllObjects];
    }
    [_logger log: @"Downloading failed with error for URL: %@", _ini_url];
    [_logger log: @"Error description: %@", [error localizedDescription]];
    [_logger log: @"Error failure reason: %@", [error localizedFailureReason]];
    [_logger log: @"Error recovery suggestion: %@", [error localizedRecoverySuggestion]];
    [_ini_errors addObject: error];
    if (_ini_current_url < [_ini_urls count] - 1)
    {
        [_logger log: @"Trying next INI URL"];
        _ini_current_url++;
        [self downloadCurrent];
        return;
    }
    NSString *errString = @"";
    for (NSError *err in _ini_errors)
    {
        errString = [NSString stringWithFormat: @"%@%@\n%@\n", errString, [err localizedDescription], [[err userInfo] objectForKey: NSURLErrorFailingURLStringErrorKey]];
    }

    [_logger log: @"No suitable mirror found"];
    [_delegate iniFailedDownloading: errString];
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
    [_logger log: @"Successfully downloaded INI from URL: %@", _ini_url];

    NSRange range = [_ini_url rangeOfString: @"/" options: NSBackwardsSearch];
    if (range.location != NSNotFound)
    {
        _ini_url_base = [_ini_url substringToIndex: range.location];
    }
    else
    {
        _ini_url_base = @"";
    }

    [_logger log: @"Resolving the base URL: %@", _ini_url_base];
    NSURL *validURL = [NSURL URLWithString: _ini_url_base];
    NSString *host = [validURL host];
    NSString *ipAddress = [[NSHost hostWithName: host] address];
    [_logger log: @"IP address: %@", ipAddress];

    [_delegate iniFinishedDownloading: _ini_location];
}

- (void)download:(NSURLDownload *)download didReceiveResponse:(NSURLResponse *)response
{
    [_logger log: @"Received response for download from URL: %@", _ini_url];
    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
    if ([httpResponse respondsToSelector: @selector(allHeaderFields)])
    {
        NSDictionary *dict = [httpResponse allHeaderFields];
        [_logger log: @"The headers: %@", dict];
    }
}

@end
