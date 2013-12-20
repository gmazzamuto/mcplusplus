#include <stdio.h>
#include "uniformrandomgenerator.h"

int main(int argc, char *argv[])
{
    UniformRandomGenerator *cosThetaGenerator = new UniformRandomGenerator();

    for (int i = 0; i < 100; ++i) {
        printf("%f\n", cosThetaGenerator->generate());
    }


    return 0;
}
