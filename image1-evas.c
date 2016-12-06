/*
 * image1-evas.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    -	Opens a window of the correct size but with black contents.
 *    - Doesn't react to Control-Q, maybe because the image is not present
 *
 *     Martin Guy <martinwguy@gmail.com>, December 2016.
 */
#include <Ecore.h>
#include <Ecore_Evas.h>

static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void quitGUI(Ecore_Evas *ee);
 
int
main(int argc, char **argv)
{
    Ecore_Evas *ee;
    Evas *canvas;
    Evas_Object *image;
    char *filename = (argc > 1) ? argv[1] : "image.jpg";

    if (!ecore_evas_init()) {
	fputs("Cannot initialize graphics subsystem.\n", stderr);
	exit(1);
    }

    ee = ecore_evas_new(NULL, 0, 0, 1, 1, NULL);
    if (!ee) {
	fputs("Can't initialise graphics system.\n", stderr);
	exit(1);
    }
    ecore_evas_show(ee);
    canvas = ecore_evas_get(ee);
#if 1
    image = evas_object_image_filled_add(canvas);
#else
    image = ecore_evas_object_image_new(ee);
    evas_object_image_filled_set(image, EINA_TRUE);
#endif
    evas_object_image_file_set(image, filename, NULL);
    {
	int w, h;
	evas_object_image_size_get(image, &w, &h);
	ecore_evas_resize(ecore_evas_object_ecore_evas_get(image), w, h);
	ecore_evas_resize(ee, w, h);
    }
    evas_object_show(image);

    //ecore_evas_callback_resize_set(ee, resize_cb);
    ecore_evas_callback_delete_request_set(ee, quitGUI);
    ecore_evas_callback_destroy_set(ee, quitGUI);
    evas_object_event_callback_add(image, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);
    ecore_evas_title_set(ee, "image1-evas");

    ecore_main_loop_begin();

    ecore_evas_free(ee);
    ecore_evas_shutdown();

    return 0;
}

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

/* Quit on Control-Q */
static void
quitGUI(Ecore_Evas *ee EINA_UNUSED)
{
	ecore_main_loop_quit();
}
