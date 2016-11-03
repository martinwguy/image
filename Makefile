ALL=image1-efl image2-efl

all: $(ALL)

image1-efl: image1-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

image2-efl: image2-efl.c
	cc $< -o $@ `pkg-config --cflags --libs elementary`

clean:
	rm $(ALL)
