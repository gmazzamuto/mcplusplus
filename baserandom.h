#ifndef BASERANDOM_H
#define BASERANDOM_H

#include <boost/random.hpp>
#include "baseobject.h"

using namespace boost::random;

class BaseRandom : public BaseObject
{
public:
    BaseRandom(BaseObject *parent=NULL);
    virtual ~BaseRandom();

    void setSeed(int seed);

protected:
    mt19937* mt;

private:
    list<BaseRandom *> randomChildList;

    void setRNG(mt19937 *mt);
    virtual void setRNG_impl();
    virtual void setParent_impl(BaseObject *parent);
    virtual void addChild_impl(BaseObject *child);
    virtual void removeChild_impl(BaseObject *child);
};

#endif // BASERANDOM_H
