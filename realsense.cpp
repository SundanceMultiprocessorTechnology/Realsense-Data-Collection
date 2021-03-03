// Author: Jack Bonnell
// Company: Sundance Multiprocessor Technology Ltd
// Email: jack.b@sundance.com
//Website: www.sundance.com
#include <librealsense2/rs.hpp> // Include RealSense Cross Platform API
#include <iostream>
#include <glog/logging.h>
#include <memory>
#include <chrono>

using namespace std;
int main(int argc, char * argv[]) try
{
    std::cout << "Realsense Vitis AI Demo" << std::endl;
    std::cout << "By Jack Bonnell" << std::endl; 
    std::cout << "Email: Jack.B@sundance.com" << std::endl;
    std::time_t t = std::time(0);
    // Declare depth colorizer for pretty visualization of depth data
    rs2::colorizer color_map;

    // Declare RealSense pipeline, encapsulating the actual device and sensors
    rs2::pipeline pipe;
    rs2::config cfg;
    cfg.enable_stream(RS2_STREAM_DEPTH,640,480,RS2_FORMAT_Z16);
    cfg.enable_stream(RS2_STREAM_COLOR,1280,720,RS2_FORMAT_BGR8);
    cfg.enable_record_to_file("./test.bag");
    // Start streaming with default recommended configuration
    rs2::pipeline_profile profile = pipe.start(cfg);
    // Each depth camera might have different units for depth pixels, so we get it here
    // Using the pipeline's profile, we can retrieve the device that the pipeline uses
    rs2::align align_to_color(RS2_STREAM_COLOR);
    //Pipeline could choose a device that does not have a color stream
    //If there is no color stream, choose to align depth to another stream
    rs2::device dev = profile.get_device();
    rs2::depth_sensor ds = dev.query_sensors().front().as<rs2::depth_sensor>();
    while (true)
    {
        clock_t beginFrame = clock();
        rs2::frameset data = pipe.wait_for_frames(); // Wait for next set of frames from the camera
	    data = align_to_color.process(data);
	    rs2::video_frame rgb = data.get_color_frame();
        uint8_t* rgb_data = (uint8_t*)rgb.get_data();
        rs2::depth_frame depth = data.get_depth_frame();
        uint16_t* depth_data = (uint16_t*)depth.get_data();
        clock_t endFrame = clock();
        double fps = endFrame-beginFrame;
        std::cout<<"FrameTime was:"<<fps<<std::endl
    }
    pipe.stop();
    std::cout << "Pipeline Stopped..." << std::endl;
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
