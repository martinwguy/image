/*
 * image1-gtk2.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - If you resize the window to minimum size (1x1), GTK goes into a
 *	100% CPU loop for about a minute, during which time it does not
 *	refresh the display, then recovers as mysteriously as it died.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* Event callbacks */
static gboolean keyPress(GtkWidget *widget, gpointer data);
static gboolean exposeImage(GtkWidget *widget, GdkEventExpose *event, gpointer data);

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
    g_signal_connect(image, "expose-event", G_CALLBACK(exposeImage), sourcePixbuf);

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

/* Check for Control-Q and quit if it was pressed */
static gboolean
keyPress(GtkWidget *widget, gpointer data)
{
    GdkEventKey *event = (GdkEventKey *) data;

    if (event->keyval == GDK_q && (event->state & GDK_CONTROL_MASK)) {
	gtk_main_quit();
	return FALSE;
    } else
	return TRUE;
}

/* If the window has been resized, resize the image to it. */
static gboolean
exposeImage(GtkWidget *widget, GdkEventExpose *event, gpointer data)
{
    GdkPixbuf *sourcePixbuf = data;	/* As read from a file */
    GdkPixbuf *imagePixbuf;	/* pixbuf of the on-screen image */

    imagePixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
    if (imagePixbuf == NULL) {
	g_message("Can't get on-screen pixbuf");
	return TRUE;
    }
    /* Recreate the displayed image if the image size has changed. */
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
