#linux
#CPPFLAGS=-std=gnu++11 -Wall -O3 -lGL -lGLU -lallegro /usr/lib/x86_64-linux-gnu/liballegro_font.so /usr/lib/x86_64-linux-gnu/liballegro_primitives.so /usr/lib/x86_64-linux-gnu/liballegro_color.so /usr/lib/x86_64-linux-gnu/liballegro_image.so
#-Wl,--stack,8388608
#-Wno-write-strings

#win
# -mwindows flag to disable running terminal
CPPFLAGS=-std=gnu++11 -Wall -mwindows -O3 -lopengl32 -lglu32 -lallegro -lallegro_font -lallegro_ttf -lallegro_primitives -lallegro_color -lallegro_image

SRC=allegro_project.cpp test.cpp


all:
#win	g++.exe -g $(SRC) -o test $(CPPFLAGS)
	g++ -g $(SRC) -o test $(CPPFLAGS)
	make run
#	make clean
run:
#win
#	./test.exe
	./test

clean:
	rm -rf test.exe
