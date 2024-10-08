# Define variables
CC = gcc                         # El compilador
CFLAGS = -Wall -g                # Flags para el compilador (habilitar todas las advertencias y depuración)
LDFLAGS = -lprom -pthread -lpromhttp  # Librerías externas que estás usando
SRC_DIR = src                    # Directorio de archivos fuente
SRC = $(SRC_DIR)/main.c          # Solo el archivo main.c
OBJ = $(SRC:.c=.o)               # Archivo objeto generado a partir del archivo fuente
TARGET = metrics                 # Nombre del ejecutable final

# Regla para compilar todo
all: $(TARGET)

# Regla para generar el archivo ejecutable
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Regla para compilar archivos .c en .o
$(SRC_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpia los archivos objeto y el ejecutable
clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
