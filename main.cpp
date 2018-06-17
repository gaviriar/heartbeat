//
// Created by unitelabs on 15/06/18.
//
#include <opencv2/videoio/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#define G3_DYNAMIC_LOGGING
#include <g3log/g3log.hpp>
#include <g3log/logworker.hpp>
#include <memory>

#include "Baseline.hpp"
#include "ArgParser.hpp"
#include "CustomSink.hpp"

#define DEFAULT_ALGORITHM "g"
#define DEFAULT_RESCAN_FREQUENCY 1
#define DEFAULT_SAMPLING_FREQUENCY 1
#define DEFAULT_MIN_SIGNAL_SIZE 5
#define DEFAULT_MAX_SIGNAL_SIZE 5
#define DEFAULT_DOWNSAMPLE 1 // x means only every xth frame is used

const LEVELS NOLOG {g3::kFatalValue + 1, "NoLogsLevel"};

bool to_bool(string s) {
    bool result;
    transform(s.begin(), s.end(), s.begin(), ::tolower);
    istringstream is(s);
    is >> boolalpha >> result;
    return result;
}

rPPGAlgorithm to_algorithm(string s) {
    rPPGAlgorithm result;
    if (s == "g") result = g;
    else if (s == "pca") result = pca;
    else if (s == "xminay") result = xminay;
    else {
        LOG(DEBUG) << "Please specify valid algorithm (g, pca, xminay)!" << std::endl;
        exit(-1);
    }
    return result;
}

using namespace g3;
using namespace cv;

int main(int argc, char * argv[]) {
    ArgParser arg_parser(argc, argv);
    g3::only_change_at_initialization::addLogLevel(NOLOG);
    LOG(WARNING) << "This log call, may or may not happend before"
                 << "the sinkHandle->call below";


    string input = arg_parser.get_arg("-i"); // Filepath for offline mode

    // algorithm setting
    rPPGAlgorithm algorithm;
    string algorithmString = arg_parser.get_arg("-a");
    if (algorithmString != "") {
        algorithm = to_algorithm(algorithmString);
    } else {
        algorithm = to_algorithm(DEFAULT_ALGORITHM);
    }

    LOG(DEBUG) << "Using algorithm " << algorithm << "." << endl;

    // rescanFrequency setting
    double rescanFrequency;
    string rescanFrequencyString = arg_parser.get_arg("-r");
    if (rescanFrequencyString != "") {
        rescanFrequency = atof(rescanFrequencyString.c_str());
    } else {
        rescanFrequency = DEFAULT_RESCAN_FREQUENCY;
    }

    // samplingFrequency setting
    double samplingFrequency;
    string samplingFrequencyString = arg_parser.get_arg("-f").c_str();
    if (samplingFrequencyString != "") {
        samplingFrequency = atof(samplingFrequencyString.c_str());
    } else {
        samplingFrequency = DEFAULT_SAMPLING_FREQUENCY;
    }

    // max signal size setting
    int maxSignalSize;
    string maxSignalSizeString = arg_parser.get_arg("-max");
    if (maxSignalSizeString != "") {
        maxSignalSize = atof(maxSignalSizeString.c_str());
    } else {
        maxSignalSize = DEFAULT_MAX_SIGNAL_SIZE;
    }

    // min signal size setting
    int minSignalSize;
    string minSignalSizeString = arg_parser.get_arg("-min");
    if (minSignalSizeString != "") {
        minSignalSize = atof(minSignalSizeString.c_str());
    } else {
        minSignalSize = DEFAULT_MIN_SIGNAL_SIZE;
    }

    // visualize baseline setting
    string baseline_input = arg_parser.get_arg("-baseline");

    if (minSignalSize > maxSignalSize) {
        LOG(FATAL) << "Max signal size must be greater or equal min signal size!" << std::endl;
        exit(0);
    }

    // Reading gui setting
    bool gui;
    string guiString = arg_parser.get_arg("-gui");
    if (guiString != "") {
        gui = to_bool(guiString);
    } else {
        gui = true;
    }

    // Reading log setting
    bool log;
    string logString = arg_parser.get_arg("-log");
    if (logString != "") {
        log = to_bool(logString);
    } else {
        log = false;
    }

    // Reading downsample setting
    int downsample;
    string downsampleString = arg_parser.get_arg("-ds");
    if (downsampleString != "") {
        downsample = atof(downsampleString.c_str());
    } else {
        downsample = DEFAULT_DOWNSAMPLE;
    }

    LEVELS level = NOLOG;
    string level_str = arg_parser.get_arg("-v");
    log_levels::disableAll();
    if (level_str != "") {
        cout << "level input: " << stoi(level_str) << endl;
        // should use masks to make this more sophisticated
        switch (stoi(level_str)){
            case g3::kDebugValue: level = DEBUG; break;
            case g3::kInfoValue: level = INFO; break;
            case g3::kWarningValue: level = WARNING; break;
            case g3::kFatalValue: level = FATAL; break;
            case g3::kFatalValue+1: level = NOLOG; break;
        }
        log_levels::setHighest(level);
    }
    std::cout << "setting LOG level: " << level.text << std::endl;
    std::unique_ptr<LogWorker> logworker{ LogWorker::createLogWorker() };
    auto sinkHandle = logworker->addSink(
            std::unique_ptr<CustomSink>(new CustomSink()),
            &CustomSink::ReceiveLogMessage);
    // initialize the logger before it can receive LOG calls
    initializeLogging(logworker.get());


    // End of argument parsing

    const string CLASSIFIER_PATH = "haarcascade_frontalface_alt.xml";

    std::ifstream test(CLASSIFIER_PATH);
    if (!test) {
        LOG(FATAL) << "Face classifier xml not found!" << std::endl;
        exit(0);
    }

    bool offlineMode = input != "";

    VideoCapture cap;
    if (offlineMode) cap.open(input);
    else cap.open(0);
    if (!cap.isOpened()) {
        return -1;
    }

    std::string title = offlineMode ? "rPPG offline" : "rPPG online";
    LOG(INFO) << title << endl;
    LOG(INFO) << "Processing " << (offlineMode ? input : "live feed") << endl;

    // Configure logfile path
    string LOG_PATH;
    if (offlineMode) {
        LOG_PATH = input.substr(0, input.find_last_of("."));
    } else {
        std::ostringstream filepath;
        filepath << "heartbeat";
        LOG_PATH = filepath.str();
    }

    // Load video information
    const int WIDTH = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    const int HEIGHT = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    const double FPS = cap.get(cv::CAP_PROP_FPS);
    const double TIME_BASE = 0.001;

    // Print video information
    LOG(INFO) << "SIZE: " << WIDTH << "x" << HEIGHT << endl;
    LOG(INFO) << "FPS: " << FPS << endl;
    LOG(INFO) << "TIME BASE: " << TIME_BASE << endl;

    std::ostringstream window_title;
    window_title << title << " - " << WIDTH << "x" << HEIGHT << " -a " << algorithm << " -r " << rescanFrequency << " -f " << samplingFrequency << " -min " << minSignalSize << " -max " << maxSignalSize << " -ds " << downsample;

    // Set up rPPG
    RPPG rppg = RPPG();
    rppg.load(algorithm,
              WIDTH, HEIGHT, TIME_BASE, downsample,
              samplingFrequency, rescanFrequency,
              minSignalSize, maxSignalSize,
              LOG_PATH, CLASSIFIER_PATH,
              log, gui, logworker);

    // Load baseline if necessary
    Baseline baseline = Baseline();
    if (baseline_input != "") {
        baseline.load(1, 0.000001, baseline_input);
    }

    LOG(INFO) << "START ALGORITHM" << endl;

    int i = 0;
    Mat frameRGB, frameGray;

    while (true) {

        // Grab RGB frame
        cap.read(frameRGB);

        if (frameRGB.empty())
            break;

        // Generate grayframe
        cvtColor(frameRGB, frameGray, CV_BGR2GRAY);
        equalizeHist(frameGray, frameGray);

        int time;
        if (offlineMode) time = (int)cap.get(CV_CAP_PROP_POS_MSEC);
        else time = (cv::getTickCount()*1000.0)/cv::getTickFrequency();

        if (i % downsample == 0) {
            rppg.processFrame(frameRGB, frameGray, time);
        } else {
            LOG(INFO) << "SKIPPING FRAME TO DOWNSAMPLE!" << endl;
        }

        if (baseline_input != "") {
            baseline.processFrame(frameRGB, time);
        }

        if (gui) {
            imshow(window_title.str(), frameRGB);
            if (waitKey(30) >= 0) break;
        }

        i++;
    }

    return 0;
}

