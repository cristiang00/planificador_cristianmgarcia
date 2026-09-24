# Simulador de planificacion de procesos
# make          construye el simulador
# make clean    borra lo generado

CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2 -Isrc
PROGRAMA = planificador
FUENTES  = src/main.cpp src/planificador.cpp src/grafica.cpp

# Un archivo objeto solo sirve en el sistema donde se compilo, de modo que cada
# uno guarda los suyos aparte. Sin esta separacion, copiar el proyecto de una
# maquina a otra deja objetos que make da por buenos y el enlazador rechaza.
ifeq ($(OS),Windows_NT)
    PLATAFORMA = windows
else
    PLATAFORMA = $(shell uname -s)
endif
OBJDIR  = obj/$(PLATAFORMA)
OBJETOS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(FUENTES))

all: $(PROGRAMA)

$(PROGRAMA): $(OBJETOS)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJETOS)

$(OBJDIR)/%.o: src/%.cpp
	mkdir -p $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

# La documentacion se genera con Doxygen. Si no existe el archivo de
# configuracion, doxygen -g escribe uno con los valores por omision.
doc: Doxyfile
	doxygen

Doxyfile:
	doxygen -g

clean:
	rm -rf obj html latex
	rm -f $(PROGRAMA) $(PROGRAMA).exe
	rm -f test/*.gpi test/*.png gantt.gpi gantt.png

.PHONY: all clean doc
