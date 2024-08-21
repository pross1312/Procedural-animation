LIB_DIR=./raylib-5.0_linux_amd64/lib/
INC_DIR=./raylib-5.0_linux_amd64/include/
FLAGS=-Wall -Wextra -ggdb

all: libplug.so fex

libplug.so: Plug.cpp
	g++ -o libplug.so -fPIC -shared $(FLAGS) -I$(INC_DIR) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) Plug.cpp -l:libraylib.so

fex: main.cpp
	g++ -o fex $(FLAGS) -I$(INC_DIR) -L$(LIB_DIR) -Wl,-rpath=$(LIB_DIR) main.cpp -l:libraylib.so
