//
//  ViewController.m
//  PhantomIOSSDK
//
//  Created by cosmic-life on 12-7-25.
//  Copyright (c) 2012Âπ?__MyCompanyName__. All rights reserved.
//

#import "ViewController.h"
#include "MyInclude.h"
//#import <iAd/iAd.h>

extern void CreateMainSDK();
extern void DestroyAllSDK();
ViewController* g_myView = 0;
extern int GetAdViewEnabled();


void	openUrl(const char* url)
{
    NSString* fileName = [[[NSString alloc]initWithUTF8String:url] autorelease];
    NSURL* nsurl = [NSURL URLWithString:fileName];
    [[UIApplication sharedApplication] openURL:nsurl];
}
int GetAppID();
void    gotoComment()
{
    NSString* str = [NSString stringWithFormat:@"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=%d", GetAppID()];
    [[UIApplication sharedApplication]openURL:[NSURL URLWithString:str]];
    
}


void    hideAd()
{
    if(!g_myView)
        return;
    [g_myView hideAd];
}

void    showAd(BOOL bAdMob, int x, int y)
{
    if(!g_myView)
        return;
    [g_myView showAd:bAdMob:x:y];//->m_adView setHidden:true];
}
void    gotoBuyRemoveAD()
{
    if(!g_myView)
        return;
    [g_myView gotoBuy];
}
@interface ViewController () {
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
@end

@implementation ViewController

@synthesize context = _context;

- (void) gotoBuy
{
    [m_store BuyItem:kNoADs];
}
- (void) hideAd
{
//    [m_adView setHidden:true];
}
- (void) showAd:(BOOL)bAdMob:(int)x:(int)y
{
    if(!GetAdViewEnabled())
        return;
//    m_adView.frame = CGRectOffset(m_adView.frame, x, y);
//    [m_adView setHidden:false];
}
- (id)initWithB:(NSString *)nibNameOrNil bundle:(NSBundle *)nibBundleOrNil
{
    return [super initWithNibName:nibNameOrNil bundle:nibBundleOrNil];
}
- (void)dealloc
{
    [_context release];
    [super dealloc];
}
- (void) viewDidAppear:(BOOL)animated
{
    
}
- (void) myLoadView
{
    g_myView = self;
    self.context = [[[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2] autorelease];
    
    if (!self.context) {
        NSLog(@"Failed to create ES context");
    }
    m_store = [[MyStoreKit alloc] init];
    GLKView *view = (GLKView *)self.view;
    view.context = self.context;
    view.drawableDepthFormat = GLKViewDrawableDepthFormat24;
    [self setupGL];
//    if(GetAdViewEnabled())
//    {
//        //iad
//        m_adView = [[[ADBannerView alloc] initWithFrame:CGRectZero] autorelease];
//        m_adView.currentContentSizeIdentifier = ADBannerContentSizeIdentifierPortrait;
//        m_adView.frame = CGRectOffset(m_adView.frame, 0, 0);
//        [self.view addSubview:m_adView];
//        [m_adView setHidden:true];
//    }

}
- (void)viewDidLoad
{
    [super viewDidLoad];
    [self myLoadView];
    
}

- (void)viewDidUnload
{
    [super viewDidUnload];
    [m_store release];
    [self tearDownGL];
    
    if ([EAGLContext currentContext] == self.context) {
        [EAGLContext setCurrentContext:nil];
    }
    self.context = nil;
}

- (void)didReceiveMemoryWarning
{
    [super didReceiveMemoryWarning];
    // Release any cached data, images, etc. that aren't in use.
}

- (BOOL)shouldAutorotateToInterfaceOrientation:(UIInterfaceOrientation)interfaceOrientation
{
    if ([[UIDevice currentDevice] userInterfaceIdiom] == UIUserInterfaceIdiomPhone) {
        return (interfaceOrientation == UIDeviceOrientationPortrait);
    } else {
        return YES;
    }
}

- (void)setupGL
{
    [EAGLContext setCurrentContext:self.context];
    CreateMainSDK();
    OnAppRestore();
    OnAppResize(0,self.view.bounds.size.width , self.view.bounds.size.height,self.view.bounds.size.width , self.view.bounds.size.height);
}

- (void)tearDownGL
{
    OnAppLost();
    DestroyAllSDK();
   [EAGLContext setCurrentContext:self.context];

}

- (void)touchesBegan:(NSSet *)touches withEvent:(UIEvent *)event
{
    int vcount = [touches count];
    for(int i=0;i<vcount;i++)
    {
        UITouch* touch = [[touches allObjects] objectAtIndex:i];
        CGPoint pt = [touch locationInView:self.view];
        OnAppInputEvent(2, pt.x, pt.y, i);
    }
    [super touchesBegan:touches withEvent:event];
}
- (void)touchesMoved:(NSSet *)touches withEvent:(UIEvent *)event
{
    int vcount = [touches count];
    for(int i=0;i<vcount;i++)
    {
        UITouch* touch = [[touches allObjects] objectAtIndex:i];
        CGPoint pt = [touch locationInView:self.view];
        OnAppInputEvent(1, pt.x, pt.y, i);
    }
    [super touchesMoved:touches withEvent:event];
}
- (void)touchesEnded:(NSSet *)touches withEvent:(UIEvent *)event
{
    int vcount = [touches count];
    for(int i=0;i<vcount;i++)
    {
        UITouch* touch = [[touches allObjects] objectAtIndex:i];
        CGPoint pt = [touch locationInView:self.view];
        OnAppInputEvent(4, pt.x, pt.y, i);
    }
    [super touchesEnded:touches withEvent:event];
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    
}
- (void)update
{
    OnAppResize(0,self.view.bounds.size.width , self.view.bounds.size.height,self.view.bounds.size.width , self.view.bounds.size.height);
    OnAppFrameMove();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    OnAppRender();
}

@end
