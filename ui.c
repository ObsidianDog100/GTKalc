/*
 * GTKalc - Enhanced User Interface Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This module implements the complete GTK4 user interface including:
 * - Multi-tab interface (Standard, Scientific, History)
 * - Real-time display and expression updates
 * - Settings dialog for customization
 * - Theme management with live switching
 * - Memory operations visualization
 * - History tracking and quick recall
 * 
 * The UI is built on GTK4 with modern styling through CSS providers.
 * All calculations are performed through callbacks connected to button signals.
 */

#include "ui.h"
#include "calculator.h"
#include "advanced.h"
#include "history.h"
#include "memory.h"
#include "theme.h"
#include "config.h"
#include <string.h>
#include <gdk/gdk.h>
#include <math.h>

/**
 * UIState - Central application state container.
 * 
 * Holds references to all major components:
 * - Calculator engine and state
 * - History, memory, theme, and config managers
 * - Display and control widgets
 * - User preferences (angle mode, etc.)
 * 
 * Passed as callback context to all button handlers.
 */
typedef struct {
    GtkWidget *display_label;          // Main numeric display
    GtkWidget *expression_label;       // Shows full expression (e.g., "5 + 3 =")
    CalculatorState *calc_state;       // Core calculator logic
    HistoryManager *history;           // Calculation history persistence
    MemoryManager *memory;             // Memory operations (M+, M-, etc.)
    ThemeManager *themes;              // Theme system with multiple presets
    Config *config;                    // User settings and preferences
    GtkWidget *scientific_box;         // Scientific functions container
    GtkWidget *programmer_box;         // Programmer mode container
    GtkWidget *history_listbox;        // History display and selection
    GtkWidget *memory_label;           // Current memory value display
    GtkWidget *mode_stack;             // Tab container for different modes
    gboolean angle_degrees;            // TRUE for degrees, FALSE for radians
} UIState;

// Forward declarations for internal functions
static void update_display(UIState *ui_state);
static void update_expression_label(UIState *ui_state);
static void update_history_view(UIState *ui_state);
static void show_settings_dialog(GtkWidget *parent, UIState *ui_state);
static void apply_theme(GdkDisplay *display, ThemeManager *themes);

/**
 * Digit button click handler.
 * Appends the digit (0-9) to the current number being entered.
 * Updates the display to show the new number.
 */
static void digit_button_clicked(GtkButton *button, UIState *ui_state) {
    const char *label = gtk_button_get_label(button);
    int digit = label[0] - '0';
    calculator_append_digit(ui_state->calc_state, digit);
    update_display(ui_state);
}


static void decimal_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_append_decimal(ui_state->calc_state);
    update_display(ui_state);
}

static void operation_button_clicked(GtkButton *button, UIState *ui_state) {
    const char *label = gtk_button_get_label(button);
    char op = label[0];
    calculator_set_operation(ui_state->calc_state, op);
    update_display(ui_state);
    update_expression_label(ui_state);
}

static void equals_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_calculate(ui_state->calc_state);
    update_display(ui_state);
    update_expression_label(ui_state);

    if (ui_state->history) {
        history_manager_add_entry(ui_state->history,
                                 calculator_get_expression(ui_state->calc_state),
                                 atof(calculator_get_display(ui_state->calc_state)));
        update_history_view(ui_state);

        if (ui_state->config && ui_state->config->save_history) {
            history_manager_save_to_file(ui_state->history, ui_state->config->history_file);
        }
    }
}

static void clear_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_clear(ui_state->calc_state);
    update_display(ui_state);
    update_expression_label(ui_state);
}

static void backspace_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_backspace(ui_state->calc_state);
    update_display(ui_state);
}

static void undo_button_clicked(GtkButton *button, UIState *ui_state) {
    if (calculator_can_undo(ui_state->calc_state)) {
        calculator_undo(ui_state->calc_state);
        update_display(ui_state);
    }
}

static void redo_button_clicked(GtkButton *button, UIState *ui_state) {
    if (calculator_can_redo(ui_state->calc_state)) {
        calculator_redo(ui_state->calc_state);
        update_display(ui_state);
    }
}

static void sin_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_sin(value, ui_state->angle_degrees);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void cos_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_cos(value, ui_state->angle_degrees);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void tan_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_tan(value, ui_state->angle_degrees);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void sqrt_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_sqrt(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void power_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_set_operation(ui_state->calc_state, '^');
    update_display(ui_state);
}

static void ln_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_ln(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void log_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_log10(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void reciprocal_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_reciprocal(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void negate_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_negate(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void factorial_button_clicked(GtkButton *button, UIState *ui_state) {
    int value = (int)atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_factorial(value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void percentage_button_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    double result = advanced_percentage(ui_state->calc_state->current_value, value);
    calculator_apply_advanced_function(ui_state->calc_state, result);
    update_display(ui_state);
}

static void pi_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_apply_advanced_function(ui_state->calc_state, M_PI);
    update_display(ui_state);
}

static void e_button_clicked(GtkButton *button, UIState *ui_state) {
    calculator_apply_advanced_function(ui_state->calc_state, M_E);
    update_display(ui_state);
}

// Memory button callbacks
static void memory_add_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    memory_manager_add(ui_state->memory, value);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "M: %.10g", memory_manager_get(ui_state->memory));
    gtk_label_set_text(GTK_LABEL(ui_state->memory_label), buffer);
}

static void memory_subtract_clicked(GtkButton *button, UIState *ui_state) {
    double value = atof(calculator_get_display(ui_state->calc_state));
    memory_manager_subtract(ui_state->memory, value);
    char buffer[64];
    snprintf(buffer, sizeof(buffer), "M: %.10g", memory_manager_get(ui_state->memory));
    gtk_label_set_text(GTK_LABEL(ui_state->memory_label), buffer);
}

static void memory_recall_clicked(GtkButton *button, UIState *ui_state) {
    if (memory_manager_has_value(ui_state->memory)) {
        double value = memory_manager_get(ui_state->memory);
        calculator_apply_advanced_function(ui_state->calc_state, value);
        update_display(ui_state);
    }
}

static void memory_clear_clicked(GtkButton *button, UIState *ui_state) {
    memory_manager_clear(ui_state->memory);
    gtk_label_set_text(GTK_LABEL(ui_state->memory_label), "M: 0");
}

static void toggle_angle_mode(GtkToggleButton *button, UIState *ui_state) {
    ui_state->angle_degrees = gtk_toggle_button_get_active(button);
    if (ui_state->config) {
        ui_state->config->angle_degrees = ui_state->angle_degrees;
    }
}

static void history_item_selected(GtkListBox *box, GtkListBoxRow *row, UIState *ui_state) {
    if (row == NULL) return;
    
    int index = gtk_list_box_row_get_index(row);
    HistoryEntry *entry = (HistoryEntry *)g_ptr_array_index(
        history_manager_get_entries(ui_state->history), index);
    
    if (entry) {
        char buffer[64];
        snprintf(buffer, sizeof(buffer), "%.10g", entry->result);
        g_string_assign(ui_state->calc_state->display, buffer);
        update_display(ui_state);
    }
}

static void update_display(UIState *ui_state) {
    gtk_label_set_text(GTK_LABEL(ui_state->display_label),
                      calculator_get_display(ui_state->calc_state));
}

static void update_expression_label(UIState *ui_state) {
    gtk_label_set_text(GTK_LABEL(ui_state->expression_label),
                      calculator_get_expression(ui_state->calc_state));
}

static void update_history_view(UIState *ui_state) {
    // Clear existing items
    GtkWidget *child;
    while ((child = gtk_widget_get_first_child(ui_state->history_listbox)) != NULL) {
        gtk_list_box_remove(GTK_LIST_BOX(ui_state->history_listbox), child);
    }

    GPtrArray *entries = history_manager_get_entries(ui_state->history);
    for (int i = (int)entries->len - 1; i >= 0; i--) {
        HistoryEntry *entry = (HistoryEntry *)g_ptr_array_index(entries, i);
        gchar *text = g_strdup_printf("%s = %.10g", entry->expression, entry->result);
        GtkWidget *label = gtk_label_new(text);
        gtk_label_set_xalign(GTK_LABEL(label), 0.0);
        gtk_widget_set_margin_start(label, 10);
        gtk_widget_set_margin_end(label, 10);
        gtk_widget_set_margin_top(label, 5);
        gtk_widget_set_margin_bottom(label, 5);
        
        gtk_list_box_append(GTK_LIST_BOX(ui_state->history_listbox), label);
        g_free(text);
    }
}

static GtkWidget *create_button(const char *label, GCallback callback, UIState *ui_state) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_add_css_class(button, "calc-button");
    g_signal_connect(button, "clicked", callback, ui_state);
    return button;
}

static GtkWidget *create_op_button(const char *label, GCallback callback, UIState *ui_state) {
    GtkWidget *button = create_button(label, callback, ui_state);
    gtk_widget_add_css_class(button, "calc-op-button");
    return button;
}

static void apply_theme(GdkDisplay *display, ThemeManager *themes) {
    GtkCssProvider *provider = theme_manager_get_css_provider(themes);
    if (provider) {
        gtk_style_context_add_provider_for_display(display,
            GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_object_unref(provider);
    }
}

static void theme_changed(GtkComboBoxText *combo, UIState *ui_state) {
    gint active = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
    if (active >= 0) {
        theme_manager_set_active_theme(ui_state->themes, active);
        apply_theme(gdk_display_get_default(), ui_state->themes);
        if (ui_state->config) {
            ui_state->config->theme_index = active;
        }
    }
}

static void show_settings_dialog(GtkWidget *parent, UIState *ui_state) {
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Settings",
        GTK_WINDOW(parent),
        GTK_DIALOG_MODAL,
        "Close", GTK_RESPONSE_OK,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_widget_set_margin_start(box, 15);
    gtk_widget_set_margin_end(box, 15);
    gtk_widget_set_margin_top(box, 15);
    gtk_widget_set_margin_bottom(box, 15);
    gtk_box_append(GTK_BOX(content), box);
    
    GtkWidget *theme_label = gtk_label_new("Theme:");
    gtk_label_set_xalign(GTK_LABEL(theme_label), 0.0);
    gtk_box_append(GTK_BOX(box), theme_label);
    
    GtkWidget *theme_combo = gtk_combo_box_text_new();
    for (gint i = 0; i < theme_manager_get_theme_count(ui_state->themes); i++) {
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(theme_combo),
                                       theme_manager_get_theme_name(ui_state->themes, i));
    }
    gtk_combo_box_set_active(GTK_COMBO_BOX(theme_combo), ui_state->themes->current_index);
    g_signal_connect(theme_combo, "changed", G_CALLBACK(theme_changed), ui_state);
    gtk_box_append(GTK_BOX(box), theme_combo);
    
    GtkWidget *angle_label = gtk_label_new("Angle Mode:");
    gtk_label_set_xalign(GTK_LABEL(angle_label), 0.0);
    gtk_box_append(GTK_BOX(box), angle_label);
    
    GtkWidget *angle_toggle = gtk_toggle_button_new_with_label(
        ui_state->angle_degrees ? "Degrees" : "Radians");
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(angle_toggle), ui_state->angle_degrees);
    g_signal_connect(angle_toggle, "toggled", G_CALLBACK(toggle_angle_mode), ui_state);
    gtk_box_append(GTK_BOX(box), angle_toggle);
    
    GtkWidget *save_history_check = gtk_check_button_new_with_label("Auto-save history");
    gtk_check_button_set_active(GTK_CHECK_BUTTON(save_history_check),
                               ui_state->config->save_history);
    gtk_box_append(GTK_BOX(box), save_history_check);
    
    gtk_widget_show(dialog);
    g_signal_connect(dialog, "response", G_CALLBACK(gtk_window_destroy), NULL);
}

static void on_window_destroy(GtkWidget *widget, UIState *ui_state) {
    // Save config
    if (ui_state->config) {
        gchar *config_dir = config_get_config_dir();
        gchar *config_file = g_build_filename(config_dir, "config.ini", NULL);
        config_save_to_file(ui_state->config, config_file);
        g_free(config_dir);
        g_free(config_file);
    }
    
    // Cleanup
    history_manager_free(ui_state->history);
    memory_manager_free(ui_state->memory);
    theme_manager_free(ui_state->themes);
    config_free(ui_state->config);
    calculator_free(ui_state->calc_state);
    g_free(ui_state);
}

static GtkWidget *create_standard_grid(UIState *ui_state) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);

    const char *buttons[5][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", ".", "~", "+"},
        {"C", "⌫", "↶", "↷"}
    };

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 4; ++col) {
            const char *label = buttons[row][col];
            GtkWidget *button = NULL;
            
            if (strcmp(label, "C") == 0) {
                button = create_button(label, G_CALLBACK(clear_button_clicked), ui_state);
                gtk_widget_add_css_class(button, "calc-clear");
            } else if (strcmp(label, "⌫") == 0) {
                button = create_button(label, G_CALLBACK(backspace_button_clicked), ui_state);
            } else if (strcmp(label, "↶") == 0) {
                button = create_button(label, G_CALLBACK(undo_button_clicked), ui_state);
            } else if (strcmp(label, "↷") == 0) {
                button = create_button(label, G_CALLBACK(redo_button_clicked), ui_state);
            } else if (strcmp(label, ".") == 0) {
                button = create_button(label, G_CALLBACK(decimal_button_clicked), ui_state);
            } else if (strcmp(label, "~") == 0) {
                // Skip placeholder - will be replaced with equals button below
                continue;
            } else if (strchr("+-*/", label[0])) {
                button = create_op_button(label, G_CALLBACK(operation_button_clicked), ui_state);
            } else {
                button = create_button(label, G_CALLBACK(digit_button_clicked), ui_state);
            }
            
            if (button) {
                gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
            }
        }
    }

    GtkWidget *equals_btn = create_button("=", G_CALLBACK(equals_button_clicked), ui_state);
    gtk_widget_add_css_class(equals_btn, "calc-equals");
    gtk_widget_set_size_request(equals_btn, -1, 150); 
    gtk_grid_attach(GTK_GRID(grid), equals_btn, 2, 3, 2, 2); 
    
    return grid;
}

static GtkWidget *create_scientific_grid(UIState *ui_state) {
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    
    GtkWidget *sin_btn = create_op_button("sin", G_CALLBACK(sin_button_clicked), ui_state);
    GtkWidget *cos_btn = create_op_button("cos", G_CALLBACK(cos_button_clicked), ui_state);
    GtkWidget *tan_btn = create_op_button("tan", G_CALLBACK(tan_button_clicked), ui_state);
    GtkWidget *sqrt_btn = create_op_button("√", G_CALLBACK(sqrt_button_clicked), ui_state);
    
    GtkWidget *ln_btn = create_op_button("ln", G_CALLBACK(ln_button_clicked), ui_state);
    GtkWidget *log_btn = create_op_button("log", G_CALLBACK(log_button_clicked), ui_state);
    GtkWidget *power_btn = create_op_button("x^y", G_CALLBACK(power_button_clicked), ui_state);
    GtkWidget *reciprocal_btn = create_op_button("1/x", G_CALLBACK(reciprocal_button_clicked), ui_state);
    
    GtkWidget *neg_btn = create_op_button("±", G_CALLBACK(negate_button_clicked), ui_state);
    GtkWidget *factorial_btn = create_op_button("n!", G_CALLBACK(factorial_button_clicked), ui_state);
    GtkWidget *percent_btn = create_op_button("%", G_CALLBACK(percentage_button_clicked), ui_state);
    GtkWidget *pi_btn = create_op_button("π", G_CALLBACK(pi_button_clicked), ui_state);
    
    GtkWidget *e_btn = create_op_button("e", G_CALLBACK(e_button_clicked), ui_state);
    GtkWidget *ma_btn = create_op_button("M+", G_CALLBACK(memory_add_clicked), ui_state);
    GtkWidget *ms_btn = create_op_button("M-", G_CALLBACK(memory_subtract_clicked), ui_state);
    GtkWidget *mr_btn = create_op_button("MR", G_CALLBACK(memory_recall_clicked), ui_state);
    
    GtkWidget *mc_btn = create_op_button("MC", G_CALLBACK(memory_clear_clicked), ui_state);
    
    gtk_grid_attach(GTK_GRID(grid), sin_btn, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), cos_btn, 1, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), tan_btn, 2, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), sqrt_btn, 3, 0, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), ln_btn, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), log_btn, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), power_btn, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), reciprocal_btn, 3, 1, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), neg_btn, 0, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), factorial_btn, 1, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), percent_btn, 2, 2, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), pi_btn, 3, 2, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), e_btn, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ma_btn, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), ms_btn, 2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), mr_btn, 3, 3, 1, 1);
    
    gtk_grid_attach(GTK_GRID(grid), mc_btn, 0, 4, 1, 1);
    
    return grid;
}

// calculator window 
GtkWidget *create_calculator_window(GtkApplication *app, CalculatorState *state) {
    // Create UI state
    UIState *ui_state = g_new0(UIState, 1);
    ui_state->calc_state = state;
    ui_state->angle_degrees = TRUE;
    
    ui_state->history = history_manager_new(100);
    ui_state->memory = memory_manager_new();
    ui_state->themes = theme_manager_new();
    ui_state->config = config_new();

    gchar *config_dir = config_get_config_dir();
    gchar *config_file = g_build_filename(config_dir, "config.ini", NULL);
    if (g_file_test(config_file, G_FILE_TEST_EXISTS)) {
        config_load_from_file(ui_state->config, config_file);
    }
    g_free(config_dir);
    g_free(config_file);
    
    // Load history if exists
    gchar *history_file = config_get_history_file();
    if (g_file_test(history_file, G_FILE_TEST_EXISTS)) {
        history_manager_load_from_file(ui_state->history, history_file);
    }
    g_free(history_file);
    
    ui_state->angle_degrees = ui_state->config->angle_degrees;
    theme_manager_set_active_theme(ui_state->themes, ui_state->config->theme_index);
    
    GtkWidget *window = gtk_application_window_new(app);
    gtk_widget_add_css_class(window, "calc-window");
    gtk_window_set_title(GTK_WINDOW(window), "GTKalc - Advanced Calculator");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 700);
    gtk_window_set_resizable(GTK_WINDOW(window), TRUE);
    
    GdkDisplay *display = gdk_display_get_default();
    apply_theme(display, ui_state->themes);

    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), ui_state);

    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 10);
    gtk_widget_set_margin_end(main_box, 10);
    gtk_widget_set_margin_top(main_box, 10);
    gtk_widget_set_margin_bottom(main_box, 10);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    GtkWidget *header_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_append(GTK_BOX(main_box), header_box);

    GtkWidget *settings_btn = gtk_button_new_from_icon_name("emblem-system");
    g_signal_connect(settings_btn, "clicked", G_CALLBACK(show_settings_dialog), ui_state);
    gtk_box_append(GTK_BOX(header_box), settings_btn);
    
    gtk_widget_set_hexpand(header_box, TRUE);

    ui_state->expression_label = gtk_label_new("");
    gtk_label_set_xalign(GTK_LABEL(ui_state->expression_label), 1.0);
    gtk_widget_set_halign(ui_state->expression_label, GTK_ALIGN_FILL);
    gtk_widget_add_css_class(ui_state->expression_label, "calc-expression");
    gtk_box_append(GTK_BOX(main_box), ui_state->expression_label);

    ui_state->display_label = gtk_label_new("0");
    gtk_label_set_xalign(GTK_LABEL(ui_state->display_label), 1.0);
    gtk_widget_set_halign(ui_state->display_label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(ui_state->display_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(ui_state->display_label, 10);
    gtk_widget_set_size_request(ui_state->display_label, -1, 80);
    gtk_widget_add_css_class(ui_state->display_label, "calc-display");
    gtk_box_append(GTK_BOX(main_box), ui_state->display_label);

    ui_state->memory_label = gtk_label_new("M: 0");
    gtk_label_set_xalign(GTK_LABEL(ui_state->memory_label), 0.0);
    gtk_widget_add_css_class(ui_state->memory_label, "calc-memory");
    gtk_box_append(GTK_BOX(main_box), ui_state->memory_label);

    GtkWidget *mode_notebook = gtk_notebook_new();
    gtk_box_append(GTK_BOX(main_box), mode_notebook);

    GtkWidget *standard_grid = create_standard_grid(ui_state);
    gtk_notebook_append_page(GTK_NOTEBOOK(mode_notebook), standard_grid,
                            gtk_label_new("Standard"));

    GtkWidget *scientific_grid = create_scientific_grid(ui_state);
    gtk_notebook_append_page(GTK_NOTEBOOK(mode_notebook), scientific_grid,
                            gtk_label_new("Scientific"));

    GtkWidget *history_scroll = gtk_scrolled_window_new();
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(history_scroll),
                                   GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    ui_state->history_listbox = gtk_list_box_new();
    gtk_list_box_set_selection_mode(GTK_LIST_BOX(ui_state->history_listbox),
                                    GTK_SELECTION_SINGLE);
    g_signal_connect(ui_state->history_listbox, "row-selected",
                    G_CALLBACK(history_item_selected), ui_state);
    gtk_scrolled_window_set_child(GTK_SCROLLED_WINDOW(history_scroll), ui_state->history_listbox);
    gtk_notebook_append_page(GTK_NOTEBOOK(mode_notebook), history_scroll,
                            gtk_label_new("History"));

    update_history_view(ui_state);
    
    return window;
}