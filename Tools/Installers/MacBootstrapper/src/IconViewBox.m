//
//  IconViewBox.m
//  UnityInstaller
//
//  Created by Oleksiy Yakovenko on 03/09/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "IconViewBox.h"

@implementation IconViewBox

// -------------------------------------------------------------------------------
//  hitTest:aPoint
// -------------------------------------------------------------------------------
- (NSView *)hitTest:(NSPoint)aPoint
{
    // don't allow any mouse clicks for subviews in this NSBox
    return nil;
}

- (NSView *)findImage:(id)obj
{
    if ([obj tag] == 1)
    {
        return obj;
    }
    NSArray *subviews = [obj subviews];
    for (NSView *curr in subviews)
    {
        NSView *v = [self findImage: curr];
        if (v)
        {
            return v;
        }
    }
    return nil;
}

- (void)drawRect:(NSRect)rect
{
    if ([self isTransparent])
    {
        [super drawRect: rect];
    }
    else
    {
        NSView *obj = [self findImage: self];

        if (obj)
        {
            NSRect b = [self bounds];
            NSRect sel = [obj bounds];
            sel.size.width += 10;
            sel.size.height += 10;
            sel.origin.x = b.origin.x + b.size.width / 2 - sel.size.width / 2;
            sel.origin.y = b.size.height - sel.size.height;
            sel.origin.y -= 10;
            NSBezierPath* circlePath = [NSBezierPath bezierPath];
            [circlePath appendBezierPathWithOvalInRect: sel];
            [[self fillColor] set];
            [circlePath fill];
            [[self borderColor] set];
            [circlePath stroke];

            //          NSImage *img = [NSImage imageNamed:NSImageNameCaution];
            //          NSSize sz = [img size];
            //          [img drawAtPoint:sel.origin fromRect:NSMakeRect(0,0,sz.width,sz.height) operation:NSCompositeSourceOver fraction:1];
        }
    }
}

@end
