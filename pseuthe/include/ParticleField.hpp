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

//creates a starfield type effect as a drawable component

#ifndef PARTICLE_FIELD_HPP_
#define PARTICLE_FIELD_HPP_

#include <Particle.hpp>
#include <Component.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

#include <vector>

class ParticleField final : public Component, public sf::Drawable
{
public:
    ParticleField(const sf::FloatRect&, MessageBus&);
    ~ParticleField() = default;
    ParticleField(const ParticleField&) = delete;
    const ParticleField& operator = (const ParticleField&) = delete;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;

    void setBlendMode(sf::BlendMode);
    void setTexture(sf::Texture&);
private:

    std::vector<Particle> m_particles;
    sf::BlendMode m_blendMode;
    sf::FloatRect m_bounds;
    sf::VertexArray m_vertices;
    sf::Texture* m_texture;

    void addVertex(const sf::Vector2f&, float, float, const sf::Color&);
    void updateVertices();
    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

#endif //PARTICLE_FIELD_HPP_