/*
 * GTKalc - Theme Management Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * Implements theme customization features.
 */

#include "theme.h"
#include <stdlib.h>
#include <string.h>

Theme *theme_new(const gchar *name,
                 const gchar *bg_color,
                 const gchar *display_bg,
                 const gchar *button_bg,
                 const gchar *button_hover,
                 const gchar *op_button_bg,
                 const gchar *op_button_hover,
                 const gchar *equals_bg,
                 const gchar *equals_hover,
                 const gchar *text_color) {
    Theme *theme = g_new(Theme, 1);
    theme->name = g_strdup(name);
    theme->bg_color = g_strdup(bg_color);
    theme->display_bg = g_strdup(display_bg);
    theme->button_bg = g_strdup(button_bg);
    theme->button_hover = g_strdup(button_hover);
    theme->op_button_bg = g_strdup(op_button_bg);
    theme->op_button_hover = g_strdup(op_button_hover);
    theme->equals_bg = g_strdup(equals_bg);
    theme->equals_hover = g_strdup(equals_hover);
    theme->text_color = g_strdup(text_color);
    return theme;
}

void theme_free(Theme *theme) {
    if (theme) {
        g_free(theme->name);
        g_free(theme->bg_color);
        g_free(theme->display_bg);
        g_free(theme->button_bg);
        g_free(theme->button_hover);
        g_free(theme->op_button_bg);
        g_free(theme->op_button_hover);
        g_free(theme->equals_bg);
        g_free(theme->equals_hover);
        g_free(theme->text_color);
        g_free(theme);
    }
}

ThemeManager *theme_manager_new(void) {
    ThemeManager *manager = g_new(ThemeManager, 1);
    manager->themes = g_ptr_array_new_with_free_func((GDestroyNotify)theme_free);
    manager->current_theme = NULL;
    manager->current_index = -1;

    g_ptr_array_add(manager->themes, theme_new("Dark",
        "#161824", "#212438", "#2c3349", "#3c4668",
        "#ff9f43", "#ff8f00", "#33d17a", "#2ebf6d", "#e8ecff"));
    
    g_ptr_array_add(manager->themes, theme_new("Light",
        "#f5f5f5", "#ffffff", "#e0e0e0", "#d0d0d0",
        "#ffa500", "#ff8c00", "#4caf50", "#45a049", "#333333"));
    
    g_ptr_array_add(manager->themes, theme_new("Blue",
        "#0d1b2a", "#1b263b", "#1e3a5f", "#2a4a7f",
        "#457b9d", "#3a6b8f", "#2a9d8f", "#21857f", "#f1faee"));
    
    g_ptr_array_add(manager->themes, theme_new("Purple",
        "#1a0033", "#330066", "#4d0099", "#6600cc",
        "#9933ff", "#8811ff", "#00ccff", "#00bbee", "#ffccff"));
    
    manager->current_index = 0;
    manager->current_theme = (Theme *)g_ptr_array_index(manager->themes, 0);
    
    return manager;
}

void theme_manager_free(ThemeManager *manager) {
    if (manager) {
        g_ptr_array_free(manager->themes, TRUE);
        g_free(manager);
    }
}

void theme_manager_add_theme(ThemeManager *manager, Theme *theme) {
    if (manager && theme) {
        g_ptr_array_add(manager->themes, theme);
    }
}

void theme_manager_set_active_theme(ThemeManager *manager, gint index) {
    if (manager && index >= 0 && index < (gint)manager->themes->len) {
        manager->current_index = index;
        manager->current_theme = (Theme *)g_ptr_array_index(manager->themes, index);
    }
}

Theme *theme_manager_get_active_theme(ThemeManager *manager) {
    return manager ? manager->current_theme : NULL;
}

gint theme_manager_get_theme_count(ThemeManager *manager) {
    return manager ? (gint)manager->themes->len : 0;
}

const gchar *theme_manager_get_theme_name(ThemeManager *manager, gint index) {
    if (!manager || index < 0 || index >= (gint)manager->themes->len) {
        return NULL;
    }
    Theme *theme = (Theme *)g_ptr_array_index(manager->themes, index);
    return theme->name;
}

GtkCssProvider *theme_manager_get_css_provider(ThemeManager *manager) {
    if (!manager || !manager->current_theme) {
        return NULL;
    }
    
    Theme *theme = manager->current_theme;
    GtkCssProvider *provider = gtk_css_provider_new();
    
    gchar *css = g_strdup_printf(
        ".calc-window { background-color: %s; }\n"
        ".calc-display { background-color: %s; color: %s; font-size: 28px; font-weight: 700; border: 2px solid rgba(255,255,255,0.15); padding: 18px; border-radius: 18px; min-height: 80px; }\n"
        ".calc-expression { color: %s; font-size: 12px; opacity: 0.7; }\n"
        ".calc-memory { color: %s; font-size: 11px; font-weight: 600; opacity: 0.8; }\n"
        ".calc-button { font-size: 18px; padding: 16px; background-color: %s; border: 3px solid rgba(255,255,255,0.12); border-radius: 16px; color: %s; min-width: 70px; min-height: 70px; font-weight: 600; transition: all 0.15s ease-out; }\n"
        ".calc-button:hover { background-color: %s; border: 3px solid rgba(255,255,255,0.25); transform: translateY(-2px); }\n"
        ".calc-button:active { transform: translateY(0px); }\n"
        ".calc-op-button { background-color: %s; color: #ffffff; font-weight: 700; border: 3px solid rgba(255,255,255,0.25); box-shadow: inset 0 2px 4px rgba(0,0,0,0.3); }\n"
        ".calc-op-button:hover { background-color: %s; border: 3px solid rgba(255,255,255,0.4); transform: translateY(-2px); }\n"
        ".calc-equals { background-color: %s; color: #ffffff; font-weight: 700; border: 3px solid rgba(255,255,255,0.3); font-size: 24px; box-shadow: 0 4px 12px rgba(51, 209, 122, 0.3), inset 0 2px 4px rgba(0,0,0,0.2); }\n"
        ".calc-equals:hover { background-color: %s; border: 3px solid rgba(255,255,255,0.5); transform: translateY(-3px); box-shadow: 0 6px 16px rgba(51, 209, 122, 0.4), inset 0 2px 4px rgba(0,0,0,0.2); }\n"
        ".calc-equals:active { transform: translateY(-1px); }\n"
        ".calc-clear { background-color: #ff5c63; color: #ffffff; font-weight: 700; border: 3px solid rgba(255,255,255,0.2); }\n"
        ".calc-clear:hover { background-color: #e74c54; border: 3px solid rgba(255,255,255,0.35); transform: translateY(-2px); }\n"
        "notebook { background-color: %s; }\n"
        "notebook tab { padding: 12px 24px; font-weight: 600; }\n",
        theme->bg_color, theme->display_bg, theme->text_color,
        theme->text_color, theme->text_color,
        theme->button_bg, theme->text_color, theme->button_hover,
        theme->op_button_bg, theme->op_button_hover,
        theme->equals_bg, theme->equals_hover,
        theme->bg_color);
    
    gtk_css_provider_load_from_data(provider, css, -1);
    g_free(css);
    
    return provider;
}
