// Author: Jack Bonnell
// Company: Sundance Multiprocessor Technology Ltd
// Email: jack.b@sundance.com
//Website: www.sundance.com
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>
#include <memory>
#include <chrono>
#include <thread>
#include <sys/stat.h>
using namespace std;
int main(int argc, char * argv[]) try
{
    
    std::cout << "Realsense Vitis AI Demo" << std::endl;
    std::cout << "By Jack Bonnell" << std::endl; 
    std::cout << "Email: Jack.B@sundance.com" << std::endl;
    string folderpath = "./bags";
    int status = mkdir("./bags", 0777);
    if (!status){
        std::cout << "created ./bag directory..." << std::endl;
    }else{
        std::cout << "./bag directory already exists..." << std::endl;
    }
    std::time_t t = std::time(0);
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_INFRARED,  1, 424, 240,RS2_FORMAT_Y8);
    cfg.enable_stream(RS2_STREAM_DEPTH, 424, 240,RS2_FORMAT_Z16);
    cfg.enable_stream(RS2_STREAM_COLOR, 320, 240,RS2_FORMAT_Y16);
    cfg.enable_record_to_file("./bags/" + std::to_string(t) + ".bag");
    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start(cfg);
    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    rs2::align align_to_color(RS2_STREAM_COLOR);
    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2::device dev = profile.get_device();
    rs2::depth_sensor ds = dev.query_sensors().front().as<rs2::depth_sensor>();
    ds.set_option(RS2_OPTION_EMITTER_ENABLED, 0.f); // Disable emitter
    clock_t clockstarted = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    int timeintervals = 1;
    
    while (true)
    {
        
        clock_t clocknow =std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() - clockstarted;
        
        clock_t beginFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
	    data = align_to_color.process(data);
	    rs2::video_frame rgb = data.get_color_frame();
        uint8_t* rgb_data = (uint8_t*)rgb.get_data();
        rs2::depth_frame depth = data.get_depth_frame();
        uint16_t* depth_data = (uint16_t*)depth.get_data();
        clock_t endFrame = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double fps = 1000/(endFrame-beginFrame);
        if (fps < 60){
        std::system("clear");
        std::cout<<"FrameTime was:"<<fps<<std::endl;
        std::cout<<"Bag running for:"<<clocknow<< " seconds" <<std::endl;
        }

        if (clocknow >= (10)){
            pipe.stop();
            std::cout << "Pipeline Stopped..." << std::endl;
            cfg.enable_record_to_file("./bags/" + std::to_string(t) + ":" + std::to_string(timeintervals) +".bag");
            std::cout << "Pipeline Starting..." << std::endl;
            std::this_thread::sleep_for(chrono::seconds(5));
            pipe.start(cfg);
            clockstarted = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            timeintervals = timeintervals + 1;
        }
    }
    return EXIT_SUCCESS;
}
catch (const rs2::error & e)
{
    std::cerr << "RealSense error calling " << e.get_failed_function() << "(" << e.get_failed_args() << "):\n    " << e.what() << std::endl;
    return EXIT_FAILURE;
}
catch (const std::exception& e)
{
    std::cerr << e.what() << std::endl;
    return EXIT_FAILURE;
}
