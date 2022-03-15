CXX = g++
CC = gcc

EXE = canvas
SOURCES = imgui_canvas_demo.cpp

IMGUI_DIR = ../imgui

# imgui
SOURCES += $(IMGUI_DIR)/imgui.cpp $(IMGUI_DIR)/imgui_demo.cpp $(IMGUI_DIR)/imgui_draw.cpp $(IMGUI_DIR)/imgui_tables.cpp $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += $(IMGUI_DIR)/backends/imgui_impl_glfw.cpp $(IMGUI_DIR)/backends/imgui_impl_opengl3.cpp

SOURCES += shader/shader.cpp
SOURCES += imgui_canvas.cpp

OBJS = $(addsuffix .o, $(basename $(notdir $(SOURCES))))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(IMGUI_DIR)/backends/ -I$(IMGUI_DIR)/ -Ishader/
CXXFLAGS += -Wall -Wformat -O3
LIBS =

# ---- GLAD OPENGL LOADER ---------------------------------------------------------------------------------------------
SOURCES += shader/libs/glad/src/glad.c
CXXFLAGS += -Ishader/libs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

# ---- LINUX ----------------------------------------------------------------------------------------------------------
ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL `pkg-config --static --libs glfw3`
	CXXFLAGS += `pkg-config --cflags glfw3`
	CFLAGS = $(CXXFLAGS)
endif

CFLAGS_STD = $(CFLAGS) -std=c11
CXXFLAGS_STD = $(CXXFLAGS) -std=c++11

# ---- BUILD RULES ----------------------------------------------------------------------------------------------------
%.o:%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

%.o:shader/%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

%.o:$(IMGUI_DIR)/%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

%.o:$(IMGUI_DIR)/backends/%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

%.o:shader/libs/glad/src/%.c
	$(CC) $(CFLAGS_STD) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean:
	rm -f $(EXE) $(OBJS)
