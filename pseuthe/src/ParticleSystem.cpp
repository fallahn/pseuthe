/*********************************************************************
Matt Marchant 2014 - 2015
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

#include <ParticleSystem.hpp>
#include <Util.hpp>
#include <Entity.hpp>
#include <MessageBus.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace
{

}

ParticleSystem::ParticleSystem(MessageBus& mb, Particle::Type type)
    : Component         (mb),
    m_type              (type),
    m_texture           (nullptr),
    m_colour            (sf::Color::White),
    m_followParent      (false),
    m_particleSize      (4.f, 4.f),
    m_randVelocity      (false),
    m_emitRate          (30.f),
    m_particleLifetime  (2.f),
    m_started           (false),
    m_accumulator       (0.f),
    m_vertices          (sf::Quads),
    m_needsUpdate       (true),
    m_duration          (0.f),
    m_releaseCount      (1u),
    m_blendMode         (sf::BlendAdd),
    m_shader            (nullptr)
{

}

//public
Component::Type ParticleSystem::type() const
{
    return Component::Type::Drawable;
}

void ParticleSystem::entityUpdate(Entity& entity, float dt)
{
    update(dt);
    if(!m_followParent)
        m_position = entity.getWorldPosition();
}

void ParticleSystem::handleMessage(const Message& msg)
{
    switch (msg.type)
    {
    case Message::Type::Physics:
        if ((msg.physics.entityId[0] == getParentUID() || msg.physics.entityId[1] == getParentUID())
            && m_type == Particle::Type::Echo)
        {
            start(1u, 0.02f);
        }
        break;
    default:break;
    }
}

void ParticleSystem::setTexture(const sf::Texture& t)
{
    m_texture = const_cast<sf::Texture*>(&t);
    m_texCoords = sf::Vector2f(m_texture->getSize());
}

void ParticleSystem::setColour(const sf::Color& colour)
{
    m_colour = colour;
}

void ParticleSystem::setBlendMode(sf::BlendMode mode)
{
    m_blendMode = mode;
}

void ParticleSystem::setShader(sf::Shader& shader)
{
    m_shader = &shader;
}

void ParticleSystem::setParticleSize(const sf::Vector2f& size)
{
    m_particleSize = size;
}

void ParticleSystem::setPosition(const sf::Vector2f& position)
{
    m_position = position;
}

void ParticleSystem::move(const sf::Vector2f& amount)
{
    m_position += amount;
}

void ParticleSystem::followParent(bool follow)
{
    m_followParent = follow;
}

void ParticleSystem::setParticleLifetime(float time)
{
    assert(time > 0);
    m_particleLifetime = time;
}

void ParticleSystem::setInitialVelocity(const sf::Vector2f& vel)
{
    m_initialVelocity = vel;
}

void ParticleSystem::setRandomInitialVelocity(const std::vector<sf::Vector2f>& randVelocities)
{
    assert(randVelocities.size());
    m_randVelocities = randVelocities;
    m_randVelocity = true;
}

void ParticleSystem::setEmitRate(float rate)
{
    assert(rate > 0.f);
    m_emitRate = rate;
}

void ParticleSystem::addAffector(Affector& a)
{
    m_affectors.push_back(a);
}

void ParticleSystem::start(sf::Uint8 releaseCount, float duration)
{
    assert(releaseCount > 0);
    assert(duration >= 0.f);
    m_releaseCount = releaseCount;
    m_duration = duration;
    m_durationClock.restart();

    m_started = true;
}

bool ParticleSystem::started() const
{
    return m_started;
}

void ParticleSystem::stop()
{
    m_started = false;
}

void ParticleSystem::update(float dt)
{
    //remove dead particles
    while (!m_particles.empty() && m_particles.front().lifetime <= 0)
    {
        m_particles.pop_front();
    }

    for (auto& p : m_particles)
    {
        p.lifetime -= dt;
        p.move(p.velocity * dt);
        for (auto& a : m_affectors)
        {
            a(p, dt);
        }
    }

    m_needsUpdate = true;

    if (m_started)
    {
        emit(dt);
        if (m_duration > 0)
        {
            if (m_durationClock.getElapsedTime().asSeconds() > m_duration)
            {
                m_started = false;
            }
        }
    }
}

void ParticleSystem::emit(float dt)
{
    const float interval = 1.f / m_emitRate;

    m_accumulator += dt;
    while (m_accumulator > interval)
    {
        m_accumulator -= interval;
        for (auto i = 0u; i < m_releaseCount; ++i)
            addParticle(m_position);
    }
}

sf::Uint32 ParticleSystem::getParticleCount() const
{
    return m_particles.size();
}

//private
void ParticleSystem::addParticle(const sf::Vector2f& position)
{
    Particle p;
    p.setPosition(position);
    p.colour = m_colour;
    p.lifetime = m_particleLifetime;
    p.velocity = (m_randVelocity) ? 
        m_randVelocities[Util::Random::value(0, m_randVelocities.size() - 1)] :
        m_initialVelocity;

    m_particles.push_back(p);
}

void ParticleSystem::addVertex(const sf::Vector2f& position, float u, float v, const sf::Color& colour) const
{
    sf::Vertex vert;
    vert.position = position;
    vert.texCoords = { u, v };
    vert.color = colour;

    m_vertices.append(vert);
}

void ParticleSystem::updateVertices() const
{
    sf::Vector2f halfSize = m_particleSize / 2.f;

    m_vertices.clear();
    for (auto& p : m_particles)
    {
        auto colour = p.colour;

        //make particle fade based on lifetime
        float ratio = p.lifetime / m_particleLifetime;
        colour.a = static_cast<sf::Uint8>(255.f * std::max(ratio, 0.f));

        auto t = p.getTransform();
        addVertex(t.transformPoint(-halfSize.x, -halfSize.y), 0.f, 0.f, colour);
        addVertex(t.transformPoint(halfSize.x, -halfSize.y), m_texCoords.x, 0.f, colour);
        addVertex(t.transformPoint(halfSize), m_texCoords.x, m_texCoords.y, colour);
        addVertex(t.transformPoint(-halfSize.x, halfSize.y), 0.f, m_texCoords.y, colour);
    }
}

void ParticleSystem::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    if (m_needsUpdate)
    {
        updateVertices();
        m_needsUpdate = false;
    }

    if (m_shader)
    {
        m_shader->setParameter("u_diffuseMap", sf::Shader::CurrentTexture);
    }

    states.texture = m_texture;
    states.shader = m_shader;
    states.blendMode = m_blendMode;
    if (!m_followParent)states.transform = sf::Transform::Identity;
    rt.draw(m_vertices, states);
}