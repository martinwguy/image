/*
 * EFL/Elementary graphics toolkit test piece to display an image and
 * resize it to fit its container area.
 *
 *     Martin Guy <martinwguy@gmail.com>, October 2016.
 */
#include <Elementary.h>
 
EAPI_MAIN int
elm_main(int argc, char **argv)
{
   Evas_Object *win;
   Evas_Object *image;
   char *filename = (argc > 1) ? argv[1] : "image.jpg";

   elm_policy_set(ELM_POLICY_QUIT, ELM_POLICY_QUIT_LAST_WINDOW_CLOSED);
 
   win = elm_win_util_standard_add("Image", "Image resizer");
   elm_win_autodel_set(win, EINA_TRUE);

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
