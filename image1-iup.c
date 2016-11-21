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
 *    -	Doesn't scale the image when you resize the window.
 *
 *	Martin Guy <martinwguy@gmail.com>, November 2016.
 */

#include <stdlib.h>
#include <stdio.h>
#include <iup.h>
#include <iupim.h>

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
    Ihandle *label;
    Ihandle *image;

    IupOpen(&argc, &argv);

    image = IupLoadImage(filename);
    if (!image) {
	fputs("Cannot read ", stderr);
	perror(filename);
	exit(1);
    }

    label = IupLabel("");
    IupSetAttributeHandle(label, "IMAGE", image);

    window = IupDialog(label);
    IupSetAttribute(window, "TITLE", "image1-iup");
    IupSetCallback(window, "K_cQ", (Icallback) quitGUI);
    IupShow(window);

    IupMainLoop();

    IupClose();
    exit(0);
}
