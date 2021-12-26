#include "../inc/mxtypes.h"
#include "../inc/misc.h"
#include "../inc/carchive.h"

using namespace chilli::lib;

#include <string>
#include <sstream>
#include <iostream>
#include <vector>

namespace chilli {

    namespace lib {

        archive& StringExtensions::SerializeString ( archive& ar, std::string& string )
        {
        LPSTR temp = nullptr;

            if ( ar.IsStoring() ) {
                ar << (char*)string.c_str() ;
            }else{
                string.clear();
                ar >> &temp ;
                if(temp!=nullptr) {
                    string = temp;
                }
            }

            SAFEFREE ( temp );
            return ar;
        }
        
        long StringExtensions::atol( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atol(value.c_str());
        }

        f32 StringExtensions::atof( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atof(value.c_str());
        }
        
        s32 StringExtensions::atoi( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atoi(value.c_str());
        }

        int StringExtensions::split ( const std::string& source, char delim, c_string& tokens )
        {
            tokens.Clear();
            
            auto result = c_string{};
            auto ss = std::stringstream{source};

            for (std::string line; std::getline(ss, line, delim);)
                tokens.push_back(line);

            return tokens.size();
        }
         
        c_string StringExtensions::split_by_newline(const std::string& str)
        {
            auto lines = c_string{};
            split(str, '\n', lines);
            return lines;
        }

        std::string StringExtensions::right(std::string& input, int amount)
        {
            int inputSize = input.size();
            return (amount > 0 && inputSize > amount) ? input.substr(inputSize - amount) : input;
        }
        
        std::string StringExtensions::replaceAll(const std::string& str, const std::string& from, const std::string& to)
        {
            size_t start_pos = 0;
            std::string temp = str;
            while((start_pos = temp.find(from, start_pos)) != std::string::npos) {
                temp.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return temp;
        }
        
        std::string StringExtensions::toUpper(const std::string& value)
        {
            std::string str = value;
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        }

    }
    // namespace types

}
// namespace tme
