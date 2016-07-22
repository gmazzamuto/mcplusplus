/*
  This file is part of MCPlusPlus.

  MCPlusPlus is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Mcplusplus is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with MCPlusPlus.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <MCPlusPlus/material.h>
#include <cmath>
#include <limits>

using namespace MCPP;

Material::Material()
{
    ls = numeric_limits<MCfloat>::infinity(); //non-scattering medium
    n = 1;
    v = LIGHT_SPEED;
}

Material::~Material()
{

}

/**
 * @brief Sets the refractive index and the speed of light in this medium
 * according to the given wavelength
 *
 * @param um wavelength in micrometers
 */

void Material::setWavelength(double um) {
    n = dispersionRelation(um);
    v = LIGHT_SPEED / n;
}

/**
 * @brief Implements the dispersion relation
 * @param lambda_um wavelength in micrometers
 * @return the refractive index at the given wavelength
 *
 * The default material is dispersionless.
 */

MCfloat Material::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    return n;
}




Air::Air() :
    Material()
{
    //n = 1.00027499;
    Temperature = 20;
    Pressure = 101400;
    RH = 30;
}

Air::~Air()
{

}


/**
 * @brief Air dispersion relation
 *
 * Dispersion relation based on modified Edlén formulae. If not otherwise
 * specified, temperature, pressure, and relative humidity default respectively
 * to 20°C, 1014 hPa and 30%.
 *
 * @param lambda_um
 * @return the refractive index of air
 */

// sse also
// http://jartweb.net/blog/wp-content/uploads/2013/09/Chapter7.pdf
// http://www.humboldtcanada.com/presentations_air/jennifer.pdf

MCfloat Air::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    MCfloat pv = (0.01 * RH) * saturatedVapourPressure();
    MCfloat S = pow(lambda_um, -2);
    MCfloat dns = 8342.54E-8 + 2406147E-8 / (130 - S) + 15998E-8 / (38.9 - S);
    MCfloat X = (1 + (0.601 - 0.00972 * Temperature) * Pressure * 1E-8)
            / (1 + 0.003661 * Temperature);
    MCfloat ntp = 1 + Pressure * dns * X / 96095.43;
    return ntp - (292.75 / (Temperature + 273.15)) * (3.7345 - 0.0401 * S) * pv
            * 1E-10;
}

/**
 * @brief IAPWS formula for saturated vapour pressure
 *
 * Taken from <a
 * href="http://emtoolbox.nist.gov/Wavelength/Documentation.asp#AppendixAI">Nist</a>
 *
 * @warning This particular implementation holds for temperatures above 0°C.
 *
 * @return \f$ p_\text{sv} (T) \f$
 */

MCfloat Air::saturatedVapourPressure()
{
    MCfloat T = Temperature + 273.15;

    MCfloat K1 = 1.16705214528E+03;
    MCfloat K2 = -7.24213167032E+05;
    MCfloat K3 = -1.70738469401E+01;
    MCfloat K4 = 1.20208247025E+04;
    MCfloat K5 = -3.23255503223E+06;
    MCfloat K6 = 1.49151086135E+01;
    MCfloat K7 = -4.82326573616E+03;
    MCfloat K8 = 4.05113405421E+05;
    MCfloat K9 = -2.38555575678E-01;
    MCfloat K10 = 6.50175348448E+02;

    MCfloat Omega = T + K9 / (T - K10);
    MCfloat A = pow(Omega, 2) + K1 * Omega + K2;
    MCfloat B = K3 * pow(Omega, 2) + K4 * Omega + K5;
    MCfloat C = K6 * pow(Omega, 2) + K7 * Omega + K8;
    MCfloat X = -B + sqrt(pow(B, 2) - 4 * A * C);

    return pow(2 * C / X, 4) * 1E6;
}


GlassSlide::GlassSlide() :
    Material()
{
    //n = 1.5203;
}

GlassSlide::~GlassSlide()
{

}

MCfloat GlassSlide::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    return sqrt(1 + 1.282086558597 * pow(lambda_um, 2) / (pow(lambda_um, 2)
                                                          - 0.01023694363174)
                + 0.05984826992046 * pow(lambda_um, 2) / (pow(lambda_um, 2)
                                                          - 12.09530215672));
}




NorlandOpticalAdhesive65::NorlandOpticalAdhesive65() :
    Material()
{
    //n = 1.514;
}

NorlandOpticalAdhesive65::~NorlandOpticalAdhesive65()
{

}




MCfloat NorlandOpticalAdhesive65::dispersionRelation(MCfloat lambda_um) {
    MC_ASSERT_MSG(lambda_um > 0, "Invalid wavelength");
    return 1.50631 + 5.43562E-3 / pow(lambda_um, 2) + 27.7798E-6 /
            pow(lambda_um, 4);
}




Vacuum::Vacuum() :
    Material()
{
}

Vacuum::~Vacuum()
{

}
