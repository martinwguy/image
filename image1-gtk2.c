/*
 *	GTK2 test piece to display an image in a window,
 *	resizing it to fit when the window is shrunk or grown.
 *
 * The original image is read from a command-line filename argument.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>

/* Event callbacks */
static gboolean exposeImage(GtkWidget *widget, gpointer data);

static GtkWidget *window;
static GdkPixbuf *sourcePixbuf = NULL;	/* As read from a file */
static GtkWidget *image;		/* As displayed on the screen */

int
main(int argc, char **argv)
{
    gtk_init(&argc, &argv);

    if (argc != 2) {
	g_message("Usage: image file");
	return 1; /* exit() */
    }

    /* Make pixbuf, then make image from pixbuf because
     * gtk_image_new_from_file() doesn't flag errors */
    {
	GError *error = NULL;
	sourcePixbuf = gdk_pixbuf_new_from_file(argv[1], &error);
	if (sourcePixbuf == NULL) {
	    g_message(error->message);
	    return 1; /* exit() */
	}
    }

    /* On expose/resize, the image's pixbuf will be overwrtten
     * but we will still need the original, so take a copy of it */
    image = gtk_image_new_from_pixbuf(gdk_pixbuf_copy(sourcePixbuf));

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Quit if they ask the window manager to close the window
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);

    // When the window is resized, scale the image to fit
    g_signal_connect(image, "expose-event",
		     G_CALLBACK(exposeImage), NULL);
	
    gtk_container_add(GTK_CONTAINER(window), image);
    gtk_widget_show_all(window);

    /* This call is "deprecated" in favour of the simpler
     * gtk_window_set_resizable(GTK_WINDOW(window), 1);
     * but that doesn't allow you to shrink the window.
     * Worse, if you enlarge the window you can't make it any smaller again
     * with a strange graphical jiggling effect if you try.  */
    gtk_window_set_policy(GTK_WINDOW(window), 1, 1, 1);

    gtk_main();

    return 0;
}

/* Callback functions */

/* If the window has been resized, resize the image to it.
 * Similarly if the image itself has changed.
 * The image-changing code ensures that the pixbuf containing a new image
 * will be loaded at a different address from the old one.
 */

static gboolean
exposeImage(GtkWidget *widget, gpointer data)
{
    GdkPixbuf *imagePixbuf;	/* pixbuf of the on-screen image */

    imagePixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
    if (imagePixbuf == NULL) {
	g_message("Can't get on-screen pixbuf", widget);
	return TRUE;
    }
    /* Recreate displayed image if source file has changed
     * or image size has changed.  */
    if (widget->allocation.width != gdk_pixbuf_get_width(imagePixbuf) ||
        widget->allocation.height != gdk_pixbuf_get_height(imagePixbuf)) {

	gtk_image_set_from_pixbuf(
	    GTK_IMAGE(widget),
	    gdk_pixbuf_scale_simple(sourcePixbuf,
				    widget->allocation.width,
				    widget->allocation.height,
				    GDK_INTERP_BILINEAR)
	);
        g_object_unref(imagePixbuf); /* Free the old one */
    }

    return FALSE;
}
