ALL=image1-efl image2-efl image1-gtk2 image2-gtk2 image1-sdl1 image1-sdl2

all: $(ALL)

image1-efl: image1-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image2-efl: image2-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image1-gtk2: image1-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image2-gtk2: image2-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image1-sdl1: image1-sdl1.c
	@# SDL1.2: apt-get install libsdl1.2-dev libsdl-image1.2-dev
	cc $< -o $@ `sdl-config --libs` -lSDL_image

image1-sdl2: image1-sdl2.c
	@# SDL2.0: apt-get install libsdl2-dev libsdl2-image-dev
	cc $< -o $@ `sdl2-config --libs` -lSDL2_image

clean:
	rm $(ALL)
