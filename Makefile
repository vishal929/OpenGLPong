all:
	g++ main.cpp -I C:/glad/include -I C:/glfw-3.3.8/glfw-3.3.8/include -L C:/glfw-3.3.8/glfw-3.3.8/build/src -lglfw3 -lopengl32 -lgdi32 -o main.exe
run:
	./main
clean:
	del *.exe