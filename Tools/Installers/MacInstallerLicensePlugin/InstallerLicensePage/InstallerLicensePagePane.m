#import "InstallerLicensePagePane.h"

@implementation InstallerLicensePagePane
{
    BOOL _haveUpdatedEula;
}

+ (NSString *)updatedEULAPath
{
    return [NSString stringWithFormat: @"%@/UnityEULA.rtf", NSTemporaryDirectory()];
}

// NSURLDownloadDelegate
- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
    [loadProgressIndicator setHidden: YES];
    [loadProgressIndicator stopAnimation: self];
    [loadProgressIndicatorLabel setHidden: YES];
    [_installerUtil adjustLicenseView: licenseTextView];
    [licenseTextViewWrapper setHidden: NO];
    self.nextEnabled = YES;
}

- (void)downloadDidFinish:(NSURLDownload *)download
{
    [licenseTextView readRTFDFromFile: [InstallerLicensePagePane updatedEULAPath]];
    [loadProgressIndicator setHidden: YES];
    [loadProgressIndicator stopAnimation: self];
    [loadProgressIndicatorLabel setHidden: YES];
    [_installerUtil adjustLicenseView: licenseTextView];
    [licenseTextViewWrapper setHidden: NO];
    self.nextEnabled = YES;
    _haveUpdatedEula = YES;
}

// end of NSURLDownloadDelegate

// HACK: find the coordinates for positioning the Print and Save buttons,
// find the Next button (which is the 2nd button in the view),
// then reparent and reposition the Print and Save buttons to be in the correct NSView.
//
// This hack is needed because there's no public API for modifying the main window layout,
// which would let us know how to position the Print and Save buttons for the License page.
- (void)hackTheLayout
{
    int y = 0;
    int n = 0;

    NSView *buttonContainer = licenseTextViewWrapper;
    while (buttonContainer)
    {
        if ([[buttonContainer subviews] count] == 6)
        {
            break;
        }
        buttonContainer = [buttonContainer superview];
    }
    if (!buttonContainer)
    {
        return;
    }

    NSArray *subviews = [buttonContainer subviews];

    for (NSView *view in subviews)
    {
        if ([view isKindOfClass: [NSButton class]])
        {
            if (n == 0)
            {
                y = [view frame].origin.y;
            }
            if (n == 1)
            {
                _nextButton = (NSButton*)view;
                break;
            }
            n++;
        }
    }

    if (!_nextButton)
    {
        return;
    }

    NSRect frame = [printButton frame];
    [printButton removeFromSuperview];
    [buttonContainer addSubview: printButton];
    frame.origin.x = [[[licenseTextViewWrapper superview] superview] frame].origin.x;
    if (frame.origin.x == 0)
    {
        frame.origin.x = [[licenseTextViewWrapper superview] frame].origin.x;
    }
    frame.origin.y = y;
    [printButton setFrame: frame];

    int x = frame.origin.x  + frame.size.width;
    frame = [saveButton frame];
    [saveButton removeFromSuperview];
    [buttonContainer addSubview: saveButton];
    frame.origin.x = x;
    frame.origin.y = y;
    [saveButton setFrame: frame];
}

- (NSString *)getBundledEulaFile
{
    return [[NSBundle bundleForClass: [self class]] pathForResource: @"License" ofType: @"rtf"];
}

- (void)didEnterPane:(InstallerSectionDirection)dir
{
    [super didEnterPane: dir];

    if (!_installerUtil)
    {
        _installerUtil = [[MacInstallerUtil alloc] init];
        [_installerUtil setURLDownloadDelegate: (id<NSURLDownloadDelegate>)self];
    }

    if (!_nextButton)
    {
        [self hackTheLayout];
    }

    [printButton setHidden: NO];
    [saveButton setHidden: NO];

    [loadProgressIndicator setHidden: NO];
    [loadProgressIndicatorLabel setHidden: NO];
    [licenseTextViewWrapper setHidden: YES];

    [loadProgressIndicator startAnimation: self];

    [licenseTextView setTextContainerInset: NSMakeSize(15.f, 5.f)];
    NSString *filePath = [self getBundledEulaFile];
    [licenseTextView readRTFDFromFile: filePath];

    _licenseAccepted = NO;
    self.nextEnabled = NO;

    NSString *licenseURL = [NSString stringWithUTF8String: LICENSE_URL];
    if ([licenseURL isEqualToString: @""])
    {
        licenseURL = @"http://unity3d.com/files/eula/License.rtf";
    }

    [_installerUtil downloadEULA: licenseURL withTempFilePath: [InstallerLicensePagePane updatedEULAPath]];
}

- (void)didExitPane:(InstallerSectionDirection)dir
{
    [printButton setHidden: YES];
    [saveButton setHidden: YES];
}

- (NSString *)title
{
    return [[NSBundle bundleForClass: [self class]] localizedStringForKey: @"PaneTitle" value: nil table: nil];
}

// Printing from the visible NSView in the Installer.app is bugged, and causes first page to be flipped.
// Create an invisible NSTextView and print from it instead.
- (NSTextView *)getPrintableLicenseView
{
    NSTextView *tv = [[NSTextView alloc] initWithFrame: NSMakeRect(0, 0, 400, 400)];
    [tv readRTFDFromFile: _haveUpdatedEula ? [InstallerLicensePagePane updatedEULAPath] : [self getBundledEulaFile]];
    return tv;
}

- (IBAction)printClicked:(id)sender
{
    [[self getPrintableLicenseView] print: self];
}

- (IBAction)saveClicked:(id)sender
{
    [_installerUtil saveLicense: [self getPrintableLicenseView]];
}

- (void)didEndLicenseSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut: self];

    if (returnCode == NSOKButton)
    {
        _licenseAccepted = YES;

        // HACK: trigger the next page, by activating the next button.
        // Needed because there's no public API to switch to the next page.
        [_nextButton sendAction: _nextButton.action to: _nextButton.target];
    }
}

- (BOOL)shouldExitPane:(InstallerSectionDirection)dir
{
    if (dir != InstallerDirectionForward || _licenseAccepted)
    {
        return YES;
    }

    [NSApp beginSheet: licenseSheet modalForWindow: [licenseTextViewWrapper window] modalDelegate: self didEndSelector: @selector(didEndLicenseSheet:returnCode:contextInfo:) contextInfo: nil];
    return NO;
}

- (IBAction)licenseSheetClose:(id)sender
{
    [NSApp endSheet: licenseSheet returnCode: ([sender tag] == 1) ? NSOKButton : NSCancelButton];
    if ([sender tag] == 0)
    {
        [NSApp terminate: self];
    }
}

@end
