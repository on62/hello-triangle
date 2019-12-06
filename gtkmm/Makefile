MODULES=triangleglarea mainwindow timercpp main
SOURCES=$(foreach MODULE, $(MODULES), src/$(MODULE).cc)
OBJECTS=$(foreach MODULE, $(MODULES), build/$(MODULE).o)
EXEC=hello-triangle
CFLAGS=`pkg-config --cflags gtkmm-3.0 glew` -std=c++17 -pthread
LDFLAGS=`pkg-config --libs gtkmm-3.0 glew` -pthread

all: build $(EXEC)

$(EXEC): $(OBJECTS)
	g++ $^ -o ${EXEC} ${LDFLAGS}

build/%.o: src/%.cc
	g++ -c $< -o $@ $(CFLAGS)

build:
	mkdir build

clean:
	rm -rf build
	rm ${EXEC}
