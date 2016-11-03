ALL=image1-efl image2-efl image1-gtk2 image2-gtk2

all: $(ALL)

image1-efl: image1-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image2-efl: image2-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image1-gtk2: image1-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

image2-gtk2: image2-gtk2.c
	cc $< -o $@ `pkg-config --cflags --libs gtk+-2.0`

clean:
	rm $(ALL)
