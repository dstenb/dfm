CC = cc
CFLAGS = -std=c99 -D_GNU_SOURCE -g -O2 -pthread -Wall -pedantic `pkg-config --cflags --libs gtk+-2.0` -export-dynamic

PREFIX = /usr/local

SRC = dfm.c
OBJ = ${SRC:.c=.o}

all: clean dfm

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

$(OBJ): config.h version.h

dfm: ${OBJ}
	$(CC) $(LDFLAGS) -o $@ $(OBJ) $(CFLAGS)

config:
	@echo creating default config.h from config.ex.h
	@cp config.ex.h config.h

clean:
	@echo cleaning directory
	@rm -f dfm ${OBJ} *.gch version.h

install: all
	@echo installing dfm to ${PREFIX}/bin
	@mkdir -p ${PREFIX}/bin
	@cp -f dfm ${PREFIX}/bin
	@chmod 755 ${PREFIX}/bin/dfm

uninstall:
	@echo removing dfm from ${PREFIX}/bin
	@rm -f ${PREFIX}/dfm

version.h:
	@echo "#ifndef _VERSION_H_" >  version.h
	@echo "#define _VERSION_H_" >> version.h
	@echo -n "#define VERSION \"" >> version.h
	@git show -s --pretty=format:"dfm commit %h (%ai)\"%n" >> version.h
	@echo "#endif" >> version.h

.PHONY: clean
