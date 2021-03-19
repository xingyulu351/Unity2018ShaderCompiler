//
//  ComponentDownloader.m
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "ComponentDownloader.h"
#import "AppDelegate.h"

@implementation ComponentDownloader
{
    id _delegate;
    dictionary *_ini;
    DownloadLogger *_logger;
    int _idx;
    NSURLRequest *_currentRequest;
    NSURLDownload *_urlDownload;
    NSInteger _fileSize;
    NSInteger _receivedSize;
    NSString *_currentPath;
    NSString *_currentURL;
    uint64_t _currentFileSize;

    NSString *_resumePath;
    NSData *_resumeData;

    NSMutableArray *_haveDownloads;

    NSString *_userAgentString;
}


static uint64_t
GetFreeSpaceForFilePath(NSString *path)
{
    NSError *err;
    NSDictionary *attrs = [[NSFileManager defaultManager] attributesOfFileSystemForPath: [path stringByDeletingLastPathComponent] error: &err];

    NSNumber *size = attrs[NSFileSystemFreeSize];

    return [size longLongValue];
}

- (void)init:(id<ComponentDownloaderDelegate>)delegate ini:(dictionary *)ini logger:(DownloadLogger *)logger userAgent:(NSString *)userAgentString
{
    _delegate = delegate;
    _ini = ini;
    _logger = logger;
    _userAgentString = userAgentString;

    _haveDownloads = [[NSMutableArray alloc] init];
}

- (void)download:(int)idx
{
    _idx = idx;

    _currentPath = [_delegate getPathForIndex: _idx];
    _currentFileSize = [_delegate componentFileSizeForIndex: _idx];

    if ([_haveDownloads containsObject: _currentPath])
    {
        [_delegate componentDownloadStarted: _idx];
        [_delegate componentFinishedDownloading: _currentPath idx: _idx];

        return;
    }

    [self downloadCurrent];
}

- (void)downloadCurrent
{
    char *sec = iniparser_getsecname(_ini, _idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:url", sec);
    char *url = iniparser_getstring(_ini, n, NULL);

    [_logger log: @"Preparing to download from URL %s", url];

    _currentRequest = nil;
    NSMutableURLRequest *req = [[NSMutableURLRequest alloc] initWithURL: [NSURL URLWithString: [NSString stringWithUTF8String: url]]];

    if (!req)
    {
        [_logger log: @"Failed to initialize URL request: %s", url];
        [AppDelegate messageBox: @"Failed to initialize url request"];
        return;
    }

    [req setCachePolicy: NSURLRequestReloadIgnoringLocalCacheData];
    [req setTimeoutInterval: 15.0];
    [req addValue: _userAgentString forHTTPHeaderField: @"User-Agent"];

    BOOL resuming = NO;

    if (_resumeData && _resumePath && [_currentPath isEqualToString: _resumePath])
    {
        resuming = YES;
        [_logger log: @"Resuming downloading from URL: %s", url];
        _urlDownload = [[NSURLDownload alloc] initWithResumeData: _resumeData delegate: (id<NSURLDownloadDelegate>)self path: _currentPath];
        _resumePath = nil;
        _resumeData = nil;
    }
    else
    {
        unlink([_currentPath UTF8String]);
        uint64_t freespace = GetFreeSpaceForFilePath(_currentPath);
        if (freespace < _currentFileSize)
        {
            [_logger log: @"Not enough disk space to download from URL: %s", url];
            [_delegate componentDownloadFailed: _idx withError: @"Not enough disk space to download the component"];
            return;
        }

        _urlDownload = [[NSURLDownload alloc] initWithRequest: req delegate: (id<NSURLDownloadDelegate>)self];
    }
    if (!_urlDownload)
    {
        [_logger log: @"Failed to initialize download request for URL: %s", url];
        [_delegate componentDownloadFailed: _idx withError: @"Failed to initialize download request"];
        return;
    }

    // Don't delete the pkg files on failures, because we support retry/resume.
    [_urlDownload setDeletesFileUponFailure: NO];

    if (!resuming)
    {
        _fileSize = 0;
        _receivedSize = 0;
        [_urlDownload setDestination: _currentPath allowOverwrite: YES];
    }

    [_logger log: @"Download has started for URL: %s", url];
    _currentURL = [NSString stringWithUTF8String: url];
    _currentRequest = req;

    [_delegate componentDownloadStarted: _idx];
}

// NSURLDownloadDelegate

- (void)download:(NSURLDownload *)download didReceiveResponse:(NSURLResponse *)response
{
    [_logger log: @"Received response for download from URL: %@", _currentURL];
    _fileSize = [response expectedContentLength];
    NSHTTPURLResponse *httpResponse = (NSHTTPURLResponse*)response;
    if ([httpResponse respondsToSelector: @selector(allHeaderFields)])
    {
        NSDictionary *dict = [httpResponse allHeaderFields];
        [_logger log: @"The headers: %@", dict];
    }
}

- (void)download:(NSURLDownload *)download didReceiveDataOfLength:(NSUInteger)length
{
    _receivedSize += length;
    [_delegate setComponentDownloadProgress: (float)_receivedSize / _fileSize idx: _idx];
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
    _resumePath = _currentPath;
    _resumeData = [_urlDownload resumeData];

    [_logger log: @"Downloading failed with error for URL: %@", _currentURL];
    [_logger log: @"Error description: %@", [error localizedDescription]];
    [_logger log: @"Error failure reason: %@", [error localizedFailureReason]];
    [_logger log: @"Error recovery suggestion: %@", [error localizedRecoverySuggestion]];
    [_delegate componentDownloadFailed: _idx withError: [error localizedDescription]];
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
    [_logger log: @"Downloading has finished successfully for URL: %@", _currentURL];
    [_haveDownloads addObject: _currentPath];
    [_delegate componentFinishedDownloading: _currentPath idx: _idx];
}

// end of NSURLDownloadDelegate

- (void)cancelCurrent
{
    [_logger log: @"Downloading has been cancelled by the user, URL: %@", _currentURL];
    [_urlDownload cancel];
    _resumePath = _currentPath;
    _resumeData = [_urlDownload resumeData];
}

- (BOOL)isPaused
{
    return _resumeData ? YES : NO;
}

@end
