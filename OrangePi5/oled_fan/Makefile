DIR_Config = ./lib/Config
DIR_OLED   = ./lib/OLED
DIR_FONTS  = ./lib/Fonts
DIR_GUI    = ./lib/GUI
DIR_SRC    = ./src
DIR_BIN    = ./bin

SRC = $(wildcard $(DIR_Config)/*.c $(DIR_OLED)/*.c $(DIR_GUI)/*.c $(DIR_FONTS)/*.c $(DIR_SRC)/*.c)
OBJ = $(patsubst %.c,$(DIR_BIN)/%.o,$(notdir $(SRC)))
vpath %.c $(DIR_Config) $(DIR_OLED) $(DIR_GUI) $(DIR_FONTS) $(DIR_SRC)

TARGET = oled_fan
PREFIX = /usr/local

CC      = gcc
CFLAGS += -O2 -Wall -I$(DIR_Config) -I$(DIR_OLED) -I$(DIR_GUI) -I$(DIR_FONTS)
LIBS    = -lm

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

$(DIR_BIN)/%.o: %.c | $(DIR_BIN)
	$(CC) $(CFLAGS) -c $< -o $@

$(DIR_BIN):
	mkdir -p $@

install: $(TARGET)
	install -m 755 $(TARGET) $(PREFIX)/bin/$(TARGET)
	install -m 644 oled_fan.service /etc/systemd/system/oled_fan.service
	systemctl daemon-reload
	systemctl enable --now oled_fan.service

uninstall:
	-systemctl disable --now oled_fan.service
	rm -f /etc/systemd/system/oled_fan.service $(PREFIX)/bin/$(TARGET)
	systemctl daemon-reload

clean:
	rm -rf $(DIR_BIN) $(TARGET)

.PHONY: install uninstall clean
