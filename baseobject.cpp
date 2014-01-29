#include "baseobject.h"

BaseObject::BaseObject(BaseObject *parent)
{
    _hasAParent = false;
    _parent = NULL;
    setParent(parent);
}

BaseObject::~BaseObject() {
    if(!_hasAParent) {
        deleteAllChildren();
    }
    else
        _parent->removeChild(this);
}

/**
 * @brief Adds the given child to the child list
 * @param child
 */

void BaseObject::addChild(BaseObject *child) {
    _childList.push_back(child);
    addChild_impl(child);
}

void BaseObject::addChild_impl(BaseObject *child) {

}

/**
 * @brief Removes the given child from the child list
 * @param child
 */

void BaseObject::removeChild(BaseObject *child) {
    _childList.remove(child);
    removeChild_impl(child);
}

void BaseObject::removeChild_impl(BaseObject *child) {

}

void BaseObject::deleteAllChildren() {
    BaseObject *child;
    while(!_childList.empty()) {
        child = _childList.back();
        _childList.remove(child);
        delete child;
    }
}

list<BaseObject *> BaseObject::childList() {
    return _childList;
}

bool BaseObject::hasAParent() {
    return _hasAParent;
}

/**
 * @brief Returns a pointer to the parent object
 */

BaseObject *BaseObject::parent() {
    return _parent;
}

/**
 * @brief Makes the object a child of parent
 * @param parent
 */

void BaseObject::setParent(BaseObject *parent) {
    if(_hasAParent)
        _parent->removeChild(this);
    _parent = parent;
    if(parent == NULL) {
        _hasAParent = false;
    }
    else {
        _hasAParent = true;
        parent->addChild(this);
        setParent_impl(parent);
    }
}

void BaseObject::setParent_impl(BaseObject *parent) {

}

bool BaseObject::inheritsRandom() {
    return _inheritsRandom;
}
