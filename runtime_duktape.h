#pragma once

#include <string>
#include "duktape.h"

void test262_harness_cpp_handle_fatal_error(void* udata, const char* msg);

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
        duk_context* context = nullptr;
        
        std::string evaluation_description;
        
        void add_fatal_error(const char* message);

        friend void ::test262_harness_cpp_handle_fatal_error(void* udata, const char* msg);
    };
}

