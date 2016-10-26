image: image.c
	cc image.c -o image `pkg-config --cflags --libs elementary`

clean:
	rm image
