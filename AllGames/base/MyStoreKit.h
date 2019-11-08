//
//  ChessStoreKit.h
//  ChinaChess
//
//  Created by ZhaoDexian on 12-8-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <StoreKit/StoreKit.h>

#define kNoADs @"com.rpg2d.chinachess.noads"
const char* GetDeviceID(char*ch);
const char* GetDeviceName();


@interface MyStoreKit : NSObject<SKProductsRequestDelegate, SKPaymentTransactionObserver>
{
    BOOL        bRequestBuy;
    NSString*   buyProductID;//it's the product id
}

- (bool)CanMakePayment;//允许程序内付费购买
- (void)BuyItem:(NSString*)productID;
//protected
- (void)RequestProductData;
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions;
- (void)PurchasedTransaction: (SKPaymentTransaction *)transaction;
- (void)completeTransaction: (SKPaymentTransaction *)transaction;
- (void)failedTransaction: (SKPaymentTransaction *)transaction;
- (void)paymentQueueRestoreCompletedTransactionsFinished: (SKPaymentTransaction *)transaction;
- (void)paymentQueue:(SKPaymentQueue *) paymentQueue restoreCompletedTransactionsFailedWithError:(NSError *)error;
- (void)restoreTransaction: (SKPaymentTransaction *)transaction;
- (void)provideContent:(NSString *)product;
- (void)recordTransaction:(NSString *)product;

@end
