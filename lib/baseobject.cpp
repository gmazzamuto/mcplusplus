/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MCPlusPlus/baseobject.h>

#include <ctime>
#ifdef __GNUC__
#include <typeinfo>
#include <cxxabi.h>
#endif
#include <stdio.h>
#include <vector>

using namespace MCPP;

BaseObject::BaseObject(BaseObject *parent)
{
    _hasAParent = false;
    _parent = NULL;
    _inheritsRandom = false;
    _wasCloned = false;
    setParent(parent);
}

BaseObject::~BaseObject() {
    if(_hasAParent)
        _parent->removeChild(this);
    deleteAllChildren();
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

BaseObject *BaseObject::clone_impl() const
{
    return new BaseObject();
}

bool BaseObject::sanityCheck_impl() const
{
    return true;
}

void BaseObject::deleteAllChildren() {
    BaseObject *child;
    while(!_childList.empty()) {
        child = _childList.back();
        _childList.remove(child);
        delete child;
    }
}

list<BaseObject *> BaseObject::childList() const {
    return _childList;
}

bool BaseObject::hasAParent() const {
    return _hasAParent;
}

/**
 * @brief Returns a pointer to the parent object
 */

BaseObject *BaseObject::parent() const {
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

/**
 * @brief Determines whether or not the object derives from BaseRandom
 * @return true if the object derives from BaseRandom, false otherwise
 */

bool BaseObject::inheritsRandom() const {
    return _inheritsRandom;
}

/**
 * @brief Returns a cloned instance of the current object
 * @return A pointer to the newly created object
 *
 * The behaviour of this function is dependent on the object type. In
 * particular, subclasses of BaseObject can provide their own specific
 * implementation through clone_impl(). Cloned object are created without a
 * parent.
 */

BaseObject *BaseObject::clone() const
{
    BaseObject *clonedObject = clone_impl();
    clonedObject->_wasCloned = true;
    return clonedObject;
}

/**
 * @brief Determines whether or not the object was cloned
 * @return true if the object was constructed by cloning another object via
 * clone(), false if not
 */

bool BaseObject::wasCloned() const
{
    return _wasCloned;
}

/**
 * @brief Prints a description of the object and its main properties.
 *
 * Subclasses can provide their own specific behaviour through describe_impl().
 */

void BaseObject::describe() const
{
    describe_impl();
}

/**
 * @brief The class name of the object.
 * @return
 */

string BaseObject::typeName() const
{
    string str;
#ifdef __GNUC__
    str = abi::__cxa_demangle(typeid(*this).name(), 0, 0, 0);
#else
    str = "UNKNOWN OBJECT";
#endif
    return str;
}

/**
 * @brief Performs a series of integrity checks on internal data to ensure that
 * the object is properly initialized and ready to be used
 *
 * @return true on success, false on failure
 *
 * This functions checks that the BaseObject pointers in the check list are not
 * NULL and will call sanityCheck() on the pointed objects. Subclasses of
 * BaseObject can provide their own specific implementation through
 * sanityCheck_impl().
 */

bool BaseObject::sanityCheck() const
{
    bool ok = true;
    const BaseObject *obj;
    std::list<const BaseObject *const *>::const_iterator iterator;
    for (iterator = objectsToCheck.begin();
         iterator != objectsToCheck.end();
         ++iterator) {
        const BaseObject *const *objpp = *iterator;
        obj = *objpp;
        if(obj == NULL) {
            ok = false;
            break;
        }
        ok = obj->sanityCheck();
        if(!ok)
            break;
    }
    if(ok)
        ok = sanityCheck_impl();

    if(!ok)
        logMessage("Error: sanity check failed");

    return ok;
}

string BaseObject::messagePrefix() const
{
    time_t rawtime;
    struct tm * timeinfo;
    char buffer [80];

    time (&rawtime);
    timeinfo = localtime (&rawtime);

    strftime (buffer,80,"[%F %T]",timeinfo);

    string str = buffer;

    str += "[";
    str += typeName();
    str += "]";

    str += " ";
    return str;
}

void BaseObject::printLogMessage(const char *fmt, va_list arguments) const
{
    if(arguments == NULL)
        fprintf(stderr,fmt,NULL);
    else
        vfprintf(stderr,fmt,arguments);
}

void BaseObject::describe_impl() const
{
    logMessage("");
}

/**
 * @brief Logs a message to stderr
 * @param msg
 *
 *
 * If the compiler used is GCC, the message will be prepended with the class
 * name
 */

void BaseObject::logMessage(const string &msg) const
{
    string str = messagePrefix();
    str += msg;
    str += "\n";
    printLogMessage(str.c_str());
}

void BaseObject::logMessage(const char *fmt, ...) const
{
    va_list arguments;
    va_start ( arguments, fmt );
    string str = messagePrefix();
    str += fmt;
    str += "\n";
    printLogMessage(str.c_str(),arguments);
}

/**
 * @brief Adds a BaseObject pointer to the list of objects to be checked
 * @param obj A pointer to a BaseObject pointer
 *
 * \see sanityCheck()
 */

void BaseObject::addObjectToCheck(const BaseObject ** const obj)
{
    objectsToCheck.push_back((BaseObject **)obj);
}

void BaseObject::clearObjectsToCheck()
{
    objectsToCheck.clear();
}

void BaseObject::copyToInternalVariable(char **dest, const char *src) {
    if(*dest!=NULL)
        free(*dest);
    *dest = (char*)malloc(sizeof(char)*strlen(src)+1);
    memcpy(*dest,src,strlen(src)+1);
}
