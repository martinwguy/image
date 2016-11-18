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
 * It seems that in GTK3 you cannot resize a window smaller than the image
 * it currently contains, which means you can enlarge the window, and
 * the image grows too, but you cannot then reduce the window size again!
 *   Someone suggested putting the image in a GtkScrolledWindow and resizing
 * the image to exactly the size of scrolled window so that the scrollbars
 * disappear. It sort of works - see the bugs - and is gross.
 *
 * Bugs:
 *    - When you shrink the window, the viewport's scrollbars appear and
 *	then don't go away when you release the mouse button.
 *    - If you shrink the window size quickly, the image slobbers down and right
 *	in the viewport, leaving a variable-width white border top and left.
 *	Sometimes, when you stop shrinking and release the mouse, the white
 *	border does not disappear and tweaking the scrollbars reveals that the
 *	image is visualised the correct size for the window but centered on a
 *	slightly larger white canvas tens of pixels larger than the image.
 *    - The window has an arbitrary minimum size of 42x42.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* Event callbacks */
static gboolean keyPress(GtkWidget *widget, gpointer data);
static gboolean sizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data);

static    GtkWidget *image;		/* As displayed on the screen */

int
main(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *viewport;
    GdkPixbuf *sourcePixbuf = NULL;	/* As read from a file */
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

    viewport = gtk_scrolled_window_new(NULL, NULL);
    /* Saying "1x1" reduces the window's minumum size from 55x55 to 42x42. */
    gtk_scrolled_window_set_min_content_width(GTK_SCROLLED_WINDOW(viewport), 1);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(viewport), 1);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "image1-gtk3");

    /* Quit if they ask the window manager to close the window */
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);
    /* Quit on control-Q. */
    g_signal_connect(window, "key-press-event", G_CALLBACK(keyPress), NULL);

    /* When the window is resized, scale the image to fit */
    g_signal_connect(viewport, "size-allocate", G_CALLBACK(sizeChanged), sourcePixbuf);

    /* The image is in a scrolled window container so that the main window
     * can be resized smaller than the current image. */
    gtk_container_add(GTK_CONTAINER(viewport), image);
    gtk_container_add(GTK_CONTAINER(window), viewport);

    //gtk_window_set_resizable(GTK_WINDOW(window), 1);
    /* Open the window the same size as the image */
    gtk_window_set_default_size(GTK_WINDOW(window),
	gdk_pixbuf_get_width(sourcePixbuf),
	gdk_pixbuf_get_height(sourcePixbuf));

    gtk_widget_show_all(window);

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

/* If the window has been resized, that resizes the scrolledwindow,
 * and we scale the image to the dimensions of the scrolledwindow so that
 * the scrollbars disappear again. Yuk! */
static gboolean
sizeChanged(GtkWidget *widget, GtkAllocation *allocation, gpointer data)
{
    GdkPixbuf *sourcePixbuf = data;	/* As read from a file */
    GdkPixbuf *imagePixbuf;	/* pixbuf of the on-screen image */

    imagePixbuf = gtk_image_get_pixbuf(GTK_IMAGE(image));
    if (imagePixbuf == NULL) {
	g_message("Can't get on-screen pixbuf");
	return TRUE;
    }
    /* Recreate the displayed image if the image size has changed. */
    if (allocation->width != gdk_pixbuf_get_width(imagePixbuf) ||
        allocation->height != gdk_pixbuf_get_height(imagePixbuf)) {

	gtk_image_set_from_pixbuf(
	    GTK_IMAGE(image),
	    gdk_pixbuf_scale_simple(sourcePixbuf,
				    allocation->width,
				    allocation->height,
				    GDK_INTERP_BILINEAR)
	);
        g_object_unref(imagePixbuf); /* Free the old one */
    }

    return FALSE;
}
