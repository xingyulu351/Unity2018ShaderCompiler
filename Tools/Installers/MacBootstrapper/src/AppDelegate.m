//
//  AppDelegate.m
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 04/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "AppDelegate.h"
#import "IniDownloader.h"
#import "ComponentDownloader.h"
#import "ComponentInstaller.h"
#import "VolumeModel.h"
#import "DownloadLogger.h"
#import "FileMD5Hash/FileHash.h"
#import "MacInstallerUtil.h"
#include "iniparser/src/iniparser.h"
#include <sys/stat.h>

#define SEC_BETWEEN_FREESPACE_CHECKS 3

@interface NSAttributedString (Hyperlink)
+ (id)hyperlinkFromString:(NSString*)inString withURL:(NSURL*)aURL;
@end

@implementation NSAttributedString (Hyperlink)

+ (id)hyperlinkFromString:(NSString*)inString withURL:(NSURL*)aURL
{
    NSMutableAttributedString* attrString = [[NSMutableAttributedString alloc] initWithString: inString];
    NSRange range = NSMakeRange(0, [attrString length]);

    [attrString beginEditing];
    [attrString addAttribute: NSLinkAttributeName value: [aURL absoluteString] range: range];
    // make the text appear in blue
    [attrString addAttribute: NSForegroundColorAttributeName value: [NSColor blueColor] range: range];
    // next make the text appear with an underline
    [attrString addAttribute: NSUnderlineStyleAttributeName value: [NSNumber numberWithInt: NSSingleUnderlineStyle] range: range];
    [attrString endEditing];

    return attrString;
}

@end

@interface AppDelegate ()
{
    AuthorizationRef _auth;
    IniDownloader *_iniDownloader;
    ComponentDownloader *_compDownloader;
    ComponentInstaller *_compInstaller;
    BOOL _userHasAcceptedLicense;
    CALayer *_layer;
    NSImage *_backimg;
    BOOL _isInstalling;
    BOOL _isDownloading;
    int _currScreen;
    int _maxScreen;
    NSString *_selectedVolume;
    BOOL _isCustomLocation;
    BOOL _md5VerifyCancel;
    NSTimer *_freespaceTimer;
    BOOL _unityIsInstalled;
    DownloadLogger *_downloadLogger;
    NSString *_userAgentString;
    NSString *_eulaURL;
    BOOL _haveCustomEulaURL;
    BOOL _updatedLicenseDownloadFinished;
    MacInstallerUtil *_installerUtil;
    BOOL _componentLicenseRequired;
    BOOL _componentLicenseAccepted;
    const char* _componentEULAMessage;
    const char* _componentEULALabel1;
    const char* _componentEULAUrl1;
    const char* _componentEULALabel2;
    const char* _componentEULAUrl2;
}
@end

@implementation AppDelegate

@synthesize volumeModelArray;

static NSString *g_DownloadLocation;

enum
{
    PAGE_INTRO,
    PAGE_LICENSE,
    PAGE_COMPONENTS,
    PAGE_COMPONENTLICENSE,
    PAGE_DEST,
    PAGE_DL,
    PAGE_INST,
    PAGE_SUMMARY,
    PAGE_MAX
};

static NSString *pageTitles[] =
{
    @"Welcome to the Unity Download Assistant",
    @"Software License Agreement",
    @"Unity component selection",
    @"End User License Agreement",
    @"Select a Destination",
    @"Downloading Unity components",
    @"Installing Unity",
    @"The installation was completed successfully."
};

static NSTabViewItem *pages[PAGE_MAX];
static NSImageView *bullets[PAGE_MAX];
static NSTextField *labels[PAGE_MAX];
static BOOL pageEnabled[PAGE_MAX];

static dictionary *ini = NULL;

static char *string_for_key_in_sec(const char *sec, const char *key, char *def)
{
    char n[100];
    snprintf(n, sizeof(n), "%s:%s", sec, key);
    return iniparser_getstring(ini, n, def);
}

static char *string_for_key(int idx, const char *key, char *def)
{
    char *sec = iniparser_getsecname(ini, idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:%s", sec, key);
    return iniparser_getstring(ini, n, def);
}

static int set_string_for_key(int idx, const char *key, const char *val)
{
    char *sec = iniparser_getsecname(ini, idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:%s", sec, key);
    return iniparser_set(ini, n, val);
}

static int bool_for_key(int idx, const char *key, int def)
{
    char *sec = iniparser_getsecname(ini, idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:%s", sec, key);
    return iniparser_getboolean(ini, n, def);
}

+ (void)messageBox:(NSString *)text
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert setMessageText: text];
    [alert runModal];
}

- (BOOL)isVolumeWritable:(NSString *)volPath
{
    NSPipe *pipe = [NSPipe pipe];
    NSFileHandle *file = pipe.fileHandleForReading;

    NSTask *task = [[NSTask alloc] init];
    task.launchPath = @"/usr/sbin/diskutil";
    task.arguments = @[@"info", volPath];
    task.standardOutput = pipe;

    [task launch];

    NSData *data = [file readDataToEndOfFile];
    [file closeFile];

    NSString *output = [[NSString alloc] initWithData: data encoding: NSUTF8StringEncoding];

    const char *str = [output UTF8String];
    str = strcasestr(str, "Read-Only Volume:");
    if (!str)
    {
        return NO;
    }

    str = strchr(str, ':');
    str++;
    while (*str && isblank(*str))
    {
        str++;
    }

    if (!*str)
    {
        return NO;
    }

    if (!strncasecmp(str, "No", 2))
    {
        return YES;
    }

    return NO;
}

- (void)initializeVolumeArray
{
    NSString *vol;
    NSImage *ic;

    NSMutableArray * tempArray = [[NSMutableArray alloc] init];

    _selectedVolume = @"/";

    int selectedVolumeIdx = -1;

    NSArray *vols = [[NSFileManager defaultManager] mountedVolumeURLsIncludingResourceValuesForKeys: nil options: NSVolumeEnumerationSkipHiddenVolumes];

    for (int vi = 0; vi < [vols count]; vi++)
    {
        NSURL *url = vols[vi];

        NSByteCountFormatter *cf = [NSByteCountFormatter alloc];
        cf.zeroPadsFractionDigits = YES;
        cf.countStyle = NSByteCountFormatterCountStyleFile;

        VolumeModel *v = [[VolumeModel alloc] init];
        vol = [url path];


        BOOL writable = YES;
        // In newer versions of MacOS the security policy has changed
        // and it has been recommended to us by Apple to not do this check
        // so we assume the volume is always writable.
        NSOperatingSystemVersion minimumSupportedOSVersion = { .majorVersion = 10, .minorVersion = 14, .patchVersion = 4 };

        if ([NSProcessInfo.processInfo isOperatingSystemAtLeastVersion: minimumSupportedOSVersion])
        {
            writable = YES;
        }
        else
        {
            writable = [self isVolumeWritable: vol];
        }

        ic = [[NSWorkspace sharedWorkspace] iconForFile: vol];
        ic.size = NSMakeSize(64, 64);
        v.icon = ic;
        v.name = [[NSFileManager defaultManager] displayNameAtPath: vol];
        BOOL removable;
        BOOL unmountable;
        NSString *descr;
        NSString *type;
        BOOL res = [[NSWorkspace sharedWorkspace] getFileSystemInfoForPath: vol isRemovable: &removable isWritable: NULL isUnmountable: &unmountable description: &descr type: &type];
        if (!res)
        {
            writable = NO;
        }
        v.path = vol;
        v.installable = writable;
        char real[PATH_MAX];
        if (realpath([vol UTF8String], real) && !strcmp(real, "/"))
        {
            selectedVolumeIdx = vi;
            _selectedVolume = v.path;
        }

        if (writable && selectedVolumeIdx == -1 && !_selectedVolume)
        {
            selectedVolumeIdx = vi;
            _selectedVolume = v.path;
        }

        NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfFileSystemForPath: vol error: nil];
        unsigned long long size = [[fileAttributes objectForKey: NSFileSystemSize] longLongValue];
        unsigned long long avail = [[fileAttributes objectForKey: NSFileSystemFreeSize] longLongValue];

        v.sizeAvailable = [NSString stringWithFormat: @"%@ available", [cf stringFromByteCount: avail]];
        v.sizeTotal = [NSString stringWithFormat: @"%@ total", [cf stringFromByteCount: size]];
        [tempArray addObject: v];
    }

    [self setVolumeModelArray: tempArray];


    [_volumeCollectionView addObserver: self forKeyPath: @"selectionIndexes" options: NSKeyValueObservingOptionNew context: NULL];
    [_volumeCollectionView setSelectionIndexes: [NSIndexSet indexSetWithIndex: selectedVolumeIdx]];
}

+ (NSString *)updatedEULAPath
{
    return [NSString stringWithFormat: @"%@/UnityEULA.rtf", NSTemporaryDirectory()];
}

- (void)initEULAFromFile:(NSString *)fname
{
    [_licenseTextView readRTFDFromFile: fname];
    [_installerUtil adjustLicenseView: _licenseTextView];
    [_pageTitle setHidden: YES];
    [_eulaTestTitle setHidden: NO];
}

// NSURLDownloadDelegate
- (void)downloadDidFinish:(NSURLDownload *)download
{
    [_licenseTextView readRTFDFromFile: [AppDelegate updatedEULAPath]];
    [_licenseLoadingProgressIndicator stopAnimation: self];
    [_licenseLoadingProgressIndicator setHidden: YES];
    [_licenseLoadingProgressLabel setHidden: YES];
    [_installerUtil adjustLicenseView: _licenseTextView];
    [_licenseTextContainer setHidden: NO];
    _updatedLicenseDownloadFinished = YES;
    if (_currScreen == PAGE_LICENSE)
    {
        [_nextButton setEnabled: YES];
    }
}

- (void)download:(NSURLDownload *)download didFailWithError:(NSError *)error
{
    [_licenseLoadingProgressIndicator stopAnimation: self];
    [_licenseLoadingProgressIndicator setHidden: YES];
    [_licenseLoadingProgressLabel setHidden: YES];
    [_licenseTextContainer setHidden: NO];
    [_installerUtil adjustLicenseView: _licenseTextView];
    _updatedLicenseDownloadFinished = YES;
    if (_currScreen == PAGE_LICENSE)
    {
        [_nextButton setEnabled: YES];
    }

    if (_haveCustomEulaURL)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle: @"Quit"];
        [alert setMessageText: [@"Could not load EULA from path: " stringByAppendingString: _eulaURL]];
        [alert setAlertStyle: NSCriticalAlertStyle];
        [alert runModal];
        [NSApp terminate: self];
    }
}

// end of NSURLDownloadDelegate

- (void)setupUserAgent
{
    _userAgentString = @"UnityDownloadAssistant/1.0";

    NSString *iniarg = @"-ua=";
    NSArray *arguments = [[NSProcessInfo processInfo] arguments];
    [arguments enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        NSString *arg = obj;
        if ([arg length] > [iniarg length] && [[arg substringToIndex: [iniarg length]] isEqualToString: iniarg])
        {
            arg = [arg substringFromIndex: [iniarg length]];
            _userAgentString = arg;
        }
    }];

    [[NSUserDefaults standardUserDefaults] registerDefaults: [NSDictionary dictionaryWithObject: _userAgentString forKey: @"UserAgent"]];
    [_downloadLogger log: @"UA String: %@", _userAgentString];
}

- (void)setupEulaURL
{
    _eulaURL = @"http://unity3d.com/files/eula/License.rtf";

    NSString *eulaarg = @"-eula=";
    NSArray *arguments = [[NSProcessInfo processInfo] arguments];
    [arguments enumerateObjectsUsingBlock:^(id obj, NSUInteger idx, BOOL *stop) {
        NSString *arg = obj;
        if ([arg length] > [eulaarg length] && [[arg substringToIndex: [eulaarg length]] isEqualToString: eulaarg])
        {
            arg = [arg substringFromIndex: [eulaarg length]];
            _eulaURL = arg;
            _haveCustomEulaURL = YES;
        }
    }];
}

- (void)logDownloadServerInfo
{
    FILE *fp = popen("host download.unity3d.com", "r");
    if (!fp)
    {
        [_downloadLogger log: @"Failed to run `host download.unity3d.com`"];
    }
    else
    {
        [_downloadLogger log: @"Running `host download.unity3d.com`..."];
        char buffer[1000];
        while (fgets(buffer, sizeof(buffer), fp))
        {
            char *p = buffer + strlen(buffer) - 1;
            while (p >= buffer && *p == '\n')
            {
                *p-- = 0;
            }
            [_downloadLogger log: @"%s", buffer];
        }
        fclose(fp);
    }
}

- (void)applicationDidFinishLaunching:(NSNotification *)aNotification
{
    _installerUtil = [[MacInstallerUtil alloc] init];
    [_installerUtil setURLDownloadDelegate: (id<NSURLDownloadDelegate>)self];

    NSBundle * myMainBundle = [NSBundle mainBundle];

    NSArray *apps = [NSRunningApplication runningApplicationsWithBundleIdentifier: [myMainBundle bundleIdentifier]];

    for (id app in apps)
    {
        if ([app isKindOfClass: [NSRunningApplication class]]
            && [app processIdentifier] != [NSProcessInfo processInfo].processIdentifier)
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle: @"Quit"];
            [alert setMessageText: @"Unity Download Assistant is already running."];
            [alert setAlertStyle: NSCriticalAlertStyle];
            [alert runModal];
            [NSApp terminate: self];
            return;
        }
    }

    // check if Unity is running, and display a warning dialog
    for (;;)
    {
        NSArray *apps = [[NSWorkspace sharedWorkspace] runningApplications];
        BOOL unityIsRunning = NO;
        for (NSRunningApplication *app in apps)
        {
            if ([[app bundleIdentifier] hasPrefix: @"com.unity3d.UnityEditor"])
            {
                unityIsRunning = YES;
                break;
            }
        }
        if (!unityIsRunning)
        {
            break;
        }
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle: @"Continue"];
        [alert addButtonWithTitle: @"Install anyway"];
        [alert addButtonWithTitle: @"Quit"];
        [alert setMessageText: @"Unity is already running on this computer."];
        [alert setInformativeText: @"Installation might cause Unity to crash, and you might lose any unsaved changes. Please save your work and close all open Unity windows, then click \"Continue\"."];
        [alert setAlertStyle: NSCriticalAlertStyle];
        NSModalResponse res = [alert runModal];
        if (res == NSAlertSecondButtonReturn)
        {
            break;
        }
        else if (res == NSAlertThirdButtonReturn)
        {
            [NSApp terminate: self];
            return;
        }
    }

    _backimg = [NSImage imageNamed: @"background"];

    [_introTextView setTextContainerInset: NSMakeSize(15.f, 5.f)];
    NSString * rtfFilePath = [myMainBundle pathForResource: @"Welcome" ofType: @"rtf"];
    [_introTextView readRTFDFromFile: rtfFilePath];

    [_licenseTextView setTextContainerInset: NSMakeSize(15.f, 5.f)];
    rtfFilePath = [myMainBundle pathForResource: @"License" ofType: @"rtf"];
    [_licenseTextView readRTFDFromFile: rtfFilePath];

    [_compLicenseTextView setTextContainerInset: NSMakeSize(15.f, 5.f)];

    [_licenseLoadingProgressIndicator startAnimation: self];

    [[NSURLCache sharedURLCache] removeAllCachedResponses];

    [self setupUserAgent];

    [self setupEulaURL];

    [_installerUtil downloadEULA: _eulaURL withTempFilePath: [AppDelegate updatedEULAPath]];

    [self initBackground];

    [self initializeVolumeArray];

    pages[PAGE_INTRO] = _introView;
    bullets[PAGE_INTRO] = _introBullet;
    labels[PAGE_INTRO] = _introLabel;

    pages[PAGE_LICENSE] = _licenseView;
    bullets[PAGE_LICENSE] = _licenseBullet;
    labels[PAGE_LICENSE] = _licenseLabel;

    pages[PAGE_COMPONENTS] = _componentsView;
    bullets[PAGE_COMPONENTS] = _compBullet;
    labels[PAGE_COMPONENTS] = _compLabel;

    pages[PAGE_COMPONENTLICENSE] = _componentsLicenseView;
    bullets[PAGE_COMPONENTLICENSE] = _compLicenseBullet;
    labels[PAGE_COMPONENTLICENSE] = _compLicenseLabel;

    pages[PAGE_DEST] = _destView;
    bullets[PAGE_DEST] = _destBullet;
    labels[PAGE_DEST] = _destLabel;

    pages[PAGE_DL] = _dlView;
    bullets[PAGE_DL] = _dlBullet;
    labels[PAGE_DL] = _dlLabel;

    pages[PAGE_INST] = _instView;
    bullets[PAGE_INST] = _instBullet;
    labels[PAGE_INST] = _instLabel;

    pages[PAGE_SUMMARY] = _summaryView;
    bullets[PAGE_SUMMARY] = _summaryBullet;
    labels[PAGE_SUMMARY] = _summaryLabel;

    for (int i = 0; i < PAGE_MAX; ++i)
        pageEnabled[i] = YES;

    [_customLocationPath setDelegate: (id<NSTextFieldDelegate>)self];
    [_customLocationPath setStringValue: @""];
    [self updateCustomLocation];

    [self setPage: PAGE_INTRO];

    [_window setIsVisible: YES];
    [NSApp activateIgnoringOtherApps: YES];

    _downloadLogger = [[DownloadLogger alloc] init];

    [self logDownloadServerInfo];

    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];
    id defAutorunObj = [def objectForKey: @"autorun_unity"];
    if (defAutorunObj)
    {
        BOOL install = [defAutorunObj boolValue];

        [_summaryViewLaunchUnityCheckButton setState: (install ? NSOnState : NSOffState)];
    }
}

- (void)controlTextDidChange:(NSNotification *)obj
{
    if ([obj object] == _customLocationPath)
    {
        [AppDelegate setDownloadLocation: (_isCustomLocation ? [_customLocationPath stringValue] : nil)];

        [self updateCustomLocation];
    }
}

+ (AuthorizationRef)requestAuthorization
{
    AuthorizationRef myAuthorizationRef;
    for (;;)
    {
        // Get the authorization
        OSStatus err = AuthorizationCreate(NULL, kAuthorizationEmptyEnvironment, kAuthorizationFlagDefaults, &myAuthorizationRef);
        if (err != errAuthorizationSuccess)
            return NULL;

        AuthorizationItem myItems = {kAuthorizationRightExecute, 0, NULL, 0};
        AuthorizationRights myRights = {1, &myItems};
        AuthorizationFlags myFlags = kAuthorizationFlagInteractionAllowed | kAuthorizationFlagPreAuthorize | kAuthorizationFlagExtendRights;

        err = AuthorizationCopyRights(myAuthorizationRef, &myRights, NULL, myFlags, NULL);
        if (err == errAuthorizationCanceled)
        {
            goto fail;
        }
        else if (err == errAuthorizationSuccess)
        {
            break;
        }

        AuthorizationFree(myAuthorizationRef, kAuthorizationFlagDefaults);
    }

    return myAuthorizationRef;
fail:
    AuthorizationFree(myAuthorizationRef, kAuthorizationFlagDefaults);
    return NULL;
}

- (uint64_t)getFreeSpaceForFilePath:(NSString *)path
{
    NSError *err;
    NSDictionary *attrs = [[NSFileManager defaultManager] attributesOfFileSystemForPath: path error: &err];

    NSNumber *size = attrs[NSFileSystemFreeSize];

    return [size longLongValue];
}

- (BOOL)verifyComponentFreeSpace
{
    if ([self getRequiredDownloadedSize] > [self getFreeSpaceForFilePath: [AppDelegate downloadLocation]])
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle: @"OK"];
        [alert setMessageText: @"Not enough free space."];
        [alert setInformativeText: @"There's not enough space to download the files. Please free up some space, or choose a different download location using the Advanced button."];
        [alert setAlertStyle: NSCriticalAlertStyle];
        [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: nil contextInfo: nil];
        return NO;
    }
    return YES;
}

- (void)setPage:(int)idx
{
    [_tabView selectTabViewItem: pages[idx]];
    _currScreen = idx;
    if (_maxScreen < idx)
    {
        _maxScreen = idx;
    }
    [self updateBulletList];

    [_saveButton setHidden: idx != PAGE_LICENSE];
    [_printButton setHidden: idx != PAGE_LICENSE];
    [_customLocationButton setHidden: idx != PAGE_DEST];

    [_downloadSpinner stopAnimation: self];
    switch (idx)
    {
        case PAGE_INST:
        case PAGE_SUMMARY:
            [_prevButton setEnabled: NO];
            break;
        default:
            [_prevButton setEnabled: YES];
            break;
    }
    switch (idx)
    {
        case PAGE_DL:
        case PAGE_INST:
            [_nextButton setEnabled: NO];
            break;
        case PAGE_LICENSE:
            [_nextButton setEnabled: _updatedLicenseDownloadFinished];
            break;
        default:
            [_nextButton setEnabled: YES];
            break;
    }

    if (idx == PAGE_SUMMARY)
    {
        int unitySelected = bool_for_key(0, "install", 0);
        [_summaryViewLaunchUnityCheckButton setHidden: !(unitySelected || _unityIsInstalled)];
    }


    if (_freespaceTimer)
    {
        [_freespaceTimer invalidate];
        _freespaceTimer = nil;
    }

    if (idx == PAGE_COMPONENTS)
    {
        _freespaceTimer = [NSTimer timerWithTimeInterval: SEC_BETWEEN_FREESPACE_CHECKS target: self selector: @selector(updateFreespace:) userInfo: nil repeats: YES];
        [[NSRunLoop currentRunLoop] addTimer: _freespaceTimer forMode: NSDefaultRunLoopMode];

        if (ini)
        {
            [_downloadSpinner stopAnimation: self];
            [_prevButton setEnabled: YES];
            [_nextButton setEnabled: YES];
            [self updateCompSizeInfo];
        }
        else
        {
            [_prevButton setEnabled: NO];
            [_nextButton setEnabled: NO];
            [_downloadSpinner startAnimation: self];
            _iniDownloader = [[IniDownloader alloc] initWithPath: [AppDelegate downloadLocation]];
            [_iniDownloader run: (id<IniDownloaderDelegate>)self logger: _downloadLogger userAgent: _userAgentString];
        }
    }
    else if (idx == PAGE_COMPONENTLICENSE)
    {
        if (_componentEULAMessage != NULL)
        {
            NSMutableAttributedString* contents = [[NSMutableAttributedString alloc] init];

            NSMutableAttributedString* eulaString = [[NSMutableAttributedString alloc] initWithString: [NSString stringWithUTF8String: _componentEULAMessage]];
            NSAttributedString *doubleNewline = [[NSAttributedString alloc] initWithString: @"\n\n"];

            [contents appendAttributedString: eulaString];
            [contents appendAttributedString: doubleNewline];

            if (_componentEULALabel1 != NULL && _componentEULAUrl1)
            {
                NSURL* url1 = [NSURL URLWithString: [NSString stringWithUTF8String: _componentEULAUrl1]];
                [contents appendAttributedString: [NSAttributedString hyperlinkFromString: [NSString stringWithUTF8String: _componentEULALabel1] withURL: url1]];
                [contents appendAttributedString: doubleNewline];
            }

            if (_componentEULALabel2 != NULL && _componentEULAUrl2)
            {
                NSURL* url2 = [NSURL URLWithString: [NSString stringWithUTF8String: _componentEULAUrl2]];
                [contents appendAttributedString: [NSAttributedString hyperlinkFromString: [NSString stringWithUTF8String: _componentEULALabel2] withURL: url2]];
                [contents appendAttributedString: doubleNewline];
            }

            [[_compLicenseTextView textStorage] setAttributedString: contents];
        }
        else
        {
            [_compLicenseTextView setString: @"Error: Unable to read EULA message"];
        }
    }
    else if (idx == PAGE_DL)
    {
        _md5VerifyCancel = NO;
        int idx = 0;
        int nsec = iniparser_getnsec(ini);
        while (idx < nsec)
        {
            int enabled = bool_for_key(idx, "install", 0);
            if (enabled)
            {
                _isDownloading = YES;
                [_compDownloader download: idx];
                return;
            }
            idx++;
        }

        [AppDelegate messageBox: @"No packages were selected to install"];
    }
    else if (idx == PAGE_INST)
    {
        _isInstalling = YES;
        [_compInstaller setVolume: _selectedVolume];
        int idx = 0;
        int nsec = iniparser_getnsec(ini);
        while (idx < nsec)
        {
            int enabled = bool_for_key(idx, "install", 0);
            if (enabled)
            {
                [_compInstaller install: idx];
                break;
            }
            idx++;
        }
        [_installProgressBar setDoubleValue: 0];
    }
    else if (idx == PAGE_DEST)
    {
        if (!_auth)
        {
            _auth = [AppDelegate requestAuthorization];
            if (!_auth)
            {
                [self setPage: idx - 1 - (_componentLicenseRequired ? 0 : 1)];
                return;
            }
        }

        [self initializeVolumeArray];
        if (!_compInstaller)
        {
            _compInstaller = [[ComponentInstaller alloc] init: (id<ComponentInstallerDelegate>)self auth: _auth];
        }
        [self updateSelectedVolume];
    }
    else if (idx == PAGE_SUMMARY)
    {
        [_nextButton setTitle: @"Close"];
    }
}

- (void)updateBulletList
{
    for (int i = 0; i < PAGE_MAX; i++)
    {
        if (!pageEnabled[i])
            continue;

        NSFontTraitMask mask = 0;

        if (i == _currScreen)
        {
            [bullets[i] setImage: [NSImage imageNamed: @"DotBlue"]];
            [labels[i]  setTextColor: [NSColor colorWithDeviceRed: 0 green: 0 blue: 0 alpha: 1]];
            mask = NSBoldFontMask;
        }
        else if (i <= _maxScreen)
        {
            [bullets[i] setImage: [NSImage imageNamed: @"DotGray"]];
            [labels[i]  setTextColor: [NSColor colorWithDeviceRed: 0 green: 0 blue: 0 alpha: 1]];
        }
        else
        {
            [bullets[i] setImage: [NSImage imageNamed: @"DotGrayDisabled"]];
            [labels[i]  setTextColor: [NSColor colorWithDeviceRed: 0.57 green: 0.57 blue: 0.57 alpha: 1]];
        }
        NSFont *fnt = [labels[i] font];
        NSFontManager *mgr = [NSFontManager sharedFontManager];
        NSFont *boldFont = [mgr fontWithFamily: [fnt familyName] traits: mask weight: 5 size: [fnt pointSize]];
        [labels[i] setFont: boldFont];
    }

    [_pageTitle setStringValue: pageTitles[_currScreen]];
}

- (void)updateSelectedVolume
{
    NSUInteger idx = [[[self volumeCollectionView] selectionIndexes] firstIndex];
    if (idx != NSNotFound)
    {
        VolumeModel *vol = [volumeModelArray objectAtIndex: idx];
        _selectedVolume = vol.path;
        NSString *descr;
        if (![vol installable])
        {
            descr = @"\n\nUnity can't be installed on this disk. The contents of this disk can't be changed.";
            [_nextButton setEnabled: NO];
        }
        else
        {
            NSByteCountFormatter *cf = [NSByteCountFormatter alloc];
            cf.zeroPadsFractionDigits = YES;
            cf.countStyle = NSByteCountFormatterCountStyleFile;
            descr = [NSString stringWithFormat: @"\nDownloading and installing this software requires %@ of space.\n\nYou have chosen to install this software on the disk “%@”", [cf stringFromByteCount: [self getRequiredInstallSize] + [self getRequiredDownloadedSize]], [vol name]];
            [_nextButton setEnabled: YES];
        }
        [[self destDescriptionTextView] setString: descr];
    }
}

- (void)observeValueForKeyPath:(NSString *)keyPath ofObject:(id)object change:(NSDictionary *)change context:(void *)context
{
    if ([keyPath isEqualToString: @"selectionIndexes"])
    {
        // update descr
        [self updateSelectedVolume];
    }
}

// collectionView delegate
- (void)insertObject:(VolumeModel *)p inVolumeModelArrayAtIndex:(NSUInteger)index
{
    [volumeModelArray insertObject: p atIndex: index];
}

- (void)removeObjectFromVolumeModelArrayAtIndex:(NSUInteger)index
{
    [volumeModelArray removeObjectAtIndex: index];
}

- (void)setVolumeModelArray:(NSMutableArray *)a
{
    volumeModelArray = a;
}

- (NSArray*)volumeModelArray
{
    return volumeModelArray;
}

/*- (BOOL)layer:(CALayer *)layer shouldInheritContentsScale:(CGFloat)newScale fromWindow:(NSWindow *)window {
    return NO;
}

- (void)drawLayer:(CALayer *)layer inContext:(CGContextRef)ctx {
    CGFloat desiredScaleFactor = [_window backingScaleFactor];
    CGFloat actualScaleFactor = [_backimg recommendedLayerContentsScale:desiredScaleFactor];
    id layerContents = [_backimg layerContentsForContentsScale:actualScaleFactor];

    layer.contents = layerContents;

}*/

- (void)initBackground
{
    [[_window contentView] setWantsLayer: YES];
    _layer = [[_window contentView] layer];
    _layer.contentsGravity = kCAGravityTopLeft;

    CGFloat desiredScaleFactor = [_window backingScaleFactor];
    CGFloat actualScaleFactor = [_backimg recommendedLayerContentsScale: desiredScaleFactor];
    id layerContents = [_backimg layerContentsForContentsScale: actualScaleFactor];

    _layer.contents = layerContents;
    [_layer setContentsScale: actualScaleFactor];
}

- (void)iniFinishedDownloading:(NSString *)path
{
    if (ini)
    {
        iniparser_freedict(ini);
        ini = NULL;
    }
    ini = iniparser_load([path UTF8String]);
    if (!ini)
    {
        [self iniFailedDownloading: @"Failed to load the downloaded installer configuration file."];
        return;
    }

    _compDownloader = [ComponentDownloader alloc];
    [_compDownloader init: (id<ComponentDownloaderDelegate>)self ini: ini logger: _downloadLogger userAgent: _userAgentString];

    [_downloadSpinner stopAnimation: self];

    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];

    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        // patch ini URLs if they are relative
        const char *url = string_for_key(i, "url", NULL);
        if (url && strncmp(url, "http://", 7) && strncmp(url, "https://", 8))
        {
            char newurl[2000];
            snprintf(newurl, sizeof(newurl), "%s/%s", [[_iniDownloader ini_url_base] UTF8String], url);
            set_string_for_key(i, "url", newurl);
        }

        // update install status from NSUserDefaults
        char *sec = iniparser_getsecname(ini, (int)i);

        NSString *key = [@"install_" stringByAppendingString: [NSString stringWithUTF8String: sec]];
        id defInstallObj = [def objectForKey: key];
        if (defInstallObj)
        {
            BOOL install = [defInstallObj boolValue];
            char name[100];
            snprintf(name, sizeof(name), "%s:install", sec);
            iniparser_set(ini, name, install ? "true" : "false");
        }

        // Use appidentifier to check if app is already installed.
        const char* appidenfifier = string_for_key(i, "appidentifier", NULL);

        if (appidenfifier)
        {
            NSString* appPath = [[NSWorkspace sharedWorkspace] absolutePathForAppBundleWithIdentifier: [NSString stringWithUTF8String: appidenfifier]];

            if (appPath != nil)
            {
                set_string_for_key(i, "install", "false");
            }
        }
    }

    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        char *sec = iniparser_getsecname(ini, (int)i);
        int install = bool_for_key(i, "install", 0);
        [self updateDependencies: (BOOL)install component: sec];
    }


    // initialize the item list
    [_iniView setDelegate: (id<NSTableViewDelegate>)self];
    [_iniView setDataSource: (id<NSTableViewDataSource>)self];
    [self updateCompSizeInfo];
    [self updateComponentLicenseEnabled];
    [_nextButton setEnabled: YES];
    [_prevButton setEnabled: YES];
}

- (void)iniFailedDownloading:(NSString *)error
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle: @"Quit"];
    [alert addButtonWithTitle: @"Retry"];
    [alert setMessageText: @"Failed to download the installer configuration."];
    [alert setInformativeText: error];
    [alert setAlertStyle: NSWarningAlertStyle];
    [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(iniDownloadFailedResponse:returnCode:contextInfo:) contextInfo: nil];
}

- (void)iniDownloadFailedResponse:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertFirstButtonReturn)
    {
        [NSApp terminate: self];
    }
    else
    {
        [_iniDownloader run: (id<IniDownloaderDelegate>)self logger: _downloadLogger userAgent: _userAgentString];
    }
}

- (void)componentDownloadStarted:(int)idx
{
    char *sec = iniparser_getsecname(ini, (int)idx);
    char n[200];
    snprintf(n, sizeof(n), "%s:title", sec);
    char *title = iniparser_getstring(ini, n, "undef");
    int nsec = 0;
    int curr_idx = 0;
    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        if (bool_for_key(i, "install", 0))
        {
            if (idx == i)
            {
                curr_idx = nsec;
            }
            nsec++;
        }
    }

    NSString *comp = [NSString stringWithFormat: @"%s (%d of %d)", title, curr_idx + 1, nsec];
    [_currentDlComponent setStringValue: comp];
}

- (void)componentDownloadFailed:(int)idx withError:(NSString *)error
{
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle: @"Quit"];
    [alert addButtonWithTitle: @"Retry"];
    [alert setMessageText: @"Failed to download the component."];
    [alert setInformativeText: error];
    [alert setAlertStyle: NSCriticalAlertStyle];
    [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(componentDownloadFailedResponse:returnCode:contextInfo:) contextInfo: nil];
}

- (void)componentDownloadFailedResponse:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertFirstButtonReturn)
    {
        _isDownloading = NO;
        [NSApp terminate: self];
    }
    else
    {
        [_compDownloader downloadCurrent];
    }
}

- (void)md5Verify:(NSString*)path
{
    int idx = _compDownloader.idx;
    if (!string_for_key(idx, "md5", NULL))
    {
        [_downloadLogger log: @"Skipping MD5 verification for file: %@", path];
        [self performSelectorOnMainThread: @selector(componentMD5Success) withObject: nil waitUntilDone: NO];
        return;
    }
    [_downloadLogger log: @"Calculating MD5 sum for file: %@", path];
    NSString *md5sum = [FileHash md5HashOfFileAtPath: path];
    if (_md5VerifyCancel)
    {
        _md5VerifyCancel = NO;
        _isDownloading = NO;
        return;
    }

    if ([md5sum isEqualToString: [NSString stringWithUTF8String: string_for_key(idx, "md5", "")]])
    {
        [_downloadLogger log: @"MD5 verification successful for file: %@", path];
        [self performSelectorOnMainThread: @selector(componentMD5Success) withObject: nil waitUntilDone: NO];
    }
    else
    {
        [_downloadLogger log: @"MD5 verification failed for file: %@", path];
        [_downloadLogger log: @"Expected: %s", string_for_key(idx, "md5", "")];
        [_downloadLogger log: @"Obtained: %@", md5sum];
        [self performSelectorOnMainThread: @selector(componentMD5Fail) withObject: nil waitUntilDone: NO];
    }
}

- (void)componentMD5Success
{
    int idx = _compDownloader.idx;
    int nsec = iniparser_getnsec(ini);
    idx++;
    while (idx < nsec)
    {
        int enabled = bool_for_key(idx, "install", 0);
        if (enabled)
        {
            [_compDownloader download: idx];
            [_prevButton setEnabled: YES];
            return;
        }
        idx++;
    }

    _isDownloading = NO;
    [_prevButton setEnabled: YES];
    [self setPage: PAGE_INST];
}

- (void)componentMD5Fail
{
    [_prevButton setEnabled: YES];
    _isDownloading = NO;
    NSAlert *alert = [[NSAlert alloc] init];
    [alert addButtonWithTitle: @"Quit"];
    [alert setMessageText: @"Component MD5 verification failed"];
    [alert setInformativeText: @"Downloaded content seems to be corrupt. Restarting the installer, and trying again, might help. The installer will quit now."];
    [alert setAlertStyle: NSCriticalAlertStyle];
    [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(componentMD5FailAlertResponse) contextInfo: nil];
}

- (void)componentMD5FailAlertResponse
{
    [NSApp terminate: self];
}

- (void)componentFinishedDownloading:(NSString *)path idx:(int)idx
{
    // md5 check
    [_compDownloadStatus setStringValue: @"Verifying the downloaded data"];

    // we can't abort md5 verification, and it's running asynchronously, so disable going back.
    [_prevButton setEnabled: NO];
    dispatch_queue_t aQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    dispatch_async(aQueue, ^{
        [self md5Verify: path];
    });
}

- (void)setComponentDownloadProgress:(float)progress idx:(int)idx
{
    [_compDownloadProgress setDoubleValue: progress * 100];

    NSByteCountFormatter *cf = [NSByteCountFormatter alloc];
    cf.zeroPadsFractionDigits = YES;
    cf.countStyle = NSByteCountFormatterCountStyleFile;
    NSString *mb = [cf stringFromByteCount: _compDownloader.receivedSize];
    NSString *dl;
    if (_compDownloader.fileSize > 0)
    {
        NSString *total = [cf stringFromByteCount: _compDownloader.fileSize];
        dl = [NSString stringWithFormat: @"Downloading: %s out of %s", [mb UTF8String], [total UTF8String]];
    }
    else
    {
        dl = [NSString stringWithFormat: @"Downloading: %s (unknown file size)", [mb UTF8String]];
    }
    [_compDownloadStatus setStringValue: dl];
}

- (void)deleteDownloadedPackages
{
    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        NSString *path = [self getPathForIndex: i];
        path = [[NSURL URLWithString: path] path];
        [[NSFileManager defaultManager] removeItemAtPath: path error: NULL];
    }

    NSString *dlPath = [AppDelegate downloadLocation];
    dlPath = [[NSURL URLWithString: dlPath] path];

    NSString *iniPath = [NSString stringWithFormat: @"%@/install.ini", dlPath];
    [[NSFileManager defaultManager] removeItemAtPath: iniPath error: NULL];

    [[NSFileManager defaultManager] removeItemAtPath: dlPath error: NULL];
}

- (NSApplicationTerminateReply)applicationShouldTerminate:(NSApplication *)sender
{
    if (_isInstalling)
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert addButtonWithTitle: @"OK"];
        [alert setMessageText: @"The Installer is busy."];
        [alert setInformativeText: @"The window can’t be closed."];
        [alert setAlertStyle: NSWarningAlertStyle];
        [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: nil contextInfo: nil];

        return NSTerminateCancel;
    }

    if (!_isCustomLocation)
    {
        [self deleteDownloadedPackages];
    }

    _compInstaller = nil;
    return NSTerminateNow;
}

- (void)componentInstallStarted
{
    int idx = _compInstaller.idx;
    char *sec = iniparser_getsecname(ini, (int)idx);
    char n[200];
    snprintf(n, sizeof(n), "%s:title", sec);
    char *title = iniparser_getstring(ini, n, "undef");

    int nsec = 0;
    int curr_idx = 0;
    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        if (bool_for_key(i, "install", 0))
        {
            if (i == idx)
            {
                curr_idx = nsec;
            }
            nsec++;
        }
    }

    NSString *comp = [NSString stringWithFormat: @"%s (%d of %d)", title, curr_idx + 1, nsec];

    [_currentInstComponent setStringValue: comp];
    [_installProgressBar setDoubleValue: 0];
}

- (void)componentInstallFinished
{
    int nsec = iniparser_getnsec(ini);
    int idx = _compInstaller.idx;
    idx++;
    while (idx < nsec)
    {
        int enabled = bool_for_key(idx, "install", 0);
        if (enabled)
        {
            [_compInstaller install: idx];
            return;
        }
        idx++;
    }

    _isInstalling = NO;

    // run the /scripts/postinstall.sh
    NSString * postinstallPath = [[NSBundle mainBundle] pathForAuxiliaryExecutable: @"postinstall.sh"];

    NSString *wrap = [NSString stringWithFormat: @"sh \"%@\"", postinstallPath];

    system([wrap UTF8String]);

    if (_isCustomLocation)
    {
        // generate the automation script
        NSString *script = @"";
        for (idx = 0; idx < nsec; idx++)
        {
            if (bool_for_key(idx, "install", 0))
            {
                NSString *cmd = [_compInstaller installCommandForIndex: idx];
                script = [script stringByAppendingString: cmd];
                script = [script stringByAppendingString: @" || exit 1\n"];
            }
        }
        if ([script length] != 0)
        {
            script = [@"#!/bin/sh\n\n# Run this script to automatically install or reinstall all packages,\n# as it's performed by the Unity Download Assistant.\n# Needs to be run with superuser permissions.\n\n" stringByAppendingString: script];
            NSString *scriptPath = [NSString stringWithFormat: @"%@/install.sh", [AppDelegate downloadLocation]];
            const char *utf8ScriptPath = [scriptPath UTF8String];
            FILE *fp = fopen(utf8ScriptPath, "w+b");
            if (fp)
            {
                const char *cscript = [script UTF8String];
                size_t res = fwrite(cscript, strlen(cscript), 1, fp);
                fclose(fp);
                if (res != 1)
                {
                    unlink(utf8ScriptPath);
                }
                else
                {
                    chmod(utf8ScriptPath, 0755);
                }
            }
        }
    }

    [self setPage: PAGE_SUMMARY];
}

- (void)componentInstallFailed:(NSString *)info
{
    _isInstalling = false;

    NSAlert *alert = [[NSAlert alloc] init];

    [alert addButtonWithTitle: @"Abort"];
    [alert addButtonWithTitle: @"Ignore"];
    [alert setMessageText: @"Error"];
    [alert setInformativeText: info];
    [alert setAlertStyle: NSCriticalAlertStyle];

    NSButton *btn = [[NSButton alloc] init];
    [btn setTitle: @"View log"];
    [btn setBordered: YES];
    [btn setBezelStyle: NSRoundRectBezelStyle];
    [btn sizeToFit];
    [btn setTarget: self];
    [btn setAction: @selector(viewInstallerLog:)];

    [alert setAccessoryView: btn];

    [alert beginSheetModalForWindow: [self window]
     modalDelegate: self
     didEndSelector: @selector(componentInstallFailedResponse:returnCode:contextInfo:)
     contextInfo: nil];
}

- (void)viewInstallerLog:(id)sender
{
    [[NSWorkspace sharedWorkspace] openFile: @"/var/log/install.log"];
}

- (void)componentInstallFailedResponse:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertFirstButtonReturn)
    {
        [NSApp terminate: self];
    }
    else
    {
        [self componentInstallFinished];
    }
}

- (void)componentInstallProgress
{
    [_installProgressBar setDoubleValue: _compInstaller.pct];
    [_compInstallPhase setStringValue: _compInstaller.phase];
}

// NSTableViewDataSource
- (NSInteger)numberOfRowsInTableView:(NSTableView *)aTableView
{
    int n = 0;

    int nsec = iniparser_getnsec(ini);
    for (int i = 0; i < nsec; i++)
    {
        const char *hidden = string_for_key(i, "hidden", NULL);
        if (!hidden || !strcmp(hidden, "false"))
        {
            n++;
        }
    }

    return n;
}

- (void)detectUnityOnVolume:(NSString *)volume ofVersion:(NSString *)expectedVersion
{
    _unityIsInstalled = NO;

    NSString *plistPath = [volume stringByAppendingPathComponent: @"/Applications/Unity/Unity.app/Contents/Info.plist"];
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (0 > stat([plistPath UTF8String], &st) || !(st.st_mode & S_IFREG))
    {
        return;
    }

    NSDictionary *plist = [NSDictionary dictionaryWithContentsOfFile: plistPath];
    if (!plist)
    {
        return;
    }

    NSString *version = plist[@"CFBundleVersion"];
    if (!version)
    {
        return;
    }

    _unityIsInstalled = [expectedVersion isEqual: version];
}

- (BOOL)haveUnityDependentComponentsSelected
{
    int nsec = iniparser_getnsec(ini);
    for (int i = 0; i < nsec; i++)
    {
        if (bool_for_key(i, "requires_unity", 0) && bool_for_key(i, "install", 0))
        {
            return YES;
        }
    }
    return NO;
}

- (id)tableView:(NSTableView *)aTableView objectValueForTableColumn:(NSTableColumn *)col row:(NSInteger)rowIndex
{
    int ini_idx = get_ini_idx_for_row((int)rowIndex);
    char *sec = iniparser_getsecname(ini, ini_idx);

    if (![[col identifier] compare: @"install"])
    {
        char n[200];
        snprintf(n, sizeof(n), "%s:install", sec);
        int install = iniparser_getboolean(ini, n, 0);
        return [NSNumber numberWithBool: install];
    }
    if (![[col identifier] compare: @"component"])
    {
        char n[200];
        snprintf(n, sizeof(n), "%s:title", sec);
        char *title = iniparser_getstring(ini, n, "undef");
        snprintf(n, sizeof(n), "%s:requires_unity", sec);
        int requnity = iniparser_getboolean(ini, n, 0);
        return requnity ? [NSString stringWithFormat: @"%s (*)", title] : [NSString stringWithUTF8String: title];
    }
    else if (![[col identifier] compare: @"size"])
    {
        char n[200];
        snprintf(n, sizeof(n), "%s:size", sec);
        int64_t size = atoll(iniparser_getstring(ini, n, "0"));
        NSByteCountFormatter *cf = [NSByteCountFormatter alloc];
        cf.zeroPadsFractionDigits = YES;
        cf.countStyle = NSByteCountFormatterCountStyleFile;
        NSString *mb = [cf stringFromByteCount: size];
        return mb;
    }
    else if (![[col identifier] compare: @"installedsize"])
    {
        char n[200];
        snprintf(n, sizeof(n), "%s:installedsize", sec);
        int64_t size = atoll(iniparser_getstring(ini, n, "0"));
        NSByteCountFormatter *cf = [NSByteCountFormatter alloc];
        cf.zeroPadsFractionDigits = YES;
        cf.countStyle = NSByteCountFormatterCountStyleFile;
        NSString *mb = [cf stringFromByteCount: size];
        return mb;
    }
    return NULL;
}

- (void)tableView:(NSTableView *)aTableView willDisplayCell:(id)aCell forTableColumn:(NSTableColumn *)col row:(int)rowIndex
{
    int ini_idx = get_ini_idx_for_row((int)rowIndex);
    char *sec = iniparser_getsecname(ini, (int)ini_idx);
    if (![[col identifier] compare: @"install"])
    {
        char n[200];
        snprintf(n, sizeof(n), "%s:mandatory", sec);
        int mandatory = iniparser_getboolean(ini, n, 0);
        [aCell setEnabled: mandatory ? NO : YES];
        snprintf(n, sizeof(n), "%s:requires_unity", sec);
    }
}

- (int64_t)getRequiredDownloadedSize
{
    int64_t reqSize = 0;
    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        if (bool_for_key(i, "install", 0))
        {
            reqSize += atoll(string_for_key(i, "size", "0"));
        }
    }
    return reqSize;
}

- (int64_t)getRequiredInstallSize
{
    int64_t reqSize = 0;
    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        if (bool_for_key(i, "install", 0))
        {
            reqSize += atoll(string_for_key(i, "installedsize", "0"));
        }
    }
    return reqSize;
}

- (int64_t)getAvailableStorageSize
{
    NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfFileSystemForPath: _selectedVolume error: nil];
    return [[fileAttributes objectForKey: NSFileSystemFreeSize] longLongValue];
}

- (void)updateFreespace:(id)data
{
    [_compSpaceRemaining setStringValue: [NSByteCountFormatter stringFromByteCount: [self getAvailableStorageSize] countStyle: NSByteCountFormatterCountStyleFile]];
}

- (void)updateCompSizeInfo
{
    int64_t size = [self getRequiredInstallSize] + [self getRequiredDownloadedSize];
    [_compSpaceRequired setStringValue: [NSByteCountFormatter stringFromByteCount: size countStyle: NSByteCountFormatterCountStyleFile]];
    [self updateFreespace: nil];
}

- (void)updateComponentLicenseEnabled
{
    _componentLicenseRequired = NO;

    for (int i = 0; i < iniparser_getnsec(ini); i++)
    {
        const char* eulaMessage = string_for_key(i, "eulamessage", NULL);

        if (bool_for_key(i, "install", 0) && eulaMessage)
        {
            _componentEULAMessage = eulaMessage;
            _componentEULALabel1 = string_for_key(i, "eulalabel1", NULL);
            _componentEULAUrl1 = string_for_key(i, "eulaurl1", NULL);
            _componentEULALabel2 = string_for_key(i, "eulalabel2", NULL);
            _componentEULAUrl2 = string_for_key(i, "eulaurl2", NULL);
            _componentLicenseRequired = YES;
            break;
        }
    }

    pageEnabled[PAGE_COMPONENTLICENSE] = _componentLicenseRequired;
}

// returns YES if the state has changed
- (BOOL)enableComponent:(const char *)name enable:(BOOL)enable
{
    char key[200];
    snprintf(key, sizeof(key), "%s:install", name);
    BOOL enabled = iniparser_getboolean(ini, key, 0) ? YES : NO;

    if (enabled == enable)
    {
        return NO;
    }

    iniparser_set(ini, key, enable ? "true" : "false");


    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];

    NSString *k = [@"install_" stringByAppendingString: [NSString stringWithUTF8String: name]];
    [def setBool: enable forKey: k];
    [def synchronize];

    return YES;
}

// if enabled: find and enable the component stated in "sync" variable
// if disabled: find and disable all components which have the stated component in "sync" variable
- (void)updateDependencies:(BOOL)enabled component:(const char *)compname
{
    BOOL need_reload = NO;

    if (enabled)
    {
        // enable soft-dependent components
        char **keys = iniparser_getseckeys(ini, (char*)compname);
        for (int i = 0; i < iniparser_getsecnkeys(ini, (char*)compname); i++)
        {
            if (!strncasecmp(keys[i] + strlen(compname) + 1, "optsync_", 8))
            {
                const char *sync = iniparser_getstring(ini, keys[i], NULL);
                if (sync && [self enableComponent: sync enable: YES])
                {
                    need_reload = YES;
                }
            }
        }
    }

    // disable component with a hard dependency on this one
    // sync enabled state of linked components
    int nsec = iniparser_getnsec(ini);
    for (int idx = 0; idx < nsec; idx++)
    {
        const char *sync = string_for_key(idx, "sync", NULL);
        if (sync && !strcasecmp(sync, compname))
        {
            if ([self enableComponent: iniparser_getsecname(ini, idx) enable: enabled])
            {
                need_reload = YES;
            }
        }
    }


    if (need_reload)
    {
        [_iniView reloadData];
    }
}

// map from tableview rows to ini rows;
// hidden rows are not present in the tableview
static int get_ini_idx_for_row(int row)
{
    int nsec = iniparser_getnsec(ini);
    for (int idx = 0; idx < nsec; idx++)
    {
        const char *hidden = string_for_key(idx, "hidden", NULL);
        if (hidden && strcmp(hidden, "false") && row >= idx)
        {
            row++;
        }
        if (row == idx)
        {
            return idx;
        }
    }
    return -1;
}

- (void)monoDevelopDisableAlertEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertSecondButtonReturn)
    {
        char n[100];
        const char *sec = "monodevelop";
        snprintf(n, sizeof(n), "%s:install", sec);
        iniparser_set(ini, n, "false");

        // save state in NSUserDefaults
        NSUserDefaults *def = [NSUserDefaults standardUserDefaults];

        NSString *key = [@"install_" stringByAppendingString: [NSString stringWithUTF8String: sec]];
        [def setBool: NO forKey: key];
        [def synchronize];

        [self updateDependencies: NO component: sec];

        [self updateCompSizeInfo];
    }
}

- (void)tableView:(NSTableView *)tableView setObjectValue:(id)value forTableColumn:(NSTableColumn *)col row:(NSInteger)row
{
    int ini_idx = get_ini_idx_for_row((int)row);
    char *sec = iniparser_getsecname(ini, ini_idx);
    if (![[col identifier] compare: @"install"])
    {
        if (!strcmp(sec, "monodevelop") && ![value boolValue])
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle: @"Cancel"];
            [alert addButtonWithTitle: @"Disable MonoDevelop"];
            [alert setMessageText: @"MonoDevelop opt-out"];
            [alert setInformativeText: @"Without MonoDevelop you won't be able to debug your scripts. A different version of MonoDevelop would not work. Are you sure you want to proceed?"];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(monoDevelopDisableAlertEnd:returnCode:contextInfo:) contextInfo: nil];
            return;
        }

        char n[100];
        snprintf(n, sizeof(n), "%s:install", sec);
        iniparser_set(ini, n, [value boolValue] ? "true" : "false");

        // save state in NSUserDefaults
        NSUserDefaults *def = [NSUserDefaults standardUserDefaults];

        NSString *key = [@"install_" stringByAppendingString: [NSString stringWithUTF8String: sec]];
        [def setBool: [value boolValue] forKey: key];
        [def synchronize];

        [self updateDependencies: [value boolValue] component: sec];
    }

    [self updateCompSizeInfo];
    [self updateComponentLicenseEnabled];
}

- (IBAction)licenseSheetClose:(id)sender
{
    [NSApp endSheet: _licenseSheet returnCode: ([sender tag] == 1) ? NSOKButton : NSCancelButton];
    if ([sender tag] == 0)
    {
        [NSApp terminate: self];
    }
}

- (void)downloadAbortAlertEnd:(NSAlert *)alert returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    if (returnCode == NSAlertSecondButtonReturn)
    {
        _md5VerifyCancel = YES;
        [_compDownloader cancelCurrent];
        [self setPage: PAGE_DEST];
    }
}

- (IBAction)printLicenseAction:(id)sender
{
    [[self licenseTextView] print: self];
}

- (IBAction)saveLicenseAction:(id)sender
{
    [_installerUtil saveLicense: _licenseTextView];
}

- (void)didEndLicenseSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut: self];

    if (returnCode == NSOKButton)
    {
        _userHasAcceptedLicense = YES;
        [self setPage: PAGE_COMPONENTS];
    }
}

- (void)didEndComponentLicenseSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut: self];

    if (returnCode == NSOKButton)
    {
        _componentLicenseAccepted = YES;
        [self setPage: PAGE_DEST];
    }
}

- (NSIndexSet *)tableView:(NSTableView *)tableView selectionIndexesForProposedSelection:(NSIndexSet *)proposedSelectionIndexes
{
    if (tableView == _iniView)
    {
        if (![proposedSelectionIndexes count])
        {
            [_compDescrTextView setString: @""];
        }
        else
        {
            int row = (int)[proposedSelectionIndexes firstIndex];
            row = get_ini_idx_for_row(row);
            char *sec = iniparser_getsecname(ini, row);
            char n[100];
            snprintf(n, sizeof(n), "%s:description", sec);
            char *d = iniparser_getstring(ini, n, "");
            [_compDescrTextView setString: [NSString stringWithUTF8String: d]];
        }
    }
    return proposedSelectionIndexes;
}

- (NSString *)getPackageFilenameForIndex:(int)idx
{
    char *sec = iniparser_getsecname(ini, idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:url", sec);
    char *url = iniparser_getstring(ini, n, NULL);
    char *slash = strrchr(url, '/');
    if (slash && [[NSString stringWithUTF8String: slash] hasSuffix: @".pkg"])
    {
        return [NSString stringWithUTF8String: (slash + 1)];
    }

    snprintf(n, sizeof(n), "%s:extension", sec);
    char *extension = iniparser_getstring(ini, n, NULL);

    if (extension)
    {
        return [NSString stringWithFormat: @"%d.%s", idx, extension];
    }

    return [NSString stringWithFormat: @"%d.pkg", idx];
}

- (NSString *)getPathForIndex:(int)idx
{
    return [NSString stringWithFormat: @"%@/%@", [AppDelegate downloadLocation], [self getPackageFilenameForIndex: idx]];
}

- (uint64_t)componentFileSizeForIndex:(int)idx
{
    char *sec = iniparser_getsecname(ini, idx);
    char n[100];
    snprintf(n, sizeof(n), "%s:size", sec);
    long long filesize = iniparser_getlonglong(ini, n, -1);
    return (uint64_t)filesize;
}

+ (NSString *)downloadLocation
{
    if (g_DownloadLocation)
        return g_DownloadLocation;

    NSString *path;

    NSArray *urls = [[NSFileManager defaultManager] URLsForDirectory: NSDownloadsDirectory inDomains: NSUserDomainMask];

    if ([urls count])
    {
        NSURL *downloads = urls[0];
        NSString *url = [downloads path];
        path = [NSString stringWithFormat: @"%@/UnityPackages", url];
    }
    else
    {
        path = [NSString stringWithFormat: @"%s/Downloads/UnityPackages", getenv("HOME")];
    }
    [[NSFileManager defaultManager] createDirectoryAtPath: path
     withIntermediateDirectories: YES
     attributes: nil
     error: nil];
    return path;
}

+ (void)setDownloadLocation:(NSString *)location
{
    g_DownloadLocation = location;
}

- (IBAction)prevButtonAction:(id)sender
{
    if (_currScreen == PAGE_DL)
    {
        if (_compDownloader)
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle: @"No"];
            [alert addButtonWithTitle: @"Yes"];
            [alert setMessageText: @"Abort the download?"];
            [alert setInformativeText: @"If you abort the download, the installer will return to the component selection screen."];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(downloadAbortAlertEnd:returnCode:contextInfo:) contextInfo: nil];
            return;
        }
        else
        {
        }
    }
    else if (_currScreen == PAGE_INTRO)
    {
        [NSApp terminate: self];
    }
    else if (_currScreen == PAGE_DEST)
    {
        [self setPage: _currScreen - 1 - (_componentLicenseRequired ? 0 : 1)];
    }
    else
    {
        [self setPage: _currScreen - 1];
    }
}

- (IBAction)nextButtonAction:(id)sender
{
    if (_currScreen == PAGE_LICENSE)
    {
        if (_userHasAcceptedLicense)
        {
            [self setPage: _currScreen + 1];
            return;
        }
        [NSApp beginSheet: _licenseSheet modalForWindow: _window modalDelegate: self didEndSelector: @selector(didEndLicenseSheet:returnCode:contextInfo:) contextInfo: nil];
    }
    else if (_currScreen == PAGE_COMPONENTLICENSE)
    {
        if (_componentLicenseAccepted)
        {
            [self setPage: _currScreen + 1];
            return;
        }

        [NSApp beginSheet: _licenseSheet modalForWindow: _window modalDelegate: self didEndSelector: @selector(didEndComponentLicenseSheet:returnCode:contextInfo:) contextInfo: nil];
    }
    else if (_currScreen == PAGE_DEST)
    {
        NSInteger disk = [[[self volumeCollectionView] selectionIndexes] firstIndex];
        if (disk == NSNotFound)
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle: @"OK"];
            [alert setMessageText: @"You must select the disk."];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: nil contextInfo: nil];
        }
        else
        {
            VolumeModel *vol = [volumeModelArray objectAtIndex: disk];

            if ([vol sizeAvailableInt] < [self getRequiredInstallSize])
            {
                NSAlert *alert = [[NSAlert alloc] init];
                [alert addButtonWithTitle: @"OK"];
                [alert setMessageText: @"Not enough free space."];
                [alert setInformativeText: @"The selected install volume doesn't have enough space to install the components. Please free up some space, or choose another disk volume."];
                [alert setAlertStyle: NSWarningAlertStyle];
                [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: nil contextInfo: nil];
                return;
            }

            if (![self verifyComponentFreeSpace])
            {
                return;
            }

            char *version = string_for_key(0, "version", "");
            [self detectUnityOnVolume: _selectedVolume ofVersion: [NSString stringWithUTF8String: version]];

            int unitySelected = bool_for_key(0, "install", 0);

            if (!_unityIsInstalled
                && !unitySelected
                && [self haveUnityDependentComponentsSelected])
            {
                NSAlert *alert = [[NSAlert alloc] init];
                [alert addButtonWithTitle: @"OK"];
                [alert setMessageText: @"Can't install the selected components on this disk."];
                [alert setInformativeText: [NSString stringWithFormat: @"You tried to install items that require Unity %s, but Unity is not installed on the disk you chose. Please either add Unity to your selection, or deselect the components which require Unity.", version]];
                [alert setAlertStyle: NSWarningAlertStyle];
                [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: @selector(didEndUnityDependencyFailedSheet:returnCode:contextInfo:) contextInfo: nil];
                return;
            }

            [self setPage: _currScreen + 1];
        }
    }
    else if (_currScreen == PAGE_COMPONENTS)
    {
        int idx = 0;
        int nsec = iniparser_getnsec(ini);
        while (idx < nsec)
        {
            int enabled = bool_for_key(idx, "install", 0);
            if (enabled)
            {
                break;
            }
            idx++;
        }

        if (idx >= nsec)
        {
            NSAlert *alert = [[NSAlert alloc] init];
            [alert addButtonWithTitle: @"OK"];
            [alert setMessageText: @"Nothing selected to install."];
            [alert setAlertStyle: NSWarningAlertStyle];
            [alert beginSheetModalForWindow: [self window] modalDelegate: self didEndSelector: nil contextInfo: nil];
            return;
        }

        [self setPage: PAGE_COMPONENTLICENSE + (_componentLicenseRequired ? 0 : 1)];
    }
    else if (_currScreen == PAGE_SUMMARY)
    {
        if ([_summaryViewLaunchUnityCheckButton state] == NSOnState)
        {
            NSString *unityPath = [_selectedVolume stringByAppendingString: @"Applications/Unity/Unity.app"];
            [_downloadLogger log: @"Running: %@", unityPath];
            [[NSWorkspace sharedWorkspace] launchApplication: unityPath];
        }
        [NSApp terminate: self];
    }
    else
    {
        [self setPage: _currScreen + 1];
    }
}

- (void)didEndUnityDependencyFailedSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [self setPage: PAGE_COMPONENTS];
}

- (BOOL)ensureCustomLocationIsWritable
{
    // verify if the folder exists
    NSString *path = [AppDelegate downloadLocation];
    NSError * error = nil;
    [[NSFileManager defaultManager] createDirectoryAtPath: path withIntermediateDirectories: YES attributes: nil error: &error];
    struct stat st;
    memset(&st, 0, sizeof(st));
    if (0 > stat([path UTF8String], &st) || !
        (st.st_mode & S_IFDIR) || !(st.st_mode & S_IWUSR))
    {
        NSAlert *alert = [[NSAlert alloc] init];
        [alert setAlertStyle: NSWarningAlertStyle];
        [alert setMessageText: @"The selected folder doesn't exist or is not writable."];
        [alert runModal];

        return NO;
    }
    return YES;
}

- (IBAction)customLocationClose:(id)sender
{
    if (![self ensureCustomLocationIsWritable])
    {
        return;
    }
    [NSApp endSheet: _customLocationSheet returnCode: NSOKButton];
}

- (void)didEndCustomLocationSheet:(NSWindow *)sheet returnCode:(NSInteger)returnCode contextInfo:(void *)contextInfo
{
    [sheet orderOut: self];
}

- (IBAction)customLocationButtonAction:(id)sender
{
    [NSApp beginSheet: _customLocationSheet modalForWindow: _window modalDelegate: self didEndSelector: @selector(didEndCustomLocationSheet:returnCode:contextInfo:) contextInfo: nil];
}

- (IBAction)customLocationBrowseAction:(id)sender
{
    NSOpenPanel* openDlg = [NSOpenPanel openPanel];
    [openDlg setCanChooseFiles: NO];
    [openDlg setAllowsMultipleSelection: NO];
    [openDlg setCanChooseDirectories: YES];
    [openDlg setCanCreateDirectories: YES];
    NSURL *url = [NSURL URLWithString: [_customLocationPath stringValue]];
    [openDlg setDirectoryURL: url];
    if ([openDlg runModal] == NSOKButton)
    {
        [_customLocationPath setStringValue: [[[openDlg URLs] objectAtIndex: 0] path]];
        [self customLocationSelectionChanged: self];
    }
}

- (IBAction)customLocationOpenFinder:(id)sender
{
    NSString *path = [AppDelegate downloadLocation];
    if ([self ensureCustomLocationIsWritable])
    {
        [[NSWorkspace sharedWorkspace] selectFile: path
         inFileViewerRootedAtPath: path];
    }
}

- (void)updateCustomLocation
{
    [_customLocationBrowseButton setEnabled: _isCustomLocation];
    [_customLocationPath setEditable: _isCustomLocation];
    [_customLocationFinderPath setStringValue: [AppDelegate downloadLocation]];
}

- (void)updateCustomLocationSelection
{
    [AppDelegate setDownloadLocation: (_isCustomLocation ? [_customLocationPath stringValue] : nil)];
    [self updateCustomLocation];
}

- (IBAction)customLocationSelectionChanged:(id)sender
{
    BOOL custom = [_customLocationSelection selectedTag] != 0;
    if (custom != _isCustomLocation)
    {
        _isCustomLocation = custom;
        if (custom)
        {
            [_customLocationPath setStringValue: [AppDelegate downloadLocation]];
        }
        else
        {
            [_customLocationPath setStringValue: @""];
        }
    }
    [self updateCustomLocationSelection];
}

- (IBAction)summaryViewLaunchUnityCheckButtonAction:(id)sender
{
    NSInteger state = [sender state];
    NSUserDefaults *def = [NSUserDefaults standardUserDefaults];
    [def setBool: state == NSOnState forKey: @"autorun_unity"];
}

@end
