LIB_DIR=./raylib-5.0_linux_amd64/lib/
INC_DIR=./raylib-5.0_linux_amd64/include/
FLAGS=-Wall -Wextra -ggdb -O3

all: libplug.so snake

libplug.so: Plug.cpp
	g++ -o libplug.so -fPIC -shared $(FLAGS) -I$(INC_DIR) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) Plug.cpp -l:libraylib.so

snake: main.cpp
	g++ -o snake $(FLAGS) -I$(INC_DIR) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) main.cpp -l:libraylib.so
