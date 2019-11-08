//
//  ChessStoreKit.m
//  ChinaChess
//
//  Created by cosmic-life on 12-8-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "MyGameCenter.h"

#pragma mark

@implementation MyGameKit
@synthesize gameCenterAvailable;
@synthesize userAuthenticated;
static MyGameKit* sharedHelper = nil;
+(MyGameKit*)shareInstance{
    if(!sharedHelper)
    {
        sharedHelper=[[MyGameKit alloc]init];
    }
    return sharedHelper;
}
-(BOOL)isGameCenterAvailable{
    Class gcClass = (NSClassFromString(@"GKLocalPlayer"));
    NSString* reqSysVer = @"4.1";
    NSString* currSysVer = [[UIDevice currentDevice]systemVersion];
    BOOL osVersionSupported = ([currSysVer compare:reqSysVer options:NSNumericSearch]!=NSOrderedAscending);
    return (gcClass&&osVersionSupported);
}
-(id)init
{
    if ((self = [super init]))
    {
        view = nil;
        gameCenterAvailable = [self isGameCenterAvailable];
        if(gameCenterAvailable){
            NSNotificationCenter* nc = [NSNotificationCenter defaultCenter];
            [nc addObserver:self selector:@selector(authenticationChanged) name:GKPlayerAuthenticationDidChangeNotificationName object:nil];
        }
    }
    return self;
}
- (void)leaderboardViewControllerDidFinish:(GKLeaderboardViewController *)viewController
{
    [view dismissModalViewControllerAnimated:YES];
}
-(void) authenticationChanged{
    if([GKLocalPlayer localPlayer].isAuthenticated&&!userAuthenticated)
    {
        NSLog(@"Authentication changed: player authenticated.");
        userAuthenticated=true;
    }
    else if(![GKLocalPlayer localPlayer].isAuthenticated&&userAuthenticated)
    {
        NSLog(@"Authentication chagned: player not authenticated");
        userAuthenticated=false;
    }
}
-(void)authenticateLocalUser{
    if(!gameCenterAvailable)return;
    NSLog(@"Authenticating local user...");
    if([GKLocalPlayer localPlayer].authenticated==NO){
        [[GKLocalPlayer localPlayer] authenticateWithCompletionHandler:nil];
    }else{
        NSLog(@"Already authenticated!");
    }
}
- (void)setParent:(UIViewController*)v
{
    view = v;
}
-(void)showLeaderboard:(NSString*)category;
{
    GKLeaderboardViewController* leaderboard = [[GKLeaderboardViewController alloc] init];
    if(leaderboard != nil)
    {
        leaderboard.leaderboardCategory = category;
        leaderboard.leaderboardDelegate=self;
        [view presentModalViewController:leaderboard animated:YES];
    }
}
- (void)showGameCenter
{
    GKGameCenterViewController* gamecenter = [[GKGameCenterViewController alloc]init];
    if(gamecenter!=nil)
    {
        gamecenter.gameCenterDelegate=self;
        [view presentModalViewController:gamecenter animated:YES];
    }
}
- (void)gameCenterViewControllerDidFinish:(GKGameCenterViewController *)gameCenterViewController
{
   [view dismissModalViewControllerAnimated:YES];
}
- (void)reportScore:(int64_t) score :(NSString*)category
{
    if(!userAuthenticated)return;
    GKScore* scoreReporter = [[[GKScore alloc]initWithCategory:category] autorelease];
    scoreReporter.value = score;
    [scoreReporter reportScoreWithCompletionHandler:^(NSError *error) {
        if(error==NULL)
        {
            NSLog(@"reportScore ok");
        }
        else{
            NSLog(@"reportScore error");
        }
    }];
}
- (void)reportAchievement:(NSString*)identifier :(float)percent
{
    if(!userAuthenticated)return;
    GKAchievement* achievement = [[[GKAchievement alloc]initWithIdentifier:identifier] autorelease];
    if(achievement)
    {
        achievement.percentComplete = percent;
        [achievement reportAchievementWithCompletionHandler:^(NSError *error) {
            if(error != nil)
            {
                NSLog(@"reportAchievement fail");
            }
            else{
                NSLog(@"reportAchievement ok");
            }
        }];
    }
}
@end
