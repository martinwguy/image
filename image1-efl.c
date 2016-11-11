/*
 * image1-elf.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs: None.
 *
 *     Martin Guy <martinwguy@gmail.com>, October 2016.
 */
#include <Elementary.h>

static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);
 
EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win;
   Evas_Object *image;
   char *filename = (argc > 1) ? argv[1] : "image.jpg";

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
   win = elm_win_util_standard_add("Image", "Image resizer");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);

   image = elm_image_add(win);
   elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);
   elm_image_aspect_fixed_set(image, EINA_FALSE);
   elm_image_file_set(image, filename, NULL);
   {
      int w, h;
      elm_image_object_size_get(image, &w, &h);
      evas_object_resize(win, w, h);
   }
   elm_win_resize_object_add(win, image);
   evas_object_show(image);
   evas_object_show(win);

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
