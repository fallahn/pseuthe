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
#include <Log.hpp>

MessageBus::MessageBus()
: m_polling(false){}

Message MessageBus::poll()
{
    m_polling = true;
    Message m = m_messages.front();
    m_messages.pop();

    return m;
}

void MessageBus::send(const Message& m)
{
    (m_polling) ? m_deferredMessages.push(m) : m_messages.push(m);
}

bool MessageBus::empty()
{
    auto empty = m_messages.empty();
    if (empty)
    {
        m_deferredMessages.swap(m_messages);
        m_polling = false;
    }

    return empty;
}