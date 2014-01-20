#ifndef MATERIAL_H
#define MATERIAL_H

class Material
{
public:
    Material();
    double ls;  /**< @brief scattering mean free path*/
    double g;  /**< @brief scattering anisotropy factor \f$ g = \left\langle \cos \theta \right \rangle \f$*/
    double n;  /**< @brief refractive index (\f$ n \in \mathbb{R} \f$)*/
};


class FreeSpace : public Material
{
public:
    FreeSpace();
};

#endif // MATERIAL_H
