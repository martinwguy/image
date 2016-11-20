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
 *    -	Opens the window 1x1.
 *    -	Doesn't display the image if you enlarge the window,
 *	just a white rectangle.
 *
 *	Martin Guy <martinwguy@gmail.com>, November 2016.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <im.h>
#include <im_image.h>

static int quitGUI(Ihandle *self)
{
    return IUP_CLOSE;
}

int
main(argc, argv)
int argc;
char **argv;
{
    char *filename = (argc > 1) ? argv[1] : "image.jpg";
    Ihandle *window;
    Ihandle *canvas;
    imImage *image;
    int error;

    IupOpen(&argc, &argv);
    IupImageLibOpen();

    image = imFileImageLoadBitmap(filename, 0, &error);
    if (error) {
	fputs("Cannot read ", stderr);
	perror(filename);
	exit(1);
    }
    imImageRemoveAlpha(image);
    if (image->color_space != IM_RGB) {
	imImage* new = imImageCreateBased(image, -1, -1, IM_RGB, -1);
	imConvertColorSpace(image, new);
	imImageDestroy(image);
	image = new;
    }

    canvas = IupCanvas(NULL);
    IupSetAttribute(canvas, "NAME", "CANVAS");
    IupSetAttribute(canvas, "DIRTY", "NO");
    imcdCanvasPutImage(canvas, image,
		       0, 0, image->width, image->height,
		       0, image->width, 0, image->height);

    window = IupDialog((Ihandle *)image);
    IupSetAttribute(window, "TITLE", "image1-iup");
    /* Quit if they press Control-Q */
    IupSetCallback(window, "K_cQ", (Icallback) quitGUI);

    IupSetAttribute((Ihandle *)image, "EXPAND", "YES");
    IupSetAttribute(window, "USERSIZE", NULL);

    IupShowXY(window, IUP_CENTER, IUP_CENTER);

    IupMainLoop();

    IupClose();
    exit(0);
}
