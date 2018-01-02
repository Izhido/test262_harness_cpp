#include "directories.h"
#include <windows.h>
#include <errno.h>
#include <stdexcept>

namespace test262_harness_cpp
{
    bool directories::is_directory(std::string& path)
    {
		std::wstring path_as_wstring(path.begin(), path.end());

		auto attributes = GetFileAttributes(path_as_wstring.c_str());

		return ((attributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY);
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
        return "\\";
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
        
		std::string path_separator(path_separator());
		std::wstring path_separator_as_wstring(path_separator.begin(), path_separator.end());

		std::wstring directory_as_wstring(directory.begin(), directory.end());

		auto location = directory_as_wstring.find_last_of(path_separator_as_wstring);

		if (location != directory_as_wstring.length() - 1)
		{
			directory_as_wstring += path_separator_as_wstring;
		}

		directory_as_wstring += L"*";

		WIN32_FIND_DATA find_data;

		auto handle = FindFirstFile(directory_as_wstring.c_str(), &find_data);
        
        if (handle == INVALID_HANDLE_VALUE)
        {
			auto error_code = GetLastError();

			LPVOID error_buffer;

			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&error_buffer, 0, nullptr);

			std::wstring error_text_as_wstring((LPTSTR)error_buffer);
            
			LocalFree(error_buffer);

			std::string error_text(error_text_as_wstring.begin(), error_text_as_wstring.end());

            throw std::runtime_error("Could not open directory " + directory + ": " + error_text);
        }

        do
        {
            std::wstring filename(find_data.cFileName);
            
            if (filename == L"." || filename == L"..")
            {
                continue;
            }

			std::string filename_as_string(filename.begin(), filename.end());

            auto to_add = combine(directory, filename_as_string);
            
            if ((find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == FILE_ATTRIBUTE_DIRECTORY)
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
        while (FindNextFile(handle, &find_data));

        FindClose(handle);
    }
}
