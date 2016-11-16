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
 *    - swscale can't rescale to smaller than 9x7.
 *
 * Features:
 *    - It's a bit slow/laggy.
 *
 *	Martin Guy <martinwguy@gmail.com>, October-November 2016.
 *
 * Inspired by
 * www.parallelrealities.co.uk/2011/09/basic-game-tutorial-1-opening-window.html
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <libswscale/swscale.h>

int
main(argc, argv)
int argc;
char **argv;
{
    SDL_Surface *window;
    SDL_Surface *screen;
    SDL_Surface *sourceImage;	/* As read from file */
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

    screen = SDL_SetVideoMode(sourceImage->w, sourceImage->h, 0, SDL_RESIZABLE);
    if (screen == NULL) {
	printf("Couldn't create window: %s\n", SDL_GetError());
	exit(1);
    }

    SDL_WM_SetCaption("image-sdl1", NULL);

    /* Convert source image to screen's native format */
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

    /* sws_scale reads the N+1'th line of the source image so allocate
     * an extra row */
    sourceImage->pixels = realloc(sourceImage->pixels,
				  sourceImage->pitch * (sourceImage->h + 1));

    SDL_BlitSurface(sourceImage, NULL, screen, NULL);
    SDL_Flip(screen);

    while (SDL_WaitEvent(&event)) switch (event.type) {
    /* Closing the window or pressing Ctrl-Q will exit the program */
    case SDL_QUIT:
	exit(0);
    case SDL_KEYDOWN:
	if (event.key.keysym.sym == SDLK_q &&
	    event.key.keysym.mod & KMOD_CTRL)
		exit(0);
	break;
    case SDL_VIDEORESIZE:
	{
	    SDL_Surface *image = NULL;	/* Scaled to window size */
	    struct SwsContext *sws_ctx;
	    static int srcStride[4], destStride[4];
	    int w = event.resize.w;
	    int h = event.resize.h;

	    /* sws_scale() can't resize to less than 9 wide or 7 deep,
	     * determined experimentally.
	     * Error is "Can't create 898x6 scale context."
	     */
	    if (w < 9) w = 9;
	    if (h < 7) h = 7;

	    /* Resize display surface to new window size */
	    screen = SDL_SetVideoMode(w, h, 0, SDL_RESIZABLE);

	    image = SDL_CreateRGBSurface(0, w, h,
		    sourceImage->format->BitsPerPixel,
		    sourceImage->format->Rmask,
		    sourceImage->format->Gmask,
		    sourceImage->format->Bmask,
		    sourceImage->format->Amask);
	    /* Add an extra row because sws_scale writes past the end
	     * of the image */
	    image->pixels = realloc(image->pixels,
				    image->pitch * (image->h + 1));

	    sws_ctx = sws_getContext(
			sourceImage->w, sourceImage->h, PIX_FMT_RGB32,
			w, h, PIX_FMT_RGB32,
			SWS_BILINEAR, NULL, NULL, NULL);
	    if (!sws_ctx) {
		fprintf(stderr, "Can't create %dx%d scale context.\n", w, h);
		exit(1);
	    }

	    srcStride[0] = sourceImage->pitch;
	    destStride[0] = image->pitch;

	    sws_scale(sws_ctx,
		      (const uint8_t * const*) &(sourceImage->pixels),
		      srcStride, 0, sourceImage->h,
		      (uint8_t * const*) &(image->pixels),
		      destStride);

	    sws_freeContext(sws_ctx);

	    SDL_BlitSurface(image, NULL, screen, NULL);
	    SDL_Flip(screen);

	    SDL_FreeSurface(image);
	}
	break;
    }
}
