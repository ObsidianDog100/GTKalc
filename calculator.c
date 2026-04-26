/*
 * GTKalc - Calculator Logic Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 */

#include "calculator.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

latorState *calculator_init(void) {
    CalculatorState *state = g_new(CalculatorState, 1);
    state->display = g_string_new("0");
    state->expression = g_string_new("");
    state->current_value = 0.0;
    state->operation = '\0';
    state->has_operation = FALSE;
    state->clear_next = FALSE;
    state->undo_stack = g_queue_new();
    state->redo_stack = g_queue_new();
    return state;
}

void calculator_free(CalculatorState *state) {
    if (state) {
        g_string_free(state->display, TRUE);
        g_string_free(state->expression, TRUE);
        
        // Free undo/redo stacks and their contents
        g_queue_free_full(state->undo_stack, g_free);
        g_queue_free_full(state->redo_stack, g_free);
        
        g_free(state);
    }
}

static void save_to_undo_stack(CalculatorState *state) {
    // Create a copy of current display state
    GString *saved = g_string_new(state->display->str);
    g_queue_push_head(state->undo_stack, saved);
    
    // Clear redo stack when a new operation is performed
    g_queue_free_full(state->redo_stack, g_free);
    state->redo_stack = g_queue_new();
    
    // Limit undo stack to prevent excessive memory usage
    while (g_queue_get_length(state->undo_stack) > 50) {
        g_free(g_queue_pop_tail(state->undo_stack));
    }
}

void calculator_append_digit(CalculatorState *state, int digit) {
    if (state->clear_next) {
        save_to_undo_stack(state);
        g_string_assign(state->display, "");
        state->clear_next = FALSE;
    }
    
    // Remove leading zero when entering new digit
    if (strcmp(state->display->str, "0") == 0) {
        g_string_assign(state->display, "");
    }
    
    g_string_append_c(state->display, '0' + digit);
}

void calculator_append_decimal(CalculatorState *state) {
    if (state->clear_next) {
        save_to_undo_stack(state);
        g_string_assign(state->display, "0");
        state->clear_next = FALSE;
    }
    
    // Only add decimal if one doesn't already exist
    if (strchr(state->display->str, '.') == NULL) {
        g_string_append_c(state->display, '.');
    }
}

void calculator_set_operation(CalculatorState *state, char op) {
    // If operation was already pending, calculate intermediate result
    if (state->has_operation) {
        calculator_calculate(state);
    }
    
    // Save the first operand
    state->current_value = atof(state->display->str);
    
    // Build expression string for display
    char op_str[2];
    snprintf(op_str, sizeof(op_str), "%c", op);
    g_string_assign(state->expression, state->display->str);
    g_string_append(state->expression, " ");
    g_string_append(state->expression, op_str);
    g_string_append(state->expression, " ");
    
    state->operation = op;
    state->has_operation = TRUE;
    state->clear_next = TRUE;  // Next digit clears display
}

void calculator_calculate(CalculatorState *state) {
    if (!state->has_operation) return;

    double second_value = atof(state->display->str);
    double result = 0.0;

    // Perform the pending operation
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
                // Display error message for division by zero
                g_string_assign(state->display, "Error");
                state->has_operation = FALSE;
                return;
            }
            break;
        default:
            return;
    }

    // Update expression to show complete calculation
    g_string_append(state->expression, state->display->str);
    g_string_append(state->expression, " = ");
    
    // Format result with up to 10 significant digits, removing trailing zeros
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.10g", result);
    g_string_assign(state->display, buffer);
    g_string_append(state->expression, buffer);

    state->current_value = result;
    state->has_operation = FALSE;
    state->clear_next = TRUE;  // Ready for next calculation
}

void calculator_clear(CalculatorState *state) {
    save_to_undo_stack(state);
    g_string_assign(state->display, "0");
    g_string_assign(state->expression, "");
    state->current_value = 0.0;
    state->operation = '\0';
    state->has_operation = FALSE;
    state->clear_next = FALSE;
}

const char *calculator_get_display(CalculatorState *state) {
    return state->display->str;
}

const char *calculator_get_expression(CalculatorState *state) {
    return state->expression->str;
}

void calculator_apply_advanced_function(CalculatorState *state, double value) {
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "%.10g", value);
    
    save_to_undo_stack(state);
    g_string_assign(state->display, buffer);
    state->current_value = value;
    state->clear_next = TRUE;
}

void calculator_backspace(CalculatorState *state) {
    if (state->display->len > 0) {
        save_to_undo_stack(state);
        g_string_truncate(state->display, state->display->len - 1);
        if (state->display->len == 0) {
            g_string_assign(state->display, "0");
        }
    }
}

void calculator_undo(CalculatorState *state) {
    if (g_queue_is_empty(state->undo_stack)) return;
    
    // Push current state to redo stack
    GString *current = g_string_new(state->display->str);
    g_queue_push_head(state->redo_stack, current);
    
    // Pop previous state from undo stack
    GString *previous = (GString *)g_queue_pop_head(state->undo_stack);
    g_string_assign(state->display, previous->str);
    g_string_free(previous, TRUE);
}

void calculator_redo(CalculatorState *state) {
    if (g_queue_is_empty(state->redo_stack)) return;
    
    // Push current state to undo stack
    GString *current = g_string_new(state->display->str);
    g_queue_push_head(state->undo_stack, current);
    
    // Pop next state from redo stack
    GString *next = (GString *)g_queue_pop_head(state->redo_stack);
    g_string_assign(state->display, next->str);
    g_string_free(next, TRUE);
}

gboolean calculator_can_undo(CalculatorState *state) {
    return !g_queue_is_empty(state->undo_stack);
}

gboolean calculator_can_redo(CalculatorState *state) {
    return !g_queue_is_empty(state->redo_stack);
}