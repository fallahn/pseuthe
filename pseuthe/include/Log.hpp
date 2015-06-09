/*********************************************************************
Matt Marchant 2014
http://trederia.blogspot.com

pseuthe Zlib license.

This software is provided 'as-is', without any express or
implied warranty. In no event will the authors be held
liable for any damages arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute
it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented;
you must not claim that you wrote the original software.
If you use this software in a product, an acknowledgment
in the product documentation would be appreciated but
is not required.

2. Altered source versions must be plainly marked as such,
and must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any
source distribution.
*********************************************************************/

//flexible logging class

#ifndef LOGGER_HPP_
#define LOGGER_HPP_

#include <string>
#include <iostream>
#include <fstream>

#ifndef _DEBUG_
    #define LOG(message, type)
#else
    #define LOG(message, type) Logger::Log(message, type);
#endif //_DEBUG_

#ifdef _MSC_VER
#include <Windows.h>
#endif //_MSC_VER


class Logger final
{
public:
    enum class Output
    {
        Console,
        File,
        All
    };

    enum class Type
    {
        Info,
        Warning,
        Error
    };

    //logs a message to a given destination.
    //message: message to log
    //type: whether this message gets tagged as information, a warning or an error
    //output: can be the console via cout, a log file on disk, or both
    static void Log(const std::string& message, Type type = Type::Info, Output output = Output::Console)
    {
        std::string outstring;
        switch (type)
        {
        case Type::Info:
        default:
            outstring = "INFO: " + message;
            break;
        case Type::Error:
            outstring = "ERROR: " + message;
            break;
        case Type::Warning:
            outstring = "WARNING: " + message;
            break;
        }

        if (output == Output::Console || output == Output::All)
        {
            (type == Type::Error) ?
                std::cerr << outstring << std::endl
            :
                std::cout << outstring <<  std::endl;
#ifdef _MSC_VER
            outstring += "\n";
            OutputDebugString(outstring.c_str());
#endif //_MSC_VER
        }
        if (output == Output::File || output == Output::All)
        {
            //output to a log file
            std::ofstream file("output.log", std::ios::app);
            if (file.good())
            { 
                file << outstring << std::endl;
                file.close();
            }
            else
            {
                Log(message, type, Output::Console);
                Log("Above message was intended for log file. Opening file probably failed.", Type::Warning, Output::Console);
            }
        }
    }

};

#endif //LOGGER_HPP_