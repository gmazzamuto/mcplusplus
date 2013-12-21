#include <stdio.h>
#include "costhetagenerator.h"

int main(int argc, char *argv[])
{

    IsotropicCosThetaGenerator *isct = new IsotropicCosThetaGenerator(0);
    AnisotropicCosThetaGenerator *anct = new AnisotropicCosThetaGenerator(0,0.99);

    for (int i = 0; i < 100; ++i) {
        printf("%lf\t%lf\n", isct->spin(),anct->spin());
    }

    return 0;
}
