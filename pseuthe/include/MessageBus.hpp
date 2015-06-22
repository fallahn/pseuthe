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

//message bus to allow inter-component communication

#ifndef MESSAGE_BUS_HPP_
#define MESSAGE_BUS_HPP_

#include <SFML/Config.hpp>
#include <PhysicsComponent.hpp>

#include <queue>

class Message final
{
public:
    enum Type
    {
        Audio = 1,
        Physics,
        Drawable,
        Entity,
        UI
    }type;

    struct AudioEvent
    {
        sf::Uint64 entityId;
    };

    struct PhysicsEvent
    {
        enum Event
        {
            Collided,
            ConstraintDestroyed
        }event;

        sf::Uint64 entityId[2];
        PhysicsComponent::Constraint* constraint;
    };

    struct DrawableEvent
    {

    };

    struct EntityEvent
    {
        bool maxCollisionsReached;
    };

    struct UIEvent
    {
        enum
        {
            RequestVolumeChange,
            RequestAudioMute,
            RequestAudioUnmute,
            MenuOpened,
            MenuClosed
        }type;
        float value;
    };

    union
    {
        AudioEvent audio;
        PhysicsEvent physics;
        DrawableEvent drawable;
        EntityEvent entity;
        UIEvent ui;
    };
};

class MessageBus final
{
public:
    MessageBus();
    ~MessageBus() = default;
    MessageBus(const MessageBus&) = delete;
    const MessageBus& operator = (const MessageBus&) = delete;

    //read and despatch all messages on the message stack
    Message poll();
    //places a message on the message stack
    void send(const Message& msg);

    bool empty();

private:
    std::queue<Message> m_messages;
};

#endif