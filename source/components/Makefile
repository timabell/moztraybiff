SHELL = /bin/sh

.PHONY: clean all

MOZILLA_LIB_PREFIX = `./mozilla-config --lib-prefix`
MOZILLA_UTIL_PREFIX = `./mozilla-config --util-prefix`

COMPONENT_NAME = libtraybiff

GTK_PACKAGE = gtk+-2.0

GTK_CFLAGS = \
	`pkg-config --cflags $(GTK_PACKAGE)`

GTK_LDFLAGS = \
	`pkg-config --libs $(GTK_PACKAGE)`

MOZ_INCLUDES = \
	`./mozilla-config --cflags`

CXXFLAGS += \
	-fno-rtti \
	-fno-exceptions \
	-fshort-wchar

ifdef DEBUG
	CXXFLAGS += -g
	CFLAGS += -g
else
	CXXFLAGS += -O2
	CFLAGS += -O2
endif

ifdef MOZ_TRUNK
	CXXFLAGS += -DMOZ_TRUNK
	CFLAGS += -DMOZ_TRUNK
endif

MOZ_LDFLAGS += \
	`./mozilla-config --libs xpcom`

all: ${COMPONENT_NAME}.so

testsystray: testsystray.c eggtrayicon.o eggstatusicon.o eggmarshalers.o
	${CC} ${GTK_CFLAGS} ${GTK_LDFLAGS} ${CFLAGS} $^ -o $@

${COMPONENT_NAME}.so: trayBiffModule.o nsMessengerFreeDesktopIntegration.o eggtrayicon.o eggstatusicon.o eggmarshalers.o
	if [ -e $@ ]; then \
		rm $@; \
	fi
	${CXX} ${MOZ_LDFLAGS} ${GTK_LDFLAGS} -shared -o $@ $^

trayBiffIcon.h: message-mail-new.png
	gdk-pixbuf-csource --raw --name=tray_biff_icon $< > $@

trayBiffModule.o: trayBiffModule.cpp nsMessengerFreeDesktopIntegration.h
	${CXX} ${MOZ_INCLUDES} ${GTK_CFLAGS} ${CXXFLAGS} -c $< -o $@

nsMessengerFreeDesktopIntegration.o: nsMessengerFreeDesktopIntegration.cpp nsMessengerFreeDesktopIntegration.h trayBiffIcon.h
	${CXX} ${MOZ_INCLUDES} ${GTK_CFLAGS} ${CXXFLAGS} -c $< -o $@

eggtrayicon.o: eggtrayicon.c eggtrayicon.h
	${CC} ${GTK_CFLAGS} ${CFLAGS} -DEGG_COMPILATION -c $< -o $@

eggstatusicon.o: eggstatusicon.c eggstatusicon.h eggmarshalers.h
	${CC} ${GTK_CFLAGS} ${CFLAGS} -DEGG_COMPILATION -c $< -o $@

eggmarshalers.o: eggmarshalers.c eggmarshalers.h
	${CC} ${GTK_CFLAGS} ${CFLAGS} -DEGG_COMPILATION -c $< -o $@

eggmarshalers.c: eggmarshalers.list
	glib-genmarshal --prefix=_egg_marshal --body $< > $@

eggmarshalers.h: eggmarshalers.list
	glib-genmarshal --prefix=_egg_marshal --header $< > $@

register:
	LD_LIBRARY_PATH=$MOZILLA_UTIL_PREFIX:`pwd` $MOZILLA_UTIL_PREFIX/regxpcom ${COMPONENT_NAME}.so

register-root:
	su -c 'LD_LIBRARY_PATH=$MOZILLA_UTIL_PREFIX:`pwd` $MOZILLA_UTIL_PREFIX/regxpcom `pwd`/${COMPONENT_NAME}.so'

unregister:
	LD_LIBRARY_PATH=$MOZILLA_UTIL_PREFIX:`pwd` $MOZILLA_UTIL_PREFIX/regxpcom -u ${COMPONENT_NAME}.so

unregister-root:
	su -c 'LD_LIBRARY_PATH=$MOZILLA_UTIL_PREFIX:`pwd` $MOZILLA_UTIL_PREFIX/regxpcom -u `pwd`/${COMPONENT_NAME}.so'

shell:
	LD_LIBRARY_PATH=$MOZILLA_UTIL_PREFIX:`pwd` $MOZILLA_UTIL_PREFIX/xpcshell

clean:
	rm -f *.o *.so testsystray; exit 0
