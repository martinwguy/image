all: image

image: image.c
	cc -o $@ $< `pkg-config --libs --cflags gtk+-2.0`

resize: resize.c
	cc -o $@ $< `pkg-config --libs --cflags gtk+-2.0`
