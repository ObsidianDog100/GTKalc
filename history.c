/*
 * GTKalc - Calculation History Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * Implements calculation history tracking and management.
 */

#include "history.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

HistoryEntry *history_entry_new(const gchar *expression, double result) {
    HistoryEntry *entry = g_new(HistoryEntry, 1);
    entry->expression = g_strdup(expression);
    entry->result = result;
    entry->timestamp = g_get_real_time() / 1000000; // Convert to seconds
    return entry;
}

void history_entry_free(HistoryEntry *entry) {
    if (entry) {
        g_free(entry->expression);
        g_free(entry);
    }
}

// Create a new history manager
HistoryManager *history_manager_new(guint max_entries) {
    HistoryManager *manager = g_new(HistoryManager, 1);
    manager->entries = g_ptr_array_new_with_free_func((GDestroyNotify)history_entry_free);
    manager->max_entries = max_entries > 0 ? max_entries : 100;
    return manager;
}

void history_manager_free(HistoryManager *manager) {
    if (manager) {
        g_ptr_array_free(manager->entries, TRUE);
        g_free(manager);
    }
}

void history_manager_add_entry(HistoryManager *manager, const gchar *expression, double result) {
    if (!manager) return;
    
    HistoryEntry *entry = history_entry_new(expression, result);
    g_ptr_array_add(manager->entries, entry);
    
    // Remove oldest entries if we exceed max_entries
    while (manager->entries->len > manager->max_entries) {
        g_ptr_array_remove_index(manager->entries, 0);
    }
}

GPtrArray *history_manager_get_entries(HistoryManager *manager) {
    return manager ? manager->entries : NULL;
}

void history_manager_clear(HistoryManager *manager) {
    if (manager) {
        g_ptr_array_remove_range(manager->entries, 0, manager->entries->len);
    }
}

void history_manager_save_to_file(HistoryManager *manager, const gchar *filename) {
    if (!manager || !filename) return;
    
    FILE *file = fopen(filename, "w");
    if (!file) return;
    
    for (guint i = 0; i < manager->entries->len; i++) {
        HistoryEntry *entry = (HistoryEntry *)g_ptr_array_index(manager->entries, i);
        fprintf(file, "%s|%.10g|%ld\n", entry->expression, entry->result, entry->timestamp);
    }
    
    fclose(file);
}

void history_manager_load_from_file(HistoryManager *manager, const gchar *filename) {
    if (!manager || !filename) return;
    
    FILE *file = fopen(filename, "r");
    if (!file) return;
    
    gchar line[512];
    while (fgets(line, sizeof(line), file)) {
        gchar **parts = g_strsplit(line, "|", 3);
        if (g_strv_length(parts) >= 2) {
            gchar *expression = parts[0];
            double result = g_strtod(parts[1], NULL);
            
            HistoryEntry *entry = history_entry_new(expression, result);
            if (parts[2]) {
                entry->timestamp = strtol(parts[2], NULL, 10);
            }
            g_ptr_array_add(manager->entries, entry);
        }
        g_strfreev(parts);
    }
    
    fclose(file);
}
