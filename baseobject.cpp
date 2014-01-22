#include "baseobject.h"

BaseObject::BaseObject(BaseObject *parent)
{
    if(parent==NULL) {
        usingInternalMt=true;
    }
    else {
        usingInternalMt = false;
        mt = parent->internalRNG();
    }
    parentObject = parent;
}

BaseObject::~BaseObject() {
    if(usingInternalMt && mt!=NULL)
        delete mt;
}

bool BaseObject::isUsingInternalRNG() {
    return usingInternalMt;
}

mt19937 *BaseObject::internalRNG() {
    return mt;
}

void BaseObject::setRNG(mt19937 *mt) {
    this->mt = mt;
    usingInternalMt = false;
}

/**
 * @brief Sets a new seed for the RNG
 * @param seed
 *
 *
 * This function has no effect if using an external RNG.
 */

void BaseObject::setSeed(int seed) {
    if(!usingInternalMt)
        return;
    if(mt != NULL)
        delete mt;
    mt = new mt19937(seed);
}

BaseObject* BaseObject::parent() {
    return parentObject;
}
