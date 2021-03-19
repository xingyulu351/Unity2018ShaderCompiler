//
//  ComponentInstaller.m
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 06/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "ComponentInstaller.h"
#import "AppDelegate.h"
#import <dlfcn.h>

@implementation ComponentInstaller

- (void)dealloc
{
    if (_commPipe)
    {
        fprintf(_commPipe, "quit\n");
        fclose(_commPipe);
        _commPipe = NULL;
    }
}

- (ComponentInstaller *)init:(id<ComponentInstallerDelegate>)delegate auth:(AuthorizationRef)auth
{
    self = [super init];
    _delegate = delegate;

    static OSStatus (*security_AuthorizationExecuteWithPrivileges)(AuthorizationRef authorization, const char *pathToTool,
        AuthorizationFlags options, char * const *arguments,
        FILE **communicationsPipe) = NULL;

    if (!security_AuthorizationExecuteWithPrivileges)
    {
        // On 10.7, AuthorizationExecuteWithPrivileges is deprecated. We want to still use it since there's no
        // good alternative (without requiring code signing). We'll look up the function through dyld and fail
        // if it is no longer accessible. If Apple removes the function entirely this will fail gracefully. If
        // they keep the function and throw some sort of exception, this won't fail gracefully, but that's a
        // risk we'll have to take for now.
        security_AuthorizationExecuteWithPrivileges = dlsym(RTLD_DEFAULT, "AuthorizationExecuteWithPrivileges");
    }
    if (!security_AuthorizationExecuteWithPrivileges)
    {
        [_delegate componentInstallFailed: @"Failed to obtain AuthorizationExecuteWithPrivileges access. This is a critical error, which may require updating the installer."];
        return self;
    }

    NSString * helperPath = [[NSBundle mainBundle] pathForAuxiliaryExecutable: @"installhelper.sh"];
    const char * const args[] = { [helperPath UTF8String],  NULL };
    OSStatus err = security_AuthorizationExecuteWithPrivileges(auth, "/bin/sh", kAuthorizationFlagDefaults, (char*const*)args, &_commPipe);

    if (err != errAuthorizationSuccess)
    {
        [_delegate componentInstallFailed: [NSString stringWithFormat: @"Failed to run installer helper script. Error code: %d", err]];
    }

    return self;
}

- (void)setVolume:(NSString*)volume
{
    _volume = volume;
}

- (void)install:(int)idx
{
    _idx = idx;

    NSString *path = [_delegate getPathForIndex: _idx];

    NSThread* myThread = [[NSThread alloc] initWithTarget: self
                          selector: @selector(installMain:)
                          object: path];
    [myThread start];
}

- (void)installMain:(NSString *)path
{
    if ([[path pathExtension] isEqual: @"dmg"])
        [self installDiskImage: path];
    else
        [self installPackage: path volume: _volume];
}

- (void)installPackage:(NSString *)pkg volume:(NSString *)volume
{
    NSString *cmd = [NSString stringWithFormat: @"installpkg\n%@\n%@\n", pkg, volume];
    NSString *errorMsg = [NSString stringWithFormat: @"The package %@ failed to install", pkg];
    [self invokeInstallHelperCommand: cmd errorMsg: errorMsg];
}

- (void)installDiskImage:(NSString *)dmg
{
    NSString *cmd = [NSString stringWithFormat: @"installdmg\n%@\n", dmg];
    NSString *errorMsg = [NSString stringWithFormat: @"The disk image %@ failed to install", dmg];
    [self invokeInstallHelperCommand: cmd errorMsg: errorMsg];
}

- (void)invokeInstallHelperCommand:(NSString*)command errorMsg:(NSString*)errorMsg
{
    [self performSelectorOnMainThread: @selector(componentInstallStarted) withObject: nil waitUntilDone: NO];

    _lastMessage = @"Unknown error";
    fprintf(_commPipe, "%s", [command UTF8String]);

    int status = 0;
    char input[2048];

    for (;;)
    {
        fgets(input, sizeof(input), _commPipe);

        char *p;

        int have_progress = 0;
        if ((p = strstr(input, "installer:%")))
        {
            _pct = atof(p + 11);
            have_progress = 1;
        }
        else if ((p = strstr(input, "installer:STATUS:")) && *(p + 17) > 0x20)
        {
            _status = [NSString stringWithUTF8String: p + 17];
            have_progress = 1;
        }
        else if ((p = strstr(input, "installer:PHASE:")))
        {
            _phase = [NSString stringWithUTF8String: p + 16];
            have_progress = 1;
        }
        else if ((p = strstr(input, "PERCENT:"))) // hdiutil emits this
        {
            float pct = atof(p + 8);

            // hdiutil can return -1 if a task takes
            // an indeterminate amount of time to complete.
            if (pct > 0.0f)
            {
                have_progress = 1;
                // Half the work is verifying and mounting the .dmg
                // Second half is copying the files.
                _pct = pct * 0.5f;
            }
        }
        else if ((p = strstr(input, "to-check="))) // rsync emits this
        {
            p += 9;
            // rsync emits this as total progress "(xfer#4923, to-check=9283/14800)"
            const char* slash = strchr(p, '/');

            if (slash == NULL)
                continue;

            slash++;

            const int filesRemaining = atoi(p);
            const int totalFiles = atoi(slash);

            if (filesRemaining < 1 || totalFiles < 1)
                continue;

            const int filesChecked = totalFiles - filesRemaining;
            const float donePercent = (float)filesChecked / (float)totalFiles;

            // Half the work is verifying and mounting the .dmg
            // Second half is copying the files.
            _pct = 50.0f + (donePercent * 50.f);
            have_progress = 1;
        }
        else if ((p = strstr(input, "EXIT:")))
        {
            status = atoi(p + 5);
            break;
        }
        if (have_progress)
        {
            [self performSelectorOnMainThread: @selector(componentInstallProgress) withObject: nil waitUntilDone: NO];
        }
        // echo /usr/bin/installer output to console
        _lastMessage = [NSString stringWithUTF8String: input];
    }

    if (!WIFEXITED(status) || (WEXITSTATUS(status) != 0))
    {
        NSString *info = [NSString stringWithFormat: @"%@ with the following error:\n\n%@\n\nAdditional information should be available in the /var/log/install.log file.", errorMsg, _lastMessage];
        [self performSelectorOnMainThread: @selector(componentInstallFailed:) withObject: info waitUntilDone: NO];
        return;
    }

    [self performSelectorOnMainThread: @selector(componentInstallFinished) withObject: nil waitUntilDone: NO];
}

- (void)componentInstallStarted
{
    [_delegate componentInstallStarted];
}

- (void)componentInstallFinished
{
    [_delegate componentInstallFinished];
}

- (void)componentInstallFailed:(NSString *)info
{
    [_delegate componentInstallFailed: info];
}

- (void)componentInstallProgress
{
    [_delegate componentInstallProgress];
}

- (NSString *)installCommandForIndex:(int)index
{
    return [NSString stringWithFormat: @"/usr/sbin/installer -pkg \"%@\" -target \"%@\" -verbose", [_delegate getPackageFilenameForIndex: index], _volume];
}

@end
