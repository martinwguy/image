/*
 * image2-agar.c: GUI toolkit test piece to display an image file.
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
 *    - When the window manager resizes the window, es. when it bumps against
 *	a screen edge or when the WM's "maximize" button is poked, the image
 *	gets offset by half a window size, apparently to a random position.
 *	It remains there as you move the window about until you resize the
 *	window using one of its corners.
 *	https://bugs.csoft.net/show_bug.cgi?id=226
 *    - When you load an image from the File-Open menu and resize the window,
 *	three times out of four the image is displayed skewed and sometimes
 *	in black and white.
 *	https://bugs.csoft.net/show_bug.cgi?id=227
 *    - When you load a new image from the File-Open menu, the window does not
 *	resize to display the image at 1:1 resolution.
 *    - The minimum window size is 32x32 instead of the right size to give a
 *	1x1 image area.
 * Features:
 *    - The scaling is done to the nearest pixel, giving a shimmering effect
 *	to the image during window resizing.
 *    - You can't action a menu item by clicking on "File", moving down and
 *	releasing on "Open". You have to click-release on "File" and again on
 *	"Open".
 *    - The addition of a vbox to contain the menu and image adds a grey
 *	3-pixel border to the image and the menu.
 *    - AG_{Surface,Pixmap}FromFile() can only open JPG, PNG and BMP files.
 *    - Even with the filetype extension selector, they can still select files
 *	with different extensions.
 */

#include <agar/core.h>
#include <agar/gui.h>

/* Event-handling routines */
static void do_OpenFile(AG_Event *event);
static void do_QuitGUI(AG_Event *event);

static    AG_Surface	*surface;
static    AG_Pixmap	*pixmap;

int
main(argc, argv)
int argc;
char **argv;
{
    char *progname;
    AG_Window	*window;
    AG_Toolbar	*toolbar;
    AG_Menu	*menu;
    AG_MenuItem	*item;
    AG_Box	*vbox;
    
    char	*imageFilename;

    if ( (progname = rindex(argv[0], '/')) != NULL) ++progname;
    else progname = argv[0];

    imageFilename = (argc > 1) ? argv[1] : NULL;

    if (AG_InitCore(progname, 0) == -1 || AG_InitGraphics(0) == -1) {
	    fprintf(stderr, "Cannot initialise graphics subsystem: %s.\n",
		    AG_GetError());
	    exit(1);
    }

    window = AG_WindowNew(0);
    if (!window) {
	fprintf(stderr, "Cannot create window: %s.\n", AG_GetError());
	exit(1);
    }
    /* Don't gobble up screen area with yet another border */
    AG_WindowSetPadding(window, 0, 0, 0, 0); /* Instead of 1 */

    /* Quit if they close the main window or press Ctrl-Q. */
    AG_BindGlobalKey(AG_KEY_Q, AG_KEYMOD_CTRL, AG_QuitGUI);
    AG_SetEvent(window, "window-close", do_QuitGUI, "");

    /* Populate the window with a menu bar at the top and the image below */
    vbox = AG_BoxNewVert(window, AG_BOX_EXPAND);
    AG_BoxSetPadding(vbox, 0);
    AG_BoxSetSpacing(vbox, 0);
    menu = AG_MenuNew(vbox, 0);
    item = AG_MenuNode(menu->root, "File", NULL);
	AG_MenuAction(item, "Open", NULL, do_OpenFile, NULL);
	AG_MenuAction(item, "Quit", NULL, do_QuitGUI, NULL);

    if (!imageFilename) {
	surface = AG_SurfaceEmpty();
    } else {
	surface = AG_SurfaceFromFile(imageFilename);
	if (!surface) {
	    fprintf(stderr, "Cannot make surface from file %s: %s.\n",
		    imageFilename, AG_GetError());
	    exit(1);
	}
    }

    pixmap = AG_PixmapFromSurface(
	vbox, AG_PIXMAP_RESCALE|AG_PIXMAP_EXPAND, surface);
    if (!pixmap) {
	fprintf(stderr, "Cannot make pixmap from surface: %s.\n",
		    AG_GetError());
	exit(1);
    }

    AG_WindowShow(window);

    AG_EventLoop();

    exit(0);
}

/* The user has chosen a new file from the file dialog. Load it up. */
static void openFile(AG_Event *event)
{
    char *filename = AG_STRING(1);	//== filetype->cfile
    AG_FileType *filetype = AG_PTR(2);
    AG_Surface *oldsurface, *newsurface;

    newsurface = AG_SurfaceFromFile(filename);
    if (!newsurface) {
	fprintf(stderr, "Cannot make surface from file %s: %s.\n",
		filename, AG_GetError());
    } else {
	AG_PixmapReplaceSurface(pixmap, pixmap->n, newsurface);
	/* Freeing the old surface leads to a "double free" error. */
    }

    /* How do we get the window to change size so the image is displayed 1:1? */

    /* Without this, the screen image doesn't change until you resize the
     * window. */
    AG_WidgetUpdate(pixmap);
}

/* Service routine for the "File-Open" menu item. */
static void
do_OpenFile(AG_Event *event)
{
    AG_Window *popup;
    AG_FileDlg *dialog;

    if (!(popup = AG_WindowNew(0)) ||
        !(dialog = AG_FileDlgNew(popup, AG_FILEDLG_LOAD |
	// Close the popup when they choose a file or Cancel.
					AG_FILEDLG_CLOSEWIN |
        // Showing the File Type selector is pointless with only one file type.
					AG_FILEDLG_NOTYPESELECT |
        // Got to have this otherwise they can still enlarge the popup but
	// the contents just sits there like a prune.
					AG_FILEDLG_EXPAND |
	// Don't show "." files by default (there is still a checkbox to
	// showidden files if they want that.)
					AG_FILEDLG_MASK_HIDDEN))) {
	fprintf(stderr, "Cannot make popup window: %s.\n", AG_GetError());
	return;
    }
    AG_FileDlgAddType(dialog, "Images",
	"*.jpg,*.JPG,*.jpeg,*.JPEG,*.png,*.PNG,*.bmp,*.BMP", /* Yuk! */
	NULL, "");

    AG_AddEvent(dialog, "file-chosen", openFile, "");

    AG_WindowShow(popup);
}

static void
do_QuitGUI(AG_Event *event)
{
	AG_QuitGUI();
}
