/*
 * EFL/Elementary graphics toolkit test piece to display an image and
 * resize it to fit its container area.
 *
 *     Martin Guy <martinwguy@gmail.com>, October 2016.
 */
#include <Elementary.h>

static void fileOpen(void *data, Evas_Object *obj, void *event_info);

EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win;
   Evas_Object *vbox, *hbox;	/* to add row of buttons along the top edge */
   Evas_Object *openButton, *quitButton;
   Evas_Object *image;
 
   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
   win = elm_win_util_standard_add("Image", "Image resizer");
   elm_win_autodel_set(win, EINA_TRUE);

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
     elm_image_file_set(image, "image.jpg", NULL);
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
   evas_object_size_hint_weight_set(image, 1.0, 1.0);

   evas_object_show(win);
   elm_run();
   return 0;
}
ELM_MAIN()

/* One day... */
static void
fileOpen(void *data, Evas_Object *obj, void *event_info)
{
printf("Open\n");
}
