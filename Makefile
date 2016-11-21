ALL=	image1-agar image2-agar \
	image1-efl image2-efl \
	image1-fltk \
	image1-gtk2 image2-gtk2 \
	image1-gtk3 \
	image1-iup \
	image1-qt4/image1-qt4 \
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

image1-fltk: image1-fltk.c
	$(CXX) $(CFLAGS) $< -o $@ `fltk-config --cflags --libs` \
		-lXft -lfontconfig -lXinerama -lXext -lX11 -ldl

image1-gtk2: image1-gtk2.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image2-gtk2: image2-gtk2.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image1-gtk3: image1-gtk3.c
	$(CC) $(CFLAGS) $< -o $@ `pkg-config --cflags --libs gtk+-3.0`

image1-iup: image1-iup.c
	$(CC) $(CFLAGS) $< -o $@ -I/usr/local/include/iup -liup -liupim \
		`pkg-config --libs gtk+-3.0` -lX11

image1-sdl1: image1-sdl1.c
	@# apt-get install libsdl1.2-dev libsdl-image1.2-dev
	$(CC) $(CFLAGS) $< -o $@ `sdl-config --libs` -lSDL_image -lswscale

image1-sdl2: image1-sdl2.c
	@#  apt-get install libsdl2-dev libsdl2-image-dev
	$(CC) $(CFLAGS) $< -o $@ `sdl2-config --libs` -lSDL2_image

image1-qt4/image1-qt4: image1-qt4/image1-qt4.cpp image1-qt4/Makefile
	cd image1-qt4 && make image1-qt4 && touch image1-qt4

image1-qt4/Makefile: image1-qt4/image1-qt4.pro
	cd image1-qt4 && qmake

image1-qt4/image1-qt4.pro:
	cd image1-qt4 && qmake -project

show: $(ALL)
	for a in $(ALL); do ./$$a image2.jpg & done

clean:
	rm $(ALL)
