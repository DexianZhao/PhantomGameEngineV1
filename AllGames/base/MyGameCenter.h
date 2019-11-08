//
//  ChessStoreKit.h
//  ChinaChess
//
//  Created by ZhaoDexian on 12-8-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//
#import <Foundation/Foundation.h>
#import <GameKit/GameKit.h>

@interface MyGameKit : NSObject<GKLeaderboardViewControllerDelegate, GKGameCenterControllerDelegate>
{
    BOOL        gameCenterAvailable;
    BOOL        userAuthenticated;
    UIViewController*   view;
}
@property(assign,readonly) BOOL gameCenterAvailable;
@property(assign,readonly) BOOL userAuthenticated;
+ (MyGameKit*) shareInstance;
- (void)setParent:(UIViewController*)v;
- (void)authenticationChanged;
- (void)authenticateLocalUser;
- (void)showLeaderboard:(NSString*)category;
- (void)showGameCenter;
- (void)reportScore:(int64_t) score :(NSString*)category;
- (void)reportAchievement:(NSString*)identifier :(float)percent;
@end
