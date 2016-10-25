#include <gtk/gtk.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

gboolean resize_image(GtkWidget *widget, GdkEvent *event, GtkWidget *window)
{
	GdkPixbuf *pixbuf =	gtk_image_get_pixbuf(GTK_IMAGE(widget));
	if (pixbuf == NULL)
	{
		g_printerr("Failed to resize image\n");
		return 1;
	}
	
	printf("Width: %i\nHeight%i\n", widget->allocation.width, widget->allocation.height);
	
	pixbuf = gdk_pixbuf_scale_simple(pixbuf, widget->allocation.width, widget->allocation.height, GDK_INTERP_BILINEAR);
	
	gtk_image_set_from_pixbuf(GTK_IMAGE(widget), pixbuf);
	
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
