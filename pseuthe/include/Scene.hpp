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

//root class containing scene entities

#ifndef SCENE_HPP_
#define SCENE_HPP_

#include <Entity.hpp>
#include <PostBloom.hpp>
#include <PostChromeAb.hpp>

#include <SFML/Graphics/Drawable.hpp>

#include <vector>

class Scene final : public sf::Drawable
{
public:
    enum Layer
    {
        BackRear = 0,
        BackMiddle,
        BackFront,
        FrontRear,
        FrontMiddle,
        FrontFront,
        UI,
        Count
    };

    explicit Scene(MessageBus&);
    ~Scene() = default;
    Scene(const Scene&) = delete;
    const Scene& operator = (const Scene&) = delete;

    void update(float);
    void handleMessage(const Message&);
    void addEntity(Entity::Ptr&, Layer);
    Entity& getLayer(Layer);

    void setView(const sf::View& v);

private:
    std::vector<Entity::Ptr> m_layers;

    int m_collisionCount;
    bool m_countCollisions;
    MessageBus& m_messageBus;

    mutable sf::RenderTexture m_sceneBufferA;
    mutable sf::RenderTexture m_sceneBufferB;
    mutable PostBloom m_bloomEffect;
    mutable PostChromeAb m_chromeAbEffect;

    void draw(sf::RenderTarget&, sf::RenderStates) const override;

};

#endif //SCENE_HPP_