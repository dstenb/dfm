CC = cc
CFLAGS = -g -O2 -pthread -Wall -pedantic `pkg-config --cflags --libs gtk+-2.0` -export-dynamic

PREFIX = /usr/local

SRC = dfm.c
OBJ = ${SRC:.c=.o}

all: dfm

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

$(OBJ): config.h

dfm: ${OBJ}
	@echo CC -o $@
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(CFLAGS)

config:
	@echo creating default config.h from config.ex.h
	@cp config.ex.h config.h

clean:
	@echo cleaning directory
	@rm -f fm dfm ${OBJ} *.gch

install: all
	@echo installing dfm to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp -f dfm ${PREFIX}/bin
	@chmod 755 ${PREFIX}/bin/dfm

uninstall:
	@echo removing dfm from ${PREFIX}/bin
	@rm -f ${PREFIX}/dfm
