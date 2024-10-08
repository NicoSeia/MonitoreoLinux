# Define variables
CC = gcc                         # El compilador
CFLAGS = -Wall -g                # Flags para el compilador (habilitar todas las advertencias y depuración)
LDFLAGS = -lprom -pthread -lpromhttp  # Librerías externas que estás usando
SRC = main.c 					 # Archivos fuente
OBJ = $(SRC:.c=.o)               # Archivos objeto generados a partir de los archivos fuente
TARGET = metrics                 # Nombre del ejecutable final

# Regla para compilar todo
all: $(TARGET)

# Regla para generar el archivo ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Regla para compilar archivos .c en .o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpia los archivos objeto y el ejecutable
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
