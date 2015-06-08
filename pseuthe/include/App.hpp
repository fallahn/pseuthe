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

//main entry point for the app

#ifndef APP_HPP_
#define APP_HPP_

#include <SFML/Graphics/RenderWindow.hpp>

#include <functional>

class App final
{
public:

    struct VideoSettings final
    {
        sf::Int32 WindowStyle;
        sf::VideoMode VideoMode;
        bool VSync;
        std::vector<sf::VideoMode> AvailableVideoModes;

        VideoSettings()
            : WindowStyle(sf::Style::Close),
            VideoMode(1024, 576),
            VSync(true){}
    };

    App();
    ~App() = default;
    App(const App&) = delete;
    const App& operator = (const App&) = delete;

    void run();
    void pause();
    void resume();

    const VideoSettings& getVideoSettings() const;

private:

    VideoSettings m_videoSettings;

    sf::RenderWindow m_renderWindow;

    void handleEvents();
    std::function<void(float)> update;
    void updateApp(float dt);
    void pauseApp(float dt);
    void draw();

    void registerStates();
};

#endif //APP_HPP_