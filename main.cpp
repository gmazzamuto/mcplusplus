#include <stdio.h>
#include "costhetagenerator.h"

int main(int argc, char *argv[])
{

    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0,0.99);
    ExponentialRandomGenerator *stepLength = new ExponentialRandomGenerator(0,0.01);

    for (int i = 0; i < 100; ++i) {
        printf("%lf\t%lf\t%f\n", isotrCosTheta->spin(),anisCosTheta->spin(),stepLength->spin());
    }

    return 0;
}
