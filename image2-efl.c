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
 * - Instead of a "File" menu there are just two buttons "Open" and "Quit".
 * - If you Open a duff file, you get a black window instead of an error.
 * - When you open a new file, the window is not resized to display it 1:1.
 * - With window >10000 pixels wide, only the left 10000 columns are displayed.
 *
 *	Martin Guy <martinwguy@gmail.com>, October-November 2016.
 */

#include <Elementary.h>

/* Event handlers */
static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void fileChosen(void *data, Evas_Object *obj, void *event_info);
static void quitGUI(void *data, Evas_Object *obj, void *event_info);

static    Evas_Object *window;
static    Evas_Object *vbox;
static    Evas_Object *image;

EAPI_MAIN int
elm_main(int argc, char **argv)
{
    Evas_Object *hbox;	/* The "menu toolbar" */
    Evas_Object *openButton;
    Evas_Object *quitButton;
    Evas_Object *menu;
    char *filename = (argc > 1) ? argv[1] : NULL;
 
    elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
    window = elm_win_add(NULL, "image", ELM_WIN_BASIC);
    elm_win_title_set(window, "Image resizer");
    elm_win_autodel_set(window, EINA_TRUE);
    evas_object_event_callback_add(window, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);

    /* Container for menu toolbar and image below it */
    vbox = elm_box_add(window);
    evas_object_show(vbox);

    /* Menu toolbar, implemented as an hbox of buttons.
     * I couldn't get the menu stuff to work. */
    hbox = elm_box_add(window);
    elm_box_align_set(hbox, 0.0, 0.0);
    elm_box_horizontal_set(hbox, EINA_TRUE);
    elm_box_pack_end(vbox, hbox);
    evas_object_show(hbox);

    openButton = elm_fileselector_button_add(hbox);
    elm_object_text_set(openButton, "Open");
    elm_fileselector_button_inwin_mode_set(openButton, EINA_FALSE);
    evas_object_smart_callback_add(openButton, "file,chosen", fileChosen, NULL);
    elm_box_pack_end(hbox, openButton);
    evas_object_show(openButton);

    quitButton = elm_button_add(hbox);
    elm_object_part_text_set(quitButton, NULL, "Quit");
    evas_object_smart_callback_add(quitButton, "pressed", quitGUI, NULL);
    evas_object_smart_callback_add(quitButton, "clicked", quitGUI, NULL);
    elm_box_pack_end(hbox, quitButton);
    evas_object_show(quitButton);

    image = elm_image_add(vbox);
    elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);
    elm_image_aspect_fixed_set(image, EINA_FALSE);
    if (filename) elm_image_file_set(image, filename, NULL);
    {
        int w, h;
        elm_image_object_size_get(image, &w, &h);
        evas_object_size_hint_min_set(image, w, h);
    }
    elm_box_pack_end(vbox, image);
    evas_object_show(image);

    evas_object_size_hint_align_set(hbox, EVAS_HINT_FILL, EVAS_HINT_FILL);
    evas_object_size_hint_weight_set(image, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_align_set(image, EVAS_HINT_FILL, EVAS_HINT_FILL);

    // Resize the window to its contents
    elm_win_resize_object_add(window, vbox);
    evas_object_show(window);

    // Then allow the user or WM to resize it
    evas_object_size_hint_weight_set(vbox, EVAS_HINT_EXPAND, EVAS_HINT_EXPAND);
    evas_object_size_hint_min_set(image, 1, 1);

    elm_run();

    return 0;
}

ELM_MAIN()

/* A file  has been chosen from file selector. Open it. */
static void unlimitImageSize(void *data, Evas_Object *obj, void *event_info);

static void
fileChosen(void *data, Evas_Object *obj, void *event_info)
{
    const char *filename = event_info;

    if (filename == NULL) return;  /* They cancelled instead of selecting */

    elm_image_file_set(image, filename, NULL);
    /* Make the window resize to display the image at 1:1 zoom
     * and when the window has resized, remove the size limits. */
    evas_object_event_callback_add(image, EVAS_CALLBACK_RESIZE,
	(Evas_Object_Event_Cb) unlimitImageSize, image);
    {
	int w, h;
	elm_image_object_size_get(image, &w, &h);
	evas_object_size_hint_min_set(image, w, h);
	evas_object_size_hint_max_set(image, w, h);
    }
}

static void
unlimitImageSize(void *data, Evas_Object *obj, void *event_info)
{
    Evas_Object *image = data;

    evas_object_size_hint_min_set(image, 1, 1);
    evas_object_size_hint_max_set(image, -1, -1);
    evas_object_event_callback_del(image, EVAS_CALLBACK_RESIZE,
	(Evas_Object_Event_Cb) unlimitImageSize);
}

/* Quit on Control-Q */
static void
keyDown(void *data, Evas *evas, Evas_Object *obj, void *event_info)
{
    const Evas_Modifier *mods;
    Evas_Event_Key_Down *ev = event_info;

    mods = evas_key_modifier_get(evas);
    if (evas_key_modifier_is_set(mods, "Control") &&
	strcmp(ev->key, "q") == 0) {
	quitGUI(data, obj, event_info);
    }
}

static void
quitGUI(void *data, Evas_Object *obj, void *event_info)
{
    exit(0);  /* There must be a more gracious way... */
}
