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

#include <GameState.hpp>
#include <CircleDrawable.hpp>
#include <GradientDrawable.hpp>
#include <ParticleSystem.hpp>
#include <ParticleField.hpp>
#include <FadeDrawable.hpp>
#include <CausticDrawable.hpp>
#include <LightPosition.hpp>
#include <InputComponent.hpp>
#include <BodypartController.hpp>
#include <AnimatedDrawable.hpp>
#include <App.hpp>
#include <Log.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    const int nubbinCount = 19;
    const std::string version("version 0.5.2");
}

GameState::GameState(StateStack& stateStack, Context context)
    : State     (stateStack, context),
    m_messageBus(context.appInstance.getMessageBus()),
    m_scene     (m_messageBus),
    m_physWorld (m_messageBus)
{
    for (int i = 0; i < nubbinCount; ++i)
    {
        auto ent = createEntity();
        m_scene.addEntity(ent, Scene::Layer::FrontMiddle);
    }

    auto grad = std::make_unique<GradientDrawable>(m_messageBus);
    m_scene.getLayer(Scene::Layer::BackRear).addComponent<GradientDrawable>(grad);

    auto caustics = std::make_unique<CausticDrawable>(m_messageBus);
    m_scene.getLayer(Scene::Layer::BackMiddle).addComponent<CausticDrawable>(caustics);
    float xPos = static_cast<float>(Util::Random::value(0, 1920));
    m_scene.getLayer(Scene::Layer::BackMiddle).setPosition(xPos, -200.f);
    auto lightPos = std::make_unique<LightPosition>(m_messageBus);
    m_scene.getLayer(Scene::Layer::BackMiddle).addComponent<LightPosition>(lightPos);

    auto particleField = std::make_unique<ParticleField>(sf::FloatRect(-30.f, -30.f, 1980.f, 1140.f), m_messageBus);
    particleField->setTexture(context.appInstance.getTexture("assets/images/particles/field.png"));
    m_scene.getLayer(Scene::Layer::BackFront).addComponent<ParticleField>(particleField);

    auto fade = std::make_unique<FadeDrawable>(m_messageBus);
    m_scene.getLayer(Scene::Layer::FrontFront).addComponent<FadeDrawable>(fade);

    m_vignette.setSize({ 1920.f, 1080.f });
    m_vignette.setTexture(&context.appInstance.getTexture("assets/images/vignette.png"));
    m_vignette.setOrigin(m_vignette.getSize() / 2.f);
    
    m_versionText.setFont(context.appInstance.getFont("assets/fonts/VeraMono.ttf"));
    m_versionText.setString(version);
    m_versionText.setCharacterSize(14u);
    m_versionText.setPosition(10.f, 10.f);

    m_audioManager.mute(context.appInstance.getAudioSettings().muted);

    spawnPlayer();
}

bool GameState::update(float dt)
{
    //probably ok to do here, although we could always raise an event when resizing window
    m_vignette.setPosition(getContext().renderWindow.getDefaultView().getCenter());    
    m_scene.setView(getContext().defaultView);

    m_audioManager.update(dt);
    m_physWorld.update(dt);
    m_scene.update(dt);

    return true;
}

void GameState::draw()
{
    auto& rw = getContext().renderWindow;
    rw.setView(getContext().renderWindow.getDefaultView());
    rw.draw(m_scene);
    rw.draw(m_vignette, sf::BlendMultiply);
    rw.draw(m_versionText);
}

bool GameState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        case sf::Keyboard::Space:
        case sf::Keyboard::Escape:
            requestStackPush(States::ID::Menu);
            break;
        default: break;
        }
    }
    return true;
}

void GameState::handleMessage(const Message& msg)
{
    m_audioManager.handleMessage(msg);
    m_physWorld.handleMessage(msg);
    m_scene.handleMessage(msg);
}

//private
Entity::Ptr GameState::createEntity()
{
    const int maxSize = 50;
    float size = static_cast<float>(Util::Random::value(10, maxSize));

    Entity::Ptr e = std::make_unique<Entity>(m_messageBus);
    CircleDrawable::Ptr cd = std::make_unique<CircleDrawable>(size, m_messageBus);
    auto colour = cd->getColour();
    e->addComponent<CircleDrawable>(cd);

    PhysicsComponent::Ptr pc = m_physWorld.addBody(size);
    e->addComponent<PhysicsComponent>(pc);

    ParticleSystem::Ptr ps = ParticleSystem::create(Particle::Type::Echo, m_messageBus);
    ps->setTexture(getContext().appInstance.getTexture("assets/images/particles/circle.png"));
    auto particleSize = size * 2.f;
    ps->setParticleSize({ particleSize, particleSize });
    ps->setColour(colour);
    e->addComponent<ParticleSystem>(ps);

    ps = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    ps->setTexture(getContext().appInstance.getTexture("assets/images/particles/circle.png"));
    particleSize = size / 12.f;
    ps->setParticleSize({ particleSize, particleSize });

    Entity::Ptr f = std::make_unique<Entity>(m_messageBus);
    f->addComponent<ParticleSystem>(ps);
    e->addChild(f);

    return std::move(e);
}

namespace
{
    const float partScale = 0.9f;
    const float partPadding = 10.f;
    const float playerSize = 35.f;
    const sf::Vector2f spawnPosition(960.f, 540.f);
}

void GameState::spawnPlayer()
{
    auto entity = std::make_unique<Entity>(m_messageBus);
    entity->setWorldPosition(spawnPosition);

    auto cd = std::make_unique<CircleDrawable>(playerSize, m_messageBus);
    cd->setColour(sf::Color(150u, 150u, 255u));
    entity->addComponent<CircleDrawable>(cd);

    auto physComponent = m_physWorld.addBody(playerSize);
    physComponent->setName("control");
    physComponent->setPosition(entity->getWorldPosition());
    auto lastPhysComponent = physComponent.get();
    entity->addComponent<PhysicsComponent>(physComponent);

    auto controlComponent = std::make_unique<InputComponent>(m_messageBus);
    entity->addComponent<InputComponent>(controlComponent);

    float constraintLength = playerSize * 2.f + partPadding;
    float nextSize = playerSize * partScale;

    for (auto i = 1; i < 6; ++i)
    {   
        auto bodyPart = std::make_unique<Entity>(m_messageBus);
        bodyPart->setWorldPosition({ spawnPosition.x - (constraintLength * i), spawnPosition.y });

        auto drawable = std::make_unique<AnimatedDrawable>(m_messageBus, getContext().appInstance.getTexture("assets/images/player/bird_diffuse.png"));
        drawable->loadAnimationData("assets/images/player/bird.cra");
        drawable->setOrigin(sf::Vector2f(drawable->getFrameSize()) / 2.f);
        drawable->setBlendMode(sf::BlendAdd);
        //TODO randomise start frame
        drawable->play(drawable->getAnimations()[0]);
        bodyPart->addComponent<AnimatedDrawable>(drawable);

        physComponent = m_physWorld.attachBody(nextSize, constraintLength, lastPhysComponent);
        physComponent->setPosition(bodyPart->getWorldPosition());
        physComponent->setVelocity(lastPhysComponent->getVelocity());
        physComponent->setName("control");
        lastPhysComponent = physComponent.get();
        bodyPart->addComponent<PhysicsComponent>(physComponent);

        auto bpCont = std::make_unique<BodypartController>(m_messageBus);
        bodyPart->addComponent<BodypartController>(bpCont);

        entity->addChild(bodyPart);

        nextSize *= partScale;
        constraintLength *= partScale;
    }
    m_scene.getLayer(Scene::Layer::FrontMiddle).addChild(entity);
}