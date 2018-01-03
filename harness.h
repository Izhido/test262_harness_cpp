#pragma once

#include <string>
#include "runtime.h"
#include "metadata.h"

namespace test262_harness_cpp
{
    class harness
    {
    public:
        void parse(int argc, const char* argv[]);
        
        void run();

        std::string tests_ran_read_message();
        
    private:
        int tests_read = 0;
        
        int tests_ran = 0;
        
        int tests_failed = 0;
        
        std::string tests_directory;
        
        std::string results_file;
        
		bool display_progress = false;

        std::string harness_directory;
        
        std::string assert_file;

        std::string assert_contents;
        
        std::string sta_file;
        
        std::string sta_contents;
        
        void read_file(std::string file, std::string& contents);
        
        bool evaluate_strict_mode(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::fstream& results_stream);

        bool evaluate_non_strict_mode(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::fstream& results_stream);

        bool evaluate_with_prefix(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::string prefix, std::string mode, std::fstream& results_stream);
    };
}
