//
//  ViewController.m
//  PhantomIOSSDK
//
//  Created by cosmic-life on 12-7-25.
//  Copyright (c) 2012Âπ?__MyCompanyName__. All rights reserved.
//

#import "ViewController.h"
#include "MyInclude.h"
#import <iAd/iAd.h>
#import "MyGameCenter.h"

extern void GetCurrentResolution(int* width, int* height, int landScape);
extern void CreateMainSDK();
extern void DestroyAllSDK();
extern void updateMotionData();
ViewController* g_myView = 0;
extern int GetAdViewEnabled();
int g_orientation = 1;
void    SetViewOrientation(int ori)
{
    g_orientation = ori;
}

void	openUrl(const char* url)
{
    NSString* fileName = [[[NSString alloc]initWithUTF8String:url] autorelease];
    NSURL* nsurl = [NSURL URLWithString:fileName];
    [[UIApplication sharedApplication] openURL:nsurl];
}
const char* onAlert(const char* message, const char* title, const char* type)
{
    NSString* _message = [[[NSString alloc]initWithUTF8String:message] autorelease];
    NSString* _title = [[[NSString alloc]initWithUTF8String:title] autorelease];

    UIAlertView *alertView =  nil;
    if(strcasecmp(type,"ok")==0)
        alertView = [[UIAlertView alloc] initWithTitle:_title message:_message delegate:nil cancelButtonTitle:@"OK" otherButtonTitles:nil];
    else if(strcasecmp(type,"yesno")==0)
        alertView = [[UIAlertView alloc] initWithTitle:_title message:_message delegate:nil cancelButtonTitle:@"Yes" otherButtonTitles:@"No"];
    else if(strcasecmp(type,"yesnocancel")==0)
        alertView = [[UIAlertView alloc] initWithTitle:_title message:_message delegate:nil cancelButtonTitle:@"Yes" otherButtonTitles:@"No",@"Cancel"];
    [alertView show];
    [alertView release];
    return "";
}
int GetAppID();

void    openRank(const char* rank);
void    gotoComment()
{
    openRank("");
//    NSString* str = [NSString stringWithFormat:@"itms-apps://ax.itunes.apple.com/WebObjects/MZStore.woa/wa/viewContentsUserReviews?type=Purple+Software&id=%d", GetAppID()];
  //  [[UIApplication sharedApplication]openURL:[NSURL URLWithString:str]];
    
}

void    shopBuy(const char* buyName)
{
    NSString* nsbuy = [[[NSString alloc]initWithUTF8String:buyName] autorelease];
    [g_myView gotoBuy:nsbuy];
}
void    shopRestore(const char* buyName)
{
    
}

void    openRank(const char* rank)
{
//    NSString* nsrank = [[[NSString alloc]initWithUTF8String:rank] autorelease];
    [[MyGameKit shareInstance] showGameCenter];// showLeaderboard:nsrank];
}
void    setRankScore(const char* rank,int score)
{
    NSString* nsrank = [[[NSString alloc]initWithUTF8String:rank] autorelease];
    [[MyGameKit shareInstance] reportScore:score :nsrank];
}
void    setAchievement(const char* a, int percent)
{
    NSString* nsrank = [[[NSString alloc]initWithUTF8String:a] autorelease];
    [[MyGameKit shareInstance] reportAchievement:nsrank:percent];
}

void    hideAd()
{
    if(!g_myView)
        return;
    [g_myView hideAd];
}

void    showAd(int x, int y, int type, int bShow)
{
    if(type==2){
        [g_myView showWall];
    }
}
void    gotoBuyRemoveAD()
{
    if(!g_myView)
        return;
    [g_myView gotoBuy:kNoADs];
}
@interface ViewController () {
}
@property (strong, nonatomic) EAGLContext *context;

- (void)setupGL;
- (void)tearDownGL;
@end

@implementation ViewController
+ (ViewController*)instance
{
    return g_myView;
}
@synthesize context = _context;

- (void) gotoBuy:(NSString*)ns
{
    [m_store BuyItem:ns];
}
- (void) hideAd
{
    if(m_adView)
        [m_adView setHidden:true];
}
- (void) showWall
{
//    if(m_dwall==nil)
//            m_dwall = [[DuoMobImpWall alloc] initv:self:@"96ZJ1luAze5kvwTAdI"];
//    if(m_dwall)
//        [m_dwall showWall:true];
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
    [[MyGameKit shareInstance] authenticateLocalUser];
    [[MyGameKit shareInstance] setParent:self];
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

}
- (void)viewDidLoad
{
    [super viewDidLoad];
    [self myLoadView];
//    m_dwall = nil;//[[DuoMobImpWall alloc] initv:self:@"96ZJ1luAze5kvwTAdI"];
}
- (NSUInteger)supportedInterfaceOrientations NS_AVAILABLE_IOS(6_0)
{
    if(g_orientation==0)
        return UIInterfaceOrientationMaskPortrait|UIInterfaceOrientationMaskPortraitUpsideDown;
    return UIInterfaceOrientationMaskLandscape;
}
- (BOOL)shouldAutorotate NS_AVAILABLE_IOS(6_0)
{
    return true;
}
- (void)didRotateFromInterfaceOrientation:(UIInterfaceOrientation)fromInterfaceOrientation
{
}
- (void)viewDidUnload
{
    [super viewDidUnload];
    [m_store release];
//    if(m_dwall)
//        [m_dwall release];
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
    int bw = self.view.bounds.size.width;
    int bh = self.view.bounds.size.height;
    int devw = 0,devh = 0;
    GetCurrentResolution(&devw, &devh, self.view.bounds.size.width>self.view.bounds.size.height);
    int n = 1;
    if(self.interfaceOrientation==UIDeviceOrientationPortrait)
        n=1;
    else if(self.interfaceOrientation==UIDeviceOrientationPortraitUpsideDown)
        n=2;
    else if(self.interfaceOrientation==UIDeviceOrientationLandscapeLeft)
        n=3;
    else if(self.interfaceOrientation==UIDeviceOrientationLandscapeRight)
        n=4;
    OnAppResize(n, bw, bh,devw,devh);
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
        int index = -1;
        for(int j=0;j<8;j++){
            if(m_touchs[j]==nil)
            {
                m_touchs[j] = touch;
                index = j;
                break;
            }
        }
        if(index>=0)
            OnAppInputEvent(2, pt.x, pt.y, index);

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
        int index = -1;
        for(int j=0;j<8;j++){
            if(m_touchs[j]==touch)
            {
                index = j;
                break;
            }
        }
        if(index>=0)
            OnAppInputEvent(1, pt.x, pt.y, index);
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
        int index = -1;
        for(int j=0;j<8;j++){
            if(m_touchs[j]==touch)
            {
                m_touchs[j] = nil;
                index = j;
                break;
            }
        }
        if(index>=0)
            OnAppInputEvent(4, pt.x, pt.y, index);
    }
    [super touchesEnded:touches withEvent:event];
}
- (void)touchesCancelled:(NSSet *)touches withEvent:(UIEvent *)event
{
    
    int vcount = [touches count];
    for(int i=0;i<vcount;i++)
    {
        UITouch* touch = [[touches allObjects] objectAtIndex:i];
        CGPoint pt = [touch locationInView:self.view];
        int index = -1;
        for(int j=0;j<8;j++){
            if(m_touchs[j]==touch)
            {
                m_touchs[j] = nil;
                index = j;
                break;
            }
        }
        if(index>=0)
            OnAppInputEvent(4, pt.x, pt.y, index);
    }
    [super touchesCancelled:touches withEvent:event];
}
- (void)update
{
    int devw = 0,devh = 0;
    int n = 1;
    if(self.interfaceOrientation==UIDeviceOrientationPortrait)
        n=1;
    else if(self.interfaceOrientation==UIDeviceOrientationPortraitUpsideDown)
        n=2;
    else if(self.interfaceOrientation==UIDeviceOrientationLandscapeLeft)
        n=3;
    else if(self.interfaceOrientation==UIDeviceOrientationLandscapeRight)
        n=4;
    GetCurrentResolution(&devw, &devh, self.view.bounds.size.width>self.view.bounds.size.height);
    OnAppResize(n, self.view.bounds.size.width , self.view.bounds.size.height, devw, devh);
    updateMotionData();
    OnAppFrameMove();
}

- (void)glkView:(GLKView *)view drawInRect:(CGRect)rect
{
    OnAppRender();
}

@end
