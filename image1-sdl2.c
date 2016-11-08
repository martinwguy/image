/*
 * SDL2 program to display a resizable image in a window.
 *
 * Inspired by
 * www.parallelrealities.co.uk/2011/09/basic-game-tutorial-1-opening-window.html
 * http://lazyfoo.net/tutorials/SDL/35_window_events
 * https://web.archive.org/web/20140306003549/http://www.programmersranch.com/2013/11/sdl2-displaying-image-in-window.html
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

int
main(argc, argv)
int argc;
char **argv;
{
    SDL_Window	*window;
    SDL_Renderer *renderer;
    SDL_Surface *image;	    /* image as read from file */
    SDL_Texture	*texture;   /* image converted to a texture of the same size */
    SDL_Rect	rect;	    /* */
    SDL_Event	event;

    if (argc != 2) {
	printf("Usage: %s image.jpg\n", argv[0]);
	exit(1);
    }

    SDL_Init(SDL_INIT_VIDEO);
    atexit(SDL_Quit);

    image = IMG_Load(argv[1]);
    if (!image) {
	fprintf(stderr, "Failed to read image: %s\n", SDL_GetError());
	perror(argv[1]);
	exit(1);
    }

    window = SDL_CreateWindow(
	argv[0], SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
	image->w, image->h, SDL_WINDOW_RESIZABLE);
    if (!window) {
	printf("Failed to create window: %s\n", SDL_GetError());
	exit(1);
    }

    renderer = SDL_CreateRenderer(window, -1, 0);
    if (!renderer) {
	fprintf(stderr, "Failed to create renderer: %s\n", SDL_GetError());
	exit(1);
    }

    texture = SDL_CreateTextureFromSurface(renderer, image);
    if (!texture) {
	fprintf(stderr, "Failed to create texture: %s\n", SDL_GetError());
	exit(1);
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);

    while (SDL_WaitEvent(&event)) switch (event.type) {
    case SDL_QUIT:
	exit(0);

    case SDL_KEYDOWN:
	switch (event.key.keysym.sym) {
	case SDLK_ESCAPE:
	    exit(0);
	}
	break;

    case SDL_WINDOWEVENT:
	switch (event.window.event) {
	case SDL_WINDOWEVENT_SIZE_CHANGED:
#if 0
	    rect.x = rect.y = 0;
	    rect.w = event.window.data1;
	    rect.h = event.window.data2;
	    SDL_RenderCopy(renderer, texture, NULL, &rect);
#else
	    SDL_RenderCopy(renderer, texture, NULL, NULL);
#endif
	    SDL_RenderPresent(renderer);
	    break;
	case SDL_WINDOWEVENT_EXPOSED:
	    SDL_RenderPresent(renderer);
	    break;
	}
	break;
    }
}
