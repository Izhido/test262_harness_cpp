#include "runtime.h"
#include <iostream>

void test262_harness_cpp_handle_fatal_error(void* udata, const char* msg)
{
    ((test262_harness_cpp::runtime*)udata)->add_fatal_error(msg);
}

int test262_harness_cpp_print(duk_context* context)
{
    int argument_count = duk_get_top(context);
    if (argument_count > 0)
    {
        const char* argument = duk_to_string(context, 0);
        if (argument != nullptr)
        {
            std::cout << argument << std::endl;
        }
    }
    return 0;
}

namespace test262_harness_cpp
{
    void runtime::create_realm()
    {
        if (context != nullptr)
        {
            return;
        }

        context = duk_create_heap(nullptr, nullptr, nullptr, this, test262_harness_cpp_handle_fatal_error);
        
        duk_push_global_object(context);
        duk_push_c_function(context, test262_harness_cpp_print, DUK_VARARGS);
        duk_put_prop_string(context, -2, "print");
        duk_pop(context);
    }
    
    bool runtime::evaluate(std::string& file, std::string source, std::string& error_type, std::string& error_description)
    {
        if (context == nullptr)
        {
            return true;
        }

        auto pos_1 = file.find("built-ins");
        if (pos_1 != std::string::npos)
        {
			auto pos_2 = file.find("Array", pos_1);
			if (pos_2 != std::string::npos)
			{
				auto pos_3 = file.find("prototype", pos_2);
				if (pos_3 != std::string::npos)
				{
					auto pos_4 = file.find("unshift", pos_3);
					if (pos_4 != std::string::npos)
					{
						auto pos_5 = file.find("length-near-integer-limit.js", pos_4);
						if (pos_5 != std::string::npos)
						{
							error_description = "This test does not return in a timely manner in MacOS.";

							return false;
						}
						pos_5 = file.find("clamps-to-integer-limit.js", pos_4);
						if (pos_5 != std::string::npos)
						{
							error_description = "This test does not return in a timely manner in MacOS.";

							return false;
						}
					}
				}
			}
			pos_2 = file.find("RegExp", pos_1);
			if (pos_2 != std::string::npos)
			{
				auto pos_3 = file.find("S15.10.2.5_A1_T5.js", pos_2);
				if (pos_3 != std::string::npos)
				{
					error_description = "This test causes an stack overflow in Windows.";

					return false;
				}
				pos_3 = file.find("S15.10.2.9_A1_T5.js", pos_2);
				if (pos_3 != std::string::npos)
				{
					error_description = "This test causes an stack overflow in Windows.";

					return false;
				}
				pos_3 = file.find("S15.10.2.8_A3_T15.js", pos_2);
				if (pos_3 != std::string::npos)
				{
					error_description = "This test causes an stack overflow in Windows.";

					return false;
				}
			}
		}
        
        evaluation_description.clear();
        
        try
        {
            auto result = duk_peval_string(context, source.c_str());

            if (evaluation_description != "")
            {
                error_description = evaluation_description;
                
                return false;
            }
            else if (result != 0)
            {
                auto top = duk_get_top_index(context);
                
                auto error_code = duk_get_error_code(context, top);
                
                switch (error_code)
                {
                    case DUK_ERR_ERROR:
                        error_type = "Error";
                        break;
                        
                    case DUK_ERR_EVAL_ERROR:
                        error_type = "EvalError";
                        break;
                        
                    case DUK_ERR_RANGE_ERROR:
                        error_type = "RangeError";
                        break;
                        
                    case DUK_ERR_REFERENCE_ERROR:
                        error_type = "ReferenceError";
                        break;
                        
                    case DUK_ERR_SYNTAX_ERROR:
                        error_type = "SyntaxError";
                        break;
                        
                    case DUK_ERR_TYPE_ERROR:
                        error_type = "TypeError";
                        break;
                        
                    case DUK_ERR_URI_ERROR:
                        error_type = "URIError";
                }
                
                error_description = duk_safe_to_string(context, top);
                
                auto position = error_description.find_first_of(":");
                
                if (position != std::string::npos)
                {
                    auto parsed_error_type = error_description.substr(0, position);
                    
                    if (error_type == "")
                    {
                        error_type = parsed_error_type;
                    }
                    
                    if (error_type == parsed_error_type)
                    {
                        error_description = error_description.substr(position + 1);
                    }
                }
                
                return false;
            }
        }
        catch (std::exception& exception)
        {
            error_description = exception.what();
            
            return false;
        }
        catch (...)
        {
            error_description = "A reason for the error was not returned.";
            
            return false;
        }
        
        return true;
    }
    
    void runtime::destroy_realm()
    {
        if (context == nullptr)
        {
            return;
        }
        
        duk_destroy_heap(context);
        
        context = nullptr;
    }

    void runtime::add_fatal_error(const char* message)
    {
        evaluation_description = message;
    }

    runtime::~runtime()
    {
        destroy_realm();
    }
}
