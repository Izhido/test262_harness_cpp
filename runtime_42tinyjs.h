#pragma once

#include <string>
#include "TinyJS.h"

namespace test262_harness_cpp
{
    class runtime
    {
    public:
        void create_realm();
        
        bool evaluate(std::string& file, std::string source, std::string& error_type, std::string& error_description);
        
        void destroy_realm();
        
        ~runtime();
        
    private:
        CTinyJS* context = nullptr;
    };
}

