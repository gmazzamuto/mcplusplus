#include "source.h"
#include "costhetagenerator.h"
#include "psigenerator.h"

Source::Source(BaseObject *parent) :
    BaseRandom(parent)
{
    walkTimeDistribution = NULL;
    addObjectToCheck((const BaseObject**)&walkTimeDistribution);

    cosThetaDistribution = NULL;
    addObjectToCheck((const BaseObject**)&cosThetaDistribution);

    psiDistribution = NULL;
    addObjectToCheck((const BaseObject**)&psiDistribution);

    for (int i = 0; i < 3; ++i) {
        r0Distribution[i] = NULL;
        addObjectToCheck((const BaseObject**)&r0Distribution[i]);
    }
    _z0 = 0;
    setWavelength(-1);

}

Source::~Source()
{

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
    cloneInto(src);
    return src;
}

void Source::cloneInto(Source *src) const
{
    for (int i = 0; i < 2; ++i) {
        if(r0Distribution[i] != NULL)
            src->r0Distribution[i] = (AbstractDistribution*)r0Distribution[i]->clone();
    }
    src->_z0 = z0();
    if(cosThetaDistribution != NULL && psiDistribution != NULL)
        src->setk0Distribution((AbstractDistribution*)cosThetaDistribution->clone(),(AbstractDistribution*)psiDistribution->clone());
    if(walkTimeDistribution != NULL)
        src->setWalkTimeDistribution((AbstractDistribution*)walkTimeDistribution->clone());
    src->setWavelength(wl);
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

void Source::setr0Distribution(AbstractDistribution *x0Distribution, AbstractDistribution *y0Distribution, MCfloat z0) {
    x0Distribution->setParent(this);
    y0Distribution->setParent(this);
    r0Distribution[0] = x0Distribution;
    r0Distribution[1] = y0Distribution;
    r0Distribution[2] = new DeltaDistribution(z0,this);
    this->_z0 = z0;
}

void Source::setk0Distribution(AbstractDistribution *cosThetaDistr, AbstractDistribution *psiDistr) {
    cosThetaDistribution = cosThetaDistr;
    cosThetaDistribution->setParent(this);
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

MCfloat Source::z0() const
{
    return _z0;
}




PencilBeamSource::PencilBeamSource(BaseObject *parent) :
    Source(parent)
{
    _z0 = -1*std::numeric_limits<MCfloat>::infinity();
    clearObjectsToCheck();
    addObjectToCheck((const BaseObject**)&walkTimeDistribution);
}

PencilBeamSource::~PencilBeamSource()
{

}

void PencilBeamSource::spinDirection(Walker *walker) const {
    walker->k0[2] = 1;
}

void PencilBeamSource::spinPosition(Walker *walker) const {
    walker->r0[2] = _z0;
}

BaseObject *PencilBeamSource::clone_impl() const
{
    PencilBeamSource *src = new PencilBeamSource();
    cloneInto(src);
    return src;
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

GaussianBeamSource::~GaussianBeamSource()
{

}

BaseObject *GaussianBeamSource::clone_impl() const
{
    GaussianBeamSource *src = new GaussianBeamSource(xFWHM,yFWHM);
    cloneInto(src);
    return src;
}

void GaussianBeamSource::init(MCfloat xFWHM, MCfloat yFWHM)
{
    this->xFWHM = xFWHM;
    this->yFWHM = yFWHM;
    AbstractDistribution *distr[2];
    distr[0] = new NormalDistribution(0,1, this);
    distr[1] = new NormalDistribution(0,1, this);
    NormalDistribution *tempNorm;
    tempNorm = (NormalDistribution*)distr[0];
    tempNorm->setFWHM(xFWHM);
    tempNorm = (NormalDistribution*)distr[1];
    tempNorm->setFWHM(yFWHM);
    setr0Distribution(distr[0], distr[1], -1*std::numeric_limits<MCfloat>::infinity());
    setk0Distribution(new DeltaDistribution(1,this), new DeltaDistribution(1,this)); //to avoid generating a useless random number (sinTheta is 0)
}




IsotropicPointSource::IsotropicPointSource(MCfloat z0, BaseObject *parent) :
    Source(parent)
{
    AbstractDistribution *distr[2];
    distr[0] = new DeltaDistribution(0);
    distr[1] = new DeltaDistribution(0);
    setr0Distribution(distr[0], distr[1], z0);
    setk0Distribution(new CosThetaGenerator(0,this), new IsotropicPsiGenerator(this));
}

IsotropicPointSource::~IsotropicPointSource()
{

}

BaseObject *IsotropicPointSource::clone_impl() const
{
    IsotropicPointSource *src = new IsotropicPointSource(z0());
    cloneInto(src);
    return src;
}

void IsotropicPointSource::describe_impl() const
{
    logMessage("z0 = %f", _z0);
}
