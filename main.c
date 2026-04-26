/*
 * GTKalc - Advanced GTK4 Calculator
 * Author: Harry Elsdon
 * Date: 26 April 2026
 *
 * Main Application Entry Point
 * 
 * This file serves as the application entry point. It:
 * 1. Creates a GTK application instance
 * 2. Initializes the calculator state
 * 3. Builds and displays the UI
 * 4. Manages the application lifecycle
 * 
 * The application uses the GTK4 application framework which handles:
 * - Window management
 * - Signal handling
 * - Application-level resource cleanup
 * 
 * Application ID: com.harryelsdon.gtkalc
 */

#include <gtk/gtk.h>
#include "calculator.h"
#include "ui.h"

/**
 * Activate callback - called when the application is ready to run.
 * This is where we create our calculator window and present it to the user.
 * 
 * Parameters:
 *   app - The GtkApplication instance
 *   user_data - User data passed to g_signal_connect (unused)
 */
static void activate(GtkApplication *app, gpointer user_data) {
    // Initialize the calculator state with default values
    CalculatorState *state = calculator_init();

    // Create the calculator window and attach it to the application
    GtkWidget *window = create_calculator_window(app, state);

    // Display the window to the user
    gtk_window_present(GTK_WINDOW(window));
}

/**
 * Main application entry point.
 * Initializes the GTK application, connects signals, and runs the main event loop.
 * 
 * Parameters:
 *   argc - Argument count
 *   argv - Argument values
 * Returns: Application exit status (0 for success)
 */
int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new(
        "com.harryelsdon.gtkalc",
        G_APPLICATION_DEFAULT_FLAGS
    );
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);

    int status = g_application_run(G_APPLICATION(app), argc, argv);

    g_object_unref(app);

    return status;
}
