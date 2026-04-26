/*
 * GTKalc - UI Implementation
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This file builds the graphical user interface for the calculator using GTK4.
 */

#include "ui.h"
#include "calculator.h"
#include <string.h>
#include <gdk/gdk.h>

// Callback functions for buttons
static void digit_button_clicked(GtkButton *button, CalculatorState *state) {
    const char *label = gtk_button_get_label(button);
    int digit = label[0] - '0';
    calculator_append_digit(state, digit);
    // Update display - we'll need to pass the label
}

static void decimal_button_clicked(GtkButton *button, gpointer user_data) {
    CalculatorState *state = (CalculatorState *)user_data;
    calculator_append_decimal(state);
}

static void operation_button_clicked(GtkButton *button, gpointer user_data) {
    CalculatorState *state = (CalculatorState *)user_data;
    const char *label = gtk_button_get_label(button);
    char op = label[0];
    calculator_set_operation(state, op);
}

static void equals_button_clicked(GtkButton *button, gpointer user_data) {
    CalculatorState *state = (CalculatorState *)user_data;
    calculator_calculate(state);
}

static void clear_button_clicked(GtkButton *button, gpointer user_data) {
    CalculatorState *state = (CalculatorState *)user_data;
    calculator_clear(state);
}

// Structure to hold UI elements
typedef struct {
    GtkWidget *display_label;
    CalculatorState *state;
} UIState;

// Update display function
static void update_display(UIState *ui_state) {
    gtk_label_set_text(GTK_LABEL(ui_state->display_label), calculator_get_display(ui_state->state));
}

// Modified callbacks to update display
static void digit_button_clicked_with_update(GtkButton *button, UIState *ui_state) {
    const char *label = gtk_button_get_label(button);
    int digit = label[0] - '0';
    calculator_append_digit(ui_state->state, digit);
    update_display(ui_state);
}

static void decimal_button_clicked_with_update(GtkButton *button, UIState *ui_state) {
    calculator_append_decimal(ui_state->state);
    update_display(ui_state);
}

static void operation_button_clicked_with_update(GtkButton *button, UIState *ui_state) {
    const char *label = gtk_button_get_label(button);
    char op = label[0];
    calculator_set_operation(ui_state->state, op);
    update_display(ui_state);
}

static void equals_button_clicked_with_update(GtkButton *button, UIState *ui_state) {
    calculator_calculate(ui_state->state);
    update_display(ui_state);
}

static void clear_button_clicked_with_update(GtkButton *button, UIState *ui_state) {
    calculator_clear(ui_state->state);
    update_display(ui_state);
}

// Create a button with callback
static GtkWidget *create_button(const char *label, GCallback callback, UIState *ui_state) {
    GtkWidget *button = gtk_button_new_with_label(label);
    gtk_widget_add_css_class(button, "calc-button");
    g_signal_connect(button, "clicked", callback, ui_state);
    return button;
}

// Window destroy callback
static void on_window_destroy(GtkWidget *widget, UIState *ui_state) {
    calculator_free(ui_state->state);
    g_free(ui_state);
}

// Create the calculator window
GtkWidget *create_calculator_window(GtkApplication *app, CalculatorState *state) {
    // Create UI state
    UIState *ui_state = g_new(UIState, 1);
    ui_state->state = state;

    // Create main window
    GtkWidget *window = gtk_application_window_new(app);
    gtk_widget_add_css_class(window, "calc-window");
    gtk_window_set_title(GTK_WINDOW(window), "GTKalc");
    gtk_window_set_default_size(GTK_WINDOW(window), 320, 450);
    gtk_window_set_resizable(GTK_WINDOW(window), FALSE);

    // Connect destroy signal for cleanup
    g_signal_connect(window, "destroy", G_CALLBACK(on_window_destroy), ui_state);

    // Create main box
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_widget_set_margin_start(main_box, 10);
    gtk_widget_set_margin_end(main_box, 10);
    gtk_widget_set_margin_top(main_box, 10);
    gtk_widget_set_margin_bottom(main_box, 10);
    gtk_window_set_child(GTK_WINDOW(window), main_box);

    // Create display label
    ui_state->display_label = gtk_label_new("0");
    gtk_label_set_xalign(GTK_LABEL(ui_state->display_label), 1.0);
    gtk_widget_set_halign(ui_state->display_label, GTK_ALIGN_FILL);
    gtk_widget_set_valign(ui_state->display_label, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(ui_state->display_label, 20);
    gtk_widget_set_size_request(ui_state->display_label, -1, 80);
    gtk_widget_add_css_class(ui_state->display_label, "calc-display");

    gtk_box_append(GTK_BOX(main_box), ui_state->display_label);

    // Style the display
    GtkCssProvider *css_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_string(css_provider,
        ".calc-display { font-size: 28px; font-weight: 700; background-color: #222433; border: 1px solid rgba(255,255,255,0.08); padding: 18px; border-radius: 18px; color: #ffffff; min-height: 80px; }");
    GdkDisplay *display = gdk_display_get_default();
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(css_provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(css_provider);

    // Create button grid
    GtkWidget *grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 5);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 5);
    gtk_box_append(GTK_BOX(main_box), grid);

    // Style buttons and window
    GtkCssProvider *button_css = gtk_css_provider_new();
    gtk_css_provider_load_from_string(button_css,
        ".calc-window { background-color: #161824; }\n"
        ".calc-display { background-color: #212438; color: #ffffff; }\n"
        ".calc-button { font-size: 18px; padding: 16px; background-color: #2c3349; border: 1px solid rgba(255,255,255,0.08); border-radius: 16px; color: #e8ecff; min-width: 70px; min-height: 70px; }\n"
        ".calc-button:hover { background-color: #3c4668; }\n"
        ".calc-op-button { background-color: #ff9f43; color: #ffffff; font-weight: 700; box-shadow: inset 0 0 0 1px rgba(255,255,255,0.15); }\n"
        ".calc-op-button:hover { background-color: #ff8f00; }\n"
        ".calc-equals { background-color: #33d17a; color: #ffffff; font-weight: 700; box-shadow: inset 0 0 0 1px rgba(255,255,255,0.15); }\n"
        ".calc-equals:hover { background-color: #2ebf6d; }\n"
        ".calc-clear { background-color: #ff5c63; color: #ffffff; font-weight: 700; }\n"
        ".calc-clear:hover { background-color: #e74c54; }");
    gtk_style_context_add_provider_for_display(display,
        GTK_STYLE_PROVIDER(button_css), GTK_STYLE_PROVIDER_PRIORITY_USER);
    g_object_unref(button_css);

    // Button layout
    const char *buttons[5][4] = {
        {"7", "8", "9", "/"},
        {"4", "5", "6", "*"},
        {"1", "2", "3", "-"},
        {"0", ".", "=", "+"},
        {"C", NULL, NULL, NULL}
    };

    for (int row = 0; row < 5; ++row) {
        for (int col = 0; col < 4; ++col) {
            const char *label = buttons[row][col];
            if (label == NULL) continue;

            GtkWidget *button;
            if (strcmp(label, "C") == 0) {
                button = create_button(label, G_CALLBACK(clear_button_clicked_with_update), ui_state);
                gtk_widget_add_css_class(button, "calc-clear");
            } else if (strcmp(label, "=") == 0) {
                button = create_button(label, G_CALLBACK(equals_button_clicked_with_update), ui_state);
                gtk_widget_add_css_class(button, "calc-equals");
            } else if (strcmp(label, ".") == 0) {
                button = create_button(label, G_CALLBACK(decimal_button_clicked_with_update), ui_state);
            } else if (strchr("+-*/", label[0])) {
                button = create_button(label, G_CALLBACK(operation_button_clicked_with_update), ui_state);
                gtk_widget_add_css_class(button, "calc-op-button");
            } else {
                button = create_button(label, G_CALLBACK(digit_button_clicked_with_update), ui_state);
            }

            // gtk_widget_set_size_request(button, 60, 60); // Handled by CSS
            gtk_grid_attach(GTK_GRID(grid), button, col, row, 1, 1);
        }
    }

    // Store ui_state in window for cleanup
    g_object_set_data(G_OBJECT(window), "ui_state", ui_state);

    return window;
}