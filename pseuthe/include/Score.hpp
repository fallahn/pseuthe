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

//read / write score data

#ifndef SCORES_HPP_
#define SCORES_HPP_

#include <StateIds.hpp>

#include <vector>
#include <string>

class Scores final
{
public:
    struct Chunk
    {
        std::size_t offset;
        std::size_t size;
    };

    struct Header
    {
        int ident;
        int version;
        Chunk chunks[3];
    };

    struct Item
    {
        char name[21];
        float score;
        std::size_t hash;
    };

    Scores() = default;
    ~Scores() = default;
    Scores(const Scores&) = delete;
    const Scores& operator = (const Scores&) = delete;

    void load();
    void save();
    int add(const std::string&, float, Difficulty);

    const std::vector<Item>& getScores(Difficulty) const;

private:
    std::vector<Item> m_easyScores;
    std::vector<Item> m_mediumScores;
    std::vector<Item> m_hardScores;
};

#endif //SCORES_HPP_