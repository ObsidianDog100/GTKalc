/*
 * GTKalc - Configuration Management
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines configuration management for user preferences.
 */

#ifndef CONFIG_H
#define CONFIG_H

#include <gtk/gtk.h>
#include <glib.h>

// Calculator modes
typedef enum {
    MODE_STANDARD,
    MODE_SCIENTIFIC,
    MODE_PROGRAMMER
} CalculatorMode;

//Config structre
typedef struct {
    CalculatorMode mode;
    gboolean angle_degrees;     // TRUE for degrees, FALSE for radians
    gboolean show_history;      // Show history panel
    gint theme_index;           // Currently selected theme
    gchar *history_file;        // Path to history file
    gboolean save_history;      // Auto-save history
    gint window_width;
    gint window_height;
} Config;

// Function declarations
Config *config_new(void);
void config_free(Config *config);
void config_save_to_file(Config *config, const gchar *filename);
void config_load_from_file(Config *config, const gchar *filename);
gchar *config_get_config_dir(void);
gchar *config_get_history_file(void);

#endif // CONFIG_H
