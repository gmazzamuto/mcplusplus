#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include <cstddef>
#include <list>
#include <boost/utility.hpp>
#include "MCglobal.h"
#include <cstdarg>
#include <sstream>

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
    bool _inheritsRandom;
    void addObjectToCheck(const BaseObject ** const obj);
    void clearObjectsToCheck();
    list<const BaseObject *const *> objectsToCheck;

private:
    bool _hasAParent;
    BaseObject *_parent;
    list<BaseObject *> _childList;
    bool _wasCloned;

    void removeChild(BaseObject *child);
    void deleteAllChildren();
    void addChild(BaseObject *child);

    virtual void setParent_impl(BaseObject *parent);
    virtual void addChild_impl(BaseObject *child);
    virtual void removeChild_impl(BaseObject *child);
    virtual BaseObject *clone_impl() const;
    virtual bool sanityCheck_impl() const;

    string messagePrefix() const;
    void printLogMessage(const char *fmt, va_list arguments=NULL) const;

    virtual void describe_impl() const;
};

}

#endif // BASEOBJECT_H
