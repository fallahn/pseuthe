/*********************************************************************
Matt Marchant 2015
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

#include <Score.hpp>
#include <Log.hpp>

#include <string>
#include <fstream>
#include <cstring>

namespace
{
    const int ident = 0x534e5542;
    const std::string scoreFile = "scores.dat";
}

void Scores::load(std::vector<Scores::Value>& dst)
{
    std::fstream file(scoreFile, std::ios::binary | std::ios::in);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open score data for reading", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    file.seekg(0, std::ios::end);
    int fileSize = static_cast<int>(file.tellg());
    file.seekg(0, std::ios::beg);

    if (fileSize < sizeof(Header))
    {
        Logger::Log("unexpected file size for score data.", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    std::vector<char>fileData(fileSize);
    file.read(fileData.data(), fileSize);
    file.close();

    Scores::Header header;
    std::memcpy(&header, fileData.data(), sizeof(Header));
    if (header.ident != ident)
    {
        Logger::Log("unusual ident found in score data", Logger::Type::Error, Logger::Output::All);
        return;
    }

    dst.resize(header.size);
    std::memcpy(dst.data(), fileData.data() + sizeof(Header), header.size * sizeof(Value));
}

void Scores::save(const std::vector<Scores::Value>& src)
{
    std::fstream file(scoreFile, std::ios::binary | std::ios::out);
    if (!file.good() || !file.is_open() || file.fail())
    {
        Logger::Log("failed to open score data for writing", Logger::Type::Error, Logger::Output::All);
        file.close();
        return;
    }

    Header header;
    header.ident = ident;
    header.size = src.size();

    file.write((char*)&header, sizeof(header));
    file.write((char*)src.data(), sizeof(Value) * header.size);
    file.close();
}