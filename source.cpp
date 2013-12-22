#include "source.h"

Source::Source()
{
    walkTimeDistribution = NULL;
    for (int i = 0; i < 3; ++i) {
       r0Distribution[i] = NULL;
       k0Distribution[i] = NULL;
    }
}

Walker* Source::constructWalker() {
    Walker *walker = new Walker();
    for (int i = 0; i < 3; ++i) {
        walker->r0[i] = r0Distribution[i]->spin();
        walker->k0[i] = k0Distribution[i]->spin();
    }
    walker->walkTime = walkTimeDistribution->spin();

    return walker;
}
