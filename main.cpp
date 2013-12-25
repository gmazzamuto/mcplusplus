#include <stdio.h>
#include "costhetagenerator.h"
#include "psigenerator.h"

int main(int argc, char *argv[])
{

    mt19937* mersenneTwister = new mt19937(0);

//    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
//    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0,0.99);
//    ExponentialDistribution *stepLength = new ExponentialDistribution(0,0.01);
//    DeltaDistribution *center = new DeltaDistribution(12.2);
//    NormalDistribution *inputPos = new NormalDistribution(1,11.1,0.1);
//    Sech2Distribution *initTime = new Sech2Distribution(mersenneTwister,12,6.3);
    PsiGenerator *randomPsi = new PsiGenerator(mersenneTwister);

    for (int i = 0; i < 1000; ++i) {
        printf("%lf\n", randomPsi->spin());
    }

    return 0;
}
