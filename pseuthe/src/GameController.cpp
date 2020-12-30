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
#include <GameController.hpp>
#include <Scene.hpp>
#include <MessageBus.hpp>
#include <App.hpp>
#include <PhysicsWorld.hpp>
#include <Entity.hpp>
#include <ParticleSystem.hpp>
#include <Util.hpp>
#include <InputComponent.hpp>
#include <BodypartController.hpp>
#include <AnimatedDrawable.hpp>
#include <TailDrawable.hpp>
#include <PlanktonController.hpp>
#include <TextDrawable.hpp>

#include <memory>

namespace
{
    const float partScale = 0.87f;
    const float partPadding = 1.f;
    const float partSize = 32.f;
    const float playerSize = 32.f;
    const sf::Vector2f spawnPosition(960.f, 540.f);
    const float wigglerOffset = 24.f;

    const float planktonSize = 32.f;
    int goodPlankton = 0;
    int badPlankton = 0;

    const int maxBodyParts = 6;
    const int maxPlankton = 8;
    const float maxBodyHealth = 100.f;

    const sf::Uint8 easyPartCount = 3u;
    const sf::Uint8 hardPartCount = 2u;

    const float easyDecayTime = 4.f;
    const float mediumDecayTime = 4.8f;
    const float hardDecayTime = 5.5f;

    const float easySpeed = 1.f;
    const float mediumSpeed = 1.3f;
    const float hardSpeed = 1.65f;

    sf::Clock spawnClock;
    const float easySpawnTime = 4.f;
    const float mediumSpawnTime = 3.f;
    const float hardSpawnTime = 2.f;
    const float spawnTimeIncrease = 0.2f;

    float score = 0.f;
    float highScore = 0.f;

    const char namechars[] = "0123456789ABCDEF";
}

GameController::GameController(Scene& scene, MessageBus& mb, App& app, PhysicsWorld& pw)
    : m_scene               (scene),
    m_messageBus            (mb),
    m_appInstance           (app),
    m_physicsWorld          (pw),
    m_player                (nullptr),
    m_constraintLength      (0.f),
    m_nextPartSize          (0.f),
    m_nextPartScale         (0.f),
    m_planktonCount         (0u),
    m_spawnTime             (easySpawnTime),
    m_initialPartCount      (easyPartCount),
    m_partDecayRate         (easyDecayTime),
    m_speedMultiplier       (easySpeed),
    m_controlType           (ControlType::Classic),
    m_difficulty            (Difficulty::Easy),
    m_scoreText             (nullptr),
    m_highScoreText         (nullptr),
    m_initialsText          (nullptr),
    m_paused                (false)
{
    //find two off screen areas for spawning teh planktons
    auto& worldBounds = m_physicsWorld.getWorldSize();
    m_planktonSpawns[0].left = worldBounds.left;
    m_planktonSpawns[0].width = std::abs(worldBounds.left);
    m_planktonSpawns[0].height = worldBounds.height;

    m_planktonSpawns[1].left = 1920.f;
    m_planktonSpawns[1].width = (worldBounds.width - 1920.f) / 2.f;
    m_planktonSpawns[1].height = worldBounds.height;

    
    auto scrText = std::make_unique<TextDrawable>(m_messageBus);
    scrText->setFont(app.getFont("assets/fonts/Ardeco.ttf"));
    scrText->setCharacterSize(44u);
    scrText->setPosition(20.f, 1000.f);
    scrText->setFillColor(sf::Color::Transparent);
    m_scoreText = scrText.get();
    m_scene.getLayer(Scene::Layer::UI).addComponent<TextDrawable>(scrText);

    scrText = std::make_unique<TextDrawable>(m_messageBus);
    scrText->setFont(app.getFont("assets/fonts/Ardeco.ttf"));
    scrText->setCharacterSize(44u);
    scrText->setPosition(20.f, 950.f);
    scrText->setFillColor(sf::Color::Transparent);
    m_highScoreText = scrText.get();
    m_scene.getLayer(Scene::Layer::UI).addComponent<TextDrawable>(scrText);

    scrText = std::make_unique<TextDrawable>(m_messageBus);
    scrText->setFont(app.getFont("assets/fonts/Ardeco.ttf"));
    scrText->setCharacterSize(44u);
    scrText->setPosition(1540.f, 1000.f);
    scrText->setFillColor(sf::Color::Transparent);
    m_initialsText = scrText.get();
    m_scene.getLayer(Scene::Layer::UI).addComponent<TextDrawable>(scrText);
}

//public
void GameController::update(float dt)
{
    if (spawnClock.getElapsedTime().asSeconds() > (m_spawnTime * m_planktonCount))
    {
        spawnClock.restart();
        if (m_planktonCount < maxPlankton && m_player)
        {
            if (!m_paused)
            {
                spawnPlankton();
                m_spawnTime += spawnTimeIncrease;
            }
        }
    }

    if(!m_paused) score += dt * m_playerPhysicsComponents.size();
    
    if (score > highScore)
    {
        m_highScoreText->setString("Longest Run: " + std::to_string(score));
    }
    m_scoreText->setString("Current Run: " + std::to_string(score));
}

void GameController::handleMessage(const Message& msg)
{
    if (msg.type == Message::Type::UI)
    {
        switch (msg.ui.type)
        {
        case Message::UIEvent::MenuClosed:
            if (msg.ui.stateId == States::ID::Menu)
            {
                m_scoreText->setFillColor(sf::Color::White);
                m_highScoreText->setFillColor(sf::Color::White);
                m_initialsText->setFillColor(sf::Color::White);
                m_initialsText->setString("Participant Initials: " + std::string(&m_appInstance.getGameSettings().playerInitials[0]));

                m_scene.getLayer(Scene::Layer::UI).getComponent<TextDrawable>("paused_text")->setFillColor(sf::Color::Transparent);

                if (!m_player)
                {
                    spawnPlayer();
                    for (auto i = 0u; i < m_initialPartCount; ++i)
                    {
                        addBodyPart();
                    }
                }

                m_paused = false;
            }
            break;
        case Message::UIEvent::MenuOpened:
            if (msg.ui.stateId == States::ID::Score)
            {
                m_scoreText->setFillColor(sf::Color::Transparent);
                m_highScoreText->setFillColor(sf::Color::Transparent);
                m_initialsText->setFillColor(sf::Color::Transparent);
            }
            else if (msg.ui.stateId == States::ID::Menu)
            {
                m_paused = true;
            }
            break;
        case Message::UIEvent::RequestDifficultyChange:
            m_difficulty = msg.ui.difficulty;
            break;
        case ::Message::UIEvent::RequestControlsArcade:
            m_controlType = ControlType::Arcade;
            if (m_player)
            {
                m_player->getComponent<InputComponent>("controller")->setControlType(ControlType::Arcade);
            }
            break;
        case ::Message::UIEvent::RequestControlsClassic:
            m_controlType = ControlType::Classic;
            if (m_player)
            {
                m_player->getComponent<InputComponent>("controller")->setControlType(ControlType::Classic);
            }
            break;
        case Message::UIEvent::RequestControllerEnable:
            if (m_player)
            {
                m_player->getComponent<InputComponent>("controller")->setControllerEnabled(true);
            }
            break;
        case Message::UIEvent::RequestControllerDisable:
            if (m_player)
            {
                m_player->getComponent<InputComponent>("controller")->setControllerEnabled(false);
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
            m_appInstance.addScore(getName(), score);
            {
                Message newMessage;
                newMessage.type = Message::Type::UI;
                newMessage.ui.type = Message::UIEvent::RequestState;
                newMessage.ui.stateId = States::ID::Score;
                m_messageBus.send(newMessage);
            }
            break;
        case Message::PlayerEvent::Spawned:
            //difficulty setting not applied until player spawned
            //so we need to get high score here
        {
            const auto& scores = m_appInstance.getScores();
            if (!scores.empty()) highScore = scores[0].score;
            else highScore = 0.f;

            m_highScoreText->setString("Longest Run: " + std::to_string(highScore));
        }
            break;
        case Message::PlayerEvent::PartRemoved:
            m_playerPhysicsComponents.pop_back();
            m_nextPartSize /= partScale;
            m_nextPartScale /= partScale;
            m_constraintLength /= partScale;
            break;
        case Message::PlayerEvent::HealthAdded:
            if (msg.player.value > 0)
            {
                int partCount = static_cast<int>(std::floor(msg.player.value / maxBodyHealth));
                float healthRemainder = msg.player.value - (partCount * maxBodyHealth);

                while (partCount-- && m_playerPhysicsComponents.size() < maxBodyParts)
                {
                    addBodyPart(maxBodyHealth);
                }
                if (m_playerPhysicsComponents.size() < maxBodyParts)
                {
                    addBodyPart(healthRemainder);
                }
            }
            break;
        default: break;
        }
    }
    else if (msg.type == Message::Type::Plankton)
    {
        switch (msg.plankton.action)
        {
        case Message::PlanktonEvent::Died:
            m_planktonCount--;
            if (msg.plankton.type == PlanktonController::Type::Bad)
            {
                badPlankton--;
            }
            else if (msg.plankton.type == PlanktonController::Type::Good)
            {
                goodPlankton--;
            }
            break;
        case Message::PlanktonEvent::Spawned:
            m_planktonCount++;
            break;
        default: break;
        }
    }
}

void GameController::setDifficulty(Difficulty difficulty)
{
    switch (difficulty)
    {
    case Difficulty::Easy:
        m_spawnTime = easySpawnTime;
        m_initialPartCount = easyPartCount;
        m_partDecayRate = easyDecayTime;
        m_speedMultiplier = easySpeed;
        break;
    case Difficulty::Medium:
        m_spawnTime = mediumSpawnTime;
        m_initialPartCount = hardPartCount;
        m_partDecayRate = mediumDecayTime;
        m_speedMultiplier = mediumSpeed;
        break;
    case Difficulty::Hard:
        m_spawnTime = hardSpawnTime;
        m_initialPartCount = hardPartCount;
        m_partDecayRate = hardDecayTime;
        m_speedMultiplier = hardSpeed;
        break;
    }
}

void GameController::spawnPlayer()
{
    auto entity = std::make_unique<Entity>(m_messageBus);
    entity->setWorldPosition(spawnPosition);

    auto cd = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/head.png"));
    cd->loadAnimationData("assets/images/player/head.cra");
    cd->setOrigin(sf::Vector2f(cd->getFrameSize()) / 2.f);
    cd->setBlendMode(sf::BlendAdd);
    const auto& anims = cd->getAnimations();
    if(!anims.empty()) cd->play(cd->getAnimations()[0]);
    cd->setName("head");
    entity->addComponent<AnimatedDrawable>(cd);

    cd = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/mouth.png"));
    cd->loadAnimationData("assets/images/player/mouth.cra");
    cd->setOrigin(sf::Vector2f(cd->getFrameSize()) / 2.f);
    cd->setBlendMode(sf::BlendAdd);
    cd->setName("mouth");
    entity->addComponent<AnimatedDrawable>(cd);

    cd = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/wiggler.png"));
    cd->loadAnimationData("assets/images/player/wiggler.cra");
    cd->setOrigin(cd->getFrameSize().x + wigglerOffset, cd->getFrameSize().y / 2.f);
    cd->setBlendMode(sf::BlendAdd);
    cd->setName("wigglerA");
    if (!cd->getAnimations().empty()) cd->play(cd->getAnimations()[0]);
    entity->addComponent<AnimatedDrawable>(cd);

    cd = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/wiggler.png"));
    cd->loadAnimationData("assets/images/player/wiggler.cra");
    cd->setOrigin(cd->getFrameSize().x + wigglerOffset, cd->getFrameSize().y / 2.f);
    cd->setBlendMode(sf::BlendAdd);
    cd->setName("wigglerB");
    if (!cd->getAnimations().empty()) cd->play(cd->getAnimations()[0]);
    entity->addComponent<AnimatedDrawable>(cd);

    auto physComponent = m_physicsWorld.addBody(playerSize);
    physComponent->setName("control");
    physComponent->setPosition(entity->getWorldPosition());
    physComponent->setVelocity({1.f, 0.f});
    m_playerPhysicsComponents.push_back(physComponent.get());
    entity->addComponent<PhysicsComponent>(physComponent);

    auto trailComponent = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    trailComponent->setTexture(m_appInstance.getTexture("assets/images/particles/circle.png"));
    trailComponent->setParticleSize({ 2.f, 2.f });
    trailComponent->setName("trail");
    entity->addComponent<ParticleSystem>(trailComponent);

    auto sparkle = ParticleSystem::create(Particle::Type::Sparkle, m_messageBus);
    sparkle->setTexture(m_appInstance.getTexture("assets/images/particles/spark.png"));
    sparkle->setName("sparkle");
    sparkle->start(1u, 0.f, 0.5f);
    entity->addComponent<ParticleSystem>(sparkle);

    auto echo = ParticleSystem::create(Particle::Type::Echo, m_messageBus);
    echo->setTexture(m_appInstance.getTexture("assets/images/particles/circle.png"));
    echo->setParticleSize({ playerSize, playerSize });
    echo->setColour({ 240u, 35u, 30u });
    echo->setName("echo");
    entity->addComponent<ParticleSystem>(echo);

    auto tail = std::make_unique<TailDrawable>(m_messageBus);
    tail->addTail({ -15.f, -15.f });
    tail->addTail({ -15.f, 15.f });
    tail->setName("tail");
    entity->addComponent(tail);

    auto controlComponent = std::make_unique<InputComponent>(m_messageBus);
    controlComponent->setName("controller");
    controlComponent->setControlType(m_controlType);
    controlComponent->setControllerEnabled(m_appInstance.getGameSettings().controllerEnabled);
    entity->addComponent<InputComponent>(controlComponent);

    m_constraintLength = playerSize + partSize + partPadding;
    m_nextPartSize = partSize;
    m_nextPartScale = partScale;

    m_player = entity.get();
    m_scene.getLayer(Scene::Layer::FrontMiddle).addChild(entity);

    resetScore();
    setDifficulty(m_difficulty);

    Message msg;
    msg.type = Message::Type::Player;
    msg.player.action = Message::PlayerEvent::Spawned;
    msg.player.value = 0.f;
    m_messageBus.send(msg);
}

void GameController::addBodyPart(float health)
{
    auto bodyPart = std::make_unique<Entity>(m_messageBus);
    auto position = m_playerPhysicsComponents.back()->getPosition();
    position -= Util::Vector::normalise(m_playerPhysicsComponents.back()->getVelocity()) * m_constraintLength;
    //bodyPart->setWorldPosition(position);

    auto physComponent = m_physicsWorld.attachBody(m_nextPartSize, m_constraintLength, m_playerPhysicsComponents.back());
    physComponent->setPosition(position);
    physComponent->setVelocity(m_playerPhysicsComponents.back()->getVelocity());
    physComponent->setName("control");
    m_playerPhysicsComponents.push_back(physComponent.get());
    bodyPart->addComponent<PhysicsComponent>(physComponent);

    auto drawable = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/bodypart01.png"));
    drawable->loadAnimationData("assets/images/player/bodypart01.cra");
    drawable->setOrigin(sf::Vector2f(drawable->getFrameSize()) / 2.f);
    drawable->setBlendMode(sf::BlendAdd);
    const auto& anims = drawable->getAnimations();
    if (!anims.empty())
    {
        drawable->play(anims[0], static_cast<std::int16_t>(drawable->getFrameCount() / maxBodyParts * m_playerPhysicsComponents.size()));
    }
    drawable->setScale(m_nextPartScale, m_nextPartScale);
    drawable->setName("drawable");
    bodyPart->addComponent<AnimatedDrawable>(drawable);

    auto sparkle = ParticleSystem::create(Particle::Type::Sparkle, m_messageBus);
    sparkle->setTexture(m_appInstance.getTexture("assets/images/particles/spark.png"));
    sparkle->setName("sparkle");
    sparkle->start(6u, 0.f, 0.6f);
    bodyPart->addComponent<ParticleSystem>(sparkle);

    auto echo = ParticleSystem::create(Particle::Type::Echo, m_messageBus);
    echo->setTexture(m_appInstance.getTexture("assets/images/particles/circle.png"));
    float size = m_nextPartSize;
    echo->setParticleSize({ size, size });
    echo->setColour({ 240u, 35u, 30u });
    echo->setName("echo");
    bodyPart->addComponent<ParticleSystem>(echo);

    auto bpCont = std::make_unique<BodypartController>(m_messageBus);
    bpCont->setHealth(health);
    bpCont->setDecayRate(m_partDecayRate);
    bodyPart->addComponent<BodypartController>(bpCont);

    m_player->addChild(bodyPart);

    m_constraintLength = m_nextPartSize + (m_nextPartSize * m_nextPartScale);
    m_nextPartSize *= partScale;
    m_nextPartScale *= partScale;    

    Message msg;
    msg.type = Message::Type::Player;
    msg.player.action = Message::PlayerEvent::PartAdded;
    msg.player.value = m_playerPhysicsComponents.back()->getMass();
    m_messageBus.send(msg);
}

void GameController::spawnPlankton()
{
    auto& spawnArea = m_planktonSpawns[Util::Random::value(0, static_cast<std::int32_t>(m_planktonSpawns.size() - 1))];
    const float posX = spawnArea.left + (spawnArea.width / 2.f);
    const float posY = static_cast<float>(Util::Random::value(0, 1080));

    float scale = Util::Random::value(0.7f, 1.1f);
    float size = planktonSize * scale;

    auto entity = std::make_unique<Entity>(m_messageBus);
    entity->setWorldPosition({ posX, posY });

    auto physComponent = m_physicsWorld.addBody(size);
    physComponent->setPosition(entity->getWorldPosition());
    physComponent->setTriggerOnly(true);
    physComponent->setName("control");
    physComponent->setVelocity(physComponent->getVelocity() * m_speedMultiplier);
    entity->addComponent<PhysicsComponent>(physComponent);


    //1 in 5 chance a bonus plankton, else other type
    PlanktonController::Type type = (Util::Random::value(0, 4) == 0) ?
        PlanktonController::Type::Bonus :
        (Util::Random::value(0, 3) == 0) ? //1 in 4 chance it'll be most populous
            (badPlankton >= goodPlankton) ? PlanktonController::Type::Bad : PlanktonController::Type::Good :
            (badPlankton < goodPlankton) ? PlanktonController::Type::Bad : PlanktonController::Type::Good;

    //----test code----//
    if (Util::Random::value(0, 22) == 0 
        && (m_playerPhysicsComponents.size() <= (maxBodyParts / 2)))
        type = PlanktonController::Type::UberLife;
    //-----------------//

    AnimatedDrawable::Ptr ad;
    auto ident = ParticleSystem::create(Particle::Type::Ident, m_messageBus);
    ident->setTexture(m_appInstance.getTexture("assets/images/particles/ident.png"));
    bool colourblind = m_appInstance.getGameSettings().colourblindMode;
    switch (type)
    {
    case PlanktonController::Type::Good:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/food01.png"));
        ad->loadAnimationData("assets/images/player/food01.cra");
        (colourblind) ? ident->setColour({ 14u, 160u, 225u }) : ident->setColour({ 84u, 150u, 75u }); //no majick pl0x!
        goodPlankton++;
        break;
    case PlanktonController::Type::Bad:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/food02.png"));
        ad->loadAnimationData("assets/images/player/food02.cra");
        (colourblind) ? ident->setColour({ 214u, 190u, 25u }) : ident->setColour({ 184u, 67u, 51u });
        badPlankton++;
        break;
    case PlanktonController::Type::Bonus:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/food03.png"));
        ad->loadAnimationData("assets/images/player/food03.cra");
        ident->setColour({ 158u, 148u, 224u });
        break;
    case PlanktonController::Type::UberLife:
        ad = std::make_unique<AnimatedDrawable>(m_messageBus, m_appInstance.getTexture("assets/images/player/bonus.png"));
        ad->loadAnimationData("assets/images/player/bonus.cra");
        ident->setColour({ 158u, 148u, 224u });
        break;
    default:break;
    }
    ident->setName("ident");
    entity->addComponent<ParticleSystem>(ident);

    ad->setBlendMode(sf::BlendAdd);
    ad->setOrigin(sf::Vector2f(ad->getFrameSize()) / 2.f);
    if(!ad->getAnimations().empty()) ad->play(ad->getAnimations()[0]);
    ad->setName("drawable");
    ad->setScale(scale, scale);
    entity->addComponent<AnimatedDrawable>(ad);

    auto trail = ParticleSystem::create(Particle::Type::Trail, m_messageBus);
    trail->setTexture(m_appInstance.getTexture("assets/images/particles/circle.png"));
    float particleSize = m_nextPartSize / 12.f;
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
        tails->setScale(scale, scale);
        tails->setName("tail");
        entity->addComponent<TailDrawable>(tails);
    }

    auto controller = std::make_unique<PlanktonController>(m_messageBus);
    assert(m_player);
    controller->setEnemyId(m_player->getUID());
    controller->setType(type);
    controller->setColourblind(colourblind);
    entity->addComponent<PlanktonController>(controller);

    //send a spawn message
    Message msg;
    msg.type = Message::Type::Plankton;
    msg.plankton.action = Message::PlanktonEvent::Spawned;
    msg.plankton.type = type;
    m_messageBus.send(msg);

    m_scene.getLayer(Scene::Layer::FrontRear).addChild(entity);
}

void GameController::resetScore()
{
    score = 0.f;
}

std::string GameController::getName() const
{
    std::string name("H");
    name += std::to_string(Util::Random::value(100, 999));
    name += (m_controlType == ControlType::Arcade) ? "A" : "C";
    name += std::to_string(Util::Random::value(19, 89));
    name += "-";
    name += std::string(&m_appInstance.getGameSettings().playerInitials[0]);
    name += "-0x";

    for (auto i = 0; i < 6; ++i)
    {
        name += namechars[Util::Random::value(0, 15)];
    }

    assert(name.size() == 20);
    return name;
}
