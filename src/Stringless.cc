/*
 * Copyright Justin Tennant <justin.tennant@sjsu.edu> 2017.
 * Distributed under the Boost Software License, Version 1.0.
 *    (See accompanying file LICENSE or copy at
 *          http://www.boost.org/LICENSE_1_0.txt)
 */

/* 
 * File:   Stringless.cc
 * Author: Justin Tennant <justin.tennant@sjsu.edu>
 *
 * Created on February 2, 2017, 11:28 AM
 */

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

#include "dlib/gui_widgets.h"
#include "../ext/optionparser.h"

#include "FrameData.h"
#include "MemoryManager.h"
#include "Writer.h"
#include "FaceDetector.h"

struct Arg: public option::Arg
{
  static void printError(const char* msg1, const option::Option& opt, const char* msg2)
  {
    fprintf(stderr, "%s", msg1);
    fwrite(opt.name, opt.namelen, 1, stderr);
    fprintf(stderr, "%s", msg2);
  }

  static option::ArgStatus Unknown(const option::Option& option, bool msg)
  {
    if (msg) printError("Unknown option '", option, "'\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Flp(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg != "" && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires an argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus NonEmpty(const option::Option& option, bool msg)
  {
    if (option.arg != 0 && option.arg[0] != 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a non-empty argument\n");
    return option::ARG_ILLEGAL;
  }

  static option::ArgStatus Numeric(const option::Option& option, bool msg)
  {
    char* endptr = 0;
    if (option.arg != 0 && strtod(option.arg, &endptr) == 0){};
    if (endptr != option.arg && *endptr == 0)
      return option::ARG_OK;

    if (msg) printError("Option '", option, "' requires a numeric argument\n");
    return option::ARG_ILLEGAL;
  }
};
    
enum optionIndex { UNKNOWN, 
                   HELP, 
                   CLEAR, 
                   CAMERA_NUM, 
                   FACE_LANDMARKS_PATH,
                   DOWNSAMPLE_RATIO,
                   SAMPLE_RATE,
                   LANDMARK_SAMPLE_PER_FRAME };

const option::Descriptor usage[] = {
    {UNKNOWN, 0, "", "", Arg::Unknown, 
            "Usage: stringless [options]\n\n"
            "Where options include:"},
    {HELP, 0, "h", "help", Arg::None, 
            "--help, -h   \t"
            "Print usage and exit."},
    {CLEAR, 0, "c", "clear", Arg::None, 
            "--clear, -c   \t"
            "Clears stringless memory allocation"},
    {CAMERA_NUM, 0, "", "cn", Arg::Numeric, 
            "--cn=<arg>  \t"
            "Specify a positive integer camera number to use for capture (0 "
            "default)."},
    {FACE_LANDMARKS_PATH, 0, "f", "flp", Arg::Flp, 
            "--flp=<arg>, -f <arg> \t"
            "Path to dlib face landmarks. (required)"},
    {DOWNSAMPLE_RATIO, 0, "d", "dsr", Arg::Numeric,
            "--dsr=<arg>, -d <arg> \t"
            "Specify a positive floating point downsample ratio (1/<arg>) >= 1 for "
            "captured frames; default is 1 (no downsampling), higher numbers = "
            "higher downsampling."},
    {SAMPLE_RATE, 0, "s", "sr", Arg::Numeric,
            "--sr=<arg>, -s <arg>  \t"
            "Specify a positive integer sampling rate >= 1 for face detection on "
            "captured frames; default is 1 (every frame is sampled), higher "
            "numbers means more frames skipped between each sampling."},
    {LANDMARK_SAMPLE_PER_FRAME, 0, "", "lspf", Arg::Numeric,
            "--lspf=<arg>  \t"
            "Specify a positive integer >=1 for how many times stringless "
            "should sample for landmarks and create an average from per frame; "
            "intended for smoother output (1 default)."},
    {UNKNOWN, 0, "", "", Arg::None, 
            "\nExamples:\n"
            "  stringless -f path/to/flp \n"
            "  stringless -f path/to/flp -d 2 -s 3 \n"    
            "  stringless --flp=path/to/flp --cn=1 \n"
            "  stringless --flp=path/to/flp --lspf=2 \n"
            "  stringless --clear \n"
            "  stringless -c \n"
            },
    {0, 0, 0, 0, 0, 0} };
    
void stringless_clear(Stringless::MemoryManager* memory_manager) {
    memory_manager->remove();
}

int main(int argc, char** argv) { 
    const std::string shared_memory_name = "/stringless";
    // Set shared memory size to two frames
    const size_t shared_memory_size = sizeof(struct Stringless::FrameData) * 2;
    // Select default camera 0 for frame capturing
    int camera_number = 0;
    // Set downsample ratio default to 1
    double downsample_ratio = 1;
    // Set sample rate default to 1
    int sample_rate = 1;
    // Set landmark sample per frame default to 1
    int landmark_sample_per_frame = 1;

    
    argc-=(argc>0); argv+=(argc>0); // skip program name argv[0] if present
    option::Stats stats(usage, argc, argv);
    
    #ifdef __GNUC__
    // GCC supports C99 VLAs for C++ with proper constructor calls.
    option::Option options[stats.options_max], buffer[stats.buffer_max];
    #else
      // use calloc() to allocate 0-initialized memory. It's not the same
      // as properly constructed elements, but good enough.
    option::Option* options = (option::Option*)calloc(stats.options_max, sizeof(option::Option));
    option::Option* buffer  = (option::Option*)calloc(stats.buffer_max,  sizeof(option::Option));
    #endif

    option::Parser parse(usage, argc, argv, options, buffer);
    
    if (parse.error())
        return 1;
    
    if (options[HELP] || argc == 0) {
        option::printUsage(std::cout, usage);
        return 0;
    }
    
    Stringless::MemoryManager memory_manager(shared_memory_name, 
                                         shared_memory_size,
                                         Stringless::MemoryManager::write);
    
    char* face_landmarks_path;
    
    bool face_landmarks_option = false;
    for (int i = 0; i < parse.optionsCount(); i++) {
        option::Option& opt = buffer[i];
        switch(opt.index()) {
            case HELP:
                // Taken care of above
            case CLEAR:
                stringless_clear(&memory_manager);
                return 0;
            case CAMERA_NUM:
                if (std::stoi(opt.arg) < 0) {
                    std::cout << "Invalid camera number, exiting..." 
                            << std::endl;
                    return 1;
                }
                camera_number = std::stoi(opt.arg);
                break;
            case FACE_LANDMARKS_PATH:
                face_landmarks_option = true;
                face_landmarks_path = const_cast<char*>(opt.arg);
                break;
            case DOWNSAMPLE_RATIO:
                if (std::stoi(opt.arg) <= 0) {
                    std::cout << "Invalid downsample ratio; input must be 1 or "
                            "greater. Exiting..." << std::endl;
                    return 1;
                }
                downsample_ratio = std::stod(opt.arg);
                break;
            case SAMPLE_RATE:
                if (std::stoi(opt.arg) <= 0) {
                    std::cout << "Invalid sample rate; input must be 1 or "
                            "greater. Exiting..." << std::endl;
                    return 1;
                }
                sample_rate = std::stoi(opt.arg);
                break;
            case LANDMARK_SAMPLE_PER_FRAME:
                if (std::stoi(opt.arg) <= 0) {
                    std::cout << "Invalid sample per frame; input must be 1 or "
                            "greater. Exiting..." << std::endl;
                    return 1;
                }
                landmark_sample_per_frame = std::stoi(opt.arg);
            case UNKNOWN:
                // Not possible, but I'll acknowledge the option anyways
                break;
        }
    }
    if (!face_landmarks_option) {
        std::cout << "-f/--flp option is required and cannot be provided empty "
                "argument, exiting..." << std::endl;
        return 1;
    }
    
    for (int i = 0; i < parse.nonOptionsCount(); ++i)
      fprintf(stdout, "Non-option argument #%d is %s\n", i, parse.nonOption(i));
    
    if (memory_manager.init()) { 
        // If shared memory allocation fails, MemoryManager already prints 
        // error, we just exit properly.
        return errno;
    }
    
    Stringless::Writer writer(memory_manager.address());
    Stringless::FaceDetector face_detector(camera_number, 
                                           downsample_ratio,
                                           sample_rate,
                                           landmark_sample_per_frame,
                                           face_landmarks_path);

    face_detector.start(writer);
    return 0;
}

