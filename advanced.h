/*
 * GTKalc - Advanced Mathematical Functions
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines advanced mathematical operations for scientific mode.
 */

#ifndef ADVANCED_H
#define ADVANCED_H

#include <glib.h>

// Trigonometric functions
double advanced_sin(double angle, gboolean degrees);
double advanced_cos(double angle, gboolean degrees);
double advanced_tan(double angle, gboolean degrees);
double advanced_asin(double value);
double advanced_acos(double value);
double advanced_atan(double value);

// Logarithmic functions
double advanced_log10(double value);
double advanced_ln(double value);
double advanced_pow(double base, double exponent);

// Utility functions
double advanced_sqrt(double value);
double advanced_factorial(int n);
double advanced_percentage(double base, double percent);
double advanced_reciprocal(double value);
double advanced_negate(double value);
double advanced_modulo(double a, double b);

#endif // ADVANCED_H
