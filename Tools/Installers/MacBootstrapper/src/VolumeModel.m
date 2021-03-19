//
//  VolumeModel.m
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 02/09/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "VolumeModel.h"

@implementation VolumeModel

@synthesize name;
@synthesize icon;
@synthesize installable;
@synthesize sizeTotal;
@synthesize sizeAvailable;

- (uint64_t)sizeAvailableInt
{
    NSDictionary* fileAttributes = [[NSFileManager defaultManager] attributesOfFileSystemForPath: self.path error: nil];
    return [[fileAttributes objectForKey: NSFileSystemFreeSize] longLongValue];
}

@end
