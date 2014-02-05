#ifndef MATERIAL_H
#define MATERIAL_H

class Material
{
public:
    Material();
    double ls;  /**< @brief scattering mean free path*/
    double g;  /**< @brief scattering anisotropy factor \f$ g = \left\langle \cos \theta \right \rangle \f$*/
    double n;  /**< @brief refractive index (\f$ n \in \mathbb{R} \f$)*/
    bool isScattering;

    virtual void setWavelegth(double um);
};


class FreeSpace : public Material
{
public:
    FreeSpace();

    void setWavelegth(double um);
};




/**
 * @brief Glass microscope slide.
 *
 * This Material aims to reproduce the optical properties of <a href="http://www.carloerbareagents.com/LabwareGruppoProdotto.aspx?id=22OB02AA&browsing=1">Forlab Microscope Slides</a>
 * purchased by Carlo Erba. Their catalogue page reads "extrawhite soda lime
 * glass" and such slides are commonly known to have a refractive index of
 * approximately 1.52@810nm (citation needed). In order to implement a usable
 * dispersion relation we referred to the Schott B270 Superwhite soda lime
 * glass, a very common multi-purpose kind of glass that happens to have the
 * expected refractive index.
 * Sadly enough, no empirical dispersion relation for this yet common kind of
 * glass happens to be known as well. On page 9 of <a href="http://www.reichmann-feinoptik.com/assets/applets/B_270_Superwite_e.pdf">this</a>
 * scanned specs datasheet a graph is shown with several values of the
 * refractive index. Based on these extrapolated values it was possible to
 * determine the appropriate <a href="http://en.wikipedia.org/wiki/Sellmeier_equation">Sellmeier coefficients</a>
 * for the empirical dispersion relation.
 */

class GlassSlide : public Material
{
public:
    GlassSlide();

    void setWavelegth(double um);
};


class NorlandOpticalAdhesive65 : public Material
{
public:
    NorlandOpticalAdhesive65();

    void setWavelegth(double um);
};

#endif // MATERIAL_H
