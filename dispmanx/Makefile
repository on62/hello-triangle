MODULES=matrix keyboard window mouse main
OBJECTS=$(foreach MODULE, ${MODULES}, build/${MODULE}.o)
INCLUDES=-I/opt/vc/include
LDFLAGS+=-L/opt/vc/lib/ -lbrcmGLESv2 -lbrcmEGL -lbcm_host -lm
EXEC=hello-triangle

all: build ${EXEC}

${EXEC}: ${OBJECTS}
	gcc $^ -o $@ ${LDFLAGS}

build/%.o : src/%.c
	gcc -c $< -o $@ ${INCLUDES}

build:
	mkdir build

clean:
	rm -rf build
	rm ${EXEC}