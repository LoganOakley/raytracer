DIRS=.obj
$(shell mkdir -p $(DIRS))
raytracer1d: main.c .obj/ImageSpecReader.o .obj/ray.o .obj/vector.o .obj/texture.o .obj/stack.o
	gcc -g main.c .obj/ImageSpecReader.o .obj/ray.o .obj/vector.o .obj/texture.o .obj/stack.o -o raytracer1d -lm -Wall

.obj/ray.o: Ray/ray.c Ray/ray.h .obj/ImageSpecReader.o .obj/vector.o .obj/stack.o
	gcc -g -c Ray/ray.c -o .obj/ray.o  -Wall

.obj/ImageSpecReader.o: ImageSpecReader/ImageSpecReader.c ImageSpecReader/ImageSpecReader.h
	gcc -g -c ImageSpecReader/ImageSpecReader.c  -o .obj/ImageSpecReader.o -Wall

.obj/vector.o: Vector/vector.c Vector/vector.h
	gcc -g -c Vector/vector.c -o .obj/vector.o -Wall

.obj/texture.o: Texture/texture.c Texture/texture.h
	gcc -g -c Texture/texture.c -o .obj/texture.o -Wall

.obj/color.o: Color/color.c color/color.h
	gcc -g -c Color/color.c -o .obj/color.o -Wall

.obj/stack.o: Stack/stack.c Stack/stack.h
	gcc -g -c Stack/stack.c -o .obj/stack.o -Wall
