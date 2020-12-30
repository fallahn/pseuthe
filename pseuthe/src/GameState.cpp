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

#include <MakeUnique.hpp>
#include <GameState.hpp>
#include <CircleDrawable.hpp>
#include <GradientDrawable.hpp>
#include <ParticleSystem.hpp>
#include <ParticleField.hpp>
#include <FadeDrawable.hpp>
#include <CausticDrawable.hpp>
#include <AnimatedDrawable.hpp>
#include <LightPosition.hpp>
#include <OrbController.hpp>
#include <TextDrawable.hpp>
#include <Shaders.hpp>

#include <App.hpp>
#include <Log.hpp>
#include <Util.hpp>

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

namespace
{
    const int nubbinCount = 19;
    const std::string version("version 1.0.4");
}

GameState::GameState(StateStack& stateStack, Context context)
    : State             (stateStack, context),
    m_messageBus        (context.appInstance.getMessageBus()),
    m_scene             (m_messageBus),
    m_physWorld         (m_messageBus),
    m_gameController    (m_scene, m_messageBus, context.appInstance, m_physWorld)
{
    m_orbShader.loadFromMemory(Shader::Orb::vertex, Shader::Orb::fragment);

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
    
    auto versionText = std::make_unique<TextDrawable>(m_messageBus);
    versionText->setFont(context.appInstance.getFont("assets/fonts/VeraMono.ttf"));
    versionText->setString(version);
    versionText->setCharacterSize(24u);
    versionText->setPosition(10.f, 10.f);
    m_scene.getLayer(Scene::Layer::UI).addComponent<TextDrawable>(versionText);

    auto pausedText = std::make_unique<TextDrawable>(m_messageBus);
    pausedText->setFont(context.appInstance.getFont("assets/fonts/Ardeco.ttf"));
    pausedText->setString("PAUSED");
    pausedText->setCharacterSize(80u);
    Util::Position::centreOrigin(*pausedText);
    pausedText->setPosition(960.f, 960.f);
    pausedText->setFillColor(sf::Color::Transparent);
    pausedText->setName("paused_text");
    m_scene.getLayer(Scene::Layer::UI).addComponent<TextDrawable>(pausedText);

    m_audioManager.mute(context.appInstance.getAudioSettings().muted);
}

bool GameState::update(float dt)
{
    /*if (sf::Mouse::isButtonPressed(sf::Mouse::Left))
    {
        auto& rw = getContext().renderWindow;
        auto pos = rw.mapPixelToCoords(sf::Mouse::getPosition(rw));
        Message msg;
        msg.type = Message::Type::UI;
        msg.ui.mouseX = pos.x;
        msg.ui.mouseY = pos.y;
        msg.ui.type = Message::UIEvent::MouseClicked;
        m_messageBus.send(msg);
    }*/
    
    
    //probably ok to do here, although we could always raise an event when resizing window
    m_vignette.setPosition(getContext().defaultView.getCenter()); //TODO this should be part of the scene
    m_scene.setView(getContext().defaultView);

    m_audioManager.update(dt);
    m_physWorld.update(dt);
    m_scene.update(dt);
    m_gameController.update(dt);

    return true;
}

void GameState::draw()
{
    auto& rw = getContext().renderWindow;
    rw.setView(getContext().renderWindow.getDefaultView());
    rw.draw(m_scene);

    rw.setView(getContext().defaultView);
    rw.draw(m_vignette, sf::BlendMultiply);
}

bool GameState::handleEvent(const sf::Event& evt)
{
    if (evt.type == sf::Event::KeyReleased)
    {
        switch (evt.key.code)
        {
        //case sf::Keyboard::Space:
        case sf::Keyboard::Escape:
        case sf::Keyboard::P:
            requestStackPush(States::ID::Menu);
            m_scene.getLayer(Scene::Layer::UI).getComponent<TextDrawable>("paused_text")->setFillColor(sf::Color::White);
            break;
        default: break;
        }
    }
    else if (evt.type == sf::Event::JoystickButtonReleased)
    {
        switch (evt.joystickButton.button)
        {
        case 7:
            requestStackPush(States::ID::Menu);
            m_scene.getLayer(Scene::Layer::UI).getComponent<TextDrawable>("paused_text")->setFillColor(sf::Color::White);
            break;
        default: break;
        }
    }
    //else if (evt.type == sf::Event::MouseButtonPressed)
    //{
    //    auto& rw = getContext().renderWindow;
    //    auto pos = rw.mapPixelToCoords(sf::Mouse::getPosition(rw));
    //    Message msg;
    //    msg.type = Message::Type::UI;
    //    msg.ui.mouseX = pos.x;
    //    msg.ui.mouseY = pos.y;
    //    msg.ui.type = Message::UIEvent::MouseClicked;
    //    m_messageBus.send(msg);
    //}
    return true;
}

void GameState::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::Drawable)
    {
        sf::Vector3f lightPos(msg.drawable.lightX, msg.drawable.lightY, 0.f);
        m_orbShader.setUniform("u_lightPosition", lightPos);
        m_orbShader.setUniform("u_lightIntensity", msg.drawable.lightIntensity);
    }

    m_audioManager.handleMessage(msg);
    m_physWorld.handleMessage(msg);
    m_scene.handleMessage(msg);
    m_gameController.handleMessage(msg);
}

//private
Entity::Ptr GameState::createEntity()
{
    const int maxSize = 46;
    float size = static_cast<float>(Util::Random::value(20, maxSize));

    Entity::Ptr e = std::make_unique<Entity>(m_messageBus);
    CircleDrawable::Ptr cd = std::make_unique<CircleDrawable>(size, m_messageBus);
    auto colour = cd->getColour();
    //cd->setShader(m_orbShader);
    //cd->setTexture(getContext().appInstance.getTexture("assets/images/particles/white.png"));
    //cd->setNormalMap(getContext().appInstance.getTexture("assets/images/particles/ball_normal.png"));
    e->addComponent<CircleDrawable>(cd);

    PhysicsComponent::Ptr pc = m_physWorld.addBody(size);
    pc->setName("phys");
    e->addComponent<PhysicsComponent>(pc);

    ParticleSystem::Ptr ps = ParticleSystem::create(Particle::Type::Echo, m_messageBus);
    ps->setTexture(getContext().appInstance.getTexture("assets/images/particles/circle.png"));
    auto particleSize = size * 2.f;
    ps->setParticleSize({ particleSize, particleSize });
    ps->setColour(colour);
    ps->setName("echo");
    e->addComponent<ParticleSystem>(ps);

    auto drawable = std::make_unique<AnimatedDrawable>(m_messageBus, getContext().appInstance.getTexture("assets/images/particles/balls.png"));
    drawable->loadAnimationData("assets/images/particles/balls.cra");
    drawable->setOrigin(sf::Vector2f(drawable->getFrameSize()) / 2.f);
    drawable->setBlendMode(sf::BlendAdd);
    const auto& anims = drawable->getAnimations();
    if (!anims.empty())
    {
        drawable->play(anims[0], Util::Random::value(0, drawable->getFrameCount() - 1));
    }
    float scale = particleSize / static_cast<float>(drawable->getFrameSize().x);
    drawable->setScale(scale, scale);
    drawable->setColour(colour);
    drawable->setShader(m_orbShader);
    drawable->setNormalMap(getContext().appInstance.getTexture("assets/images/particles/ball_normal_animated.png"));
    drawable->setName("drawable");
    e->addComponent<AnimatedDrawable>(drawable);

    auto oc = std::make_unique<OrbController>(m_messageBus);
    e->addComponent<OrbController>(oc);

    return e;
}
