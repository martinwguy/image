/*
 * image1-agar.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the border, the application
 * should quit.
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

    if (AG_InitCore(NULL, 0) == -1 ||
	AG_InitGraphics(0) == -1) {
	    fprintf(stderr, "Cannot initialise graphics subsystem: %s.\n",
		    AG_GetError());
	    exit(1);
    }


    surface = AG_SurfaceFromFile(argc > 1 ? argv[1] : "image.jpg");
    window = AG_WindowNew(0);

    AG_BindGlobalKey(AG_KEY_Q, AG_KEYMOD_CTRL, AG_QuitGUI);
    AG_SetEvent(window, "window-close", QuitGUI_handler, "");

    /* Without EXPAND, the image is never made bigger than original size
     * but can b made smaller, in which case it scales in the horizontal
     * direction but is truncated in the verical (!) */
    pixmap = AG_PixmapFromSurface(
	window, AG_PIXMAP_RESCALE|AG_PIXMAP_EXPAND, surface);

    AG_WindowShow(window);

    AG_EventLoop();

    exit(0);
}