#pragma once

#include <string>
#include <vector>

namespace test262_harness_cpp
{
    struct metadata
    {
        bool is_negative = false;
        std::string negative_phase;
        std::string negative_type;
        
        std::vector<std::string> includes;
        
        bool only_strict = false;
        bool no_strict = false;
        bool module = false;
        bool raw = false;
        bool async = false;

        void get_metadata(std::string& contents);
    };
}


