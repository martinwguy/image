/*
 * audio1-evas.c: GUI toolkit test piece to play an audio file.
 *
 * The image file is given as a command-line argument (default: audio.wav).
 * A window should open showing a graphical representation of the audio file:
 * each frame of audio samples is shown as a vertical bar whose gray value
 * is proportional to the average RMS of the audio in that period.
 * At startup, the start of the piece is in the centre of the window with
 * the first frames of the audio file represented shown right of center.
 * If you hit play (press 'space'), the audio starts playing and the
 * display scrolls left so that the current playing position remains at the
 * centre of the window. Another space should pause the playback, another
 * make it continue from where it left off. At end of piece, the playback stops;
 * pressing space makes it start again from the beginning.
 * 
 * The user can resize the window, in which case the displayed image is zoomed..
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 */

/*
 * The Emotion API notifies events by Evas Object Smart Callbacks,
 * according to Emotion.h in e17:
 * - "playback_started" - Emitted when the playback starts (no! it's emitted
 *			  when playback finishes in emotion 0.28!)
 * - "playback_finished" - Emitted when the playback finishes
 * - "frame_decode" - Emitted every time a frame is decoded
 *		      Seems video-only and not emitted during audio playback.
 * - "open_done" - Emitted when the media file is opened
 * - "position_update" - Emitted when emotion_object_position_set is called
 * - "decode_stop" - Emitted after the last frame is decoded. Maybe video-only.
 * Of these, we need to be able to
 * - set the playback position (with emotion_object_position_set()?)
 * - start/stop/pause/resume playback
 * - Know when the piece ends, to stop scrolling (react to playback_finished)
 * See https://docs.enlightenment.org/auto/emotion_main.html
 * See https://www.enlightenment.org/program_guide/threading_pg
 *
 * Bugs:
 *    -	It doesn't work yet.
 *    - The "playback started" event is delivered when playback finishes (!)
 *	(in emotion 0.28) so mark time from when you start the audio playing.
 *
 *	Martin Guy <martinwguy@gmail.com>, December 2016.
 */

#define EFL_EO_API_SUPPORT
#define EFL_BETA_API_SUPPORT

#include <Ecore.h>
#include <Ecore_Evas.h>
#include <Evas.h>
#include <Emotion.h>

#define Eo_Event void

/* Callback-handling funtions */
static void keyDown(void *data, Evas *e, Evas_Object *obj, void *event_info);
static void quitGUI(Ecore_Evas *ee);
static void playback_finished_cb(void *data, Evas_Object *obj, void *ev);
 
int
main(int argc, char **argv)
{
    Ecore_Evas *ee;
    Evas *canvas;
    Evas_Object *em;
    Evas_Object *image;
    char *filename = (argc > 1) ? argv[1] : "audio.wav";
    int w, h;

    if (!ecore_evas_init() ||
        !(ee = ecore_evas_new(NULL, 0, 0, 1, 1, NULL))) {
	fputs("Cannot initialize graphics subsystem.\n", stderr);
	exit(1);
    }
    ecore_evas_callback_delete_request_set(ee, quitGUI);
    ecore_evas_title_set(ee, "audio1-evas");
    ecore_evas_show(ee);

fprintf(stderr, "Time is %8.8f\n", ecore_time_get());

    canvas = ecore_evas_get(ee);

    image = evas_object_image_add(canvas);
    evas_object_image_filled_set(image, EINA_TRUE);

    evas_object_show(image);

    /* Propagate resize events from the container to the image */
    ecore_evas_object_associate(ee, image, 0);
    evas_object_resize(image, 320, 200);

    evas_object_focus_set(image, EINA_TRUE); // Without this, no keydown events
    evas_object_event_callback_add(image, EVAS_CALLBACK_KEY_DOWN, keyDown, NULL);

    em = emotion_object_add(canvas);
    if (!em) {
	fputs("Couldn't initialize audio subsystem.\n", stderr);
	exit(1);
    }
    emotion_object_init(em, NULL);
    emotion_object_video_mute_set(em, EINA_TRUE);
    if (emotion_object_file_set(em, filename) != EINA_TRUE) {
	fputs("Couldn't load audio file.\n", stderr);
	exit(1);
    }
    evas_object_show(em);

    evas_object_smart_callback_add(em, "playback_finished", playback_finished_cb, NULL);

    /* Start playing */
    emotion_object_play_set(em, EINA_TRUE);

    ecore_main_loop_begin();

    ecore_evas_free(ee);
    ecore_evas_shutdown();

    return 0;
}

static void
keyDown(void *data, Evas *evas, Evas_Object *obj, void *einfo)
{
    Evas_Event_Key_Down *ev = einfo;
    const Evas_Modifier *mods = evas_key_modifier_get(evas);

    if (strcmp(ev->key, " ") == 0) {
	/* If playing, pause. If paused or stopped, play. */
    }
    if (evas_key_modifier_is_set(mods, "Control") &&
	strcmp(ev->key, "q") == 0) {
	ecore_main_loop_quit();
    }
}

/* Quit on Control-Q */
static void
quitGUI(Ecore_Evas *ee EINA_UNUSED)
{
    ecore_main_loop_quit();
}

static void
playback_finished_cb(void *data, Evas_Object *obj, void *ev)
{
    printf("playback_finished\n");
}
