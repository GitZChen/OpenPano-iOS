//
//  StitchingWrapper.hh
//  libopano
//
//  Created by GitZChen on 7/3/16.
//  Copyright © 2019 GitZChen. All rights reserved.
//

#ifndef StitchingWrapper_hh
#define StitchingWrapper_hh

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

@interface StitchingWrapper : NSObject
+ (UIImage *) stitchImagesOfPaths :(NSArray*)imagePaths;
@end

#endif /* StitchingWrapper_hh */
