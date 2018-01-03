#include "directories.h"
#include <dirent.h>
#include <errno.h>
#include <stdexcept>
#include <cstring>

namespace test262_harness_cpp
{
    bool directories::is_directory(std::string& path)
    {
        auto directory_entry = opendir(path.c_str());
        
        if (directory_entry == nullptr)
        {
            return false;
        }
        
        closedir(directory_entry);
        
        return true;
    }
    
    std::string directories::get_directory(std::string& path)
    {
        auto location = path.find_last_of(path_separator());
        
        if (location == std::string::npos)
        {
            return "";
        }
        
        return path.substr(0, location);
    }
    
    std::string directories::path_separator()
    {
        return "/";
    }

    std::string directories::combine(std::string path, std::string file)
    {
        auto location = path.find_last_of(path_separator());
        
        if (location == path.length() - 1)
        {
            path.pop_back();
        }
        
        do
        {
            location = file.find_first_of(path_separator());
            
            if (location == std::string::npos)
            {
                return path + path_separator() + file;
            }

            auto component = file.substr(0, location);
            
            file.erase(0, location + 1);
            
            if (component == "..")
            {
                location = path.find_last_of(path_separator());

                if (location == std::string::npos)
                {
                    throw std::runtime_error("Directory combination moved beyond original path.");
                }
                
                path.erase(0, location);
            }
            else if (component != ".")
            {
                path += path_separator();
                path += component;
            }
        }
        while (true);
        
        return path;
    }

    void directories::get_test_files(std::string& directory, std::vector<std::string>& files)
    {
        const std::string to_exclude("_FIXTURE.js");
        const std::string to_include(".js");
        
        auto directory_entry = opendir(directory.c_str());
        
        if (directory_entry == nullptr)
        {
            auto error_text = strerror(errno);
            
            throw std::runtime_error("Could not open directory " + directory + ": " + std::string(error_text));
        }

        do
        {
            auto entry = readdir(directory_entry);
            
            if (entry == nullptr)
            {
                break;
            }
            
            std::string filename(entry->d_name);
            
            if (filename == "." || filename == "..")
            {
                continue;
            }

            auto to_add = combine(directory, filename);
            
            if ((entry->d_type & DT_DIR) == DT_DIR)
            {
                get_test_files(to_add, files);
                
                continue;
            }
            
            if (to_add.length() >= to_exclude.length())
            {
                auto ending = to_add.substr(to_add.length() - to_exclude.length());
                
                if (ending == to_exclude)
                {
                    continue;
                }
            }
            
            if (to_add.length() <= to_include.length())
            {
                continue;
            }

            auto ending = to_add.substr(to_add.length() - to_include.length());
            
            if (ending != to_include)
            {
                continue;
            }

            files.push_back(to_add);
        }
        while (true);

        closedir(directory_entry);
    }
}
