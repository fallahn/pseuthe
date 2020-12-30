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

//observes the scene and controls spawning of entities affecting game play

#ifndef GAME_CONTROLLER_HPP_
#define GAME_CONTROLLER_HPP_

#include <StateIds.hpp>

#include <SFML/Config.hpp>
#include <SFML/Graphics/Rect.hpp>

#include <vector>
#include <array>
#include <string>

class Scene;
class MessageBus;
class Message;
class App;
class PhysicsWorld;
class PhysicsComponent;
class TextDrawable;
class Entity;
class GameController final
{
public:
    GameController(Scene&, MessageBus&, App&, PhysicsWorld&);
    ~GameController() = default;
    GameController(const GameController&) = delete;
    const GameController& operator = (const GameController&) = delete;

    void update(float);
    void handleMessage(const Message&);


private:
    Scene& m_scene;
    MessageBus& m_messageBus;
    App& m_appInstance;
    PhysicsWorld& m_physicsWorld;

    Entity* m_player;

    float m_constraintLength;
    float m_nextPartSize;
    float m_nextPartScale;
    std::vector<PhysicsComponent*> m_playerPhysicsComponents;

    std::array<sf::FloatRect, 2> m_planktonSpawns;
    sf::Uint8 m_planktonCount;
    float m_spawnTime;
    sf::Uint8 m_initialPartCount;
    float m_partDecayRate;
    float m_speedMultiplier;

    ControlType m_controlType;

    Difficulty m_difficulty;
    void setDifficulty(Difficulty);

    void spawnPlayer();
    void addBodyPart(float health = 100.f);
    void spawnPlankton();

    TextDrawable* m_scoreText;
    TextDrawable* m_highScoreText;
    TextDrawable* m_initialsText;
    bool m_paused;
    void resetScore();
    std::string getName() const;
};

#endif //GAME_CONTROLLER_HPP_