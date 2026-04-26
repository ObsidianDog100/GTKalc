/*
 * GTKalc - Main Application
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * This is the main entry point for the GTKalc calculator application.
 * It initializes GTK, creates the calculator state, builds the UI, and runs the application.
 */

#include <gtk/gtk.h>
#include "calculator.h"
#include "ui.h"

static void activate(GtkApplication *app, gpointer user_data) {
    // Initialize calculator state
    CalculatorState *state = calculator_init();

    // Create the calculator window
    GtkWidget *window = create_calculator_window(app, state);

    // Present the window
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv) {
    // Create GTK application
    GtkApplication *app = gtk_application_new("com.harryelsdon.gtkalc", G_APPLICATION_DEFAULT_FLAGS);

    // Connect activate signal
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    // Run the application
    int status = g_application_run(G_APPLICATION(app), argc, argv);

    // Clean up
    g_object_unref(app);

    return status;
}
