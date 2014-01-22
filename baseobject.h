#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <boost/random.hpp>

using namespace boost;
using namespace boost::random;

class BaseObject
{
public:
    BaseObject(BaseObject *parent=NULL);
    virtual ~BaseObject();

    mt19937* internalRNG();
    bool isUsingInternalRNG();
    void setRNG(mt19937 *mt);
    void setSeed(int seed);
    BaseObject *parent();

protected:
    mt19937* mt;

private:
    bool usingInternalMt; /**< \brief true if using its own internal RNG*/
    BaseObject *parentObject;
};

#endif // BASEOBJECT_H
