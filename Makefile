CFLAGS = -O3  -Wall -Wno-unused-function -Wno-pointer-to-int-cast -Wno-int-to-pointer-cast -DSTB_DIVIDE_TEST
CDEFINES = 
#ARCHDEFS = -DVECTOR -DALTIVEC
raytracer: src/Camera.c src/raytracerCore.c src/Window.c src/FrameBuffer.c src/Vertex.c src/Triangle.c src/BvhNode.c src/Maths.c src/Ray.c src/Box.c src/Raytracer.c src/Texture.o src/Geometry.c src/Mesh.c vendor/fast_obj/fast_obj.c
	gcc $(CFLAGS) $(CDEFINES) -o raytracer src/Camera.c src/raytracerCore.c src/Window.c src/FrameBuffer.c src/Vertex.c src/Triangle.c src/BvhNode.c src/Maths.c src/Ray.c src/Box.c src/Raytracer.c src/Texture.o src/Geometry.c vendor/fast_obj/fast_obj.c  src/Mesh.c -lGL -lGLU -lglfw3 -lX11 -ldl -lGLEW -lm -lXxf86vm -lXrandr -lpthread -lXi -lXinerama -lXcursor 

texture: src/Texture.c
	gcc $(CFLAGS) -c src/Texture.c

clean:
	rm raytracer src/*.o