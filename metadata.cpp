#include "metadata.h"

namespace test262_harness_cpp
{
    void metadata::get_metadata(std::string& contents)
    {
        auto state = 0;
        
        std::string attribute;
        std::string key;
        std::string entry;
        
        for (auto c : contents)
        {
            switch (state)
            {
                case 0:
                    if (c == '\r' || c == '\n')
                    {
                        state = 1;
                    }
                    
                    break;
                    
                case 1:
                    if (c <= ' ')
                    {
                        if (c != '\r' && c != '\n')
                        {
                            state = 0;
                        }
                        
                        break;
                    }
                    
                    attribute.clear();
                    
                    attribute.push_back(c);
                    
                    state = 2;
                    
                    break;
                    
                case 2:
                    if (c == ':')
                    {
                        if (attribute == "negative")
                        {
                            is_negative = true;
                            
                            state = 3;
                        }
                        else if (attribute == "includes")
                        {
                            state = 6;
                        }
                        else if (attribute == "flags")
                        {
                            state = 13;
                        }
                        else
                        {
                            state = 0;
                        }
                        
                        break;
                    }
                    
                    attribute.push_back(c);
                    
                    break;
                    
                case 3:
                    if (c == ':')
                    {
                        if (key == "phase")
                        {
                            state = 4;
                        }
                        else if (key == "type")
                        {
                            state = 5;
                        }
                        
                        break;
                    }
                    else if (c <= ' ')
                    {
                        break;
                    }
                    
                    key.push_back(c);
                    
                    break;
                    
                case 4:
                    if (c <= ' ')
                    {
                        if (negative_phase == "")
                        {
                            break;
                        }
                        
                        if (negative_type == "")
                        {
                            key.clear();
                            
                            state = 3;
                            
                            break;
                        }
                        
                        state = 0;
                        
                        break;
                    }
                    
                    negative_phase.push_back(c);
                    
                    break;
                    
                case 5:
                    if (c <= ' ')
                    {
                        if (negative_type == "")
                        {
                            break;
                        }
                        
                        if (negative_phase == "")
                        {
                            key.clear();
                            
                            state = 3;
                            
                            break;
                        }
                        
                        state = 0;
                        
                        break;
                    }
                    
                    negative_type.push_back(c);
                    
                    break;
                    
                case 6:
                    if (c == '[')
                    {
                        entry.clear();
                        
                        state = 7;
                    }
                    else if (c == '-')
                    {
                        state = 8;
                    }
                    
                    break;

                case 7:
                    if (c == ']' || c == ',')
                    {
                        if (entry != "")
                        {
                            includes.push_back(entry);
                            
                            entry.clear();
                        }
                        
                        if (c == ']')
                        {
                            attribute.clear();
                            
                            state = 0;
                        }
                        
                        break;
                    }
                    else if (c <= ' ')
                    {
                        break;
                    }
                    
                    entry.push_back(c);
                    
                    break;

                case 8:
                    if (c == ' ')
                    {
                        entry.clear();
                        
                        state = 9;
                    }
                    else
                    {
                        state = 0;
                    }
                    
                    break;
                    
                case 9:
                    if (c <= ' ')
                    {
                        if (entry == "")
                        {
                            break;
                        }
                        
                        includes.push_back(entry);
                        
                        entry.clear();
                        
                        if (c == '\r' || c == '\n')
                        {
                            state = 11;
                        }
                        else
                        {
                            state = 10;
                        }
                        
                        break;
                    }
                    
                    entry.push_back(c);
                    
                    break;
                    
                case 10:
                    if (c == '\r' || c == '\n')
                    {
                        state = 11;
                    }
                    
                    break;

                case 11:
                    if (c <= ' ')
                    {
                        if (c != '\r' && c != '\n')
                        {
                            state = 12;
                        }
                        
                        break;
                    }
                    
                    attribute.clear();
                    
                    attribute.push_back(c);
                    
                    state = 2;
                    
                    break;
                    
                case 12:
                    if (c == '-')
                    {
                        state = 8;
                    }
                    
                    break;

                case 13:
                    if (c == '[')
                    {
                        entry.clear();
                        
                        state = 14;
                    }
                    
                    break;
                    
                case 14:
                    if (c == ']' || c == ',')
                    {
                        if (entry == "onlyStrict")
                        {
                            only_strict = true;
                        }
                        else if (entry == "noStrict")
                        {
                            no_strict = true;
                        }
                        else if (entry == "module")
                        {
                            module = true;
                        }
                        else if (entry == "raw")
                        {
                            raw = true;
                        }
                        else if (entry == "async")
                        {
                            async = true;
                        }
                        
                        if (c == ']')
                        {
                            state = 0;
                        }
                        
                        break;
                    }
                    else if (c <= ' ')
                    {
                        break;
                    }
                    
                    entry.push_back(c);
                    
                    break;
            }
        }
    }
}

