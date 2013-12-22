#include <stdio.h>
#include "costhetagenerator.h"

int main(int argc, char *argv[])
{

    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0,0.99);
    ExponentialDistribution *stepLength = new ExponentialDistribution(0,0.01);
    DeltaDistribution *center = new DeltaDistribution(12.2);
    NormalDistribution *inputPos = new NormalDistribution(1,11.1,0.1);

    for (int i = 0; i < 1000000; ++i) {
        printf("%lf\t%lf\t%f\t%f\t%f\n", isotrCosTheta->spin(),anisCosTheta->spin(),stepLength->spin(),center->spin(),inputPos->spin());
    }

    return 0;
}
