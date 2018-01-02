#include "runtime.h"
#include "TinyJS_Functions.h"
#include "TinyJS_MathFunctions.h"

namespace test262_harness_cpp
{
    void runtime::create_realm()
    {
        if (context != nullptr)
        {
            return;
        }

        context = new CTinyJS();
        
        registerFunctions(context);
        registerMathFunctions(context);
    }
    
    bool runtime::evaluate(std::string& file, std::string source, std::string& error_type, std::string& error_description)
    {
        if (context == nullptr)
        {
            return true;
        }

        try
        {
            context->execute(source);
        }
        catch (CScriptException* exception)
        {
            error_description = exception->text;
            
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
        delete context;
        
        context = nullptr;
    }

    runtime::~runtime()
    {
        destroy_realm();
    }
}
