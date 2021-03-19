//
//  DNDView.m
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 04/01/2017.
//  Copyright © 2017 Unity Technologies. All rights reserved.
//

#import "DNDView.h"
#import "AppDelegate.h"

@implementation DNDView
- (void)awakeFromNib
{
    [self registerForDraggedTypes: [NSArray arrayWithObjects: NSFilenamesPboardType, nil]];
}

- (NSDragOperation)draggingEntered:(id<NSDraggingInfo>)sender
{
    return NSDragOperationGeneric;
}

- (BOOL)performDragOperation:(id<NSDraggingInfo>)sender
{
    NSPasteboard *pboard = [sender draggingPasteboard];
    if ([[pboard types] containsObject: NSFilenamesPboardType])
    {
        NSArray *paths = [pboard propertyListForType: NSFilenamesPboardType];
        AppDelegate *d = (AppDelegate*)[[NSApplication sharedApplication] delegate];
        [d initEULAFromFile: paths[0]];
    }
    return YES;
}

@end
