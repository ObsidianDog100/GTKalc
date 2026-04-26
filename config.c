/*
 * GTKalc - Configuration Management Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * Implements configuration management and persistence.
 */

#include "config.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <glib/gstdio.h>

Config *config_new(void) {
    Config *config = g_new(Config, 1);
    config->mode = MODE_STANDARD;
    config->angle_degrees = TRUE;
    config->show_history = FALSE;
    config->theme_index = 0;
    config->save_history = TRUE;
    config->window_width = 320;
    config->window_height = 450;
    
    gchar *config_dir = config_get_config_dir();
    config->history_file = g_build_filename(config_dir, "history.txt", NULL);
    g_free(config_dir);
    
    return config;
}

void config_free(Config *config) {
    if (config) {
        g_free(config->history_file);
        g_free(config);
    }
}

gchar *config_get_config_dir(void) {
    gchar *config_home = g_build_filename(
        g_get_user_config_dir(),
        "gtkalc",
        NULL);
    
    // Create directory if it doesn't exist
    g_mkdir_with_parents(config_home, 0755);
    
    return config_home;
}

gchar *config_get_history_file(void) {
    gchar *config_dir = config_get_config_dir();
    gchar *history_file = g_build_filename(config_dir, "history.txt", NULL);
    g_free(config_dir);
    return history_file;
}

void config_save_to_file(Config *config, const gchar *filename) {
    if (!config || !filename) return;
    
    FILE *file = fopen(filename, "w");
    if (!file) return;
    
    fprintf(file, "[Calculator]\n");
    fprintf(file, "mode=%d\n", (int)config->mode);
    fprintf(file, "angle_degrees=%d\n", config->angle_degrees ? 1 : 0);
    fprintf(file, "show_history=%d\n", config->show_history ? 1 : 0);
    fprintf(file, "theme_index=%d\n", config->theme_index);
    fprintf(file, "save_history=%d\n", config->save_history ? 1 : 0);
    fprintf(file, "window_width=%d\n", config->window_width);
    fprintf(file, "window_height=%d\n", config->window_height);
    
    fclose(file);
}

void config_load_from_file(Config *config, const gchar *filename) {
    if (!config || !filename) return;
    
    FILE *file = fopen(filename, "r");
    if (!file) return;
    
    gchar line[256];
    while (fgets(line, sizeof(line), file)) {
        g_strstrip(line);
        
        if (sscanf(line, "mode=%d", (int *)&config->mode) == 1) continue;
        
        int bool_val;
        if (sscanf(line, "angle_degrees=%d", &bool_val) == 1) {
            config->angle_degrees = (bool_val != 0);
            continue;
        }
        if (sscanf(line, "show_history=%d", &bool_val) == 1) {
            config->show_history = (bool_val != 0);
            continue;
        }
        if (sscanf(line, "save_history=%d", &bool_val) == 1) {
            config->save_history = (bool_val != 0);
            continue;
        }
        
        if (sscanf(line, "theme_index=%d", &config->theme_index) == 1) continue;
        if (sscanf(line, "window_width=%d", &config->window_width) == 1) continue;
        if (sscanf(line, "window_height=%d", &config->window_height) == 1) continue;
    }
    
    fclose(file);
}
