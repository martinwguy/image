/*
 * image1-agar.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - When the window manager resizes the window, es. when it bumps against
 *	a screen edge or when the WM's "maximize" button is poked, the image
 *	gets offset by half a window size, apparently to a random position.
 *	It remains there as you move the window about until you resize the
 *	window using one of its corners.
 * Features:
 *    - The minimum window size is about 16x16 instead of 1x1.
 *    - The scaling is done to the nearest pixel, giving a shimmering effect
 *	to the image during window resizing.
 */

#include <agar/core.h>
#include <agar/gui.h>

/* Called when they hit the [X] in the title bar to make the application quit */
static void
QuitGUI_handler(AG_Event *event)
{
	AG_QuitGUI();
}

int
main(argc, argv)
int argc;
char **argv;
{
    AG_Window	*window;
    AG_Surface	*surface;
    AG_Pixmap	*pixmap;
    char *imageFilename = (argc > 1) ? argv[1] : "image.jpg";

    if (AG_InitCore(NULL, 0) == -1 ||
	AG_InitGraphics(0) == -1) {
	    fprintf(stderr, "Cannot initialise graphics subsystem: %s.\n",
		    AG_GetError());
	    exit(1);
    }

    surface = AG_SurfaceFromFile(imageFilename);
    if (!surface) {
	fprintf(stderr, "Cannot make surface from file %s: %s.\n",
		imageFilename, AG_GetError());
	exit(1);
    }

    window = AG_WindowNew(0);
    if (!window) {
	fprintf(stderr, "Cannot create window: %s.\n", AG_GetError());
	exit(1);
    }
    /* Don't gobble up screen area with extra borders */
    AG_WindowSetPadding(window, 0, 0, 0, 0); /* Instead of 1 */

    AG_BindGlobalKey(AG_KEY_Q, AG_KEYMOD_CTRL, AG_QuitGUI);
    AG_SetEvent(window, "window-close", QuitGUI_handler, "");

    /* Without EXPAND, the image is never made bigger than its original size.
     * The window can be made smaller, in which case the image is scaled in
     * the horizontal direction but truncated in the vertical. */
    pixmap = AG_PixmapFromSurface(
	window, AG_PIXMAP_RESCALE|AG_PIXMAP_EXPAND, surface);
    if (!pixmap) {
	fprintf(stderr, "Cannot make pixmap from surface: %s.\n",
		AG_GetError());
	exit(1);
    }

    AG_WindowShow(window);

    AG_EventLoop();

    exit(0);
}
