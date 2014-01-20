#ifndef MATERIAL_H
#define MATERIAL_H

class Material
{
public:
    Material();
    double ls;
    double g;
    double lt;
    double refractiveIndex;
};


class FreeSpace : public Material
{
public:
    FreeSpace();
};

#endif // MATERIAL_H
