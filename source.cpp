#include "source.h"

Source::Source(BaseObject *parent) :
    BaseRandom(parent)
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



PencilBeamSource::PencilBeamSource(BaseObject *parent) :
    Source(parent)
{

}

Walker* PencilBeamSource::constructWalker() {
    Walker *walker = new Walker();
    walker->k0[2] = 1;

    return walker;
}



GaussianBeamSource::GaussianBeamSource(double FWHM, BaseObject *parent) :
    Source(parent)
{
    NormalDistribution *distr = new NormalDistribution(0,FWHM, this);
    for (int i = 0; i < 2; ++i) {
        r0Distribution[i] = distr;
    }
}

GaussianBeamSource::GaussianBeamSource(double xFWHM, double yFWHM, BaseObject *parent) :
    Source(parent)
{
    r0Distribution[0] = new NormalDistribution(0,xFWHM, this);
    r0Distribution[1] = new NormalDistribution(0,yFWHM, this);
}

Walker* GaussianBeamSource::constructWalker() {
    Walker *walker = new Walker();
    for (int i = 0; i < 2; ++i) {
        walker->r0[i] = r0Distribution[i]->spin();
    }
    if(k0Distribution[2] != NULL) //check if one of the 3 k0Distributions has been overridden
        for (int i = 0; i < 3; ++i) {
            walker->k0[i] = k0Distribution[i]->spin();
        }
    else
        walker->k0[2] = 1;

    walker->walkTime = walkTimeDistribution->spin();

    return walker;
}
