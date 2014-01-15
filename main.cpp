#include <stdio.h>
#include "source.h"
#include "costhetagenerator.h"
#include "psigenerator.h"


int main(int argc, char *argv[])
{
    IsotropicCosThetaGenerator *isotrCosTheta = new IsotropicCosThetaGenerator(0);
    AnisotropicCosThetaGenerator *anisCosTheta = new AnisotropicCosThetaGenerator(0.99,0);
    ExponentialDistribution *stepLength = new ExponentialDistribution(0.01,0);
    DeltaDistribution *center = new DeltaDistribution(12.2);
    NormalDistribution *inputPos = new NormalDistribution(11.1,0.1,0);
    Sech2Distribution *initTime = new Sech2Distribution(12,6.3,0);
    PsiGenerator *randomPsi = new PsiGenerator(0);
    UniformDistribution *uniform = new UniformDistribution(0,10,0);



    AbstractDistribution *distribution = uniform;

    for (int i = 0; i < 10; ++i) {
        printf("%lf\n", distribution->spin());
    }

    printf("-----------\n");
    distribution->setSeed(0);

    for (int i = 0; i < 10; ++i) {
        printf("%lf\n", distribution->spin());
    }

    return 0;
}
