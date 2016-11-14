/*
 * image2-gtk2.c: GUI toolkit test piece to display an image file.
 *
 * An optional image file is given as a command-line argument and, if given,
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 * A single "File" menu has two voices: "Open" to change file and,
 * if successful, resize the the window to fit the image at 1:1 zoom,
 * and "Quit".
 *
 * Bugs:
 *    - You can grow the image but can't resize the image smaller than 1:1.
 *    - When you load a new image, the window is not resized to display
 *	the image at 1:1 zoom.
 *    - You can't start it without a filename.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

/* The original image is read from a command-line filename argument and by the
 * File-Open menu and is stored in sourcePixbuf.
 *
 * The displayed image is recreated from the sourcePixbuf on an expose event,
 * when the window size has changed or when the source image changes.
 *
 * When an image file is read we try to resize the window so that the image is
 * displayed with one screen pixel per image pixel, though the window manager
 * may immediately resize the window to fit the screen.
 */

#include <gtk/gtk.h>

/* Event callbacks */
static void openFile(GtkWidget *widget, gpointer data);
static gboolean exposeImage(GtkWidget *widget, gpointer data);

/* Utility functions */
static void show_error(char *message);

// openFile() needs both "window" to open the dialog and "image" to be able
// to change the displayed image. We should put them both in a struct and pass
// a pointer to that as the callback data but I can't be bothered.
// Instead, we make "window" global. After all, there's only one.

static GtkWidget *window;
static GdkPixbuf *sourcePixbuf = NULL;	/* As read from a file */
static GtkWidget *image;		/* As displayed on the screen */

int
main(int argc, char **argv)
{
    /* UI components */
    GtkWidget *vbox;
    GtkWidget *menubar;
    GtkWidget *fileMenu;
    GtkWidget *fileMi;
    GtkWidget *openMi;
    GtkWidget *quitMi;
    GtkWidget *sep;

    GtkAccelGroup *accel_group;

    gtk_init(&argc, &argv);

    /* I haven't figured out how to open the app without an initial image yet */
    if (argc > 1) {
	GError *error = NULL;

	/* Make pixbuf, then make image from pixbuf because
	 * gtk_image_new_from_file() doesn't flag errors */
	sourcePixbuf = gdk_pixbuf_new_from_file(argv[1], &error);
	if (sourcePixbuf == NULL) {
	    g_message("%s", error->message);
	    return 1; /* exit() */
	}
	/* on expose/resize, the iamge's pixbuf will be overwrtten
	 * but we still need the original, so use a copy of it */
	image = gtk_image_new_from_pixbuf(gdk_pixbuf_copy(sourcePixbuf));
    } else {
	g_message("Usage: image file");
	return 1; /* exit() */
    }

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Quit if they ask the window manager to close the window.
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);
    // or of they press control-Q (bundled with the stock item used below)
    accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(window), accel_group);

    vbox = gtk_vbox_new(FALSE, 1);
    gtk_container_add(GTK_CONTAINER(window), vbox);

    menubar = gtk_menu_bar_new();
    fileMenu = gtk_menu_new();
    fileMi = gtk_menu_item_new_with_mnemonic("_File");

    gtk_menu_item_set_submenu(GTK_MENU_ITEM(fileMi), fileMenu);
    openMi = gtk_image_menu_item_new_from_stock(GTK_STOCK_OPEN, accel_group);
    quitMi = gtk_image_menu_item_new_from_stock(GTK_STOCK_QUIT, accel_group);
    sep = gtk_separator_menu_item_new();

    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), openMi);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), sep);
    gtk_menu_shell_append(GTK_MENU_SHELL(fileMenu), quitMi);
    g_signal_connect(G_OBJECT(openMi), "activate",
		     G_CALLBACK(openFile), NULL);
    g_signal_connect(G_OBJECT(quitMi), "activate",
		     G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);

    // When the window is resized, scale the image to fit
    g_signal_connect(image, "expose-event",
		     G_CALLBACK(exposeImage), NULL);
	
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}

/* Callback functions */

static void
openFile(GtkWidget *widget, gpointer data)
{
    GtkWidget *dialog = gtk_file_chooser_dialog_new("Open File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
	char *filename;		/* File name from chooser */
	GdkPixbuf *newPixbuf;	/* image read from file */
	GError *error = NULL;

	filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	newPixbuf = gdk_pixbuf_new_from_file(filename, &error);
	if (newPixbuf == NULL) {
	    show_error(error->message);
	} else {
	    /* Set the displayed image to the same size as new image so
	     * that it starts out at 1:1 zoom. The window manager may
	     * resize this to fit the screen.
	     * On-screen image will change at next expose event.
	     */
	    GdkPixbuf *oldPixbuf = sourcePixbuf;
	    sourcePixbuf = newPixbuf;
	    g_object_unref(oldPixbuf);
	}
	g_free(filename);
    }
    gtk_widget_destroy (dialog);
}

/* If the window has been resized, resize the image to it.
 * Similarly if the image itself has changed.
 * The image-changing code ensures that the pixbuf containing a new image
 * will be loaded at a different address from the old one.
 */

static gboolean
exposeImage(GtkWidget *widget, gpointer data)
{
    /* The last source pixbuf we saw. We remember it so that, if the image
     * changes, we repaint the on-screen image too. */
    static GdkPixbuf *oldPixbuf = NULL;
    GdkPixbuf *imagePixbuf;	/* pixbuf of the on-screen image */

    imagePixbuf = gtk_image_get_pixbuf(GTK_IMAGE(widget));
    if (imagePixbuf == NULL) {
	g_message("Can't get on-screen pixbuf");
	return TRUE;
    }
    /* Recreate displayed image if source file has changed
     * or image size has changed.  */
    if (sourcePixbuf != oldPixbuf ||
	(widget->allocation.width != gdk_pixbuf_get_width(imagePixbuf) ||
         widget->allocation.height != gdk_pixbuf_get_height(imagePixbuf))) {

	gtk_image_set_from_pixbuf(
	    GTK_IMAGE(widget),
	    gdk_pixbuf_scale_simple(sourcePixbuf,
				    widget->allocation.width,
				    widget->allocation.height,
				    GDK_INTERP_BILINEAR)
	);
        g_object_unref(imagePixbuf); /* Free the old one */

	/* If the image has changed, resize the window to 1:1 zoom */
	if (sourcePixbuf != oldPixbuf) {
	    gtk_widget_set_size_request(image,
					gdk_pixbuf_get_width(sourcePixbuf),
					gdk_pixbuf_get_height(sourcePixbuf));
	    oldPixbuf = sourcePixbuf;
	}
    }

    return FALSE;
}

/* Utility functions */

static void
show_error(char *message)
{
  GtkWidget *dialog;
  dialog = gtk_message_dialog_new(GTK_WINDOW(window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "%s", message);
  gtk_window_set_title(GTK_WINDOW(dialog), "Error");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}
