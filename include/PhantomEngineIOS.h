//
//  AppDelegate.m
//  PhantomGameEngineIOS.h
//
//  Created by ZhaoDexian on 12-7-8.
//  Copyright (c) 2012年 phantom. All rights reserved.
//

#include "CInclude.h"
#include "MyInclude.h"
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreGraphics/CoreGraphics.h>
#import <UIKit/UIKit.h>


@interface PhantomEngineIOS : NSObject
{
}
+(PhantomEngineIOS*)sharedInstance;
-(UIImage *) imageFromText:(NSString*)text fontName:(NSString *)fontName nFontHeight:(int)nFontHeight w:(int*)w h:(int*)h s:(int)s align:(int)align;

@end

