# Definir el compilador
CC = g++

# Bandera de compilación (ahora con -fopenmp)
CFLAGS = -Wall -std=c++17 -fopenmp

# Nombre del ejecutable
TARGET = programa_buddy

# Archivos fuente
SRCS = main.cpp imagen.cpp buddy_allocator.cpp stb_wrapper.cpp

# Archivos objeto generados
OBJS = $(SRCS:.cpp=.o)

# Regla para compilar el programa
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lm

# Regla para compilar cada archivo fuente a objeto
%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@

# Limpieza de archivos objeto y ejecutables
clean:
	rm -f $(OBJS) $(TARGET)

# Ejecutar el programa buddy system (usa rotación y escalado en paralelo con OpenMP)
run_buddy: all
	./$(TARGET) img.jpg result.jpg -angulo 9.5 -escalar 2 -buddy

# Ejecutar el programa con new/delete
run_nobuddy: all
	./$(TARGET) img.jpg result.jpg -angulo 9.5 -escalar 2 -no-buddy
