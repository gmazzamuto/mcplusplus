#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <boost/random.hpp>
#include <list>

using namespace boost;
using namespace boost::random;
using namespace std;

class BaseObject
{
public:
    BaseObject(BaseObject *parent=NULL);
    virtual ~BaseObject();

    void setSeed(int seed);

    BaseObject *parent();
    void setParent(BaseObject *parent);

protected:
    mt19937* mt;

private:
    bool hasAParent;
    BaseObject *parentObject;
    list<BaseObject *> childList;

    void removeChild(BaseObject *child);
    void deleteAllChildren();
    void addChild(BaseObject *child);
    void setRNG(mt19937 *mt);

    virtual void setRNG_impl();
};

#endif // BASEOBJECT_H
