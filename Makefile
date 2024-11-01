# Nombre del ejecutable
TARGET = metrics

# Directorios de código fuente, encabezados y librerías locales
SRC_DIR = src
INCLUDE_DIR = include
PROM_CLIENT_DIR = prometheus-client-c
LIB_DIR = lib

# Archivos fuente
SRCS = $(SRC_DIR)/main.c $(SRC_DIR)/metrics.c $(SRC_DIR)/expose_metrics.c

# Librerías
LIBS = -lprom -pthread -lpromhttp
LDFLAGS = -L$(PROM_CLIENT_DIR)/lib
CFLAGS = -I$(INCLUDE_DIR) -I$(PROM_CLIENT_DIR)/include

# Exportar la variable de entorno LD_LIBRARY_PATH
export LD_LIBRARY_PATH := $(PROM_CLIENT_DIR)/lib:$(LD_LIBRARY_PATH)

# Regla por defecto (se ejecuta al correr 'make')
all: $(TARGET)

# Regla para compilar el programa
$(TARGET): $(SRCS)
	$(CC) $(SRCS) $(CFLAGS) $(LDFLAGS) $(LIBS) -o $(TARGET)

# Regla para limpiar los archivos generados
clean:
	rm -f $(TARGET)

