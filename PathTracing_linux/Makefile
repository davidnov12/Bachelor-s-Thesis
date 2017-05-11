SRC = src/main.cpp src/Camera.cpp src/FPSMeter.cpp src/ModelLoader.cpp src/Octree.cpp src/Render.cpp src/Scene.cpp src/Shader.cpp src/Sphere.cpp src/Window.cpp src/png/lodepng.cpp

all:
	g++ -std=c++11 ${SRC} -o PathTracing -lGLEW -lglfw3 -lGL -lGLU -lX11 -lpthread -lXrandr -lXi -lXxf86vm -ldl -lXinerama -lXcursor
