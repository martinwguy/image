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
 *    - It doesn't allow you to start it without an initial image.
 *    - There are two buttons instead of the usual menu.
 *    - File-Open doesn't do anything.
 *
 *     Martin Guy <martinwguy@gmail.com>, October 2016.
 */
#include <Elementary.h>

static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);

static void fileOpen(void *data, Evas_Object *obj, void *event_info);

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win;
   Evas_Object *vbox, *hbox;	/* to add row of buttons along the top edge */
   Evas_Object *openButton, *quitButton;
   Evas_Object *image;
   char *filename = (argc > 1) ? argv[1] : "image.jpg";
 
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
   win = elm_win_util_standard_add("Image", "Image resizer");
   elm_win_autodel_set(win, EINA_TRUE);
   evas_object_event_callback_add(win, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);

   vbox = elm_box_add(win);

    // top line: menu bar composed of a few labelled buttons ("Open", "Quit")
    hbox = elm_box_add(vbox);
    elm_box_horizontal_set(hbox, EINA_TRUE);
    evas_object_box_align_set(hbox, 0.0, 0.0);
     openButton = elm_button_add(hbox);
     elm_object_text_set(openButton, "Open");
     evas_object_smart_callback_add(openButton, "clicked", fileOpen, NULL);
     evas_object_show(openButton);
     elm_box_pack_start(hbox, openButton);
    elm_box_pack_start(vbox, hbox);

    image = elm_image_add(vbox);
     elm_image_resizable_set(image, EINA_TRUE, EINA_TRUE);
     elm_image_aspect_fixed_set(image, EINA_FALSE);
     elm_image_file_set(image, filename, NULL);
     {
      int w, h;
      elm_image_object_size_get(image, &w, &h);
      evas_object_size_hint_min_set(image, w, h);
     }
     evas_object_show(image);
    elm_box_pack_end(vbox, image);

   /* The following sequence should resize the window to fit the image's
    * natural size and then allow the user to resize the window and
    * as a consequence resize the image. */
   elm_win_resize_object_add(win, image);
   //evas_object_size_hint_weight_set(image, 1.0, 1.0);

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

/* One day... */
static void
fileOpen(void *data, Evas_Object *obj, void *event_info)
{
printf("Open\n");
}
