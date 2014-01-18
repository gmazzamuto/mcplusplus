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

void Source::setr0Distribution(AbstractDistribution **distrArray) {
    for (int i = 0; i < 3; ++i) {
        r0Distribution[i] = distrArray[i];
    }
}

void Source::setk0Distribution(AbstractDistribution **distrArray) {
    for (int i = 0; i < 3; ++i) {
        k0Distribution[i] = distrArray[i];
    }
}

void Source::setWalkTimeDistribution(AbstractDistribution *distr) {
    walkTimeDistribution = distr;
}

void Source::setWavelength(double nm) {
    wl = nm;
}

double Source::wavelength() {
    return wl;
}


PencilBeamSource::PencilBeamSource() :
    Source()
{

}

Walker* PencilBeamSource::constructWalker() {
    Walker *walker = new Walker();
    walker->k0[2] = 1;

    return walker;

}
