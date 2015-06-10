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

#ifndef UTIL_HPP_
#define UTIL_HPP_

#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>

#include <cmath>
#include <random>
#include <ctime>
#include <cassert>
#include <sstream>

namespace //is this not moot here as the anonymous namespace gets included in any TU which use this?
{
    static std::default_random_engine rndEngine(static_cast<unsigned long>(std::time(0)));
}

namespace Util
{
    namespace String
    {
        //converts a comma delimited string of floats into an array
        static std::vector<float> toFloatArray(const std::string& str)
        {
            std::vector<float> values;
            auto start = 0u;
            auto next = str.find_first_of(',');
            while (next != std::string::npos && start < str.length())
            {
                try
                {
                    values.push_back(std::stof(str.substr(start, next)));
                }
                catch (...)
                {
                    values.push_back(0.f);
                }
                start = ++next;
                next = str.find_first_of(',', start);
                if (next > str.length()) next = str.length();
            }
            return values;
        }

        //splits a soring with a given token and returns a vector of results
        static std::vector<std::string> tokenize(const std::string& str, char delim, bool keepEmpty = false)
        {
            assert(!str.empty());
            std::stringstream ss(str);
            std::string token;
            std::vector<std::string> output;
            while (std::getline(ss, token, delim))
            {
                if (!token.empty() ||
                    (token.empty() && keepEmpty))
                {
                    output.push_back(token);
                }
            }
            return output;
        }

        //returns string as all lower case
        static std::string toLower(const std::string& str)
        {
            std::string result = str;
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            return result;
        }
    }

    namespace Vector
    {
        //calculates dot product of 2 vectors
        static float dot(const sf::Vector2f& lv, const sf::Vector2f& rv)
        {
            return lv.x * rv.x + lv.y * rv.y;
        }
        //Returns a given vector with its length normalized to 1
        static sf::Vector2f normalise(sf::Vector2f source)
        {
            float length = std::sqrt(dot(source, source));
            if (length != 0) source /= length;
            return source;
        }
        //returns length squared
        static float lengthSquared(const sf::Vector2f& source)
        {
            return dot(source, source);
        }
        //Returns length of a given vector
        static float length(const sf::Vector2f& source)
        {
            return std::sqrt(lengthSquared(source));
        }

        static sf::Vector2f reflect(const sf::Vector2f& velocity, const sf::Vector2f& normal)
        {
            return -2.f * dot(velocity, normal) * normal + velocity;
        }

        //converts a comma delimited string to vector 2
        template <typename T>
        static sf::Vector2<T> vec2FromString(const std::string& str)
        {
            sf::Vector2<T> retVec;
            auto values = String::toFloatArray(str);
            switch (values.size())
            {
            case 2:
                retVec.y = static_cast<T>(values[1]);
            case 1:
                retVec.x = static_cast<T>(values[0]);
                break;
            default: break;
            }
            return retVec;
        }
    }

    namespace Position
    {
        static void centreOrigin(sf::Sprite& sprite)
        {
            sf::FloatRect bounds = sprite.getLocalBounds();
            sprite.setOrigin(std::floor(bounds.width / 2.f), std::floor(bounds.height / 2.f));
        }
        static void centreOrigin(sf::Text& text)
        {
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(std::floor(bounds.width / 2.f), std::floor(bounds.height / 2.f));
        }
    }

    namespace Random
    {
        static float value(float begin, float end)
        {
            assert(begin < end);
            std::uniform_real_distribution<float> dist(begin, end);
            return dist(rndEngine);
        }

        static int value(int begin, int end)
        {
            assert(begin < end);
            std::uniform_int_distribution<int> dist(begin, end);
            return dist(rndEngine);
        }
    }

    namespace Math
    {
        template <typename T>
        static T clamp(const T& n, const T& lower, const T& upper)
        {
            return std::max(lower, std::min(n, upper));
        }
    }
}

#endif //UTIL_HPP_