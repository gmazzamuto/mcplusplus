#include "baserandom.h"

BaseRandom::BaseRandom(BaseObject *parent) :
    BaseObject(NULL), mt(NULL)
{
    _inheritsRandom = true;

    // we need to call the BaseObject constructor with NULL parent, and then
    // call setParent() here because we want setParent_impl() to be called (but
    // this function doesn't exist until BaseObject and then BaseRandom are
    // fully constructed)
    setParent(parent);
}

BaseRandom::~BaseRandom() {
    if(!hasAParent() || !parent()->inheritsRandom())
        if(mt!=NULL)
            delete mt;
}

/**
 * @brief Sets a new seed for the RNG
 * @param seed
 *
 *
 * This function has no effect if the object has a parent of type BaseRandom.
 * On the parent, this makes all child objects' RNG pointers point to the
 * calling object's RNG.
 */

void BaseRandom::setSeed(int seed) {
    if(hasAParent() && parent()->inheritsRandom())
        return;
    if(mt != NULL)
        delete mt;
    setRNG(new mt19937(seed));
}

void BaseRandom::setRNG(mt19937 *mt) {
    if(mt == NULL)
        return;
    this->mt = mt;
    setRNG_impl();
    std::list<BaseRandom *>::const_iterator iterator;
    for (iterator = randomChildList.begin(); iterator != randomChildList.end(); ++iterator) {
        BaseRandom *rnd = *iterator;
        rnd->setRNG(mt);
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

void BaseRandom::setRNG_impl() {

}

void BaseRandom::addChild_impl(BaseObject *child) {
    if(!child->inheritsRandom())
        return;

    BaseRandom *rnd = (BaseRandom *)child;
    randomChildList.push_back(rnd);
}

void BaseRandom::removeChild_impl(BaseObject *child) {
    if(!child->inheritsRandom())
        return;

    BaseRandom *rnd = (BaseRandom *)child;
    randomChildList.remove(rnd);
}

void BaseRandom::setParent_impl(BaseObject *parent) {
    if(parent == NULL || !parent->inheritsRandom())
        return;
    BaseRandom *randomParent = (BaseRandom*)parent;
    setRNG(randomParent->mt);
}
