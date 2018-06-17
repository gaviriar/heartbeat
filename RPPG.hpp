//
//  RPPG.hpp
//  ArgParser
//
//  Created by Philipp Rouast on 7/07/2016.
//  Copyright © 2016 Philipp Roüast. All rights reserved.
//

#ifndef RPPG_hpp
#define RPPG_hpp

#include <stdio.h>
#include <fstream>
#include <string>

#include <opencv2/objdetect/objdetect.hpp>
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include "CsvSink.hpp"

/**
 * RPPG class performs heartrate calculation from input image
 */
enum rPPGAlgorithm { g, pca, xminay };

class RPPG {

public:

    // Constructor
    RPPG() {;}

    // Load Settings
    bool load(const rPPGAlgorithm algorithm,
              const int width, const int height, const double timeBase, const int downsample,
              const double samplingFrequency, const double rescanFrequency,
              const int minSignalSize, const int maxSignalSize,
              const std::string &logPath, const std::string &classifierPath,
              const bool log, const bool gui, std::unique_ptr<g3::LogWorker>& logWorker);

    /**
     * Process frame and estimate the heartrate
     *
     * @param frameRGB
     * @param frameGray
     * @param time
     */
    void processFrame(cv::Mat &frameRGB, cv::Mat &frameGray, int time);

    void exit();

    typedef std::vector<cv::Point2f> Contour2f;

private:

    void detectFace(cv::Mat &frameGray);
    void setNearestBox(std::vector<cv::Rect> boxes);
    void detectCorners(cv::Mat &frameGray);
    void trackFace(cv::Mat &frameGray);
    void updateMask(cv::Mat &frameGray);
    void updateROI();
    void extractSignal_g();
    void extractSignal_pca();
    void extractSignal_xminay();
    double estimateHeartrate();
    void draw(cv::Mat &frameRGB);
    void invalidateFace();
    void log();

    // The algorithm
    rPPGAlgorithm algorithm;

    // The classifiers
    cv::CascadeClassifier classifier;

    // Settings
    cv::Size minFaceSize;
    int maxSignalSize;
    int minSignalSize;
    double rescanFrequency;
    double samplingFrequency;
    double timeBase;
    bool logMode;
    bool guiMode;

    // State variables
    int64_t time;
    double fps;
    int high;
    int64_t lastSamplingTime;
    int64_t lastScanTime;
    int low;
    int64_t now;
    bool faceValid;
    bool rescanFlag;

    // Tracking
    cv::Mat lastFrameGray;
    Contour2f corners;

    // Mask
    cv::Rect box;
    cv::Mat1b mask;
    cv::Rect roi;

    // Raw signal
    cv::Mat1d s;
    cv::Mat1d t;
    cv::Mat1b re;

    // Estimation
    cv::Mat1d s_f;
    cv::Mat1d bpms;
    cv::Mat1d powerSpectrum;
    double bpm = 0.0;
    double meanBpm;
    double minBpm;
    double maxBpm;

    // Logfiles
    std::ofstream logfile;
    std::ofstream logfileDetailed;
    std::string logfilepath;
    std::shared_ptr<CsvSink> _signalLog;
    std::shared_ptr<CsvSink> _bpmSink;
    std::shared_ptr<CsvSink> _bpmAllSink;
};


#endif /* RPPG_hpp */
