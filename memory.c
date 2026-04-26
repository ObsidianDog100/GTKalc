/*
 * GTKalc - Memory Management Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * Implements calculator memory operations.
 */

#include "memory.h"
#include <stdlib.h>

// Create a new memory manager
MemoryManager *memory_manager_new(void) {
    MemoryManager *manager = g_new(MemoryManager, 1);
    manager->value = 0.0;
    manager->has_value = FALSE;
    return manager;
}

// Free the memory manager
void memory_manager_free(MemoryManager *manager) {
    if (manager) {
        g_free(manager);
    }
}

// Set memory value
void memory_manager_set(MemoryManager *manager, double value) {
    if (manager) {
        manager->value = value;
        manager->has_value = TRUE;
    }
}

// Add to memory
void memory_manager_add(MemoryManager *manager, double value) {
    if (manager) {
        manager->value += value;
        manager->has_value = TRUE;
    }
}

// Subtract from memory
void memory_manager_subtract(MemoryManager *manager, double value) {
    if (manager) {
        manager->value -= value;
        manager->has_value = TRUE;
    }
}

// Get memory value
double memory_manager_get(MemoryManager *manager) {
    return manager ? manager->value : 0.0;
}

// Clear memory
void memory_manager_clear(MemoryManager *manager) {
    if (manager) {
        manager->value = 0.0;
        manager->has_value = FALSE;
    }
}

// Check if memory has a value
gboolean memory_manager_has_value(MemoryManager *manager) {
    return manager ? manager->has_value : FALSE;
}
