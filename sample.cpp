#include "sample.h"

Sample::Sample()
{
}

Sample::setThickness(double L) {
    totalThickness = L;
}

Sample::setScatteringMeanFreePath(double ls) {
    this->ls = ls;
}

Sample::setScatteringAnisotropy(double g) {
    this->g = g;
}

Sample::setExtRefrIndex(double nExt) {
    this->nExt = nExt;
}

Sample::setIntRefrIndex(double nInt) {
    this->nInt = nInt;
}
