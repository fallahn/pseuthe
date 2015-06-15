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

#include <CausticDrawable.hpp>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderStates.hpp>
#include <SFML/Graphics/Texture.hpp>

namespace
{
    const sf::Vertex emptyVertex(sf::Vector2f(), sf::Color::Transparent);
    const sf::Color rayColour(255u, 250u, 190u, 60u);
}

CausticDrawable::CausticDrawable(MessageBus& mb)
    : Component     (mb),
    m_vertexArray   (sf::PrimitiveType::TrianglesStrip),
    m_texture       (nullptr)
{
    m_rays.emplace_back();
}

//public
Component::Type CausticDrawable::type() const
{
    return Component::Type::Drawable;
}

void CausticDrawable::entityUpdate(Entity&, float dt)
{
    for (auto& ray : m_rays)
        ray.update(dt);

    updateVertexArray();
}

void CausticDrawable::handleMessage(const Message&)
{}

void CausticDrawable::setTexture(sf::Texture& t)
{
    m_texture = &t;
    sf::Vector2f texCoords(t.getSize());

    for (auto& ray : m_rays)
    {
        //set tex coords (because they aren't normalised in sfml)
        ray.vertices[1].texCoords = { texCoords.x, 0.f };
        ray.vertices[2].texCoords = texCoords;
        ray.vertices[3].texCoords = { 0.f, texCoords.y };
    }
}

//private
void CausticDrawable::updateVertexArray()
{
    m_vertexArray.clear();

    for (const auto& ray : m_rays)
    {
        auto transform = ray.getTransform();
        for (const auto& vertex : ray.vertices)
        {
            auto newVert = vertex;
            newVert.position = transform.transformPoint(newVert.position);
            m_vertexArray.append(newVert);
        }
        //provide spacing between rays (should this be 4 ?)
        //m_vertexArray.append(emptyVertex);
        //m_vertexArray.append(emptyVertex);
    }
}

void CausticDrawable::draw(sf::RenderTarget& rt, sf::RenderStates states) const
{
    states.texture = m_texture;
    //states.blendMode = sf::BlendAdd;
    rt.draw(m_vertexArray, states);
}


////////////ray class////////////
CausticDrawable::Ray::Ray()
{
    //TODO create wavetables with random amp / freq to animate bottom vertices
    //TODO better way to define these dimensions than plucking them out of the air
    
    vertices.emplace_back(sf::Vector2f(-450.f, 1000.f), sf::Color::White);
    vertices.emplace_back(sf::Vector2f(-250.f, 0.f), sf::Color::White);
    vertices.emplace_back(sf::Vector2f(0.f, 1000.f), sf::Color::Red);
    vertices.emplace_back(sf::Vector2f(), sf::Color::Red);
    vertices.emplace_back(sf::Vector2f(450.f, 1000.f), sf::Color::White);
    vertices.emplace_back(sf::Vector2f(250.f, 0.f), sf::Color::White);
}

void CausticDrawable::Ray::update(float dt)
{
    //TODO animate vertices 0 and 4 based on wave tables
}