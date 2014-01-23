#include "baseobject.h"

BaseObject::BaseObject(BaseObject *parent)
{
    setParent(parent);
}

BaseObject::~BaseObject() {
    if(!hasAParent) {
        deleteAllChildren();
        if(mt!=NULL)
            delete mt;
    }
    else
        parentObject->removeChild(this);
}

void BaseObject::setRNG(mt19937 *mt) {
    if(mt == NULL)
        return;
    this->mt = mt;
    setRNG_impl();
    std::list<BaseObject *>::const_iterator iterator;
    for (iterator = childList.begin(); iterator != childList.end(); ++iterator) {
        BaseObject *child = *iterator;
        child->setRNG(mt);
    }
}

void BaseObject::addChild(BaseObject *child) {
    childList.push_back(child);
    child->setRNG(mt);
}

void BaseObject::removeChild(BaseObject *child) {
    childList.remove(child);
}

void BaseObject::deleteAllChildren() {
    BaseObject *child;
    while(!childList.empty()) {
        child = childList.back();
        childList.remove(child);
        delete child;
    }
}

/**
 * @brief Sets a new seed for the RNG
 * @param seed
 *
 *
 * This function has no effect if using an external RNG.
 */

void BaseObject::setSeed(int seed) {
    if(hasAParent)
        return;
    if(mt != NULL)
        delete mt;
    setRNG(new mt19937(seed));
}

BaseObject* BaseObject::parent() {
    return parentObject;
}

void BaseObject::setParent(BaseObject *parent) {
    parentObject = parent;
    if(parent == NULL) {
        hasAParent = false;
        mt = NULL;
    }
    else {
        hasAParent = true;
        parent->addChild(this);
    }
}

/**
 * @brief Custom further implementation of setRNG(), specified in derived
 * classes.
 *
 * This function will be called by setRNG() after a new RNG has been set to
 * allow derived classes to perform further operations related to the setting
 * of the new RNG.
 *
 * The default implementation does nothing.
 */

void BaseObject::setRNG_impl() {

}
