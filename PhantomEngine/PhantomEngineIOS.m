//
//  AppDelegate.m
//  PhantomGameEngine
//
//  Created by cosmic-life on 12-7-8.
//  Copyright (c) 2012年 __MyCompanyName__. All rights reserved.
//

#include "CInclude.h"
#include "MyInclude.h"
#import <AVFoundation/AVFoundation.h>
#import <AudioToolbox/AudioToolbox.h>
#import <CoreGraphics/CoreGraphics.h>
#import <PhantomEngineIOS.h>
#import <UIKit/UIKit.h>
#import <CoreMotion/CoreMotion.h>
#import <GLKit/GLKMatrix4.h>
#pragma pack(push,1)
#ifdef __ARM_NEON__
struct matrix4x4_t{
    float32x4_t val[4];
};
#pragma pack(pop)
float* 			matrixMultiply2( float *pOut, const float *pM1, const float *pM2 )
{
#if defined(__ARM_NEON__)
    struct matrix4x4_t* iMatrixLeft = (struct matrix4x4_t *)pM2;
    struct matrix4x4_t* iMatrixRight = (struct matrix4x4_t *)pM1;
    struct matrix4x4_t m;
    
    m.val[0] = vmulq_n_f32(iMatrixLeft->val[0], vgetq_lane_f32(iMatrixRight->val[0], 0));
    m.val[1] = vmulq_n_f32(iMatrixLeft->val[0], vgetq_lane_f32(iMatrixRight->val[1], 0));
    m.val[2] = vmulq_n_f32(iMatrixLeft->val[0], vgetq_lane_f32(iMatrixRight->val[2], 0));
    m.val[3] = vmulq_n_f32(iMatrixLeft->val[0], vgetq_lane_f32(iMatrixRight->val[3], 0));
    
    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft->val[1], vgetq_lane_f32(iMatrixRight->val[0], 1));
    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft->val[1], vgetq_lane_f32(iMatrixRight->val[1], 1));
    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft->val[1], vgetq_lane_f32(iMatrixRight->val[2], 1));
    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft->val[1], vgetq_lane_f32(iMatrixRight->val[3], 1));
    
    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft->val[2], vgetq_lane_f32(iMatrixRight->val[0], 2));
    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft->val[2], vgetq_lane_f32(iMatrixRight->val[1], 2));
    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft->val[2], vgetq_lane_f32(iMatrixRight->val[2], 2));
    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft->val[2], vgetq_lane_f32(iMatrixRight->val[3], 2));
    
    m.val[0] = vmlaq_n_f32(m.val[0], iMatrixLeft->val[3], vgetq_lane_f32(iMatrixRight->val[0], 3));
    m.val[1] = vmlaq_n_f32(m.val[1], iMatrixLeft->val[3], vgetq_lane_f32(iMatrixRight->val[1], 3));
    m.val[2] = vmlaq_n_f32(m.val[2], iMatrixLeft->val[3], vgetq_lane_f32(iMatrixRight->val[2], 3));
    m.val[3] = vmlaq_n_f32(m.val[3], iMatrixLeft->val[3], vgetq_lane_f32(iMatrixRight->val[3], 3));
    struct matrix4x4_t* ret = (struct matrix4x4_t*)pOut;
    *ret = m;
    return pOut;//*(GLKMatrix4 *)&m;
#endif
//    GLKMatrix4* po = (GLKMatrix4*)pOut;
//    *po = GLKMatrix4Multiply(*(GLKMatrix4*)pM2, *(GLKMatrix4*)pM1);
//    return pOut;
}
float* 			matrixInverse2( float *pOut, float *pDeterminant, const float *pM )
{
    GLKMatrix4* po = (GLKMatrix4*)pOut;
    *po = GLKMatrix4Invert(*(GLKMatrix4*)pM, 0);
    return pOut;
}
#endif

void	onVibrator(float fSecond)
{
    AudioServicesPlaySystemSound(kSystemSoundID_Vibrate);
}

#define MAX_THREAD_COUNT 5
NSThread* g_threads[MAX_THREAD_COUNT];

struct ThreadParam
{
    ThreadProc proc;
    const void* param;
};

#define MAX_ONE_WAVE_COUNT  20
struct AudioData
{
    AVAudioPlayer*  wave[MAX_ONE_WAVE_COUNT];
	int				maxWave;
    double          lastTime;
    int             index;
    NSURL*          filePath;
};
#define MAX_AUDIO_COUNT 200
struct AudioData   g_audios[MAX_AUDIO_COUNT];
void DeleteDirectorys(const char* folders)
{
    NSString* str = [NSString stringWithUTF8String:folders];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSArray *contents = [fileManager contentsOfDirectoryAtPath:str error:NULL];
    NSEnumerator *e = [contents objectEnumerator];
    NSString *filename;
    while ((filename = [e nextObject])) {
        [fileManager removeItemAtPath:[str stringByAppendingPathComponent:filename] error:NULL];
    }
}
extern void	OnSearchFile(const char* szFile, const void* ptr);
void SearchFiles(const char* folders, const void* ptr)
{
    NSString* str = [NSString stringWithUTF8String:folders];
    NSFileManager *fileManager = [NSFileManager defaultManager];
    NSDirectoryEnumerator* direnum = [fileManager enumeratorAtPath:str];
    NSString *filename;
    while ((filename = [direnum nextObject])) {
        
		OnSearchFile([filename UTF8String], ptr);
        //[fileManager removeItemAtPath:[str stringByAppendingPathComponent:filename] error:NULL];
    }
}
void    UpdateAcc(float accx,float accy,float accz, long timestamp);
void    UpdateGryo(float accx,float accy,float accz, long timestamp);
void updateMotionData()//update motion data to engine
{
    static CMMotionManager* mn = 0;
    if(!mn){
        mn = [[CMMotionManager alloc]init];
        mn.deviceMotionUpdateInterval = 1.0/30.0;
        if(mn.isDeviceMotionAvailable)
            [mn startDeviceMotionUpdates];
//        mn.accelerometerUpdateInterval=1.0/30.0;
//        static NSOperationQueue* queue = 0;
//        queue = [[NSOperationQueue alloc]init];
//        if(mn.isAccelerometerAvailable){
//            [mn startAccelerometerUpdatesToQueue:queue withHandler:^(CMAccelerometerData *accelerometerData, NSError *error) {
//                if(error){
//                    
//                }
//                else{
//                    UpdateAcc(-accelerometerData.acceleration.x*10.0, accelerometerData.acceleration.y*10.0, accelerometerData.acceleration.z*10.0,accelerometerData.timestamp);
//                }
//            }];
//        }
//        if(mn.isGyroAvailable){
//            [mn startGyroUpdatesToQueue:queue withHandler:^(CMGyroData *gyroData, NSError *error) {
//                if(error){
//                    
//                }
//                else{
//                    UpdateGryo(-gyroData.rotationRate.x*10.0,gyroData.rotationRate.y*10.0,gyroData.rotationRate.z*10.0, gyroData.timestamp);
//                }
//            }];
//        }
    }
//    CMAccelerometerData *newestAccel = mn.accelerometerData;
//    if(newestAccel!=nil){
//        UpdateAcc(newestAccel.acceleration.x, newestAccel.acceleration.y, newestAccel.acceleration.z,newestAccel.timestamp);
//    }
//    if(mn.gyroActive&&mn.gyroAvailable){
//        UpdateGryo(mn.gyroData.rotationRate.x,mn.gyroData.rotationRate.y,mn.gyroData.rotationRate.z, mn.gyroData.timestamp);
//    }
    //
    if(mn.deviceMotion==nil)return;
    double gravityY = mn.deviceMotion.gravity.x;
    double gravityX = mn.deviceMotion.gravity.y;
    double gravityZ = mn.deviceMotion.gravity.z;
    
    double zTheta = atan2(gravityZ,sqrtf(gravityX*gravityX+gravityY*gravityY))/M_PI*180.0;
    
    double xyTheta = atan2(gravityX,gravityY)/M_PI*180.0;
    
    CMRotationRate rotationRate = mn.deviceMotion.rotationRate;
    double rotationX = rotationRate.x;
    double rotationY = rotationRate.y;
    double rotationZ = rotationRate.z;
    
    double roll    = mn.deviceMotion.attitude.roll;
    double pitch   = mn.deviceMotion.attitude.pitch;
    double yaw     = mn.deviceMotion.attitude.yaw;
    
//    double w = mn.deviceMotion.attitude.quaternion.w;
//    double wx = mn.deviceMotion.attitude.quaternion.x;
//    double wy = mn.deviceMotion.attitude.quaternion.y;
//    double wz = mn.deviceMotion.attitude.quaternion.z;
    UpdateMotion(gravityX, gravityY, gravityZ, zTheta, xyTheta, rotationX, rotationY, rotationZ, roll, pitch, yaw);
}
void GetCurrentResolution(int* width, int* height, int landScape)
{
    if(UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone){
        if ([[UIScreen mainScreen] respondsToSelector: @selector(scale)]) {
            CGSize result = [[UIScreen mainScreen] bounds].size;
            result = CGSizeMake(result.width * [UIScreen mainScreen].scale, result.height * [UIScreen mainScreen].scale);
            if (result.height <= 480.0f)
            {
				*width = 320;
				*height = 480;
			}
            else if(result.height > 960)
			{
				*width = 640;
				*height = 1136;
			}
			else
			{
				*width = 640;
				*height = 960;
			}
        }
		else
        {
			*width = 320;
			*height = 480;
		}
    }
	else
	{
        if([[UIScreen mainScreen] respondsToSelector: @selector(scale)])
		{
			*width = 1024;
			*height = 768;
		}
		else{
			*width = 2048;
			*height= 1536;
		}
	}
	if(landScape){
		int temp = height;
		height = width;
		width = temp;
	}
}

int             GetOriginalSize(int w){
    int n = 2;
    for(int i=0;i<32;i++)
    {
        if(w<=n)
            return n;
        n=n*2;
    }
    return 1024;
}

int            DrawTextIOS_N(NSString* text, int textLength, int fontHeight, int fontStyle, const char* fontName, unsigned short*inoutW, unsigned short*inoutH, unsigned short*outImageW, unsigned short*outImageH, unsigned char** ret, int alignment)
{
    PhantomEngineIOS* ios = [PhantomEngineIOS sharedInstance];
    int width = 0;
    int height = 0;
    UIImage* img = [ios imageFromText:text fontName:nil nFontHeight:(fontHeight>0?fontHeight:-fontHeight) w:&width h:&height s:fontStyle align:alignment];
    if(img==nil)
        return false;
    int inW = *inoutW;
    int inH = *inoutH;
    unsigned int bpp = CGImageGetBitsPerPixel(img.CGImage);
    assert(bpp==32);
    inW = img.size.width;//GetOriginalSize(width);
    inH = img.size.height;//GetOriginalSize(height);
    int bytes = 4;//(bpp / 8);
    unsigned char* pixels = (unsigned char*)PhantomAlloc(bytes * inW * inH);
    memset(pixels, 0, bytes * inW * inH);
    CGColorSpaceRef colorSpace = CGImageGetColorSpace(img.CGImage);
    CGBitmapInfo info = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
//    size_t compPer = CGImageGetBytesPerRow(img.CGImage);
    size_t comp = CGImageGetBitsPerComponent(img.CGImage);
    CGContextRef context = CGBitmapContextCreate( pixels, inW, inH, comp, bytes * inW, colorSpace, info );
    CGContextClearRect(context, CGRectMake(0, 0, inW, inH));
    //CGContextTranslateCTM(context, 0, 0);
    CGContextDrawImage(context, CGRectMake(0, 0, inW, inH), img.CGImage);
    //
    CGContextRelease(context);
    //[img release];
    //
    *ret = pixels;
    *inoutW = inW;
    *inoutH = inH;
    *outImageW = width;
    *outImageH = height;
    return true;
}
//int            DrawTextIOS(const unsigned short* text, int textLength, int fontHeight, int fontStyle, const char* fontName, int*inoutW, int*inoutH, int*outImageW, int*outImageH, unsigned char** ret)
//{
//    NSString* str=[[[NSString alloc]initWithBytes:text length:textLength encoding:NSUTF16LittleEndianStringEncoding] autorelease];
//    return DrawTextIOS_N(str, textLength, fontHeight, fontStyle, fontName, inoutW, inoutH, outImageW, outImageH, ret);
//}
int Utf8ToWideOS(const char* text, int textlen, unsigned short* ret)
{
    NSString* str=[[[NSString alloc]initWithUTF8String:text] autorelease];
    NSUInteger retlen = 0;
    NSData* data = [str dataUsingEncoding:NSUTF16LittleEndianStringEncoding];
    retlen = data.length;
    memcpy(ret, data.bytes, retlen);
    return retlen/2;
}
int WideToUtf8OS(const unsigned short* text, int textlen, char* ret)
{
    NSString* str=[[[NSString alloc]initWithBytes:text length:textlen encoding:NSUTF16LittleEndianStringEncoding] autorelease];
    strcpy(ret, [str UTF8String]);
    return str.length;
}
int            DrawTextIOSA(const char* text, int textLength, int fontHeight, int fontStyle, const char* fontName, unsigned short*inoutW, unsigned short*inoutH, unsigned short*outImageW, unsigned short*outImageH, unsigned char** ret, int alignment)
{
    NSString* str = [NSString stringWithUTF8String:text];
    return DrawTextIOS_N(str, textLength, fontHeight, fontStyle, fontName, inoutW, inoutH, outImageW, outImageH, ret, alignment);
}
unsigned int	OpenWave(const char* szDir, const char* szFileName, const char* szExt, int ndup)
{
    if(szExt[0] == '.')
        szExt++;
    NSString* dir = [[[NSString alloc]initWithUTF8String:szDir] autorelease];
    NSString* fileName = [[[NSString alloc]initWithUTF8String:szFileName] autorelease];
    NSString* ext = [[[NSString alloc]initWithUTF8String:szExt] autorelease];
    NSURL* filePath = [[NSBundle mainBundle] URLForResource:[fileName lowercaseString] withExtension:[ext lowercaseString] subdirectory:[dir lowercaseString]];
    NSString* path = [[NSString alloc] initWithFormat:@"%@%@.%@", dir, fileName,ext];
    BOOL bPath = false;
    if (filePath == 0)
    {
        bPath = true;
        filePath = [[NSURL alloc] initFileURLWithPath:path];
        if(!filePath)
            return 0;
    }
    double dMax = 0;
    int iMax = 0;
    double dTime = GetAppTime();
    for(int i=0;i<MAX_AUDIO_COUNT;i++)
    {
        double diff = dTime - g_audios[i].lastTime;
        if(diff > dMax)
        {
            dMax = diff;
            iMax = i;
        }
        if(g_audios[i].filePath!=NULL)
            continue;
        if(bPath)
            g_audios[i].filePath = [[NSURL alloc] initFileURLWithPath:path];
        else
            g_audios[i].filePath = [[NSBundle mainBundle] URLForResource:[fileName lowercaseString] withExtension:[ext lowercaseString] subdirectory:[dir lowercaseString]];
        g_audios[i].maxWave=ndup;
//        g_wavPlayer[i] = [[AVAudioPlayer alloc] initWithContentsOfURL: filePath error: nil];
        return i + 1;
    }
    for(int i=0;i<g_audios[iMax].maxWave;i++)
    {
        if(g_audios[iMax].wave[i])
            [g_audios[iMax].wave[i] release];
        g_audios[iMax].wave[i] = 0;
    }
    g_audios[iMax].lastTime = dTime;
    g_audios[iMax].index = 0;
    if(bPath)
        g_audios[iMax].filePath = [[NSURL alloc] initFileURLWithPath:path];
    else
        g_audios[iMax].filePath = [[NSBundle mainBundle] URLForResource:[fileName lowercaseString] withExtension:[ext lowercaseString] subdirectory:[dir lowercaseString]];
    g_audios[iMax].maxWave=ndup;
    return 0;
}

void			CloseWave(unsigned int w)
{
    w--;
    if(w >= MAX_AUDIO_COUNT)
        return;
    if(g_audios[w].filePath)
    {
        for(int i=0;i<g_audios[w].maxWave;i++)
        {
            if(g_audios[w].wave[i])
                [g_audios[w].wave[i] release];
            g_audios[w].wave[i] = 0;
        }
        g_audios[w].lastTime=0;
        g_audios[w].filePath=NULL;
    }
}
void			SetupWave(unsigned int w, int dupCount)
{
    w--;
    if(w >= MAX_AUDIO_COUNT)
        return;
    if(g_audios[w].filePath)
        g_audios[w].maxWave = dupCount;
}
void			PlayWave(unsigned int w, float fVolume, float fPan, float fRate)
{
    w--;
    if(w >= MAX_AUDIO_COUNT)
        return;
    if(g_audios[w].filePath)
    {
        double dtime = GetAppTime();
        double diff = dtime - g_audios[w].lastTime;
        if(diff < 1)
        {
            g_audios[w].index++;
        }
        else{
            g_audios[w].index = 0;
        }
        g_audios[w].lastTime = dtime;
        g_audios[w].index = g_audios[w].index % g_audios[w].maxWave;
        if(g_audios[w].wave[g_audios[w].index]==NULL)
        {
            NSURL* url = g_audios[w].filePath;
            g_audios[w].wave[g_audios[w].index] = [[AVAudioPlayer alloc] initWithContentsOfURL:url  error: nil];
        }
        AVAudioPlayer* player = g_audios[w].wave[g_audios[w].index];
        player.numberOfLoops = 0;
        [player prepareToPlay];
        [player play];
        player.volume = 1;//fVolume;
        player.pan = fPan;
#ifdef IOS_5_1
        player.enableRate = (fRate < 0.99 || fRate > 1.01);
        player.rate = fRate;
#endif
    }
}

void			StopWave(unsigned int w)
{
    // StopMp3(w);
}



@interface MyThreadObj: NSObject
+ (void)threadMethod:(id)param;
@end
@implementation MyThreadObj

+ (void)threadMethod:(id)param
{
    struct ThreadParam* v = (struct ThreadParam*)param;
    (*v->proc)(v->param);
}
@end
//
////create the thread from objective c, and return the thread id, return -1 it not working.
//int           ThreadCreate(const void* param, ThreadProc proc)
//{
//    struct ThreadParam v;
//    v.proc  =   proc;
//    v.param =   param;
//    void* id = (void*)&v;
//    for(int i=0;i<MAX_THREAD_COUNT;i++)
//    {
//        if(g_threads[i] == 0)
//        {
//            [NSThread detachNewThreadSelector:@selector(threadMethod:) toTarget:[MyThreadObj class] withObject:id];
//            return i;
//        }
//    }
//    return -1;
//}
//void          ThreadDelete(int id)
//{
//}
//create thread lock
void*           CreateLock()
{
    NSLock* lock = [[NSLock alloc] init];
    return (void*)lock;
}
void          DeleteLock(void* id)
{
    if(!id)
        return;
    NSLock* lock = (NSLock*)id;
    [lock release];
}
void          SetLock(void* id)
{
    if(!id)
        return;
    NSLock* lock = (NSLock*)id;
    [lock lock];
    
}
void          SetUnlock(void* id)
{
    if(!id)
        return;
    NSLock* lock = (NSLock*)id;
    [lock unlock];
}
int IPhoneGetRealPath(char* retRealPath, const char* szDir, const char* szFileName, const char* szExt)
{
    if(!szExt || !szFileName)
        return false;
    szExt++;
    NSString* fileName = [[[NSString alloc]initWithUTF8String:szFileName] autorelease];
    NSString* ext = [[[NSString alloc]initWithUTF8String:szExt] autorelease];    
    NSString* dir = [[[NSString alloc]initWithUTF8String:szDir] autorelease];    
    NSString* binTxtPath = [[NSBundle mainBundle] pathForResource:[fileName lowercaseString] ofType:[ext lowercaseString] inDirectory:[dir lowercaseString]];

    if(binTxtPath)//[[NSFileManager defaultManager] fileExistsAtPath:appFile])//!appFile)
    {
        const char* szBinPath = [binTxtPath cStringUsingEncoding:NSUTF8StringEncoding];
        strcpy(retRealPath, szBinPath);
        return true;
    }
    return false;
}

int OSLoadImage(const char* szDir, const char* szFileName, const char* szExt, unsigned char** retPtr, unsigned int* retWidth, unsigned int* retHeight, unsigned int* retBpp, int nDiv)
{
    if(!szExt || !szFileName)
        return false;
    szExt++;
    NSString* fileName = [[[NSString alloc]initWithUTF8String:szFileName] autorelease];
    NSString* ext = [[[NSString alloc]initWithUTF8String:szExt] autorelease];    
    NSString* dir = [[[NSString alloc]initWithUTF8String:szDir] autorelease];    
    NSString* binTxtPath = [[NSBundle mainBundle] pathForResource:[fileName lowercaseString] ofType:[ext lowercaseString] inDirectory:[dir lowercaseString]];
    if(binTxtPath)//[[NSFileManager defaultManager] fileExistsAtPath:appFile])//!appFile)
    {
        NSData* fileContents = [NSData dataWithContentsOfFile:binTxtPath];
        UIImage* img = [[UIImage alloc] initWithData:fileContents];
        if(!img)
        {
            NSLog(@"File Png ReadFail %@", binTxtPath);
            return false;
        }
        unsigned int bpp = CGImageGetBitsPerPixel(img.CGImage);
        unsigned int width = CGImageGetWidth(img.CGImage);
        unsigned int height = CGImageGetHeight(img.CGImage);
        unsigned int origWidth = width;
        unsigned int origHeight = height;
        width = width / nDiv;
        height = height / nDiv;
        //
        if(bpp < 24)
        {
           [img release];
           NSLog(@"File Png ReadFail, pixelPerbytes must to be 24,or 32. %@", binTxtPath);
           return false;
        }
        NSLog(@"File Png ReadOK %@, size:%d, img:%u,%u,%u", binTxtPath, [fileContents length],
              width, height, bpp);
        int bytes = 4;//(bpp / 8);
        unsigned char* pixels = (unsigned char*)PhantomAlloc(bytes * width * height);
        CGColorSpaceRef colorSpace = CGImageGetColorSpace(img.CGImage);
        CGBitmapInfo info = kCGImageAlphaPremultipliedLast | kCGBitmapByteOrder32Big;
        if(bpp == 24)
            info = kCGImageAlphaNoneSkipLast;
        size_t compPer = CGImageGetBytesPerRow(img.CGImage);
        size_t comp = CGImageGetBitsPerComponent(img.CGImage);
        CGContextRef context = CGBitmapContextCreate( pixels, width, height, comp, bytes * width, colorSpace, info );
        CGContextClearRect(context, CGRectMake(0, 0, width, height));
        //CGContextTranslateCTM(context, 0, 0);
        CGContextDrawImage(context, CGRectMake(0, 0, width, width), img.CGImage);
        //
        CGContextRelease(context);
        [img release];
        //
        *retPtr = pixels;
        *retWidth = width;
        *retHeight = height;
        *retBpp = bytes * 8;
        return true;
    }
    return false;
}

void    LogPrint(const char* szText, ...)
{
    char buf[2048];
    buf[0] = 0;
    va_list args;
    va_start(args, szText);
    vsnprintf( buf, 2047, szText, args);
    NSString* text = [[[NSString alloc]initWithUTF8String:buf] autorelease];
    NSLog(@"%@", text);
}
extern     void    SetAppDocumentDir(const char* szDir);
BOOL CreatePhantom(int nDiv, int nOS, int* authcode)
{
    for(int i=0;i<MAX_THREAD_COUNT;i++)
        g_threads[i] = 0;
    SetRealPathGetting(IPhoneGetRealPath);
    SetCharLocale("chs", 0);
    memset(g_audios, 0, sizeof(g_audios));
	for(int i=0;i<MAX_AUDIO_COUNT;i++)
		g_audios[i].maxWave = 3;
    NSString* documentDir = [NSHomeDirectory() stringByAppendingPathComponent:@"Documents/"];
    const char* szBinPath = [documentDir cStringUsingEncoding:NSUTF8StringEncoding];
    SetAppDocumentDir(szBinPath);
    OnAppCreate(0, nDiv, nOS, authcode);
    NSLog(@"OnAppCreateOK");
    return true;
}
//
//unsigned int	OpenWave(const char* szDir, const char* szFileName, const char* szExt)
//{
//    if(szExt[0] == '.')
//        szExt++;
//    NSString* dir = [[[NSString alloc]initWithUTF8String:szDir] autorelease];
//    NSString* fileName = [[[NSString alloc]initWithUTF8String:szFileName] autorelease];
//    NSString* ext = [[[NSString alloc]initWithUTF8String:szExt] autorelease];
//    NSURL* filePath = [[NSBundle mainBundle] URLForResource:[fileName lowercaseString] withExtension:[ext lowercaseString] subdirectory:[dir lowercaseString]];
//    if (filePath == 0)
//        return 0;
//    SystemSoundID id = 0; 
//    AudioServicesCreateSystemSoundID((CFURLRef)filePath, &id);
//    return id;
//}
//
//void			CloseWave(unsigned int w)
//{
//    AudioServicesDisposeSystemSoundID(w);
//}
//
//void			PlayWave(unsigned int w, float fVolume, float fPan, float fRate)
//{
//    AudioServicesPlaySystemSound(w);
//}
//
//void			StopWave(unsigned int w)
//{
//}


#define MAX_AV_LOADER   10
AVAudioPlayer*  g_avPlayer[MAX_AV_LOADER];
void    DestroyAllMusic()
{
    for(int w=0;w<MAX_AUDIO_COUNT;w++){
    if(g_audios[w].filePath)
    {
        for(int i=0;i<g_audios[w].maxWave;i++)
        {
            if(g_audios[w].wave[i])
                [g_audios[w].wave[i] release];
            g_audios[w].wave[i] = 0;
        }
        g_audios[w].lastTime=0;
        g_audios[w].filePath=NULL;
    }
    }
    for(int i=0;i<MAX_AV_LOADER;i++)
    {
        if(g_avPlayer[i])
        {
            [g_avPlayer[i] release];
            g_avPlayer[i] = 0;
        }
    }
}
unsigned int	OpenMp3(const char* szDir, const char* szFileName, const char* szExt)
{
    if(szExt[0] == '.')
        szExt++;
    NSString* dir = [[[NSString alloc]initWithUTF8String:szDir] autorelease];
    NSString* fileName = [[[NSString alloc]initWithUTF8String:szFileName] autorelease];
    NSString* ext = [[[NSString alloc]initWithUTF8String:szExt] autorelease];
    NSURL* filePath = [[NSBundle mainBundle] URLForResource:[fileName lowercaseString] withExtension:[ext lowercaseString] subdirectory:[dir lowercaseString]];
    NSString* path = [[NSString alloc] initWithFormat:@"%@%@.%@", dir, fileName,ext];
    if (filePath == 0)
    {
        filePath = [[NSURL alloc] initFileURLWithPath:path];
        if (filePath == 0)
            return 0;
    }
    for(int i=0;i<MAX_AV_LOADER;i++)
    {
        if(g_avPlayer[i])
            continue;
        g_avPlayer[i] = [[AVAudioPlayer alloc] initWithContentsOfURL: filePath error: nil];
        return i + 1;
    }
    return 0;
}

void			CloseMp3(unsigned int w)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        [g_avPlayer[w] release];
        g_avPlayer[w] = 0;
    }
}

int             IsPlayingMp3(unsigned int w)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return 0;
    if(g_avPlayer[w])
    {
        return [g_avPlayer[w] isPlaying];
    }
    return 0;
}
void			PlayMp3(unsigned int w, float fVolume, float fPan, float fRate, int loopcount)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        g_avPlayer[w].numberOfLoops = loopcount;
        [g_avPlayer[w] prepareToPlay];
        [g_avPlayer[w] play];
        [g_avPlayer[w] setCurrentTime:0 ];
        g_avPlayer[w].volume = fVolume;
        g_avPlayer[w].pan = fPan;
#ifdef IOS_5_1
        g_avPlayer[w].enableRate = (fRate < 0.99 || fRate > 1.01);
        g_avPlayer[w].rate = fRate;
#endif
    }

}
void			SetVolumeMp3(unsigned int w, float fVolume)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        g_avPlayer[w].volume = fVolume;
    }
}
void			SetPanMp3(unsigned int w, float fPan)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        g_avPlayer[w].pan = fPan;
    }
}

void			StopMp3(unsigned int w)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        [g_avPlayer[w] stop]; 
    }
}

void			PauseMp3(unsigned int w)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        [g_avPlayer[w] pause]; 
    }

}

void			ResumeMp3(unsigned int w)
{
    w--;
    if(w >= MAX_AV_LOADER)
        return;
    if(g_avPlayer[w])
    {
        [g_avPlayer[w] play]; 
    }
}



@implementation PhantomEngineIOS

-(id)init
{
    if ((self = [super init]))
    {
    }
    return self;
}

+(PhantomEngineIOS*)sharedInstance
{
    static PhantomEngineIOS* ios = 0;
    if(!ios){
        ios = [[PhantomEngineIOS alloc]init];
    }
    return ios;
}
-(UIImage *)imageFromText:(NSString *)text fontName:(NSString *)fontName nFontHeight:(int)nFontHeight w:(int*)w h:(int*)h s:(int)s align:(int)align
{
    UIFont *font=nil;
    if(fontName==nil)
    {
        if(s==1)
            font = [UIFont boldSystemFontOfSize:nFontHeight];
        else if(s==2)
            font = [UIFont italicSystemFontOfSize:nFontHeight];
        else
            font = [UIFont systemFontOfSize:nFontHeight];
    }
    else
        font = [UIFont fontWithName:fontName size:nFontHeight];
    int len = [text length];
    int last = 0;
    NSMutableArray* strary = [[[NSMutableArray alloc]init]autorelease];
    CGSize slist[1024];
    int maxW = 0;
    int maxH = 0;
    int line = 0;
    for(int i=0;i<=len;i++){
        bool bend = false;
        if(i==len)
        {
            bend=true;
            i++;
        }
        else{
            unichar ch = [text characterAtIndex:i];
            if(ch=='\r')
                continue;
            if(ch=='\n')
                bend = true;
        }
        if(bend && line<1024){
            NSRange range;
            range.location = last;
            range.length = i - last-1;
            last = i+1;
            NSString* substr = [text substringWithRange:range];
            [strary addObject:substr];
            CGSize size2 = [substr sizeWithFont:font];
            slist[line++] = size2;
            maxH += size2.height;
            if(size2.width>maxW)
                maxW = size2.width;
        }
    }

    // set the font type and size
//    CGSize size = [text sizeWithFont:font];//[UIFont fontWithName:fontName size:nFontHeight]];//
    *w = maxW;
    *h = maxH;
    CGSize size = CGSizeMake(GetOriginalSize(maxW), GetOriginalSize(maxH));
    if (UIGraphicsBeginImageContextWithOptions != NULL)
        UIGraphicsBeginImageContextWithOptions(size,NO,0.0);
    else
        UIGraphicsBeginImageContext(size);
    CGContextRef ctx = UIGraphicsGetCurrentContext();
    CGContextSetShadowWithColor(ctx, CGSizeMake(1.0, 1.0), 0, [[UIColor grayColor] CGColor]);
    CGContextSetFillColorWithColor(ctx, [[UIColor whiteColor]CGColor]);
    int top = 0;
    for(int i=0;i<strary.count;i++)
    {
        CGSize temp = slist[i];
        NSString* str = [strary objectAtIndex:i];
        int left = 0;
        if((align&0x0001)>0)
            left = (maxW-temp.width)/2;
        else if((align&0x0002)>0)
            left = maxW - temp.width;
        [str drawInRect:CGRectMake(left, top, temp.width, temp.height) withFont:font];
        top += temp.height;
    }
    //[text drawInRect:CGRectMake(0, 0, size.width, size.height) withFont:font];
    UIImage *image = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    strary = nil;
    return image;
}



@end








