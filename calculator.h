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

typedef struct {
    GString *display; 
    GString *expression;  
    double current_value;  
    char operation;       
    gboolean has_operation; 
    gboolean clear_next;    
    GQueue *undo_stack;     
    GQueue *redo_stack;     
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
const char *calculator_get_expression(CalculatorState *state);
void calculator_apply_advanced_function(CalculatorState *state, double value);
void calculator_backspace(CalculatorState *state);
void calculator_undo(CalculatorState *state);
void calculator_redo(CalculatorState *state);
gboolean calculator_can_undo(CalculatorState *state);
gboolean calculator_can_redo(CalculatorState *state);

#endif // CALCULATOR_H