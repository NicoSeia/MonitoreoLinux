# Define variables
CC = gcc                         # El compilador
CFLAGS = -Wall -g -Iinclude      # Flags para el compilador (habilitar todas las advertencias y depuración) y agregar la ruta de los encabezados
LDFLAGS = -lprom -pthread -lpromhttp  # Librerías externas que estás usando

# Especifica los archivos fuente
SRC = src/main.c                 # Archivo fuente principal
OBJ = $(SRC:.c=.o)               # Archivos objeto generados a partir de los archivos fuente
TARGET = metrics                 # Nombre del ejecutable final

# Regla para compilar todo
all: $(TARGET)

# Regla para generar el archivo ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Regla para compilar archivos .c en .o
%.o: src/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpia los archivos objeto y el ejecutable
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
