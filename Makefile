# http://stackoverflow.com/a/1139628/2856519

CC=g++
TARGET=adsb-scope
UIGUIDIR=sdl2-simple-ui

SOURCE = adsb-scope.cpp curl_fopen.cpp INIReader.cpp ini.cpp

SOURCE += $(UIGUIDIR)/UIControl.cpp \
	$(UIGUIDIR)/UIButton.cpp \
	$(UIGUIDIR)/UICaption.cpp \
	$(UIGUIDIR)/UIElement.cpp \
	$(UIGUIDIR)/UIGUI.cpp \
	$(UIGUIDIR)/UIScreen.cpp \
	$(UIGUIDIR)/UIImage.cpp \
	$(UIGUIDIR)/UIImageButton.cpp \
	$(UIGUIDIR)/UIMenu.cpp \
	$(UIGUIDIR)/UICanvasControl.cpp \
	$(UIGUIDIR)/UITouchCalibrator.cpp \
	$(UIGUIDIR)/UITouchCalibratorControl.cpp \
	$(UIGUIDIR)/EGalax.cpp \
	$(UIGUIDIR)/MQTTScreen.cpp \
	$(UIGUIDIR)/CURLDownloader.cpp

## End sources definition
INCLUDE = -I.
INCLUDE = -I $(UIGUIDIR)
## end more includes

VPATH=$(UIGUIDIR)
OBJ=$(join $(addsuffix obj/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cpp=.o))) 

## Fix dependency destination to be .dep relative to the src dir
DEPENDS=$(join $(addsuffix .dep/, $(dir $(SOURCE))), $(notdir $(SOURCE:.cpp=.d)))

# Update these paths to match your installation
# You may also need to update the linker option rpath, which sets where to look for
# the SDL2 libraries at runtime to match your install
SDL_LIB = `sdl2-config --libs` -lSDL2_image -lSDL2_ttf
# -L/usr/local/lib -lSDL2 -Wl,-rpath=/usr/local/lib -lSDL2_image
SDL_INCLUDE = `sdl2-config --cflags`
# -I/usr/local/include/SDL2
# You may need to change -std=c++11 to -std=c++0x if your compiler is a bit older
CFLAGS = -Wall -std=c++0x $(SDL_INCLUDE) -Wall -ggdb -I/usr/local/include -Iuigui
LDFLAGS = $(SDL_LIB) -L/usr/local/lib

# libusb 1.0 (include location may vary)
CFLAGS += -Wall -I/usr/local/include/libusb-1.0 -I/usr/include/libusb-1.0
LDFLAGS += -L/usr/local/lib  -lusb-1.0

# mosquitto
LDFLAGS +=  -lmosquitto -lmosquittopp

# curl
LDFLAGS +=  -lcurl

# jansson
CFLAGS += `pkg-config --cflags jansson`
LDFLAGS += `pkg-config --libs jansson`


## Default rule executed
all: $(TARGET)
	@true

## Clean Rule
clean:
	@-rm -f $(TARGET) $(OBJ) $(DEPENDS)
	@rm -rf .dep obj


## Rule for making the actual target
$(TARGET): $(OBJ)
	@echo "--- Linking target $@"
	@$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

## Generic compilation rule
%.o : %.cpp
	@mkdir -p $(dir $@)
	@echo "--- Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@


## Rules for object files from cpp files
## Object file for each file is put in obj directory
## one level up from the actual source directory.
obj/%.o : %.cpp
	@mkdir -p $(dir $@)
	@echo "--- Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

# Rule for "other directory"  You will need one per "other" dir
$(UIGUIDIR)/obj/%.o : %.cpp
	@mkdir -p $(dir $@)
	@echo "--- Compiling $<"
	@$(CC) $(CFLAGS) -c $< -o $@

## Make dependancy rules
.dep/%.d: %.cpp
	@mkdir -p $(dir $@)
	@echo --- Building dependencies file for $*.o
	@echo $(CFLAGS)
	@$(SHELL) -ec '$(CC) -M $(CFLAGS) $< | sed "s^$*\.o^obj/$*\.o^" > $@'

## Dependency rule for "other" directory
$(UIGUIDIR)/.dep/%.d: %.cpp
	@mkdir -p $(dir $@)
	@echo --- Building dependencies file for $*.o
	@$(SHELL) -ec '$(CC) -M $(CFLAGS) $< | sed "s^$*\.o^$(UIGUIDIR)/obj/$*\.o^" > $@'
## Had to escape the dots

## Include the dependency files
-include $(DEPENDS)
