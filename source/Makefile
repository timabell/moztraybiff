SUBDIRS = components chrome
# PLATFORM = `gcc -dumpmachine`
.PHONY: all clean subdirs ${SUBDIRS} content locale
XPI_NAME = mozTrayBiff-1.0.5

export DEBUG MOZ_TRUNK

all: subdirs ${XPI_NAME}.xpi

clean:
	rm ${XPI_NAME}.xpi 2>/dev/null; true
	[ -e xpi/components ] && rm -rf xpi/components; true
	for DIR in ${SUBDIRS}; do \
		${MAKE} -C $$DIR $@; \
	done

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@

${XPI_NAME}.xpi: subdirs
	rm -rf $@; \
	zip $@ \
		components/libtraybiff.so \
		chrome/tray-biff.jar \
		defaults/preferences/tray-biff.js \
		install.rdf \
		install.js
