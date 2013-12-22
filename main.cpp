#include <stdio.h>
#include "costhetagenerator.h"

int main(int argc, char *argv[])
{

    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0,0.99);
    ExponentialRandomDistribution *stepLength = new ExponentialRandomDistribution(0,0.01);
    DeltaDistribution *center = new DeltaDistribution(12.2);

    for (int i = 0; i < 100; ++i) {
        printf("%lf\t%lf\t%f\t%f\n", isotrCosTheta->spin(),anisCosTheta->spin(),stepLength->spin(),center->spin());
    }

    return 0;
}
