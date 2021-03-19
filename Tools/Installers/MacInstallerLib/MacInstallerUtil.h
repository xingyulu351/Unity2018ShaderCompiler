#pragma once

#import <Cocoa/Cocoa.h>

@interface MacInstallerUtil : NSObject
{
    id<NSURLDownloadDelegate> _urlDownloadDelegate;
}

- (void)downloadEULA:(NSString *)url withTempFilePath:(NSString *)tempFilePath;
- (void)setURLDownloadDelegate:(id<NSURLDownloadDelegate>)delegate;
- (void)adjustLicenseView:(NSTextView *)licenseView;
- (void)saveLicense:(NSTextView *)licenseView;
@end
