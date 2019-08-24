//
//  StitchingWrapper.mm
//  libopano
//
//  Created by GitZChen on 7/3/16.
//  Copyright © 2019 GitZChen. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>
#import <CoreGraphics/CoreGraphics.h>
#import <iostream>
#import "StitchingWrapper.hh"
#import "main.hh"

using namespace std;

@implementation StitchingWrapper : NSObject

+ (UIImage *) stitchImagesOfPaths :(NSArray*)imagePaths{
    if (imagePaths.count==0) {
        return nil;
    } else if (![imagePaths[0] isKindOfClass:[NSString class]]) {
        cerr << "ERROR: NSString array required for image paths!\n";
        return nil;
    }
    const char* charArrays[imagePaths.count];
    for (int i=0; i<imagePaths.count; i++) {
        charArrays[i] = [imagePaths[i] UTF8String];
    }
    NSString* configFilePath = [[[NSBundle mainBundle] URLForResource:@"config" withExtension:@"cfg"] path];
    
    NSString* outputFilePath = [NSTemporaryDirectory() stringByAppendingString:@"pano.jpg"];
    Matuc panoMat = stitchPanoWithImagePathsAndConfig((int)imagePaths.count, charArrays, [configFilePath UTF8String], [outputFilePath UTF8String]);
    
    return [StitchingWrapper UIImageFromMat:panoMat];
    
    print_debug("finished stitching!");
    UIImage* panoImg = [UIImage imageWithContentsOfFile:outputFilePath];
    
    // TODO: Add proper tmp pano removal mechanisms.
    /*if(remove([outputFilePath UTF8String])>0){
        NSLog(@"ERROR: Cannot remove pano in tmp");
    }*/
    return panoImg;
}

+ (UIImage *) UIImageFromMat:(Matuc)mat {
    NSData *data = [NSData dataWithBytes:mat.ptr() length:mat.pixels()*mat.channels()*sizeof(unsigned char)];
    CGColorSpace* colorSpace = CGColorSpaceCreateDeviceRGB();
    CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef)data);
    
    CGImage* cgImg = CGImageCreate(mat.width(),                                 //width
                                   mat.height(),                                //height
                                   8,                                           //bits per component 8 or 32?
                                   8 * mat.channels(),                          //bits per pixel
                                   1 * mat.channels() * mat.width(),            //bytesPerRow
                                   colorSpace,                                  //colorspace
                                   kCGImageAlphaNone|kCGBitmapByteOrderDefault, //bitmap info
                                   provider,                                    //CGDataProviderRef
                                   NULL,                                        //decode
                                   false,                                       //should interpolate
                                   kCGRenderingIntentDefault                    //intent
                                   );
    UIImage *finalImage = [UIImage imageWithCGImage:cgImg];
    
    CGImageRelease(cgImg);
    CGDataProviderRelease(provider);
    CGColorSpaceRelease(colorSpace);
    
    return finalImage;
}


@end
