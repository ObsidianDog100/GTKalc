/*
 * GTKalc - Calculator Logic Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This file implements the calculator's mathematical operations and state management.
 */

#include "calculator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

// Initialize calculator state
CalculatorState *calculator_init(void) {
    CalculatorState *state = g_new(CalculatorState, 1);
    state->display = g_string_new("0");
    state->current_value = 0.0;
    state->operation = '\0';
    state->has_operation = FALSE;
    state->clear_next = FALSE;
    return state;
}

// Free calculator state
void calculator_free(CalculatorState *state) {
    if (state) {
        g_string_free(state->display, TRUE);
        g_free(state);
    }
}

// Append a digit to the display
void calculator_append_digit(CalculatorState *state, int digit) {
    if (state->clear_next) {
        g_string_assign(state->display, "");
        state->clear_next = FALSE;
    }
    if (strcmp(state->display->str, "0") == 0) {
        g_string_assign(state->display, "");
    }
    g_string_append_c(state->display, '0' + digit);
}

// Append decimal point
void calculator_append_decimal(CalculatorState *state) {
    if (state->clear_next) {
        g_string_assign(state->display, "0");
        state->clear_next = FALSE;
    }
    if (strchr(state->display->str, '.') == NULL) {
        g_string_append_c(state->display, '.');
    }
}

// Set the operation
void calculator_set_operation(CalculatorState *state, char op) {
    if (state->has_operation) {
        calculator_calculate(state);
    }
    state->current_value = atof(state->display->str);
    state->operation = op;
    state->has_operation = TRUE;
    state->clear_next = TRUE;
}

// Perform the calculation
void calculator_calculate(CalculatorState *state) {
    if (!state->has_operation) return;

    double second_value = atof(state->display->str);
    double result = 0.0;

    switch (state->operation) {
        case '+':
            result = state->current_value + second_value;
            break;
        case '-':
            result = state->current_value - second_value;
            break;
        case '*':
            result = state->current_value * second_value;
            break;
        case '/':
            if (second_value != 0.0) {
                result = state->current_value / second_value;
            } else {
                g_string_assign(state->display, "Error");
                state->has_operation = FALSE;
                return;
            }
            break;
        default:
            return;
    }

    // Format result, remove trailing zeros
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.10g", result);
    g_string_assign(state->display, buffer);

    state->current_value = result;
    state->has_operation = FALSE;
    state->clear_next = TRUE;
}

// Clear the calculator
void calculator_clear(CalculatorState *state) {
    g_string_assign(state->display, "0");
    state->current_value = 0.0;
    state->operation = '\0';
    state->has_operation = FALSE;
    state->clear_next = FALSE;
}

// Get the current display text
const char *calculator_get_display(CalculatorState *state) {
    return state->display->str;
}