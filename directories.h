#pragma once

#include <string>
#include <vector>

namespace test262_harness_cpp
{
    struct directories
    {
        static bool is_directory(std::string& path);
        
        static std::string get_directory(std::string& path);

        static std::string path_separator();
        
        static std::string combine(std::string path, std::string file);

        static void get_test_files(std::string& directory, std::vector<std::string>& files);
    };
}

