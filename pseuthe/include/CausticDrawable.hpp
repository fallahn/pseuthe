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

//emulates light rays as caustic effect

#ifndef CAUSTIC_DRAWABLE_HPP_
#define CAUSTIC_DRAWABLE_HPP_

#include <Component.hpp>
#include <ShaderResource.hpp>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/VertexArray.hpp>

class CausticDrawable final : public Component, public sf::Drawable
{
public:
    explicit CausticDrawable(MessageBus&);
    ~CausticDrawable() = default;
    CausticDrawable(const CausticDrawable&) = delete;
    const CausticDrawable& operator = (const CausticDrawable&) = delete;

    Component::Type type() const override;
    void entityUpdate(Entity&, float) override;
    void handleMessage(const Message&) override;

private:

    class Ray : public sf::Transformable
    {
    public:
        Ray();
        ~Ray() = default;
        std::vector<sf::Vertex> vertices;
        void update(float);
    private:
        //wavetable for motion
        std::vector<float> m_wavetable;
        int m_currentIndex;
        float m_amplitude;
    };

    sf::VertexArray m_vertexArray;
    std::vector<Ray> m_rays;
    ShaderResource m_shaders;
    sf::Shader* m_shader;

    void updateVertexArray();
    void draw(sf::RenderTarget&, sf::RenderStates) const override;
};

#endif //CAUSTIC_DRAWABLE_HPP_