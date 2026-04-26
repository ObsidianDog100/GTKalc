/*
 * GTKalc - Advanced Mathematical Functions
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This module provides advanced scientific and mathematical operations:
 * - Trigonometric functions (sin, cos, tan and their inverses)
 * - Logarithmic functions (natural log, base-10 log)
 * - Exponential operations (power, square root)
 * - Special functions (factorial, percentage, reciprocal, negate)
 * 
 * All operations handle edge cases:
 * - Domain restrictions (e.g., log of negative numbers)
 * - Angle mode switching (degrees vs radians)
 * - Graceful error handling with NaN returns
 */

#include "advanced.h"
#include <math.h>

// Mathematical constants for angle conversion
#define DEG_TO_RAD(deg) ((deg) * M_PI / 180.0)    // Convert degrees to radians
#define RAD_TO_DEG(rad) ((rad) * 180.0 / M_PI)    // Convert radians to degrees

double advanced_sin(double angle, gboolean degrees) {
    if (degrees) angle = DEG_TO_RAD(angle);
    return sin(angle);
}

double advanced_cos(double angle, gboolean degrees) {
    if (degrees) angle = DEG_TO_RAD(angle);
    return cos(angle);
}

double advanced_tan(double angle, gboolean degrees) {
    if (degrees) angle = DEG_TO_RAD(angle);
    return tan(angle);
}

double advanced_asin(double value) {
    if (value < -1.0 || value > 1.0) return NAN;
    return RAD_TO_DEG(asin(value));
}

double advanced_acos(double value) {
    if (value < -1.0 || value > 1.0) return NAN;
    return RAD_TO_DEG(acos(value));
}

double advanced_atan(double value) {
    return RAD_TO_DEG(atan(value));
}

double advanced_log10(double value) {
    if (value <= 0) return NAN;
    return log10(value);
}

double advanced_ln(double value) {
    if (value <= 0) return NAN;
    return log(value);
}

double advanced_pow(double base, double exponent) {
    return pow(base, exponent);
}

double advanced_sqrt(double value) {
    if (value < 0) return NAN;
    return sqrt(value);
}

double advanced_factorial(int n) {
    if (n < 0) return NAN;
    if (n == 0 || n == 1) return 1.0;
    
    double result = 1.0;
    for (int i = 2; i <= n; i++) {
        result *= i;
    }
    return result;
}

double advanced_percentage(double base, double percent) {
    return (base * percent) / 100.0;
}

double advanced_reciprocal(double value) {
    if (value == 0) return NAN;
    return 1.0 / value;
}

double advanced_negate(double value) {
    return -value;
}

double advanced_modulo(double a, double b) {
    if (b == 0) return NAN;
    return fmod(a, b);
}
