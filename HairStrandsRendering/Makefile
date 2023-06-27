#
# Cross Platform Makefile
# Compatible with MSYS2/MINGW, Ubuntu 14.04.1 and Mac OS X
#
# You will need GLFW (http://www.glfw.org):
# Linux:
#   apt-get install libglfw-dev
#   apt-get install libassimp-dev
# Mac OS X:
#   brew install glfw
#   brew install assimp

#CXX = g++
#CXX = clang++

EXE = HairStrandsRendering
EIGEN_DIR = /Users/yuefan/Documents/3rdparty/eigen-3.4.0
GLM_DIR = /Users/yuefan/Documents/3rdparty/glm
# SOURCES = main.cpp viewer.cpp render/shader.cpp render/opengl_buffer_manager.cpp render/opengl_context.cpp render/ui_context.cpp elems/mesh.cpp ui/property_panel.cpp ui/scene_view.cpp
SOURCES = $(wildcard glad/*.c) $(wildcard imgui/*.cpp) $(wildcard ui/imguiplugins/*.cpp) $(wildcard ./*.cpp) $(wildcard elems/*.cpp) $(wildcard render/*.cpp) $(wildcard ui/*.cpp)
OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)
LINUX_GL_LIBS = -lGL

CXXFLAGS = -std=c++20 -Iimgui -I$(GLM_DIR) -I$(EIGEN_DIR)
CXXFLAGS += -g -Wall -Wformat
LIBS = -lassimp

##---------------------------------------------------------------------
## OPENGL ES
##---------------------------------------------------------------------

## This assumes a GL ES library available in the system, e.g. libGLESv2.so
# CXXFLAGS += -DIMGUI_IMPL_OPENGL_ES2
# LINUX_GL_LIBS = -lGLESv2

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += $(LINUX_GL_LIBS) `pkg-config --static --libs glfw3`

	CXXFLAGS += `-I. pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
	LIBS += -L/usr/local/lib -L/opt/homebrew/lib
	# LIBS += -L/opt/local/lib 
	# LIBS += -lglfw3
	LIBS += -lglfw

	CXXFLAGS += -I/usr/local/include -I/opt/local/include -I/opt/homebrew/include -I./
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(OS), Windows_NT)
	ECHO_MESSAGE = "MinGW"
	LIBS += -lglfw3 -lgdi32 -lopengl32 -limm32

	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------
%.o:glad/%.c
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:render/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:elems/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:ui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:imgui/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o:ui/imguiplugins/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
