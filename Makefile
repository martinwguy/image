ALL=	image1-agar image2-agar \
	image1-efl image2-efl \
	image1-gtk2 image2-gtk2 \
	image1-sdl1 image1-sdl2

all: $(ALL)

image1-agar: image1-agar.c
	$(CC) $(CFLAGS) $< -o $@ `agar-config --cflags --libs`

image2-agar: image2-agar.c
	$(CC) $(CFLAGS) $< -o $@ `agar-config --cflags --libs`

image1-efl: image1-efl.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs elementary`

image2-efl: image2-efl.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs elementary`

image1-gtk2: image1-gtk2.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image2-gtk2: image2-gtk2.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image1-sdl1: image1-sdl1.c
	@# apt-get install libsdl1.2-dev libsdl-image1.2-dev
	$(CC) $(CFLAGS) $< -o $@ `sdl-config --libs` -lSDL_image

image1-sdl2: image1-sdl2.c
	@#  apt-get install libsdl2-dev libsdl2-image-dev
	$(CC) $(CFLAGS) $< -o $@ `sdl2-config --libs` -lSDL2_image

show: $(ALL)
	for a in $(ALL); do ./$$a image2.jpg & done

clean:
	rm $(ALL)
