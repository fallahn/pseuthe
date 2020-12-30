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
#include <HelpState.hpp>
#include <App.hpp>
#include <Util.hpp>
#include <AnimatedDrawable.hpp>
#include <TailDrawable.hpp>
#include <ParticleSystem.hpp>
#include <TextDrawable.hpp>

#include <SFML/Window/Event.hpp>

namespace
{
    const sf::Uint32 charSize = 46u;
    const float fadeTime = 0.25f;

    const float planktonSize = 32.f;
}

HelpState::HelpState(StateStack& stack, Context context)
    : State     (stack, context),
    m_mode      (Mode::FadeIn),
    m_messageBus(context.appInstance.getMessageBus()),
    m_physWorld (m_messageBus),
    m_rootNode  (m_messageBus),
    m_ticker    (context.appInstance.getFont("")),
    m_fadeTime  (0.f)
{
    m_menuSprite.setTexture(context.appInstance.getTexture("assets/images/help_menu.png"));
    Util::Position::centreOrigin(m_menuSprite);
    m_menuSprite.setPosition(context.defaultView.getCenter().x, 500.f);

    m_rectangleShape.setSize({ 1920.f, 1080.f });

    addText();

    addPlankton(PlanktonController::Type::Bad);
    addPlankton(PlanktonController::Type::Good);
    addPlankton(PlanktonController::Type::Bonus);
    addPlankton(PlanktonController::Type::UberLife);

    m_ticker.setSize({ 0.f, 0.f, 1930.f, 40.f });
    m_ticker.setPosition(0.f, 950.f);
    m_ticker.addItem("Acknowledgements (in no particular order): ");
    m_ticker.addItem("eXpl0it3r, ");
    m_ticker.addItem("Tank, ");
    m_ticker.addItem("select_this, ");
    m_ticker.addItem("jagoly, ");
    m_ticker.addItem("jonny, ");
    m_ticker.addItem("malone, ");
    m_ticker.addItem("jamz, ");
    m_ticker.addItem("Nim, ");
    m_ticker.addItem("the denizens of #SFML on irc.boxbox.org... ");
    m_ticker.addItem("everyone who donated on itch.io   Thanks! ");
}

bool HelpState::update(float dt)
{
    m_ticker.update(dt);
    
    sf::Color frontColour = sf::Color::White;
    sf::Color backColour = sf::Color::Black;

    m_fadeTime += dt;
    switch (m_mode)
    {
    case Mode::FadeIn:
        if (m_fadeTime < fadeTime)
        {
            frontColour.a = static_cast<sf::Uint8>((m_fadeTime / fadeTime) * 255.f);
        }
        else
        {
            m_mode = Mode::Static;
        }
        break;
    case Mode::FadeOut:
        if (m_fadeTime < fadeTime)
        {
            frontColour.a = static_cast<sf::Uint8>(1.f - ((m_fadeTime / fadeTime) * 255.f));
        }
        else
        {
            Message msg;
            msg.type = Message::Type::UI;
            msg.ui.type = Message::UIEvent::MenuClosed;
            msg.ui.stateId = States::ID::Help;
            m_messageBus.send(msg);
            requestStackPop();
        }
        break;
    case Mode::Static:
        m_physWorld.update(dt);
        m_rootNode.update(dt);
    default: return true;
    }

    backColour.a = frontColour.a / 2;
    m_rectangleShape.setFillColor(backColour);

    m_menuSprite.setColor(frontColour);
    for (auto& t : m_texts)
    {
        t.setFillColor(frontColour);
    }

    auto& children = m_rootNode.getChildren();
    for (auto& c : children)
    {
        c->getComponent<AnimatedDrawable>("drawable")->setColour(frontColour);
        c->getComponent<TextDrawable>("text")->setFillColor(frontColour);

        auto tail = c->getComponent<TailDrawable>("tail");
        if (tail) tail->setColour(frontColour);
    }

    m_ticker.setColour(frontColour);

    return true;
}

bool HelpState::handleEvent(const sf::Event& evt)
{
    switch (evt.type)
    {
    case sf::Event::KeyReleased:
    case sf::Event::MouseButtonReleased:
    case sf::Event::JoystickButtonReleased:
        m_fadeTime = 0.f;
        m_mode = Mode::FadeOut;
        break;
    default: break;
    }

    return false;
}

void HelpState::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::Plankton && 
        msg.plankton.action == Message::PlanktonEvent::Died)
    {
        addPlankton(msg.plankton.type);
    }

    //m_physWorld.handleMessage(msg);
    m_rootNode.handleMessage(msg);
}

void HelpState::draw()
{
    auto& rw = getContext().renderWindow;
    rw.draw(m_rectangleShape);
    rw.draw(m_rootNode);
    rw.draw(m_menuSprite);

    for (const auto& t : m_texts)
        rw.draw(t);

    rw.draw(m_ticker);
}

//private
void HelpState::addText()
{
    auto& font = getContext().appInstance.getFont("assets/fonts/N_E_B.ttf");

    sf::Vector2f centrePos = getContext().defaultView.getCenter();

    m_texts.emplace_back("How To Play", font, 80u);
    auto& titleText = m_texts.back();
    Util::Position::centreOrigin(titleText);
    titleText.setPosition(centrePos);
    titleText.move(0.f, -100.f);

    m_texts.emplace_back("Absorb green plankton by hovering over them", font, charSize);
    auto& text01 = m_texts.back();
    Util::Position::centreOrigin(text01);
    text01.setPosition(centrePos);
    //text01.move(0.f, -10.f);

    m_texts.emplace_back("Beware red plankton which will reduce your health", font, charSize);
    auto& text02 = m_texts.back();
    Util::Position::centreOrigin(text02);
    text02.setPosition(centrePos);
    text02.move(0.f, 70.f);

    m_texts.emplace_back("Look out for special plankton to eat", font, charSize);
    auto& text03 = m_texts.back();
    Util::Position::centreOrigin(text03);
    text03.setPosition(centrePos);
    text03.move(0.f, 140.f);

    m_texts.emplace_back("Use WASD or Arrow keys or a controller.", font, charSize);
    auto& text04 = m_texts.back();
    Util::Position::centreOrigin(text04);
    text04.setPosition(centrePos);
    text04.move(0.f, 210.f);
}

void HelpState::addPlankton(PlanktonController::Type type)
{
    auto entity = std::make_unique<Entity>(m_messageBus);
    entity->setWorldPosition({ static_cast<float>(Util::Random::value(0, 1920)), static_cast<float>(Util::Random::value(0, 1080)) });

    auto physComponent = m_physWorld.addBody(planktonSize);
    physComponent->setPosition(entity->getWorldPosition());
    physComponent->setTriggerOnly(true);
    physComponent->setName("control");
    entity->addComponent<PhysicsComponent>(physComponent);

    auto& appInstance = getContext().appInstance;
    bool colourblind = appInstance.getGameSettings().colourblindMode;
    AnimatedDrawable::Ptr ad;
    auto ident = ParticleSystem::create(Particle::Type::Ident, m_messageBus);
    ident->setTexture(appInstance.getTexture("assets/images/particles/ident.png"));
    auto text = std::make_unique<TextDrawable>(m_messageBus);
    text->setFont(appInstance.getFont("assets/fonts/Ardeco.ttf"));
    switch (type)
    {
    case PlanktonController::Type::Good:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, appInstance.getTexture("assets/images/player/food01.png"));
        ad->loadAnimationData("assets/images/player/food01.cra");
        (colourblind) ? ident->setColour({ 14u, 160u, 225u }) : ident->setColour({ 84u, 150u, 75u });
        text->setString("+50 HP");
        break;
    case PlanktonController::Type::Bad:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, appInstance.getTexture("assets/images/player/food02.png"));
        ad->loadAnimationData("assets/images/player/food02.cra");
        (colourblind) ? ident->setColour({ 214u, 190u, 25u }) : ident->setColour({ 184u, 67u, 51u });
        text->setString("-35 HP");
        break;
    case PlanktonController::Type::Bonus:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, appInstance.getTexture("assets/images/player/food03.png"));
        ad->loadAnimationData("assets/images/player/food03.cra");
        ident->setColour({ 158u, 148u, 224u });
        text->setString("+100 HP");
        break;
    case PlanktonController::Type::UberLife:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, appInstance.getTexture("assets/images/player/bonus.png"));
        ad->loadAnimationData("assets/images/player/bonus.cra");
        ident->setColour({ 158u, 148u, 224u });
        text->setString("FULL HEALTH");
        break;
    default:break;
    }
    ident->setName("ident");
    entity->addComponent<ParticleSystem>(ident);

    text->setCharacterSize(40u);
    Util::Position::centreOrigin(*text);
    text->setPosition(0.f, 40.f);
    text->setName("text");
    entity->addComponent<TextDrawable>(text);

    ad->setBlendMode(sf::BlendAdd);
    ad->setOrigin(sf::Vector2f(ad->getFrameSize()) / 2.f);
    if (!ad->getAnimations().empty()) ad->play(ad->getAnimations()[0]);
    ad->setName("drawable");
    entity->addComponent<AnimatedDrawable>(ad);

    auto trail = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    trail->setTexture(appInstance.getTexture("assets/images/particles/circle.png"));
    float particleSize = planktonSize / 12.f;
    trail->setParticleSize({ particleSize, particleSize });
    trail->setName("trail");
    trail->setEmitRate(10.f);
    entity->addComponent<ParticleSystem>(trail);

    if (type == PlanktonController::Type::UberLife)
    {
        auto tails = std::make_unique<TailDrawable>(m_messageBus);
        tails->addTail(sf::Vector2f(-18.f, -15.f));
        tails->addTail(sf::Vector2f(-8.f, -5.f));
        tails->addTail(sf::Vector2f(-8.f, 5.f));
        tails->addTail(sf::Vector2f(-18.f, 15.f));
        tails->setName("tail");
        entity->addComponent<TailDrawable>(tails);
    }

    auto controller = std::make_unique<PlanktonController>(m_messageBus);
    controller->setType(type);
    controller->setDecayRate(0.f);
    controller->setColourblind(colourblind);
    entity->addComponent<PlanktonController>(controller);

    m_rootNode.addChild(entity);
}
