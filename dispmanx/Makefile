MODULES=matrix keyboard window mouse main
OBJECTS=$(foreach MODULE, ${MODULES}, build/${MODULE}.o)
CFLAGS=-I/opt/vc/include `pkg-config --libs cairo`
LDFLAGS+=-L/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lm `pkg-config --libs cairo`
EXEC=hello-triangle

all: build ${EXEC}

${EXEC}: ${OBJECTS}
	gcc $^ -o $@ ${LDFLAGS}

build/%.o : src/%.c
	gcc -c $< -o $@ ${CFLAGS}

build:
	mkdir build

clean:
	rm -rf build
	rm ${EXEC}