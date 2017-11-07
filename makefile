# Compiler:
CC = g++

# Optimization:
CFLAGS = -O3

# Libs and Frameworks:
FRAMEWORKS = -framework OpenGL -lglew -lglfw -lAntTweakBar -lpng -framework Cocoa -framework IOKit -framework CoreVideo -std=c++11

# Location for libs:
LIBFOLD = -L"/usr/lib"
# Location for includes:
INCFOLD = -I"/usr/include"
INCFOLD_LOCAL = -I"/usr/local/include"
INCFOLD_OPENGL = -I"/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/System/Library/Frameworks/OpenGL.framework/Versions/A"
#INCFOLD_GLFW = -I"/usr/local/Cellar/glfw3/3.2.1/include/"
INCFOLD_GLFW = -I"/Users/martingrad/Documents/Projects/OpenGL_transparency/include"

# Files:
FILES = $(wildcard src/*.cpp) $(wildcard shaders/*.cpp) $(wildcard src/utils/*.cpp) $(wildcard src/utils/*.c) 
EXTERNFILES = $(wildcard extern/OpenGL_Graphics_Engine/src/*.cpp) $(wildcard extern/OpenGL_Graphics_Engine/src/utils/*.cpp) $(wildcard extern/OpenGL_Graphics_Engine/src/utils/*.c)

# Binary folder:
BINFOLD = bin/

# Binary name:
BINNAME = OpenGL_Transparency


all: compile
.PHONY: all

compile: $(FILES)
	$(CC) $(CFLAGS) $(EXTERNFILES) $(FILES) -o $(BINFOLD)$(BINNAME) $(LIBFOLD) $(INCFOLD) $(INCFOLD_LOCAL) $(INCFOLD_GLFW) $(FRAMEWORKS)
.PHONY: compile

run:
	./$(BINFOLD)$(BINNAME)
.PHONY: run

clean:
	rm -f $(BINFOLD)*
.PHONY: clean

sense: room1 clean all room2 run

room1:
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo "=========== cleaning and compiling ==========="
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""

room2:
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo "================== running =================="
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""
	@echo ""

make: 
	@say ”Nu du Martin har du allt skitit i det blå skåpet”