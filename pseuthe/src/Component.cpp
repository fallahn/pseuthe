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

#include <Component.hpp>
#include <MessageBus.hpp>

Component::Component(MessageBus& m)
    : m_messageBus  (m),
    m_destroyed     (false)
{

}

//public
void Component::destroy()
{
    m_destroyed = true;
}

bool Component::destroyed() const
{
    return m_destroyed;
}

//protected

void Component::sendMessage(const Message& m)
{
    m_messageBus.send(m);
}