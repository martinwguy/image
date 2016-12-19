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
 * If you put an image in a GTK window, you cannot resize the window
 * smaller than the image it currently contains, which means you can
 * enlarge the window, and the image grows too, but you cannot then
 * reduce the window size again.
 * We get round this by displaying a cairo drawing area inside a 1x1 grid
 * container, suggested by Eric Cecashon on the gtk-list mailing list.
 *
 * Bugs:
 *    -	If you resize the window to 1x1, it goes into a 100% CPU loop. If
 *	you then enlarge the window again you are left with a white area
 *	and it doesn't respond to [x] or Control-Q any more. It sometimes
 *	recovers, but during the paralysis its VM usage goes from 46M to 940M.
 *	See https://bugzilla.gnome.org/show_bug.cgi?id=80925
 *    - Opening an image larger than the desktop (in this case 4728x864) and
 *	then moving it left to expose new area on the right provokes:
(image1-gtk3:22689): GdkPixbuf-CRITICAL **: gdk_pixbuf_get_n_channels: assertion 'GDK_IS_PIXBUF (pixbuf)' failed
(image1-gtk3:22689): GdkPixbuf-CRITICAL **: gdk_pixbuf_get_height: assertion 'GDK_IS_PIXBUF (pixbuf)' failed
(image1-gtk3:22689): GdkPixbuf-CRITICAL **: gdk_pixbuf_get_width: assertion 'GDK_IS_PIXBUF (pixbuf)' failed
**
Gdk:ERROR:/build/gtk+3.0-b165l9/gtk+3.0-3.14.5/./gdk/gdkcairo.c:193:gdk_cairo_surface_paint_pixbuf: assertion failed: (cairo_image_surface_get_format (surface) == CAIRO_FORMAT_RGB24 || cairo_image_surface_get_format (surface) == CAIRO_FORMAT_ARGB32)
Aborted (core dumped)
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

/* Event callbacks */
static gboolean keyPress(GtkWidget *widget, gpointer data);
static gboolean draw_picture(GtkWidget *widget, cairo_t *cr, gpointer data);

int
main(int argc, char **argv)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *drawing_area;
    GdkPixbuf *pixbuf = NULL;	/* As read from a file */
    char *filename =  (argc > 1) ? argv[1] : "image.jpg";

    gtk_init(&argc, &argv);

    /* Read source image from file */
    {
	GError *error = NULL;
	pixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (pixbuf == NULL) {
	    g_message("%s", error->message);
	    return 1; /* exit() */
	}
    }

    drawing_area = gtk_drawing_area_new();
    gtk_widget_set_hexpand(drawing_area, TRUE);
    gtk_widget_set_vexpand(drawing_area, TRUE);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "image1-gtk3");

    /* Quit if they ask the window manager to close the window */
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);
    /* Quit on control-Q. */
    g_signal_connect(window, "key-press-event", G_CALLBACK(keyPress), NULL);

    /* When the window is resized, scale the image to fit */
    g_signal_connect(drawing_area, "draw", G_CALLBACK(draw_picture), pixbuf);

    grid = gtk_grid_new();
    gtk_grid_attach(GTK_GRID(grid), drawing_area, 0, 0, 1, 1);

    gtk_container_add(GTK_CONTAINER(window), grid);

    /* Open the window the same size as the image */
    gtk_window_set_default_size(GTK_WINDOW(window),
	gdk_pixbuf_get_width(pixbuf),
	gdk_pixbuf_get_height(pixbuf));

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

/* When the window is resized, that resizes the drawing area
 * so we scale the image to that.
 * Paramater "widget" is the drawing_area. */
static gboolean
draw_picture(GtkWidget *drawing_area, cairo_t *cr, gpointer data)
{
    GdkPixbuf *source = data;	/* As read from a file */
    GdkPixbuf *image;		/* Scaled to the window */
    gint width = gtk_widget_get_allocated_width(drawing_area);
    gint height = gtk_widget_get_allocated_height(drawing_area);
    GdkPixbuf *readFrom;	/* the image that needs scaling */

    readFrom = source;

    /* Recreate the displayed image if the image size has changed. */

    /*
     * GTK2 and 3 have a bug in the image scaler whereby, when downscaling
     * by a large factor, it creates a humungous image kernel which makes it
     * bloat to 900MB active RAM and 100% CPU usage for tens of seconds.
     * See https://bugzilla.gnome.org/show_bug.cgi?id=80925
     * Work round this by handling pathological cases separately, of which the
     * worst (and the easiest) is when downscaling to width of height of 1.
     * For the 1x1 case, do width reduction first as that is the more
     * VM-friendly.
     */
    /* For reduction to a width of 1 */
    if (width != gdk_pixbuf_get_width(readFrom) && width == 1) {
	image = gdk_pixbuf_scale_simple(readFrom,
				    width,
				    gdk_pixbuf_get_height(readFrom),
				    GDK_INTERP_BILINEAR);
	readFrom = image;
    }
    /* and for reduction to a height of 1 */
    if (height != gdk_pixbuf_get_height(readFrom) &&
	height == 1) {
	image = gdk_pixbuf_scale_simple(readFrom,
				    gdk_pixbuf_get_width(readFrom),
				    height,
				    GDK_INTERP_BILINEAR);
	readFrom = image;
    }

    /* Now the real thing */
    if (width != gdk_pixbuf_get_width(readFrom) ||
        height != gdk_pixbuf_get_height(readFrom)) {
	image = gdk_pixbuf_scale_simple(readFrom,
				    width, height,
				    GDK_INTERP_BILINEAR);
    }

    gdk_cairo_set_source_pixbuf(cr, image, 0, 0);
    cairo_paint(cr);
    g_object_unref(image);

    return FALSE;
}
