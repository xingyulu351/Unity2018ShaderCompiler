//
//  AppDelegate.h
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 04/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import <Cocoa/Cocoa.h>
#import "IniDownloader.h"
#import "ComponentDownloader.h"
#import "ComponentInstaller.h"


@interface AppDelegate : NSObject<NSApplicationDelegate>

+ (void)messageBox:(NSString *)text;
+ (NSString *)downloadLocation;

- (void)initEULAFromFile:(NSString *)fname;
@property (weak) IBOutlet NSTextField *eulaTestTitle;


- (IBAction)prevButtonAction:(id)sender;
- (IBAction)nextButtonAction:(id)sender;
- (IBAction)printLicenseAction:(id)sender;
- (IBAction)saveLicenseAction:(id)sender;

- (IBAction)licenseSheetClose:(id)sender;

- (IBAction)customLocationButtonAction:(id)sender;
- (IBAction)customLocationClose:(id)sender;

- (IBAction)customLocationBrowseAction:(id)sender;
- (IBAction)customLocationOpenFinder:(id)sender;
- (IBAction)customLocationSelectionChanged:(id)sender;


@property (assign) IBOutlet NSWindow *window;
@property (unsafe_unretained) IBOutlet NSTextField *pageTitle;
@property (unsafe_unretained) IBOutlet NSProgressIndicator *downloadSpinner;

@property (unsafe_unretained) IBOutlet NSTabView *tabView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *introView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *licenseView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *componentsView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *componentsLicenseView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *destView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *dlView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *instView;
@property (unsafe_unretained) IBOutlet NSTabViewItem *summaryView;

@property (weak) IBOutlet NSButton *summaryViewLaunchUnityCheckButton;
- (IBAction)summaryViewLaunchUnityCheckButtonAction:(id)sender;

@property (unsafe_unretained) IBOutlet NSButton *prevButton;
@property (unsafe_unretained) IBOutlet NSButton *nextButton;
@property (unsafe_unretained) IBOutlet NSButton *printButton;
@property (unsafe_unretained) IBOutlet NSButton *saveButton;

// screen names
@property (unsafe_unretained) IBOutlet NSImageView *introBullet;
@property (unsafe_unretained) IBOutlet NSTextField *introLabel;
@property (unsafe_unretained) IBOutlet NSImageView *licenseBullet;
@property (unsafe_unretained) IBOutlet NSTextField *licenseLabel;
@property (unsafe_unretained) IBOutlet NSImageView *destBullet;
@property (unsafe_unretained) IBOutlet NSTextField *destLabel;
@property (unsafe_unretained) IBOutlet NSImageView *compBullet;
@property (unsafe_unretained) IBOutlet NSTextField *compLabel;
@property (unsafe_unretained) IBOutlet NSImageView *compLicenseBullet;
@property (unsafe_unretained) IBOutlet NSTextField *compLicenseLabel;
@property (unsafe_unretained) IBOutlet NSImageView *dlBullet;
@property (unsafe_unretained) IBOutlet NSTextField *dlLabel;
@property (unsafe_unretained) IBOutlet NSImageView *instBullet;
@property (unsafe_unretained) IBOutlet NSTextField *instLabel;
@property (unsafe_unretained) IBOutlet NSImageView *summaryBullet;
@property (unsafe_unretained) IBOutlet NSTextField *summaryLabel;

@property (unsafe_unretained) IBOutlet NSTableView *iniView;
@property NSMutableArray *volumeModelArray;
@property (unsafe_unretained) IBOutlet NSCollectionView *volumeCollectionView;
@property (unsafe_unretained) IBOutlet NSTextView *destDescriptionTextView;
@property (unsafe_unretained) IBOutlet NSTextField *compSpaceRequired;
@property (unsafe_unretained) IBOutlet NSTextField *compSpaceRemaining;

@property (strong) IBOutlet NSTextView *introTextView;
@property (strong) IBOutlet NSTextView *licenseTextView;
@property (weak) IBOutlet NSProgressIndicator *licenseLoadingProgressIndicator;
@property (weak) IBOutlet NSTextField *licenseLoadingProgressLabel;
@property (weak) IBOutlet NSScrollView *licenseTextContainer;


@property (unsafe_unretained) IBOutlet NSTextView *compDescrTextView;
@property (unsafe_unretained) IBOutlet NSProgressIndicator *compDownloadProgress;
@property (unsafe_unretained) IBOutlet NSTextField *compDownloadStatus;
@property (unsafe_unretained) IBOutlet NSTextField *currentDlComponent;
@property (unsafe_unretained) IBOutlet NSProgressIndicator *installProgressBar;
@property (unsafe_unretained) IBOutlet NSTextField *currentInstComponent;
@property (unsafe_unretained) IBOutlet NSTextField *compInstallPhase;
@property (unsafe_unretained) IBOutlet NSPanel *licenseSheet;

@property (unsafe_unretained) IBOutlet NSTextView *compLicenseTextView;


@property (unsafe_unretained) IBOutlet NSButton *customLocationButton;

@property (unsafe_unretained) IBOutlet NSPanel *customLocationSheet;
@property (unsafe_unretained) IBOutlet NSTextField *customLocationPath;
@property (unsafe_unretained) IBOutlet NSPopUpButton *customLocationSelection;
@property (unsafe_unretained) IBOutlet NSTextField *customLocationFinderPath;
@property (unsafe_unretained) IBOutlet NSButton *customLocationBrowseButton;


@end
