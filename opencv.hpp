//
//  opencv.hpp
//  Hearbeat
//
//  Created by Philipp Rouast on 3/03/2016.
//  Copyright © 2016 Philipp Roüast. All rights reserved.
//

#ifndef opencv_hpp
#define opencv_hpp

#include <stdio.h>

#include <iostream>
#include <opencv2/core/core.hpp>

/***
 * Set of opencv utilities for image manipuation and processing
 */
namespace cvutils {

    const cv::Scalar BLACK    (  0,   0,   0);
    const cv::Scalar BLUE     (255,   0,   0);
    const cv::Scalar GREEN    (  0, 255,   0);
    const cv::Scalar RED      (  0,   0, 255);
    const cv::Scalar WHITE    (255, 255, 255);
    const cv::Scalar ZERO     (0);
    const cv::Scalar ONE      (1);

    /* COMMON FUNCTIONS */
    double getFps(cv::Mat &t, const double timeBase);
    void push(cv::Mat &m);
    void plot(cv::Mat &mat);
    
    /* FILTERS */
    void normalization(cv::InputArray _a, cv::OutputArray _b);
    void denoise(cv::InputArray _a, cv::InputArray _jumps, cv::OutputArray _b);
    void detrend(cv::InputArray _a, cv::OutputArray _b, int lambda);
    void movingAverage(cv::InputArray _a, cv::OutputArray _b, int n, int s);
    void bandpass(cv::InputArray _a, cv::OutputArray _b, double low, double high);
    void butterworth_bandpass_filter(cv::Mat &filter, double cutin, double cutoff, int n);
    void butterworth_lowpass_filter(cv::Mat &filter, double cutoff, int n);
    void frequencyToTime(cv::InputArray _a, cv::OutputArray _b);
    void timeToFrequency(cv::InputArray _a, cv::OutputArray _b, const bool magnitude);
    void pcaComponent(cv::InputArray _a, cv::OutputArray _b, cv::OutputArray _pc, int low, int high);

    /* LOGGING */
    void printMatInfo(const std::string &name, cv::InputArray _a);

    template<typename T>
    void printMat(const std::string &name, cv::InputArray _a,
                  int rows = -1,
                  int cols = -1,
                  int channels = -1)
    {
        printMatInfo(name, _a);

        cv::Mat a = _a.getMat();
        if (-1 == rows) rows = a.rows;
        if (-1 == cols) cols = a.cols;
        if (-1 == channels) channels = a.channels();

        for (int y = 0; y < rows; y++) {
            std::cout << "[";
            for (int x = 0; x < cols; x++) {
                T* e = &a.at<T>(y, x);
                std::cout << "(" << e[0];
                for (int c = 1; c < channels; c++) {
                    std::cout << ", " << e[c];
                }
                std::cout << ")";
            }
            std::cout << "]" << std::endl;
        }
        std::cout << std::endl;
    }

}

#endif /* opencv_hpp */
