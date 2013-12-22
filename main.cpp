#include <stdio.h>
#include "costhetagenerator.h"

int main(int argc, char *argv[])
{

//    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
//    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0,0.99);
//    ExponentialDistribution *stepLength = new ExponentialDistribution(0,0.01);
//    DeltaDistribution *center = new DeltaDistribution(12.2);
//    NormalDistribution *inputPos = new NormalDistribution(1,11.1,0.1);
    Sech2Distribution *initTime = new Sech2Distribution(0,12,6.3);

    for (int i = 0; i < 1000000; ++i) {
        printf("%lf\n", initTime->spin());
    }

    return 0;
}
