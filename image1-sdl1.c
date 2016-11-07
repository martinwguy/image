/*
 * SDL1.2 program to display a resizable image in a window.
 *
 * Inspired by
 * www.parallelrealities.co.uk/2011/09/basic-game-tutorial-1-opening-window.html
 */

#include <SDL/SDL.h>
#include <SDL/SDL_image.h>

main(argc, argv)
int argc;
char **argv;
{
    SDL_Surface *screen;
    SDL_Surface *sourceImage, *image;
    SDL_Rect rect;
    SDL_Event	event;

    if (argc != 2) {
	printf("Usage: %s image.jpg\n", argv[0]);
	exit(1);
    }

    SDL_Init(SDL_INIT_AUDIO|SDL_INIT_VIDEO|SDL_DOUBLEBUF);

    sourceImage = IMG_Load(argv[1]);
    if (!sourceImage) {
	fputs("Couldn't read ", stderr);
	perror(argv[1]);
	goto fail;
    }

    screen = SDL_SetVideoMode(sourceImage->w, sourceImage->h, 0, SDL_HWPALETTE);
    if (screen == NULL) {
	printf("Couldn't set screen mode: %s\n", SDL_GetError());
	exit(1);
    }

    SDL_WM_SetCaption(argv[0], NULL);

    /* Convert image to screen's native format */
    image = SDL_DisplayFormat(sourceImage);
    if (!image) {
	fputs("Couldn't convert image to display format", stderr);
	goto fail;
    }

    rect.x = rect.y = 0;
    rect.w = image->w;
    rect.h = image->h;
    SDL_BlitSurface(image, NULL, screen, &rect);
    SDL_Flip(screen);

    while (1) {
	while (SDL_PollEvent(&event)) switch (event.type) {
	/* Closing the Window or pressing Escape will exit the program */
	case SDL_QUIT:
	    goto quit;
	case SDL_KEYDOWN:
	    switch (event.key.keysym.sym) {
	    case SDLK_ESCAPE:
		goto quit;
	    }
	    break;
	}
	SDL_Delay(20);
    }

quit:
    SDL_Quit();
    exit(0);
fail:
    SDL_Quit();
    exit(1);
}
