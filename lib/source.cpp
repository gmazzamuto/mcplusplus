#include "source.h"
#include "costhetagenerator.h"
#include "psigenerator.h"

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
    MCfloat cosTheta = cosThetaDistribution->spin();
    MCfloat sinTheta = sqrt(1 - pow(cosTheta,2));
    MCfloat psi = psiDistribution->spin();
    MCfloat cosPsi = cos(psi);
    MCfloat sinPsi = sin(psi);

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

BaseObject *Source::clone_impl() const
{
    Source *src = new Source();
    AbstractDistribution *r0Distr[3];
    for (int i = 0; i < 3; ++i) {
        r0Distr[i] = (AbstractDistribution*)r0Distribution[i]->clone();
    }
    src->setr0Distribution(r0Distr);
    src->setk0Distribution((AbstractDistribution*)cosThetaDistribution->clone(),(AbstractDistribution*)psiDistribution->clone());
    src->setWalkTimeDistribution((AbstractDistribution*)walkTimeDistribution->clone());
    src->setWavelength(wl);
    return src;
}

/**
 * @brief Constructs a new walker.
 * @return A pointer to the newly constructed walker.
 */

void Source::spin(Walker *walker) const {
    walker->reset();
    spinPosition(walker); //the calling order is critical! GaussianRayBundleSource must spin the walker's position BEFORE calculating the direction vector
    spinDirection(walker);
    spinTime(walker);
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

void Source::setWavelength(MCfloat um) {
    wl = um;
}

MCfloat Source::wavelength() const {
    return wl;
}




PencilBeamSource::PencilBeamSource(BaseObject *parent) :
    Source(parent)
{

}

void PencilBeamSource::spinDirection(Walker *walker) const {
    walker->k0[2] = 1;
}

void PencilBeamSource::spinPosition(Walker *walker) const {

}

void PencilBeamSource::spinTime(Walker *walker) const {

}

BaseObject *PencilBeamSource::clone_impl() const
{
    return new PencilBeamSource();
}




/**
 * @brief Constructs a cylindrically symmetric gaussian 2D intensity profile in
 *        the \f$ z=0\f$ plane
 * @param FWHM full width half maximum of the profile
 * @param parent
 */

GaussianBeamSource::GaussianBeamSource(MCfloat FWHM, BaseObject *parent) :
    Source(parent)
{
    init(FWHM,FWHM);
}

/**
 * @brief Constructs a non-cylindrically symmetric gaussian 2D intensity profile
 *        in the \f$ z=0\f$ plane
 * @param xFWHM full width half maximum of the profile along the \f$ x \f$ axis
 * @param yFWHM full width half maximum of the profile along the \f$ y \f$ axis
 * @param parent
 */

GaussianBeamSource::GaussianBeamSource(MCfloat xFWHM, MCfloat yFWHM, BaseObject *parent) :
    Source(parent)
{
    init(xFWHM,yFWHM);
}

BaseObject *GaussianBeamSource::clone_impl() const
{
    GaussianBeamSource *src = new GaussianBeamSource(xFWHM,yFWHM);
    if(cosThetaDistribution != NULL && psiDistribution != NULL)
        src->setk0Distribution((AbstractDistribution*)cosThetaDistribution->clone(),(AbstractDistribution*)psiDistribution->clone());
    src->setWalkTimeDistribution((AbstractDistribution*)walkTimeDistribution->clone());
    return src;
}

void GaussianBeamSource::init(MCfloat xFWHM, MCfloat yFWHM)
{
    this->xFWHM = xFWHM;
    this->yFWHM = yFWHM;
    r0Distribution[0] = new NormalDistribution(0,xFWHM, this);
    r0Distribution[1] = new NormalDistribution(0,yFWHM, this);
    r0Distribution[2] = new DeltaDistribution(0, this);
    cosThetaDistribution = new DeltaDistribution(1,this);
    psiDistribution = new DeltaDistribution(1,this); //to avoid generating a useless random number (sinTheta is 0)
}




IsotropicPointSource::IsotropicPointSource(MCfloat z0, BaseObject *parent) :
    Source(parent)
{
    depth = z0;
    cosThetaDistribution = new CosThetaGenerator(0,this);
    psiDistribution = new IsotropicPsiGenerator(this);
}

void IsotropicPointSource::spinPosition(Walker *walker) const {
    walker->r0[2] = depth;
}

void IsotropicPointSource::spinTime(Walker *walker) const {

}

BaseObject *IsotropicPointSource::clone_impl() const
{
    return new IsotropicPointSource(depth);
}

void IsotropicPointSource::describe_impl() const
{
    logMessage("z0 = %f", depth);
}
