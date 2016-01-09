
CPPFLAGS=-std=gnu++11 -Wall -O3 -lGL -lGLU -lallegro /usr/lib/x86_64-linux-gnu/liballegro_font.so /usr/lib/x86_64-linux-gnu/liballegro_primitives.so /usr/lib/x86_64-linux-gnu/liballegro_color.so /usr/lib/x86_64-linux-gnu/liballegro_image.so
#-Wl,--stack,8388608
#-Wno-write-strings

SRC=allegro_project.cpp test.cpp

all:
	g++ -g $(SRC) -o test $(CPPFLAGS)
	make run
	make clean
run:
	./test

clean:
	rm -rf test
