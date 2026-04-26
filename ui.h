/*
 * GTKalc - UI Header
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines the UI building functions for the calculator.
 */

#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include "calculator.h"

// Function to create the main calculator window
GtkWidget *create_calculator_window(GtkApplication *app, CalculatorState *state);

#endif // UI_H