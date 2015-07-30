/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef BASERANDOM_H
#define BASERANDOM_H

#include <boost/random.hpp>
#include "baseobject.h"

using namespace boost;
using namespace boost::random;

namespace MCPP {

#ifdef DOUBLEPRECISION
    typedef mt19937 MCEngine;
#endif

#ifdef LONGDOUBLEPRECISION
    typedef mt19937_64 MCEngine;
#endif

#ifdef SINGLEPRECISION
    typedef mt19937 MCEngine;
#endif

/**
 * @brief The BaseRandom class is the base class for all the objects needing a
 * RNG.
 *
 * BaseRandom objects can be instantiated as stand-alone objects if they are
 * created without specifying a parent. In this case they will use their own
 * RNG which is initialized with setSeed(). Therefore it is mandatory to call
 * setSeed() for stand-alone objects before any call to functions needing RNG.
 *
 * If a parent / child relationship involving BaseRandom objects exists, then
 * the following holds:
 * - if the parent is of type BaseRandom, the object will \em share the parent's
 * RNG. Also, all child objects of type BaseRandom will \em share
 * that same RNG.
 * - if the object has no parent or has a parent which is not a BaseRandom,
 * calling setSeed() will construct a RNG that will be \em shared among all
 * child objects of type BaseRandom;
 * - if the object has a parent of type BaseRandom, calling setSeed()
 * does nothing. In this case setSeed() has to be called in the parent.
 *
 * To sum up: parent / child relationships can be used to create objects which
 * \em share the same instance of the RNG; this instance is created by calling
 * setSeed() on the parent object. Again, calling setSeed() is mandatory
 * to initialize the RNG, but it needs to be called only once (in the topmost
 * BaseRandom parent).
 *
 * The methods loadGeneratorState() and setGeneratorState() allow to specify a
 * custom internal state of the RNG and can be used in place of setSeed(), with
 * everything described above still holding.
 */

class BaseRandom : public BaseObject
{
public:
    BaseRandom(BaseObject *parent=NULL);
    virtual ~BaseRandom();

    void setSeed(unsigned int seed);
    unsigned int currentSeed() const;

    void dumpGeneratorState(const char *fileName) const;
    void loadGeneratorState(const char *fileName);
    string generatorState() const;
    void setGeneratorState(string state);

protected:
    MCEngine* mt;
    unsigned int _currentSeed;

private:
    void setRNG(MCEngine *mt);

    virtual void setRNG_impl();
    virtual void setParent_impl(BaseObject *parent);
    virtual void addChild_impl(BaseObject *child);
    virtual void removeChild_impl(BaseObject *child);
    virtual BaseObject *clone_impl() const;
    virtual bool sanityCheck_impl() const;

    list<BaseRandom *> randomChildList;
};

}
#endif // BASERANDOM_H
