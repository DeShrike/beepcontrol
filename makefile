CC=gcc
AR=ar
LIBS=-lwiringPi -lpthread
CFLAGS=-Wall -O3
OBJ=beepcontrol.o ini.o config.o

all: beepcontrol

rmbin:
	rm beepcontrol

beepcontrol: $(OBJ)
	$(CC) -o beepcontrol $(OBJ) $(LIBS)

fancontrol.o: beepcontrol.c config.h beepcontrol.h
	$(CC) -c $(CFLAGS) beepcontrol.c

config.o: config.c config.h beepcontrol.h
	$(CC) -c $(CFLAGS) config.c

ini.o: ini.c ini.h
	$(CC) -c $(CFLAGS) ini.c

clean:
	rm -v *.o

install: beepcontrol beepcontrol.service
	sudo cp beepcontrol /usr/bin
	sudo cp beepcontrol.service /etc/systemd/system
	sudo cp beepcontrol.ini /etc
	sudo systemctl daemon-reload

uninstall: beepcontrol beepcontrol.service
	sudo systemctl stop beepcontrol
	sudo systemctl disable beepcontrol
	sudo rm /usr/bin/beepcontrol
	sudo rm /etc/systemd/system/beepcontrol.service
	sudo rm /etc/beepcontrol.ini
	sudo systemctl daemon-reload

start:
	sudo systemctl start beepcontrol

stop:
	sudo systemctl stop beepcontrol

enable:
	sudo systemctl enable beepcontrol

disable:
	sudo systemctl disable beepcontrol
