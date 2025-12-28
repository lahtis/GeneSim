# ==========================================
# GeneSim NameGen v2.0.0 Makefile
# NewYear-Edition
# ==========================================

# K‰‰nt‰j‰ ja liput
CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS =

# Tiedostopolut
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# L‰hdekoodit (kaikki .c tiedostot src-kansiosta)
SRCS = $(wildcard $(SRC_DIR)/*.c) $(SRC_DIR)/utils/cJSON.c
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

# Lopullinen ohjelma
TARGET = $(BIN_DIR)/namegen.exe

# Oletusajo (k‰‰nt‰‰ ohjelman)
all: setup $(TARGET)

# Luo tarvittavat kansiot
setup:
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(OBJ_DIR)/utils
	@mkdir -p $(BIN_DIR)

# Linkitys
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)
	@echo "------------------------------------------------"
	@echo "Build Complete: GeneSim NameGen v2.0.0 Created"
	@echo "Run with: ./bin/namegen.exe --version"
	@echo "------------------------------------------------"

# Objektitiedostojen k‰‰nt‰minen
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Puhdistus
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "Project cleaned."

.PHONY: all setup clean
