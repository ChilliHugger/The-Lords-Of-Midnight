#include "../inc/mxtypes.h"
#include "../inc/misc.h"
#include "../inc/carchive.h"

using namespace chilli::lib;

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <cctype>

namespace chilli {

    namespace lib {

        namespace StringExtensions {
    
        archive& SerializeString ( archive& ar, std::string& string )
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
        
        long atol( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atol(value.c_str());
        }

        f32 atof( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atof(value.c_str());
        }
        
        s32 atoi( std::string& value )
        {
            if ( value.empty() ) return 0;
            return ::atoi(value.c_str());
        }

        int split ( const std::string& source, char delim, c_string& tokens )
        {
            tokens.Clear();
            
            auto result = c_string{};
            auto ss = std::stringstream{source};

            for (std::string line; std::getline(ss, line, delim);)
                tokens.push_back(line);

            return (int)tokens.size();
        }
         
        c_string split_by_newline(const std::string& str)
        {
            auto lines = c_string{};
            split(str, '\n', lines);
            return lines;
        }

        std::string right(std::string& input, int amount)
        {
            int inputSize = (int)input.size();
            return (amount > 0 && inputSize > amount) ? input.substr(inputSize - amount) : input;
        }
        
        std::string replaceAll(const std::string& str, const std::string& from, const std::string& to)
        {
            size_t start_pos = 0;
            std::string temp = str;
            while((start_pos = temp.find(from, start_pos)) != std::string::npos) {
                temp.replace(start_pos, from.length(), to);
                start_pos += to.length();
            }
            return temp;
        }
        
        std::string toUpper(const std::string& value)
        {
            std::string str = value;
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        }
        
        std::string Format ( LPCSTR format, ... )
        {
        const size_t BufferSize = 1024;
        char buffer[BufferSize];
        va_list arglist ;
              va_start( arglist, format ) ;
              vsnprintf( buffer, BufferSize-3, format, arglist );
              va_end( arglist ) ;
            return buffer;
        }
        
        bool stringicompare(const std::string& str1, const std::string& str2)
        {
            return c_stricmp(str1.c_str(), str2.c_str()) == 0;

//            return ((str1.size() == str2.size())
//                && std::equal(str1.begin(), str1.end(), str2.begin(),
//                [] (char & c1, char & c2) {
//                    return (c1 == c2 || std::toupper(c1) == std::toupper(c2));
//                }
//                ));
        }
        
        int GetToken ( const std::string& token, token_t array[], int max )
        {
            if ( !token.empty() ) {
                for ( int ii=0; ii<max; ii++ ) {
                    if (c_stricmp( array[ii].token, token.c_str() ) == 0 )
                        return array[ii].value;
                }
            }
            return 0 ;
        }
            
    }
    // namespace StringExtensions
    
    }
    // namespace types

}
// namespace tme
