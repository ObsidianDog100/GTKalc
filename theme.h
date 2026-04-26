/*
 * GTKalc - Theme Management
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This header defines theme management for customization.
 */

#ifndef THEME_H
#define THEME_H

#include <gtk/gtk.h>
#include <glib.h>

// Theme structure
typedef struct {
    gchar *name;
    gchar *bg_color;
    gchar *display_bg;
    gchar *button_bg;
    gchar *button_hover;
    gchar *op_button_bg;
    gchar *op_button_hover;
    gchar *equals_bg;
    gchar *equals_hover;
    gchar *text_color;
} Theme;

// Theme manager
typedef struct {
    GPtrArray *themes;
    Theme *current_theme;
    gint current_index;
} ThemeManager;

// Function declarations
Theme *theme_new(const gchar *name,
                 const gchar *bg_color,
                 const gchar *display_bg,
                 const gchar *button_bg,
                 const gchar *button_hover,
                 const gchar *op_button_bg,
                 const gchar *op_button_hover,
                 const gchar *equals_bg,
                 const gchar *equals_hover,
                 const gchar *text_color);
void theme_free(Theme *theme);
ThemeManager *theme_manager_new(void);
void theme_manager_free(ThemeManager *manager);
void theme_manager_add_theme(ThemeManager *manager, Theme *theme);
void theme_manager_set_active_theme(ThemeManager *manager, gint index);
Theme *theme_manager_get_active_theme(ThemeManager *manager);
gint theme_manager_get_theme_count(ThemeManager *manager);
const gchar *theme_manager_get_theme_name(ThemeManager *manager, gint index);
GtkCssProvider *theme_manager_get_css_provider(ThemeManager *manager);

#endif // THEME_H
