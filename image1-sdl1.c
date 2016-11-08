/*
 * SDL1.2 program to display a resizable image in a window.
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

    if (argc != 2) {
	printf("Usage: %s image.jpg\n", argv[0]);
	exit(1);
    }

    SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF);
    atexit(SDL_Quit);

    sourceImage = IMG_Load(argv[1]);
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

    SDL_WM_SetCaption(argv[0], NULL);

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
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	    exit(0);
	}
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
