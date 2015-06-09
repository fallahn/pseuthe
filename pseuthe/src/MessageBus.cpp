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

#include <MessageBus.hpp>

MessageBus::MessageBus(){}

bool MessageBus::poll(Message& m)
{
    m = m_messages.back();
    m_messages.pop_back();
    return !m_messages.empty();
}

void MessageBus::send(const Message& m)
{
    //TODO we ought to prevent this from being called
    //from within a message handler, as it has potential
    //for inifite loops??
    m_messages.push_front(m);
}