//
//  ComponentInstaller.h
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

@protocol ComponentInstallerDelegate<NSObject>
@optional
- (NSString *)getPathForIndex:(int)idx;
- (void)componentInstallFinished;
- (void)componentInstallFailed:(NSString *)info;
- (void)componentInstallStarted;
- (void)componentInstallProgress;
@end


@interface ComponentInstaller : NSObject
{
    id _delegate;
    int _idx;
    FILE *_commPipe;
    float _pct;
    NSString *_status;
    NSString *_phase;
    NSString *_volume;
    NSString *_lastMessage;
}

@property (readonly) float pct;
@property (readonly) int idx;
@property (readonly) NSString *phase;

- (float)pct;
- (ComponentInstaller *)init:(id<ComponentInstallerDelegate>)delegate auth:(AuthorizationRef)auth;
- (void)setVolume:(NSString*)volume;
- (void)install:(int)idx;
- (void)installMain:(NSString*)pkg;
- (NSString *)installCommandForIndex:(int)index;
@end
