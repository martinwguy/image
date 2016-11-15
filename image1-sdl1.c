/*
 * image1-sdl1.c: GUI toolkit test piece to display an image file.
 *
 * The image file is given as a command-line argument (default: image.jpg).
 * The window should open to exactly fit the image at one-pixel-per-pixel size.
 * The user can then resize the window in which case the image scales to fit
 * the window without keeping its aspect ratio.
 * If they hit Control-Q or poke the [X] icon in the window's titlebar,
 * the application should quit.
 *
 * Bugs:
 *    - The image doesn't scale; it is cropped top left.
 *	SDL1 doesn't do image scaling. We can do it with libswscale or other.
 *	Debian has two packages providing libswscale. It is the same code but
 *	libavutil2 and libavutil3 come from libavutil and has libswscale 3.0.0
 *	libavutil4 comes from libffmpeg and hsa libswscale 4.2.1
 *
 *	Martin Guy <martinwguy@gmail.com>, October-November 2016.
 *
 * Inspired by
 * www.parallelrealities.co.uk/2011/09/basic-game-tutorial-1-opening-window.html
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

int
main(argc, argv)
int argc;
char **argv;
{
    SDL_Surface *screen;
    SDL_Surface *sourceImage, *image;
    SDL_Event	event;
    char *filename = (argc > 1) ? argv[1] : "image.jpg";

    SDL_Init(SDL_INIT_VIDEO|SDL_DOUBLEBUF);
    atexit(SDL_Quit);

    sourceImage = IMG_Load(filename);
    if (!sourceImage) {
	fputs("Couldn't read ", stderr);
	perror(argv[1]);
	exit(1);
    }

    screen = SDL_SetVideoMode(sourceImage->w, sourceImage->h, 0, SDL_HWPALETTE|SDL_RESIZABLE);
    if (screen == NULL) {
	printf("Couldn't create window: %s\n", SDL_GetError());
	exit(1);
    }

    SDL_WM_SetCaption("image-sdl1", NULL);

    /* Convert image to screen's native format */
    {
        SDL_Surface *temp;
        temp = SDL_DisplayFormat(sourceImage);
        if (!temp) {
	    fputs("Couldn't convert image to display format", stderr);
	    exit(1);
	}
	SDL_FreeSurface(sourceImage);
	sourceImage = temp;
    }

    SDL_BlitSurface(sourceImage, NULL, screen, NULL);
    SDL_Flip(screen);

    while (SDL_WaitEvent(&event)) switch (event.type) {
    /* Closing the Window or pressing Escape will exit the program */
    case SDL_QUIT:
	exit(0);
    case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_q &&
	    event.key.keysym.mod & KMOD_CTRL)
		exit(0);
	break;
    case SDL_VIDEORESIZE:
	// imageRect.w = event.resize.w;
	// imageRect.h = event.resize.h;
	// Need to scale sourceImage into image here and Blit that.
	// see https://wiki.allegro.cc/index.php?title=Bilinear_resize

	SDL_BlitSurface(sourceImage, NULL, screen, NULL);
	SDL_Flip(screen);
	break;
    }
}
