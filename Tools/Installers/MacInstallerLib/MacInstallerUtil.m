// Shared utility code for MacBootstrapper and InstallerLicensePage

#import <Foundation/Foundation.h>
#import "MacInstallerUtil.h"

@implementation MacInstallerUtil

- (void)downloadEULA:(NSString *)url withTempFilePath:(NSString *)tempFilePath
{
    NSURLRequest *req = [NSURLRequest requestWithURL: [NSURL URLWithString: url]
                         cachePolicy: NSURLRequestReloadIgnoringCacheData
                         timeoutInterval: 15.0];

    if (!req)
    {
        return;
    }

    NSURLDownload *urlDownload = [[[NSURLDownload alloc] initWithRequest: req delegate: _urlDownloadDelegate] autorelease];
    [urlDownload setDeletesFileUponFailure: YES];

    [urlDownload setDestination: tempFilePath allowOverwrite: YES];
}

- (void)setURLDownloadDelegate:(id<NSURLDownloadDelegate>)delegate
{
    _urlDownloadDelegate = delegate;
}

- (void)adjustLicenseView:(NSTextView *)licenseView
{
    // The RTF file fonts look much smaller on Mac, than on Windows.
    // Make them slightly bigger.
    NSTextStorage *res = [licenseView textStorage];
    [res beginEditing];
    [res enumerateAttribute: NSFontAttributeName inRange: NSMakeRange(0, res.length) options: 0 usingBlock:^(id value, NSRange range, BOOL *stop) {
        if (value)
        {
            NSFont *oldFont = (NSFont*)value;
            NSFont *newFont = [NSFont fontWithDescriptor: oldFont.fontDescriptor size: oldFont.pointSize * 1.4f];
            [res removeAttribute: NSFontAttributeName range: range];
            [res addAttribute: NSFontAttributeName value: newFont range: range];
        }
    }];
    [res endEditing];
    [licenseView setNeedsDisplay: YES];
}

- (void)saveLicense:(NSTextView *)licenseView
{
    NSSavePanel* saveDlg = [NSSavePanel savePanel];
    [saveDlg setNameFieldStringValue: @"License.pdf"];

    if ([saveDlg runModal] == NSOKButton)
    {
        NSPrintInfo *printInfo;
        NSPrintInfo *sharedInfo;
        NSPrintOperation *printOp;
        NSMutableDictionary *printInfoDict;
        NSMutableDictionary *sharedDict;

        sharedInfo = [NSPrintInfo sharedPrintInfo];
        sharedDict = [sharedInfo dictionary];
        printInfoDict = [NSMutableDictionary dictionaryWithDictionary: sharedDict];

        [printInfoDict setObject: NSPrintSaveJob forKey: NSPrintJobDisposition];
        [printInfoDict setObject: [saveDlg URL] forKey: NSPrintJobSavingURL];

        printInfo = [[[NSPrintInfo alloc] initWithDictionary: printInfoDict] autorelease];
        [printInfo setHorizontalPagination: NSAutoPagination];
        [printInfo setVerticalPagination: NSAutoPagination];
        [printInfo setVerticallyCentered: NO];

        printOp = [NSPrintOperation printOperationWithView: licenseView printInfo: printInfo];
        [printOp setShowsProgressPanel: NO];
        [printOp setShowsPrintPanel: NO];
        [printOp runOperation];
    }
}

@end
