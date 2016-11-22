/*
 * image1-iup.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - Normally it keeps up with resizing, albeit with some lag, but if you
 *	make the window minimum width or height and enlarge it again, it 
 *	gets into a mode whereby the X Window system generates a resize event
 *	for every pixel that the mouse moves and the application hangs, using
 *	100% CPU (because it's resizing continually without repainting) for
 *	tens of seconds before finally repainting the window.
 *	It recovers from this mode when you release the mouse and grap the
 *	resize handle again.
 * Features:
 *    - When the window is smaller than the original image, the image scaler's
 *	"bilinear interpolation" only bases each output pixel on four pixels
 *	of the source image instead of averaging an area, so the result is
 *	similar to the "nearest" interpolator. As a result, at small sizes,
 *	the screen image ripples and sparkles as it is resized.
 *    - The image resizer is slow and can only do 3 or 4 resizes per second.
 *	Not only does this make the display laggy but it makes it get behind,
 *	which provokes the above-mentioned bug.
 *
 *	Martin Guy <martinwguy@gmail.com>, November 2016.
 */

#include <stdio.h>
#include <stdlib.h>
#include <iup.h>
#include <im/im.h>
#include <im/im_image.h>
#include <iupim.h>

static int resizeImage(Ihandle *data);
static int quitGUI(Ihandle *self);

static Ihandle *window;
static Ihandle *box;
static Ihandle *label;
static imImage *imimage;	/* As read from the file */
static Ihandle *image;	/* As displayed on the screen, maybe scaled */

int
main(argc, argv)
int argc;
char **argv;
{
    IupOpen(&argc, &argv);

    /* Read image from file */
    {
        char *filename = (argc > 1) ? argv[1] : "image.jpg";
	int error;

	imimage = imFileImageLoad(filename, 0, &error);
	if (error != IM_ERR_NONE) {
	    fputs("Cannot read ", stderr);
	    perror(filename);
	    exit(1);
	}
	image = IupImageFromImImage(imimage);
	/* The image rescaler doesn't do bilinear on images with color_space
	 * MAP (palette) and BINARY (bitmap) and falls back to "nearest"
	 * so convert those to RGB.
	 */
	switch (imimage->color_space) {
	case IM_MAP:
	case IM_BINARY:
	    {
	    imImage *new = imImageCreateBased(imimage, -1, -1, IM_RGB, -1);
	    imConvertColorSpace(imimage, new);
	    imImageDestroy(imimage);
	    imimage = new;
	    }
	}
    }

    /* Make the user interface: Just a label displaying the image */
    label = IupLabel(NULL);
    IupSetAttributeHandle(label, "IMAGE", image);

    window = IupDialog(label);
    IupSetAttribute(window, "TITLE", "image1-iup");
    /* Allow containers to have a size smaller than their "natural size"
     * otherwise the label refuses to go any smaller than the image it
     * already contains. */
    IupSetAttribute(window, "SHRINK", "YES");
    /* Quit on Control-Q */
    IupSetCallback(window, "K_cQ", (Icallback) quitGUI);
    /* Scale the image when the window is resized */
    IupSetCallback(window, "RESIZE_CB", (Icallback) resizeImage);

    IupShow(window);
    IupMainLoop();
    IupClose();
    exit(0);
}

static int quitGUI(Ihandle *self)
{
    return IUP_CLOSE;
}

static int
resizeImage(Ihandle *data)
{
    int w, h;	/* New size of window client area */

    if (IupGetIntInt(window, "CLIENTSIZE", &w, &h) != 2) {
	fprintf(stderr, "Cannot get window size\n");
	return(IUP_DEFAULT);
    }

    /* Scale image to window */
    {
	imImage *new;
	Ihandle *oldimage = image;

	new = imImageCreateBased(imimage, w, h, -1, -1);
	imProcessResize(imimage, new, 1);
        image = IupImageFromImImage(new);
	imImageDestroy(new);
	IupSetAttributeHandle(label, "IMAGE", image);
	IupDestroy(oldimage);
    }

    /* Resize the label too (seems not to be necessary on Linux/X but...) */
    IupSetStrAttribute(label, "RASTERSIZE",
		       IupGetAttribute(window, "CLIENTSIZE"));

    return(IUP_DEFAULT);
}
