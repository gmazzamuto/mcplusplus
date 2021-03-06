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

#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <cstddef>
#include <list>
#include <boost/utility.hpp>
#include "MCglobal.h"
#include <cstdarg>
#include <sstream>
#include <string.h>

namespace MCPP {

/**
 * @brief The BaseObject class is the base class for many MCPlusPlus objects.
 *
 * BaseObjects can have a parent, specified in the constructor or at a later
 * time with setParent(). In this case, the parent takes ownership of the
 * object and adds it to its own list of child objects.
 *
 * A parent / child relationship implies the following:
 * - when a child object is deleted, it is removed from the parent's child list
 *
 * - when the parent object is deleted, all child objects are deleted too
 *
 * BaseObjects have neither a copy constructor nor an assignment operator (=).
 * This means that you cannot copy a BaseObject into another BaseObject by a
 * simple assignment and that you cannot use container classes (such as
 * vectors) to store BaseObjects. In this case you will be forced to only use
 * pointers to BaseObjects. For a nice explanation on this design choice, see
 * Qt's <a
 * href="http://qt-project.org/doc/qt-5.0/qtcore/object.html#identity-vs-value">
 * Identity vs Value</a> which explains the same concept applied to QObjects.
 *
 * BaseObject can be cloned using clone().
 */

class BaseObject : private boost::noncopyable
{
public:
    BaseObject(BaseObject *parent=NULL);
    virtual ~BaseObject();

    list<BaseObject *> childList() const;
    bool hasAParent() const;
    BaseObject *parent() const;
    void setParent(BaseObject *parent);
    bool inheritsRandom() const;
    BaseObject *clone() const;
    bool wasCloned() const;
    void describe() const;
    string typeName() const;
    bool sanityCheck() const;

protected:
    void logMessage(const string &msg) const;
    void logMessage(const char* fmt, ...) const;
    void addObjectToCheck(const BaseObject ** const obj);
    void clearObjectsToCheck();
    void copyToInternalVariable(char **dest, const char *src);

    bool _inheritsRandom;
    list<const BaseObject *const *> objectsToCheck;

private:
    void removeChild(BaseObject *child);
    void deleteAllChildren();
    void addChild(BaseObject *child);
    string messagePrefix() const;
    void printLogMessage(const char *fmt, va_list arguments=NULL) const;

    virtual BaseObject *clone_impl() const;
    virtual void setParent_impl(BaseObject *parent);
    virtual void addChild_impl(BaseObject *child);
    virtual void removeChild_impl(BaseObject *child);
    virtual bool sanityCheck_impl() const;
    virtual void describe_impl() const;

    bool _hasAParent;
    BaseObject *_parent;
    list<BaseObject *> _childList;
    bool _wasCloned;
};

}

#endif // BASEOBJECT_H
