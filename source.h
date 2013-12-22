#ifndef SOURCE_H
#define SOURCE_H

#include "walker.h"

class AbsractSource
{
public:
    AbsractSource();
    virtual Walker *constructWalker() = 0;
};

#endif // SOURCE_H
