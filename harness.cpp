#include "harness.h"
#include <stdexcept>
#include "directories.h"
#include <fstream>
#include <sstream>
#include <iostream>

namespace test262_harness_cpp
{
    void harness::parse(int argc, const char* argv[])
    {
        auto state = 0;
        
        for (auto i = 1; i < argc; i++)
        {
            std::string argument(argv[i]);
            
            switch (state)
            {
                case 0:
                    if (argument == "-t")
                    {
                        state = 1;
                    }
                    else if (argument == "-o")
                    {
                        state = 2;
                    }
					else if (argument == "-p")
					{
						display_progress = true;
					}
					else
                    {
                        throw std::runtime_error("Unrecognized argument: " + argument);
                    }

                    break;
                    
                case 1:
                    tests_directory = argument;
                    
                    state = 0;
                    
                    break;
                    
                case 2:
                    results_file = argument;
                    
                    state = 0;
                    
                    break;
            }
        }
        
        if (tests_directory == "")
        {
            throw std::runtime_error("Missing tests directory argument.");
        }
        
        if (results_file == "")
        {
            throw std::runtime_error("Missing results file argument.");
        }
        
        if (!directories::is_directory(tests_directory))
        {
            throw std::runtime_error("Specified tests directory is not valid.");
        }
    }
    
    void harness::run()
    {
        const std::string metadata_start("/*---");
        const std::string metadata_end("---*/");

        harness_directory = directories::combine(tests_directory, "harness");
        
        assert_file = directories::combine(harness_directory, "assert.js");
        
        sta_file = directories::combine(harness_directory, "sta.js");
        
        read_file(assert_file, assert_contents);
        
        read_file(sta_file, sta_contents);
        
        std::fstream results_stream(results_file, std::ios_base::out | std::ios_base::trunc);
        
        if (results_stream.fail())
        {
            throw std::runtime_error("Could not create file " + results_file);
        }

        std::vector<std::string> test_files;
        
        auto start_directory = directories::combine(tests_directory, "test");
        
        directories::get_test_files(start_directory, test_files);
        
        tests_read = (int)test_files.size();
		
		//auto starting_point_for_debug = 0;
        
		for (auto& test_file : test_files)
		{
			//starting_point_for_debug++; if (starting_point_for_debug < 11660) continue;

            std::string test_contents;
            
            read_file(test_file, test_contents);

            metadata metadata;
            
            auto metadata_start_position = test_contents.find(metadata_start);
            
            if (metadata_start_position != std::string::npos)
            {
                auto metadata_end_position = test_contents.find(metadata_end);
                
                if (metadata_end_position != std::string::npos)
                {
                    auto metadata_contents = test_contents.substr(metadata_start_position + metadata_start.length(), metadata_end_position - metadata_start_position - metadata_start.length());
                    
                    metadata.get_metadata(metadata_contents);
                }
            }
            
            runtime runtime;
            
            bool test_succeeded;
            
            if (metadata.no_strict || metadata.raw)
            {
                test_succeeded = evaluate_non_strict_mode(runtime, test_file, metadata, test_contents, results_stream);
            }
            else if (metadata.only_strict)
            {
                test_succeeded = evaluate_strict_mode(runtime, test_file, metadata, test_contents, results_stream);
            }
            else
            {
                test_succeeded = evaluate_non_strict_mode(runtime, test_file, metadata, test_contents, results_stream);
                test_succeeded &= evaluate_strict_mode(runtime, test_file, metadata, test_contents, results_stream);
            }

            tests_ran++;
            
            if (!test_succeeded)
            {
                tests_failed++;
            }

			if (display_progress)
			{
				std::cout << tests_ran_read_message() << std::endl;
			}
        }
    }
    
    std::string harness::tests_ran_read_message()
    {
        if (tests_read == 0)
        {
            return "No tests were found.";
        }
        else if (tests_ran == 0)
        {
            return "None of the " + std::to_string(tests_read) + " tests were ran.";
        }
        else if (tests_ran == tests_read)
        {
            return "All of the " + std::to_string(tests_read) + " tests were ran, " + std::to_string(tests_failed) + " tests failed.";
        }
        else
        {
            return std::to_string(tests_ran) + " of " + std::to_string(tests_read) + " tests were ran, " + std::to_string(tests_failed) + " tests failed.";
        }
    }

    void harness::read_file(std::string file, std::string& contents)
    {
        std::fstream test_file_stream(file, std::ios_base::in);
        
        if (test_file_stream.fail())
        {
            throw std::runtime_error("Could not open file " + file);
        }
        
        std::stringstream test_file_intermediate_stream;
        
        test_file_intermediate_stream << test_file_stream.rdbuf();
        
        if (test_file_stream.fail())
        {
            throw std::runtime_error("Could not read file " + file);
        }
        
        contents = test_file_intermediate_stream.str();
    }

    bool harness::evaluate_strict_mode(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::fstream& results_stream)
    {
        return evaluate_with_prefix(runtime, test_file, metadata, source, "\"use strict\";\n", "STRICT", results_stream);
    }
    
    bool harness::evaluate_non_strict_mode(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::fstream& results_stream)
    {
        return evaluate_with_prefix(runtime, test_file, metadata, source, "", "NON_STRICT", results_stream);
    }

    bool harness::evaluate_with_prefix(runtime& runtime, std::string test_file, metadata& metadata, std::string& source, std::string prefix, std::string mode, std::fstream& results_stream)
    {
        runtime.create_realm();
        
        std::string error_type;
        std::string error_description;

        if (runtime.evaluate(assert_file, assert_contents, error_type, error_description))
        {
            if (runtime.evaluate(sta_file, sta_contents, error_type, error_description))
            {
                auto success = true;
                
                if (metadata.async)
                {
                    auto done_print_handle_file = directories::combine(harness_directory, "doneprintHandle.js");
                    
                    std::string done_print_handle_contents;
                    
                    read_file(done_print_handle_file, done_print_handle_contents);
                    
                    success = runtime.evaluate(done_print_handle_file, done_print_handle_contents, error_type, error_description);
                }

                if (success)
                {
                    for (auto include : metadata.includes)
                    {
                        auto include_file = directories::combine(harness_directory, include);
                        
                        std::string include_contents;
                        
                        read_file(include_file, include_contents);
                        
                        success = runtime.evaluate(include_file, include_contents, error_type, error_description);
                        
                        if (!success)
                        {
                            break;
                        }
                    }
                    
                    if (success)
                    {
                        runtime.evaluate(test_file, prefix + source, error_type, error_description);
                    }
                }
            }
        }
        
        runtime.destroy_realm();
        
        bool pass;
        
        if (metadata.is_negative)
        {
            pass = (error_type == metadata.negative_type);
        }
        else
        {
            pass = (error_description == "");
        }

        results_stream << "[->";
        
        if (pass)
        {
            results_stream << "PASS";
        }
        else
        {
            results_stream << "FAIL";
        };
        
        results_stream << ":" << mode << "<-] " << test_file << std::endl;
        
        if (!pass)
        {
            if (metadata.is_negative)
            {
                results_stream << "Expected: \"" << metadata.negative_type << "\", returned: \"" << error_type << "\" - " << error_description << std::endl;
            }
            else
            {
                if (error_type != "")
                {
                    results_stream << error_type << " - ";
                }
                
                results_stream << error_description << std::endl;
            }
        }
        
        return pass;
    }
}
