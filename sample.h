#ifndef SAMPLE_H
#define SAMPLE_H

class Sample
{
public:
    Sample();
    void setThickness(double L);
    void setScatteringMeanFreePath(double ls);
    void setScatteringAnisotropy(double g);
    void setExtRefrIndex(double nExt);
    void setIntRefrIndex(double nInt);

protected:
    double totalThickness;
    double ls;
    double g;
    double nExt, nInt;
};

#endif // SAMPLE_H
