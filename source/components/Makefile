SHELL = /bin/sh

.PHONY: clean all

ifndef REAL_CONFIG
$(error Missing REAL_CONFIG variable, which should point to the real mozilla-config from your Mozilla installation. Please run the top-level Makefile.)
endif

MOZILLA_CONFIG = ./mozilla-config --real-config $(REAL_CONFIG)
MOZILLA_LIB_PREFIX := \
	$(shell $(MOZILLA_CONFIG) --lib-prefix)
MOZILLA_UTIL_PREFIX := \
	$(shell $(MOZILLA_CONFIG) --util-prefix)

# This doesn't work, so we'll hardcode for now:
MOZILLA_IDLFLAGS := \
	$(shell $(MOZILLA_CONFIG) --idlflags)

COMPONENT_NAME = libtraybiff

GTK_PACKAGE = gtk+-2.0

GTK_CFLAGS := \
	$(shell pkg-config --cflags $(GTK_PACKAGE))

GTK_LDFLAGS := \
	$(shell pkg-config --libs $(GTK_PACKAGE))

MOZ_INCLUDES := \
	$(shell $(MOZILLA_CONFIG) --cflags)

CFLAGS += \
	-fPIC

CXXFLAGS += \
	-fno-rtti \
	-fno-exceptions \
	-fshort-wchar \
	-fPIC

INTERNAL_FLAGS = -DMOZILLA_INTERNAL_API

ifdef DEBUG
	CXXFLAGS += -g -DDEBUG -D_DEBUG -DTRACING
	CFLAGS += -g -DDEBUG -D_DEBUG -DTRACING
else
	CXXFLAGS += -O2
	CFLAGS += -O2
endif

ifdef MOZ_TRUNK
	CXXFLAGS += -DMOZ_TRUNK
	CFLAGS += -DMOZ_TRUNK
endif

MOZ_LDFLAGS := \
	$(shell $(MOZILLA_CONFIG) --libs xpcom)

all: ${COMPONENT_NAME}.so ${COMPONENT_NAME}.xpt

testsystray: testsystray.c eggtrayicon.o eggstatusicon.o eggmarshalers.o
	${CC} ${GTK_CFLAGS} ${GTK_LDFLAGS} ${CFLAGS} $^ -o $@

${COMPONENT_NAME}.so: trayBiffModule.o nsMessengerFreeDesktopIntegration.o eggtrayicon.o eggstatusicon.o eggmarshalers.o
	if [ -f $@ ]; then \
		rm $@; \
	fi
	${CXX} ${MOZ_LDFLAGS} ${GTK_LDFLAGS} -Wl,--discard-all -Wl,-Bsymbolic -Wl,--version-script=${COMPONENT_NAME}.version_script -shared -o $@ $^

${COMPONENT_NAME}.xpt: nsIMessengerFreeDesktopIntegration.xpt
	LD_LIBRARY_PATH=$(MOZILLA_UTIL_PREFIX):`pwd` $(MOZILLA_UTIL_PREFIX)/xpt_link $@ $<

%.xpt: %.idl
	LD_LIBRARY_PATH=$(MOZILLA_UTIL_PREFIX):`pwd` $(MOZILLA_UTIL_PREFIX)/xpidl $(MOZILLA_IDLFLAGS) -m typelib -e $@ $<

%.h: %.idl
	LD_LIBRARY_PATH=$(MOZILLA_UTIL_PREFIX):`pwd` $(MOZILLA_UTIL_PREFIX)/xpidl $(MOZILLA_IDLFLAGS) -m header -e $@ $<

nsIMessengerFreeDesktopIntegration.xpt: nsIMessengerFreeDesktopIntegration.idl

nsIMessengerFreeDesktopIntegration.h: nsIMessengerFreeDesktopIntegration.idl

trayBiffIcon.h: message-mail-new.png
	gdk-pixbuf-csource --raw --name=tray_biff_icon $< > $@

trayBiffModule.o: trayBiffModule.cpp nsMessengerFreeDesktopIntegration.h nsIMessengerFreeDesktopIntegration.h
	${CXX} ${MOZ_INCLUDES} ${GTK_CFLAGS} ${CXXFLAGS} ${INTERNAL_FLAGS} -c $< -o $@

nsMessengerFreeDesktopIntegration.o: nsMessengerFreeDesktopIntegration.cpp nsMessengerFreeDesktopIntegration.h trayBiffIcon.h nsIMessengerFreeDesktopIntegration.h
	${CXX} ${MOZ_INCLUDES} ${GTK_CFLAGS} ${CXXFLAGS} ${INTERNAL_FLAGS} -c $< -o $@

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

xulAppInfo: xulAppInfo.cpp
	${CXX} -g ${CXXFLAGS} ${MOZ_LDFLAGS} ${MOZ_INCLUDES} $< -o $@

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
	rm -f *.o *.so *.xpt nsIMessengerFreeDesktopIntegration.h testsystray; exit 0
