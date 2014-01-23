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
    void setParent(BaseObject *parent);

protected:
    mt19937* mt;

private:
    bool hasAParent;
    BaseObject *parentObject;

    virtual void setSeed_impl();
};

#endif // BASEOBJECT_H
