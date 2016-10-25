#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

/* The window has been exposed, maybe because it has been resized.
 * If it was resized, scale the image to fit.
 *
 * If a "delete-event" callback returns FALSE, GTK emits a "destroy" signal;
 * if it returns TRUE, the window is not destroyed.
 * In our case, "expose_event", we have to return FALSE for the window to be
 * repainted. If we return TRUE, it is not repainted.
 */
gboolean resize_image(GtkWidget *widget, GdkEvent *event, gpointer *data)
{
    GdkPixbuf *pixbuf =	gtk_image_get_pixbuf(GTK_IMAGE(widget));
    if (pixbuf == NULL)
    {
	    g_printerr("Failed to resize image\n");
	    return TRUE;
    }

g_print("Width: %i  Height: %i\n", widget->allocation.width,
				 widget->allocation.height);
    
    if (widget->allocation.width != gdk_pixbuf_get_width(pixbuf) ||
	widget->allocation.height != gdk_pixbuf_get_height(pixbuf)) {
    
	gtk_image_set_from_pixbuf(
	    GTK_IMAGE(widget),
	    gdk_pixbuf_scale_simple(
	    pixbuf, widget->allocation.width, widget->allocation.height,
	    GDK_INTERP_BILINEAR)
	);
	g_object_unref(pixbuf);  // Free the old image
    }
    return FALSE;
}

int main(int argc, char **argv)
{
	GtkWidget *window = NULL;
	GtkWidget *image = NULL;

	if (argc < 2 || argc > 3)
	{
		g_printerr("Usage: %s <image>\n", argv[0]);
		return 1;
	}

	/* Initialize */
	gtk_init(&argc, &argv);
	
	/* Creat Widgets */
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	image = gtk_image_new_from_file(argv[1]);
	if (image == NULL)
	{
		g_printerr("Could not open \"%s\"\n", argv[1]);
		return 1;
	}
	
	/* attach standard event handlers */
	g_signal_connect(window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
	g_signal_connect(image, "expose-event", G_CALLBACK(resize_image), (gpointer)window);
	
	gtk_container_add(GTK_CONTAINER(window), image);
	gtk_widget_show_all(GTK_WIDGET(window));

	gtk_main();

	return 0;
}
