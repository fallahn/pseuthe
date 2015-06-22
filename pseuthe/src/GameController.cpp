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

#include <GameController.hpp>
#include <Scene.hpp>
#include <MessageBus.hpp>
#include <App.hpp>
#include <PhysicsWorld.hpp>
#include <Entity.hpp>
#include <ParticleSystem.hpp>

#include <InputComponent.hpp>
#include <BodypartController.hpp>
#include <AnimatedDrawable.hpp>

#include <memory>

namespace
{
    const int maxBodyParts = 6;
}

GameController::GameController(Scene& scene, MessageBus& mb, App& app, PhysicsWorld& pw)
    : m_scene       (scene),
    m_messageBus    (mb),
    m_appInstance   (app),
    m_physicsWorld  (pw),
    m_player        (nullptr)
{

}

//public
void GameController::update(float dt)
{

}

void GameController::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::UI)
    {
        switch (msg.ui.type)
        {
        case Message::UIEvent::MenuClosed:
            if (!m_player)
            {
                spawnPlayer();
            }
            break;
        case Message::UIEvent::MenuOpened:
            if (m_player)
            {
                
            }
            break;
        default:break;
        }
    }
    else if (msg.type == Message::Type::Player)
    {
        switch (msg.player.action)
        {
        case Message::PlayerEvent::Died:
            m_player = nullptr;
            break;
        default: break;
        }
    }
}

namespace
{
    const float partScale = 0.9f;
    const float partPadding = 6.f;
    const float playerSize = 32.f;
    const sf::Vector2f spawnPosition(960.f, 540.f);
}

void GameController::spawnPlayer()
{
    auto entity = std::make_unique<Entity>(m_messageBus);
    entity->setWorldPosition(spawnPosition);

    auto cd = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/head.png"));
    cd->loadAnimationData("assets/images/player/head.cra");
    cd->setOrigin(sf::Vector2f(cd->getFrameSize()) / 2.f);
    cd->setBlendMode(sf::BlendAdd);
    cd->play(cd->getAnimations()[0]);
    cd->setName("drawable");
    entity->addComponent<AnimatedDrawable>(cd);

    auto physComponent = m_physicsWorld.addBody(playerSize);
    physComponent->setName("control");
    physComponent->setPosition(entity->getWorldPosition());
    auto lastPhysComponent = physComponent.get();
    entity->addComponent<PhysicsComponent>(physComponent);

    auto trailComponent = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    trailComponent->setTexture(m_appInstance.getTexture("assets/images/particles/circle.png"));
    trailComponent->setParticleSize({ 2.f, 2.f });
    trailComponent->setName("trail");
    entity->addComponent<ParticleSystem>(trailComponent);

    auto controlComponent = std::make_unique<InputComponent>(m_messageBus);
    entity->addComponent<InputComponent>(controlComponent);

    float constraintLength = playerSize * 2.f + partPadding;
    float nextSize = playerSize * partScale;
    float nextScale = partScale;

    for (auto i = 1; i < maxBodyParts / 2; ++i)
    {
        auto bodyPart = std::make_unique<Entity>(m_messageBus);
        bodyPart->setWorldPosition({ spawnPosition.x - (constraintLength * i), spawnPosition.y });

        auto drawable = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/bodypart01.png"));
        drawable->loadAnimationData("assets/images/player/bodypart01.cra");
        drawable->setOrigin(sf::Vector2f(drawable->getFrameSize()) / 2.f);
        drawable->setBlendMode(sf::BlendAdd);
        drawable->play(drawable->getAnimations()[0], drawable->getFrameCount() / maxBodyParts * i);
        drawable->setScale(nextScale, nextScale);
        drawable->setName("drawable");
        bodyPart->addComponent<AnimatedDrawable>(drawable);

        physComponent = m_physicsWorld.attachBody(nextSize, constraintLength, lastPhysComponent);
        physComponent->setPosition(bodyPart->getWorldPosition());
        physComponent->setVelocity(lastPhysComponent->getVelocity());
        physComponent->setName("control");
        lastPhysComponent = physComponent.get();
        bodyPart->addComponent<PhysicsComponent>(physComponent);

        auto bpCont = std::make_unique<BodypartController>(m_messageBus);
        bodyPart->addComponent<BodypartController>(bpCont);

        entity->addChild(bodyPart);

        nextSize *= partScale;
        nextScale *= partScale;
        constraintLength *= partScale;
    }
    m_player = entity.get();
    m_scene.getLayer(Scene::Layer::FrontMiddle).addChild(entity);
}