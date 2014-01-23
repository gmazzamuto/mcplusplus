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
    setSeed_impl();
}

BaseObject* BaseObject::parent() {
    return parentObject;
}

/**
 * @brief Custom further implementation of setSeed(), specified in derived
 * classes.
 *
 * This function will be called by setSeed() after the RNG has been initialized
 * to allow derived classes to perform further operations related to the setting
 * of the new seed.
 *
 * The default implementation does nothing.
 */

void BaseObject::setSeed_impl() {

}
