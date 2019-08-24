// File: main.cc
// Date: Wed Jun 17 20:29:58 2015 +0800
// Author: Yuxin Wu <ppwwyyxxc@gmail.com>
// Modified: GitZChen

#define _USE_MATH_DEFINES
#include <cmath>

#include "main.hh"
#include "lib/config.hh"
#include "lib/imgproc.hh"
#include "lib/timer.hh"
#include "stitch/cylstitcher.hh"
#include "stitch/stitcher.hh"
#include <ctime>
#include <cassert>

using namespace std;
using namespace pano;
using namespace config;

bool TEMPDEBUG = false;

void init_config(const char* config_file) {
#define CFG(x) \
x = Config.get(#x)
    ConfigParser Config(config_file);
    CFG(CYLINDER);
    CFG(TRANS);
    CFG(ESTIMATE_CAMERA);
    if (int(CYLINDER) + int(TRANS) + int(ESTIMATE_CAMERA) >= 2)
        error_exit("You set two many modes...\n");
    if (CYLINDER)
        print_debug("Run with cylinder mode.\n");
    else if (TRANS)
        print_debug("Run with translation mode.\n");
    else if (ESTIMATE_CAMERA)
        print_debug("Run with camera estimation mode.\n");
    else
        print_debug("Run with naive mode.\n");
    
    CFG(ORDERED_INPUT);
    if (!ORDERED_INPUT && !ESTIMATE_CAMERA)
        error_exit("Require ORDERED_INPUT under this mode!\n");
    
    CFG(CROP);
    CFG(STRAIGHTEN);
    CFG(FOCAL_LENGTH);
    CFG(MAX_OUTPUT_SIZE);
    CFG(LAZY_READ);	// TODO in cyl mode
    
    CFG(SIFT_WORKING_SIZE);
    CFG(NUM_OCTAVE);
    CFG(NUM_SCALE);
    CFG(SCALE_FACTOR);
    CFG(GAUSS_SIGMA);
    CFG(GAUSS_WINDOW_FACTOR);
    CFG(JUDGE_EXTREMA_DIFF_THRES);
    CFG(CONTRAST_THRES);
    CFG(PRE_COLOR_THRES);
    CFG(EDGE_RATIO);
    CFG(CALC_OFFSET_DEPTH);
    CFG(OFFSET_THRES);
    CFG(ORI_RADIUS);
    CFG(ORI_HIST_SMOOTH_COUNT);
    CFG(DESC_HIST_SCALE_FACTOR);
    CFG(DESC_INT_FACTOR);
    CFG(MATCH_REJECT_NEXT_RATIO);
    CFG(RANSAC_ITERATIONS);
    CFG(RANSAC_INLIER_THRES);
    CFG(INLIER_IN_MATCH_RATIO);
    CFG(INLIER_IN_POINTS_RATIO);
    CFG(SLOPE_PLAIN);
    CFG(LM_LAMBDA);
    CFG(MULTIPASS_BA);
    CFG(MULTIBAND);
#undef CFG
}

void planet(const char* fname) {
    Mat32f test = read_img(fname);
    int w = test.width(), h = test.height();
    const int OUTSIZE = 1000, center = OUTSIZE / 2;
    Mat32f ret(OUTSIZE, OUTSIZE, 3);
    fill(ret, Color::NO);
    
    REP(i, OUTSIZE) REP(j, OUTSIZE) {
        real_t dist = hypot(center - i, center - j);
        if (dist >= center || dist == 0) continue;
        dist = dist / center;
        //dist = sqr(dist);	// TODO you can change this to see different effect
        dist = h - dist * h;
        
        real_t theta;
        if (j == center) {
            if (i < center)
                theta = M_PI / 2;
            else
                theta = 3 * M_PI / 2;
        } else {
            theta = atan((real_t)(center - i) / (center - j));
            if (theta < 0) theta += M_PI;
            if ((theta == 0) && (j > center)) theta += M_PI;
            if (center < i) theta += M_PI;
        }
        m_assert(0 <= theta);
        m_assert(2 * M_PI + EPS >= theta);
        
        theta = theta / (M_PI * 2) * w;
        
        update_min(dist, (real_t)h - 1);
        Color c = interpolate(test, dist, theta);
        float* p = ret.ptr(i, j);
        c.write_to(p);
    }
    write_rgb("planet.jpg", ret);
}

Matuc work(int argc, const char* argv[], const char* outputFilePath) {
    /*
     *  vector<Mat32f> imgs(argc - 1);
     *  {
     *    GuardedTimer tm("Read images");
     *#pragma omp parallel for schedule(dynamic)
     *    REPL(i, 1, argc)
     *      imgs[i-1] = read_img(argv[i]);
     *  }
     */
    vector<string> imgs;
    REPL(i, 0, argc) imgs.emplace_back(argv[i]);
    Mat32f res;
    if (CYLINDER) {
        CylinderStitcher p(move(imgs));
        res = p.build();
    } else {
        Stitcher p(move(imgs));
        res = p.build();
    }
    
    if (CROP) {
        int oldw = res.width(), oldh = res.height();
        res = crop(res);
        print_debug("Crop from %dx%d to %dx%d\n", oldh, oldw, res.height(), res.width());
    }
    return cvt_f2uc(res);
    //return res;
    {
        GuardedTimer tm("Writing image");
        write_rgb(outputFilePath, res);
    }
    print_debug("outputFilePath: %s\n", outputFilePath);
    //return res;
}

Matuc stitchPanoWithImagePathsAndConfig(int numImages, const char* imagePaths[], const char* configFilePath, const char* outputFilePath) {
    if (numImages < 2)
        error_exit("Need at least two images to stitch.\n");
    TotalTimerGlobalGuard _g;
    srand(time(NULL));
    init_config(configFilePath);
    return work(numImages, imagePaths, outputFilePath);
}

