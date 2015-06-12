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

//convenience function for creating particle system presets

#include <Particles.hpp>
#include <Util.hpp>

ParticleSystem::Ptr ParticleSystem::create(Particle::Type type, MessageBus& mb)
{
    auto ps = std::make_unique<ParticleSystem>(mb, type);

    switch (type)
    {
    case Particle::Type::Trail:
    {
        ps->setEmitRate(Util::Random::value(1.f, 2.f));

        ForceAffector fa({ 10.f, -170.f });
        ps->addAffector<ForceAffector>(fa);

        const float scale = Util::Random::value(4.f, 6.f);
        ScaleAffector sa({ scale, scale });
        ps->addAffector<ScaleAffector>(sa);

        ps->start();
    }
        break;

    case Particle::Type::Echo:
    {
        ScaleAffector sa({ 1.6f, 1.6f });
        ps->addAffector<ScaleAffector>(sa);

        ps->setParticleLifetime(0.95f);
        ps->followParent(true);
    }
        break;
    default: break;
    }


    return std::move(ps);
}