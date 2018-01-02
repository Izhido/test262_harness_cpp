#include "harness.h"
#include <stdexcept>
#include <cstdlib>
#include <iostream>

int main(int argc, const char* argv[])
{
    test262_harness_cpp::harness harness;
    
    try
    {
        harness.parse(argc, argv);
        
        harness.run();
        
        std::cout << harness.tests_ran_read_message() << std::endl;

        return EXIT_SUCCESS;
    }
    catch (std::exception& harness_runtime_error)
    {
        std::cout << harness_runtime_error.what() << std::endl;
        
        std::cout << std::endl << harness.tests_ran_read_message() << std::endl;
        
        return EXIT_FAILURE;
    }
    catch (...)
    {
        std::cout << "Unexpected error ocurred while running the tests - a reason for the error was not returned." << std::endl;
        
        std::cout << std::endl << harness.tests_ran_read_message() << std::endl;

        return EXIT_FAILURE;
    }
}
