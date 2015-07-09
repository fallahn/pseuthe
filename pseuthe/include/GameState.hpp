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

//main state of the game

#ifndef GAME_STATE_HPP_
#define GAME_STATE_HPP_

#include <State.hpp>
#include <Entity.hpp>
#include <Scene.hpp>
#include <PhysicsWorld.hpp>
#include <AudioManager.hpp>
#include <GameController.hpp>

#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Shader.hpp>

#include <vector>

namespace sf
{
    class Color;
}

class GameState final : public State
{
public:
    GameState(StateStack& stateStack, Context context);
    ~GameState() = default;

    bool update(float dt) override;
    void draw() override;
    bool handleEvent(const sf::Event& evt) override;
    void handleMessage(const Message&) override;

private :

    MessageBus& m_messageBus;
    Scene m_scene;
    PhysicsWorld m_physWorld;
    AudioManager m_audioManager;
    GameController m_gameController;

    sf::RectangleShape m_vignette;
    sf::Text m_versionText;

    sf::Shader m_orbShader;
    Entity::Ptr createEntity();
};

#endif //GAME_STATE_HPP_