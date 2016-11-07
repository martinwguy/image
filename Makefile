ALL=image1-efl image2-efl image1-gtk2 image2-gtk2 image1-sdl1

all: $(ALL)

image1-efl: image1-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image2-efl: image2-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image1-gtk2: image1-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image2-gtk2: image2-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

# SDL1.2: apt-get install libsdl1.2-dev libsdl-image1.2-dev
image1-sdl1: image1-sdl1.c
	cc $< -o $@ `sdl-config --libs` -lSDL_image

clean:
	rm $(ALL)
