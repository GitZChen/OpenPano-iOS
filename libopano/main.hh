//
//  main.hh
//  libopano
//
//  Created by GitZChen on 7/3/16.
//  Copyright © 2019 GitZChen. All rights reserved.
//

#ifndef main_hh
#define main_hh

#import "lib/mat.h"

void init_config(const char* config_file);
void planet(const char* fname);
Matuc work(int argc, const char* argv[]);
Matuc stitchPanoWithImagePathsAndConfig(int numImages, const char* imagePaths[], const char* configPath, const char* outputFilePath);

#endif /* main_hh */
