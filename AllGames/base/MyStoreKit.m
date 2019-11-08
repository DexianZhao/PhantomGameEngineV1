//
//  ChessStoreKit.m
//  ChinaChess
//
//  Created by cosmic-life on 12-8-4.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#import "MyStoreKit.h"
#import <CoreMotion/CoreMotion.h>

const char* GetDeviceName(char* ch){
    NSString* name = [[UIDevice currentDevice] name];
    strcpy(ch, [name UTF8String]);
    return ch;
}
extern void   SetBuyADOK(const char* payid, const char* transid, BOOL bFail);


@implementation MyStoreKit

-(id)init
{
    if ((self = [super init]))
    {
        //----监听购买结果
        [[SKPaymentQueue defaultQueue] addTransactionObserver:self];
        //申请购买
        //[self buy:IAP24p99];
        bRequestBuy = false;
    }
    return self;
}

-(void)BuyItem:(NSString*)type
{
    if(bRequestBuy)
        return;
    bRequestBuy = true;
    buyProductID = [[NSString alloc] initWithString:type];
    if ([SKPaymentQueue canMakePayments])
    {
        //[[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
        [self RequestProductData];
        NSLog(@"允许程序内付费购买");
    }
    else
    {
        NSLog(@"不允许程序内付费购买");
        UIAlertView *alerView =  [[UIAlertView alloc] initWithTitle:@"Alert" message:@"没允许应用程序内购买" delegate:nil cancelButtonTitle:NSLocalizedString(@"Close（关闭）",nil) otherButtonTitles:nil];
        [alerView show];
        [alerView release];
    }
} 

-(bool)CanMakePayment
{
    return [SKPaymentQueue canMakePayments];
}

-(void)RequestProductData
{
    NSLog(@"---------请求对应的产品信息------------");
    NSArray *product = nil;
    product=[[NSArray alloc] initWithObjects:buyProductID,nil];
    NSSet *nsset = [NSSet setWithArray:product];
    SKProductsRequest *request=[[SKProductsRequest alloc] initWithProductIdentifiers: nsset];
    request.delegate=self;
    [request start];
    [product release];
}
//<SKProductsRequestDelegate> 请求协议
//收到的产品信息
- (void)productsRequest:(SKProductsRequest *)request didReceiveResponse:(SKProductsResponse *)response
{
    NSLog(@"-----------收到产品反馈信息--------------");
    NSArray *myProduct = response.products;
    NSLog(@"产品Product ID:%@",response.invalidProductIdentifiers);
    NSLog(@"产品付费数量: %d", [myProduct count]);
    // populate UI
    SKProduct *product = nil;
    SKProduct *buyProduct = nil;
    for(product in myProduct)
    {
        if([product.productIdentifier isEqualToString: buyProductID])
        {
            buyProduct = product;
        }
        NSLog(@"product info");
        NSLog(@"SKProduct 描述信息%@", [product description]); 
        NSLog(@"产品标题 %@" , product.localizedTitle);
        NSLog(@"产品描述信息: %@" , product.localizedDescription);
        NSLog(@"价格: %@" , product.price);
        //NSLog(@"Product id: %@" , product.productIdentifier);
    }
    if(buyProduct != nil)
    {
        SKPayment *payment = nil;//[SKPayment paymentWithProductIdentifier:buyProductID];//
        payment  = [SKPayment paymentWithProduct:buyProduct];//支付$0.99
#ifdef DEBUG
        NSLog(@"---------%@------------", buyProduct.productIdentifier);
#endif
        NSLog(@"---------发送购买请求------------");
        [[SKPaymentQueue defaultQueue] addPayment:payment];
    }
    else {
        bRequestBuy = false;
    }
    [request autorelease];
}

//弹出错误信息
- (void)request:(SKRequest *)request didFailWithError:(NSError *)error
{
    NSLog(@"-------弹出错误信息----------");
    UIAlertView *alerView =  [[UIAlertView alloc] initWithTitle:NSLocalizedString(@"Alert",NULL) message:[error localizedDescription] delegate:nil cancelButtonTitle:NSLocalizedString(@"Close",nil) otherButtonTitles:nil];
    [alerView show];
    [alerView release];
}

-(void) requestDidFinish:(SKRequest *)request
{
    NSLog(@"----------反馈信息结束--------------");
}

-(void) PurchasedTransaction: (SKPaymentTransaction *)transaction
{
    NSLog(@"-----PurchasedTransaction----");
    NSArray *transactions =[[NSArray alloc] initWithObjects:transaction, nil];
    [self paymentQueue:[SKPaymentQueue defaultQueue] updatedTransactions:transactions];
    [transactions release];
}
//<SKPaymentTransactionObserver> 千万不要忘记绑定，代码如下：
//----监听购买结果
//[[SKPaymentQueue defaultQueue] addTransactionObserver:self];
- (void)paymentQueue:(SKPaymentQueue *)queue updatedTransactions:(NSArray *)transactions//交易结果
{
    bRequestBuy = false;
    NSLog(@"-----paymentQueue--------");
    for (SKPaymentTransaction *transaction in transactions)
    {
        switch (transaction.transactionState)
        {
            case SKPaymentTransactionStatePurchased://交易完成
                [self completeTransaction:transaction];
                NSLog(@"-----交易完成 --------");
                UIAlertView *alerView =  [[UIAlertView alloc] initWithTitle:@"Congratulations" message:@"购买成功" delegate:nil cancelButtonTitle:NSLocalizedString(@"Close（关闭）",nil) otherButtonTitles:nil];
                [alerView show];
                [alerView release];
                break;
            case SKPaymentTransactionStateFailed://交易失败
                [self failedTransaction:transaction];
                NSLog(@"-----交易失败 --------");
                UIAlertView *alerView2 =  [[UIAlertView alloc] initWithTitle:@"Info" message:@"购买失败，请重新尝试购买" delegate:nil cancelButtonTitle:NSLocalizedString(@"Close（关闭）",nil) otherButtonTitles:nil];
                [alerView2 show];
                [alerView2 release];
                break;
            case SKPaymentTransactionStateRestored://已经购买过该商品
                [self restoreTransaction:transaction];
                NSLog(@"-----已经购买过该商品 --------");
            case SKPaymentTransactionStatePurchasing:  //商品添加进列表
                NSLog(@"-----商品添加进列表 --------");
                break;
            default:
                break;
        }
    }
}

- (void) completeTransaction: (SKPaymentTransaction *)transaction
{
    NSString* transid = [transaction transactionIdentifier];
    NSString* payid = [[transaction payment] productIdentifier];
    NSLog(@"-----completeTransaction----%@---%@", payid, transid);
    SetBuyADOK([payid cStringUsingEncoding:NSUTF8StringEncoding], [transid cStringUsingEncoding:NSUTF8StringEncoding], false);
    // Your application should implement these two methods.
    NSString *product = transaction.payment.productIdentifier;
    if ([product length] > 0)
    {
        NSArray *tt = [product componentsSeparatedByString:@"."];
        NSString *bookid = [tt lastObject];
        if ([bookid length] > 0) {
            [self recordTransaction:bookid];
            [self provideContent:bookid];
        }
    }
    // Remove the transaction from the payment queue.
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

//记录交易
-(void)recordTransaction:(NSString *)product
{
    NSLog(@"-----记录交易--------");
}

//处理下载内容
-(void)provideContent:(NSString *)product
{
    NSLog(@"-----下载--------");
}

- (void) failedTransaction: (SKPaymentTransaction *)transaction
{
    NSString* transid = [transaction transactionIdentifier];
    NSString* payid = [[transaction payment] productIdentifier];
    NSLog(@"-----failedTransaction----%@---%@", payid, transid);
    SetBuyADOK([payid cStringUsingEncoding:NSUTF8StringEncoding], [transid cStringUsingEncoding:NSUTF8StringEncoding], false);
    if (transaction.error.code != SKErrorPaymentCancelled)
    {
    }
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}
-(void) paymentQueueRestoreCompletedTransactionsFinished: (SKPaymentTransaction *)transaction
{
    
}

- (void) restoreTransaction: (SKPaymentTransaction *)transaction
{
    NSString* transid = [transaction transactionIdentifier];
    NSString* payid = [[transaction payment] productIdentifier];
    NSLog(@"-----restoreTransaction----%@---%@", payid, transid);
    SetBuyADOK([payid cStringUsingEncoding:NSUTF8StringEncoding], [transid cStringUsingEncoding:NSUTF8StringEncoding], false);
    NSLog(@"交易恢复处理. SetBuyADOK");
    [[SKPaymentQueue defaultQueue] finishTransaction: transaction];
}

-(void) paymentQueue:(SKPaymentQueue *) paymentQueue restoreCompletedTransactionsFailedWithError:(NSError *)error
{
    NSLog(@"-------paymentQueue----");
}

#pragma mark connection delegate
- (void)connection:(NSURLConnection *)connection didReceiveData:(NSData *)data
{
    NSLog(@"%@",  [[[NSString alloc] initWithData:data encoding:NSUTF8StringEncoding] autorelease]);
}
- (void)connectionDidFinishLoading:(NSURLConnection *)connection{
    
}


- (void)connection:(NSURLConnection *)connection didReceiveResponse:(NSURLResponse *)response
{
    switch([(NSHTTPURLResponse *)response statusCode])
    {
        case 200:
        case 206:
            break;
        case 304:
            break;
        case 400:
            break;
        case 404:
            break;
        case 416:
            break;
        case 403:
            break;
        case 401:
        case 500:
            break;
        default:
            break;
    } 
}

- (void)connection:(NSURLConnection *)connection didFailWithError:(NSError *)error
{
    NSLog(@"test");
}

-(void)dealloc
{
    [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];//解除监听
    [super dealloc];
}

@end
