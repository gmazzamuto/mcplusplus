#include "source.h"
#include "math.h"

Source::Source(BaseObject *parent) :
    BaseRandom(parent)
{
    walkTimeDistribution = NULL;
    cosThetaDistribution = NULL;
    psiDistribution = NULL;
    for (int i = 0; i < 3; ++i) {
        r0Distribution[i] = NULL;
    }
}

void Source::spinDirection(Walker *walker) const {
    double cosTheta = cosThetaDistribution->spin();
    double cosPsi = cos(psiDistribution->spin());
    double sinTheta = sqrt(1 - pow(cosTheta,2));
    double sinPsi = sin(psiDistribution->spin());

    walker->k0[0] = sinTheta*cosPsi;
    walker->k0[1] = sinTheta*sinPsi;
    walker->k0[2] = cosTheta;
}

void Source::spinPosition(Walker *walker) const {
    for (int i = 0; i < 3; ++i) {
        walker->r0[i] = r0Distribution[i]->spin();
    }
}

void Source::spinTime(Walker *walker) const {
    walker->walkTime = walkTimeDistribution->spin();    // overwrapping?
}

Walker* Source::constructWalker() const {
    Walker *walker = new Walker();

    spinDirection(walker);
    spinPosition(walker);
    spinTime(walker);

    return walker;
}

void Source::setr0Distribution(AbstractDistribution **distrArray) {
    for (int i = 0; i < 3; ++i) {
        if(r0Distribution[i] != NULL)
            delete r0Distribution[i];
        r0Distribution[i] = distrArray[i];
        r0Distribution[i]->setParent(this);
    }
}

void Source::setk0Distribution(AbstractDistribution *cosThetaDistr, AbstractDistribution *psiDistr) {
    if(cosThetaDistribution != NULL)
        delete cosThetaDistribution;
    cosThetaDistribution = cosThetaDistr;
    cosThetaDistribution->setParent(this);
    if(psiDistribution != NULL)
        delete psiDistribution;
    psiDistribution = psiDistr;
    psiDistribution->setParent(this);
}

void Source::setWalkTimeDistribution(AbstractDistribution *distr) {
    walkTimeDistribution = distr;
    walkTimeDistribution->setParent(this);
}

void Source::setWavelength(double nm) {
    wl = nm;
}

double Source::wavelength() const {
    return wl;
}



PencilBeamSource::PencilBeamSource(BaseObject *parent) :
    Source(parent)
{

}

Walker* PencilBeamSource::constructWalker() const {
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
    r0Distribution[2] = new DeltaDistribution(0, this);
}

GaussianBeamSource::GaussianBeamSource(double xFWHM, double yFWHM, BaseObject *parent) :
    Source(parent)
{
    r0Distribution[0] = new NormalDistribution(0,xFWHM, this);
    r0Distribution[1] = new NormalDistribution(0,yFWHM, this);
    r0Distribution[2] = new DeltaDistribution(0, this);
}

/**
 * @brief Constructs a new walker.
 * @return A pointer to the newly constructed walker.
 * \pre walkTimeDistribution has to be valid
 */

Walker* GaussianBeamSource::constructWalker() const {
    Walker *walker;
    if(cosThetaDistribution != NULL || psiDistribution != NULL) //check if one of the k0Distributions has been overridden
        walker = Source::constructWalker(); //r0Distributions should already be set by now
    else {
        walker = new Walker();
        walker->k0[2] = 1;

        spinTime(walker);
    }
    return walker;
}
