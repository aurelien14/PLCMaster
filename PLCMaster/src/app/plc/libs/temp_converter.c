#include <stdio.h>
#include <math.h>

float pt100_resistance_to_temperature(float resistance) {
    const float R0 = 100.0f;
    const float A = 3.9083e-3f;
    const float B = -5.775e-7f;
    const float C = -4.183e-12f;

    if (resistance < 18.52f || resistance > 390.0f) {
        return NAN;
    }

    if (resistance >= R0) {
        float discriminant = A * A - 4.0f * B * (1.0f - resistance / R0);

        if (discriminant < 0) {
            return NAN;
        }

        float temp = (-A + sqrtf(discriminant)) / (2.0f * B);
        return temp;
    }
    else {
        float temp = (resistance / R0 - 1.0f) / A;

        for (int i = 0; i < 5; i++) {
            float R_calc = R0 * (1.0f + A * temp + B * temp * temp +
                C * (temp - 100.0f) * temp * temp * temp);
            float dR_dT = R0 * (A + 2.0f * B * temp +
                C * (4.0f * temp * temp * temp - 300.0f * temp * temp));
            temp = temp - (R_calc - resistance) / dR_dT;
        }

        return temp;
    }
}