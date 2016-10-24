/*
 *	GTK test piece to display an image in a window,
 *	resizing it to the size of the window.
 *
 *	Layout: a menu along the top and the image in the rest of the window.
 *
 *	Martin Guy <martinwguy@gmail.com>, October 2016.
 */

#include <gtk/gtk.h>

static GtkWidget *window;

static void
openFile(GtkWidget *widget, gpointer data)
{
    GtkImage *image = GTK_IMAGE(data);
    GtkWidget *dialog = gtk_file_chooser_dialog_new ("Open File",
				      GTK_WINDOW(window),
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);
    if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
	char *filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
	gtk_image_set_from_file(image, filename);
	g_free (filename);
    }
    gtk_widget_destroy (dialog);
}

int
main(int argc, char **argv)
{
    GtkWidget *vbox;
    GtkWidget *menubar;
    GtkWidget *fileMenu;
    GtkWidget *fileMi;
    GtkWidget *openMi;
    GtkWidget *quitMi;
    GtkWidget *sep;

    GtkWidget *image;

    GtkAccelGroup *accel_group;

    // If filename not given, new_from_file gives a default icon
    char *imageFilename = "";

    gtk_init(&argc, &argv);

    if (argc > 1) imageFilename = argv[1];
    image = gtk_image_new_from_file(imageFilename);

    window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

    // Quit if they ask the window manager to close the window.
    g_signal_connect(G_OBJECT(window), "destroy",
		     G_CALLBACK(gtk_main_quit), NULL);

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
		     G_CALLBACK(openFile), (gpointer) image);
    g_signal_connect(G_OBJECT(quitMi), "activate",
		     G_CALLBACK(gtk_main_quit), NULL);
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), fileMi);
	
    gtk_box_pack_start(GTK_BOX(vbox), menubar, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(vbox), image, TRUE, TRUE, 0);
    gtk_widget_show_all(window);

    gtk_main();

    return 0;
}
