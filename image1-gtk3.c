/*
 * image1-gtk3.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - You can grow the window but you can't shrink it again.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* Event callbacks */
static gboolean keyPress(GtkWidget *widget, gpointer data);
static gboolean sizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data);

int
main(int argc, char **argv)
{
    GtkWidget *window;
    GdkPixbuf *sourcePixbuf = NULL;	/* As read from a file */
    GtkWidget *image;		/* As displayed on the screen */
    char *filename =  (argc > 1) ? argv[1] : "image.jpg";

    gtk_init(&argc, &argv);

    /* Make pixbuf, then make image from pixbuf because
     * gtk_image_new_from_file() doesn't flag errors */
    {
	GError *error = NULL;
	sourcePixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (sourcePixbuf == NULL) {
	    g_message("%s", error->message);
	    return 1; /* exit() */
	}
    }

    /* On expose/resize, the image's pixbuf will be overwritten
     * but we will still need the original image so take a copy of it */
    image = gtk_image_new_from_pixbuf(gdk_pixbuf_copy(sourcePixbuf));

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "image1-gtk2");

    /* Quit if they ask the window manager to close the window */
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);
    /* Quit on control-Q. */
    g_signal_connect(window, "key-press-event", G_CALLBACK(keyPress), NULL);

    /* When the window is resized, scale the image to fit */
    g_signal_connect(image, "size-allocate", G_CALLBACK(sizeChanged), sourcePixbuf);

    gtk_container_add(GTK_CONTAINER(window), image);
    gtk_widget_show_all(window);

    /* gtk_window_set_resizable(GTK_WINDOW(window), 1); has been removed
     * in favour of these, which don't allow you to shrink the window. */
    gtk_widget_set_size_request(image, 1, 1);
    gtk_window_set_default_size(GTK_WINDOW(window), 1, 1);
    gtk_window_set_resizable(GTK_WINDOW(window), 1);
    {
	GdkGeometry geometry;
	GdkWindowHints mask = GDK_HINT_MIN_SIZE;
	geometry.min_width = 1;
	geometry.min_height = 1;
	gtk_window_set_geometry_hints(GTK_WINDOW(window), image,
	    &geometry, mask);
    }

    gtk_main();

    return 0;
}

/* Callback functions */

/* Check for Control-Q and quit if it was pressed */
static gboolean
keyPress(GtkWidget *widget, gpointer data)
{
    GdkEventKey *event = (GdkEventKey *) data;

    if (event->keyval == GDK_KEY_q && (event->state & GDK_CONTROL_MASK)) {
	gtk_main_quit();
	return FALSE;
    } else
	return TRUE;
}

/* If the window has been resized, resize the image to it. */
static gboolean
sizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
    GdkPixbuf *sourcePixbuf = data;	/* As read from a file */
    GdkPixbuf *imagePixbuf;	/* pixbuf of the on-screen image */

    imagePixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
    if (imagePixbuf == NULL) {
	g_message("Can't get on-screen pixbuf");
	return TRUE;
    }
    /* Recreate the displayed image if the image size has changed. */
    if (allocation->width != gdk_pixbuf_get_width(imagePixbuf) ||
        allocation->height != gdk_pixbuf_get_height(imagePixbuf)) {

	gtk_image_set_from_pixbuf(
	    GTK_IMAGE(widget),
	    gdk_pixbuf_scale_simple(sourcePixbuf,
				    allocation->width,
				    allocation->height,
				    GDK_INTERP_BILINEAR)
	);
        g_object_unref(imagePixbuf); /* Free the old one */
    }

    return FALSE;
}
