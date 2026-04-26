/*
 * GTKalc - A GTK4 Calculator Application
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header file defines the calculator logic functions.
 */

#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <glib.h>

// Calculator state structure
typedef struct {
    GString *display;      // Current display text
    double current_value;  // Current numerical value
    char operation;        // Pending operation (+, -, *, /)
    gboolean has_operation; // Whether an operation is pending
    gboolean clear_next;   // Whether to clear display on next input
} CalculatorState;

// Function declarations
CalculatorState *calculator_init(void);
void calculator_free(CalculatorState *state);
void calculator_append_digit(CalculatorState *state, int digit);
void calculator_append_decimal(CalculatorState *state);
void calculator_set_operation(CalculatorState *state, char op);
void calculator_calculate(CalculatorState *state);
void calculator_clear(CalculatorState *state);
const char *calculator_get_display(CalculatorState *state);

#endif // CALCULATOR_H