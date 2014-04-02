#ifndef BASERANDOM_H
#define BASERANDOM_H

#include <boost/random.hpp>
#include "baseobject.h"

using namespace boost::random;

/**
 * @brief The BaseRandom class is the base class for all the objects needing a
 * RNG.
 *
 * BaseRandom objects can be instantiated as stand-alone objects if they are
 * created without specifying a parent. In this case they will use their own RNG
 * which is initialized with setSeed(). Therefore it is mandatory to call
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
 */

class BaseRandom : public BaseObject
{
public:
    BaseRandom(BaseObject *parent=NULL);
    virtual ~BaseRandom();

    void setSeed(unsigned int seed);

protected:
    mt19937* mt;

private:
    list<BaseRandom *> randomChildList;

    void setRNG(mt19937 *mt);
    virtual void setRNG_impl();
    virtual void setParent_impl(BaseObject *parent);
    virtual void addChild_impl(BaseObject *child);
    virtual void removeChild_impl(BaseObject *child);
    virtual BaseObject *clone_impl() const;
};

#endif // BASERANDOM_H
