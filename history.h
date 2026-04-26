/*
 * GTKalc - Calculation History
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines history management for calculations.
 */

#ifndef HISTORY_H
#define HISTORY_H

#include <glib.h>

// History entry structure
typedef struct {
    gchar *expression;     // Full expression (e.g., "5 + 3")
    double result;         // Result of the calculation
    gint64 timestamp;      // Unix timestamp
} HistoryEntry;

// History manager structure
typedef struct {
    GPtrArray *entries;    // Array of HistoryEntry pointers
    guint max_entries;     // Maximum history entries to keep
} HistoryManager;

// Function declarations
HistoryManager *history_manager_new(guint max_entries);
void history_manager_free(HistoryManager *manager);
void history_manager_add_entry(HistoryManager *manager, const gchar *expression, double result);
GPtrArray *history_manager_get_entries(HistoryManager *manager);
void history_manager_clear(HistoryManager *manager);
HistoryEntry *history_entry_new(const gchar *expression, double result);
void history_entry_free(HistoryEntry *entry);
void history_manager_save_to_file(HistoryManager *manager, const gchar *filename);
void history_manager_load_from_file(HistoryManager *manager, const gchar *filename);

#endif // HISTORY_H
