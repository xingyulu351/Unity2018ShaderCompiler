//
//  NSTransparentView.m
//  Unity Installer
//
//  Created by Oleksiy Yakovenko on 12/08/2014.
//  Copyright (c) 2014 Unity Technologies. All rights reserved.
//

#import "TransparentView.h"

@implementation TransparentView

- (id)initWithFrame:(NSRect)frame
{
    self = [super initWithFrame: frame];
    if (self)
    {
        // Initialization code here.
    }
    return self;
}

- (void)drawRect:(NSRect)rect
{
    [[NSColor colorWithCalibratedWhite: 1 alpha: 0.9] set];
    NSRectFillUsingOperation(rect, NSCompositeSourceOver);

    rect.origin.x += 0.5;
    rect.origin.y += 0.5;
    rect.size.width -= 1;
    rect.size.height -= 1;

    [[NSColor controlShadowColor] set];
    [NSBezierPath strokeRect: rect];
}

@end
