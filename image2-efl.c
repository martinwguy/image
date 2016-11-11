/*
 * image2-efl: GUI toolkit test piece to display an image file.
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
 * - It doesn't allow you to start it without an initial image.
 * - File-Open doesn't do anything.
 * - You can resize the window but the image stays the same size
 * - You have to click and release the File menu before clicking the menu
 *	items (presumably because the menubar is implemented a toolbar).
 *
 *	Martin Guy <martinwguy@gmail.com>, October-November 2016.
 */

#include <Elementary.h>

/* Event handlers */
static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void fileOpen(void *data, Evas_Object *obj, void *event_info);
static void quitGUI(void *data, Evas_Object *obj, void *event_info);

EAPI_MAIN int
elm_main(int argc, char **argv)
{
    Evas_Object *window;
    Evas_Object *vbox;
    Evas_Object *toolbar;
    Evas_Object *image;
    char *filename = (argc > 1) ? argv[1] : "image.jpg";
 
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
    window = elm_win_add(NULL, "image", ELM_WIN_BASIC);
    elm_win_title_set(window, "Image resizer");
    elm_win_autodel_set(window, EINA_TRUE);
    evas_object_event_callback_add(window, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);

    vbox = elm_box_add(window);
    evas_object_show(vbox);

    toolbar = elm_toolbar_add(vbox);
    // Put the "File" item on the left hand side
    elm_toolbar_align_set(toolbar, 0.0);
    {
        Evas_Object *fileMenu;
        Elm_Object_Item *fileItem;

        fileItem = elm_toolbar_item_append(toolbar, NULL, "File", NULL, NULL);
        elm_toolbar_item_menu_set(fileItem, EINA_TRUE);
        fileMenu = elm_toolbar_item_menu_get(fileItem);
        elm_menu_item_add(fileMenu, NULL, NULL, "Open", fileOpen, NULL);
        elm_menu_item_add(fileMenu, NULL, NULL, "Quit", quitGUI, NULL);
    }
    elm_box_pack_end(vbox, toolbar);
    evas_object_show(toolbar);

    image = elm_image_add(vbox);
    elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);
    elm_image_aspect_fixed_set(image, EINA_FALSE);
    elm_image_file_set(image, filename, NULL);
    {
        int w, h;
        elm_image_object_size_get(image, &w, &h);
        evas_object_size_hint_min_set(image, w, h);
    }
    elm_box_pack_end(vbox, image);
    evas_object_show(image);

    evas_object_size_hint_weight_set(toolbar, 0.0, 0.0);
    //evas_object_size_hint_align_set(toolbar, 0.0, 0.0);
    evas_object_size_hint_align_set(toolbar, EVAS_HINT_FILL, 0.0);
    evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);

    // Resize the window to its contents
    elm_win_resize_object_add(window, vbox);
    evas_object_show(window);

    // Then allow the user or WM to resize the contents
    evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(image, 1, 1);


    elm_run();

    return 0;
}

ELM_MAIN()

/* Quit on Control-Q */
static void
keyDown(void *data, Evas *evas, Evas_Object *obj, void *einfo)
{
    const Evas_Modifier *mods;
    Evas_Event_Key_Down *ev = einfo;

    mods = evas_key_modifier_get(evas);
    if (evas_key_modifier_is_set(mods, "Control") &&
	strcmp(ev->key, "q") == 0) {
	exit(0);	/* There has to be a more graceful way! */
    }
}

/* One day... */
static void
fileOpen(void *data, Evas_Object *obj, void *event_info)
{
printf("Open\n");
}

static void
quitGUI(void *data, Evas_Object *obj, void *event_info)
{
    exit(0);  /* There must be a more gracious way... */
}
