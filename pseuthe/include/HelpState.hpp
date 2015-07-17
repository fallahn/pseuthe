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

//displays the help menu

#ifndef HELP_STATE_HPP_
#define HELP_STATE_HPP_

#include <State.hpp>
#include <PhysicsWorld.hpp>
#include <Entity.hpp>
#include <PlanktonController.hpp>

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

class MessageBus;
class HelpState final : public State
{
public:
    HelpState(StateStack&, Context);
    ~HelpState() = default;

    bool update(float) override;
    void draw() override;
    bool handleEvent(const sf::Event&) override;
    void handleMessage(const Message&) override;

private:

    enum class Mode
    {
        FadeIn,
        Static,
        FadeOut
    } m_mode;

    MessageBus& m_messageBus;
    PhysicsWorld m_physWorld;
    Entity m_rootNode;

    sf::Sprite m_menuSprite;
    sf::RectangleShape m_rectangleShape;
    std::vector<sf::Text> m_texts;

    float m_fadeTime;

    void addText();
    void addPlankton(PlanktonController::Type);
};

#endif //HELP_STATE_HPP_