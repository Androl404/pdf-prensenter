#include <gtk/gtk.h>
#include <poppler.h>

#include "pdf.h"

void load_PDF_file(char* path) {
    // Load PDF document
    GError *error = NULL;
    char *uri = g_filename_to_uri(realpath(path, pdf_data.absolute_PDF_path), NULL, &error);

    if (!uri) {
        g_print("Failed to convert filename to URI: %s\n", error->message);
        g_error_free(error);
        return;
    }

    document = poppler_document_new_from_file(uri, NULL, &error);
    g_free(uri);

    if (!document) {
        g_print("Failed to open PDF: %s\n", error->message);
        g_error_free(error);
        return;
    }
}

// This function is called each time the drawing area gets resized
void draw_function(GtkDrawingArea *area, cairo_t *cr, int width, int height, gpointer user_data) {
    if (!document) return;

    PopplerPage *page = poppler_document_get_page(document, pdf_data.current_page);
    if (!page) return;

    // Get page dimensions
    double page_width, page_height;
    poppler_page_get_size(page, &page_width, &page_height);

    // Calculate scale to fit the page to the window while maintaining aspect ratio
    double scale_x = width / page_width;
    double scale_y = height / page_height;
    double scale = scale_x < scale_y ? scale_x : scale_y;

    // Clear background
    cairo_set_source_rgb(cr, 1, 1, 1); // To set the background color
    cairo_paint(cr);

    // Center the page
    cairo_translate(cr, (width - page_width * scale) / 2, (height - page_height * scale) / 2);

    // Apply scaling
    cairo_scale(cr, scale, scale);

    // Render the page
    poppler_page_render(page, cr);

    g_object_unref(page);
}
