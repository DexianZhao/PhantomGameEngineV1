//
//  ViewController.h
//  PhantomIOSSDK
//
//  Created by cosmic-life on 12-7-25.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <GLKit/GLKit.h>
#import <iAd/iAd.h>
#import "MyStoreKit.h"

@interface ViewController : GLKViewController
{
    ADBannerView*   m_adView;
    MyStoreKit*  m_store;
}
- (id)initWithB:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil;
- (void) myLoadView;
- (void) gotoBuy;
- (void) showWall;
- (void) hideAd;

@end
