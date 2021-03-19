//
//  VolumeModel.h
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 02/09/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface VolumeModel : NSObject
{
    NSString *_path;
    NSString *_name;
    NSImage *_icon;
    BOOL _installable;
    NSString *_sizeTotal;
    NSString *_sizeAvailable;
}

@property(retain, readwrite) NSString *path;
@property(retain, readwrite) NSString *name;
@property(retain, readwrite) NSImage *icon;
@property(readwrite) BOOL installable;
@property(retain, readwrite) NSString *sizeTotal;
@property(retain, readwrite) NSString *sizeAvailable;
- (uint64_t)sizeAvailableInt;

@end
