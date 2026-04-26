/*
 * GTKalc - Memory Management
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines memory operations for the calculator.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <glib.h>

// Memory manager structure
typedef struct {
    double value;
    gboolean has_value;
} MemoryManager;

// Function declarations
MemoryManager *memory_manager_new(void);
void memory_manager_free(MemoryManager *manager);
void memory_manager_set(MemoryManager *manager, double value);
void memory_manager_add(MemoryManager *manager, double value);
void memory_manager_subtract(MemoryManager *manager, double value);
double memory_manager_get(MemoryManager *manager);
void memory_manager_clear(MemoryManager *manager);
gboolean memory_manager_has_value(MemoryManager *manager);

#endif // MEMORY_H
