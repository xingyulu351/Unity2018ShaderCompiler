#import <InstallerPlugins/InstallerPlugins.h>
#import "MacInstallerUtil.h"

@interface InstallerLicensePagePane : InstallerPane
{
    BOOL _licenseAccepted;
    NSButton *_nextButton;

    IBOutlet NSView *licenseTextViewWrapper;
    IBOutlet NSTextView *licenseTextView;
    IBOutlet NSProgressIndicator *loadProgressIndicator;
    IBOutlet NSTextField *loadProgressIndicatorLabel;
    IBOutlet NSButton *printButton;
    IBOutlet NSButton *saveButton;
    IBOutlet NSPanel *licenseSheet;
    MacInstallerUtil *_installerUtil;
}
- (IBAction)printClicked:(id)sender;
- (IBAction)saveClicked:(id)sender;
- (IBAction)licenseSheetClose:(id)sender;
@end
