SUBDIRS = components chrome
.PHONY: all clean subdirs ${SUBDIRS} content locale
VERSION = 1.2.1
XPI_NAME = mozTrayBiff
XPI_VERSION := $(VERSION)-$(PLATFORM)
XPI_FILENAME = $(XPI_NAME)-$(XPI_VERSION).xpi

ifeq ($(PLATFORM),)
# Attempt to detect the platform name
PLATFORM = `./get-platform`
endif

# This can be either 'suite' or 'tbird'
MOZILLA_PLATFORM = 
MOZILLA_VERSION = $(shell $(REAL_CONFIG) --version)

ifeq ($(MOZILLA_PLATFORM),suite)
REAL_CONFIG = mozilla-config
XPI_VERSION := $(XPI_VERSION)-moz${MOZILLA_VERSION}
XPI_EXTRA_FILES = install.js
endif
ifeq ($(MOZILLA_PLATFORM),tbird)
REAL_CONFIG = mozilla-thunderbird-config
XPI_VERSION := $(XPI_VERSION)-tb${MOZILLA_VERSION}
XPI_EXTRA_FILES = install.rdf
endif
ifeq ($(MOZILLA_PLATFORM),)
$(error You must specify the target Mozilla platform to build for ('tbird' or 'suite'). e.g. make MOZILLA_PLATFORM=tbird)
endif

ifeq ($(MOZILLA_VERSION),1.5)
	MOZ_TRUNK = 1
endif

ifdef MOZ_TRUNK
	TRUNK_TARGETS = platform
	PLATFORM_FILES = platform/
else
	PLATFORM_FILES = components/libtraybiff.so
endif

export DEBUG MOZ_TRUNK MOZILLA_PLATFORM REAL_CONFIG

all: subdirs ${TRUNK_TARGETS} ${XPI_FILENAME}

distclean: clean
	find . \( -name 'CVS' -or -name '*~' -or -name '.#*' \) -print | xargs /bin/rm -rf

clean:
	[ -e *.xpi ] && rm *.xpi; true
	[ -e xpi/components ] && rm -rf xpi/components; true
	[ -e install.rdf ] && rm install.rdf; true
	for DIR in ${SUBDIRS}; do \
		${MAKE} -C $$DIR $@; \
	done

platform: components
	mkdir platform
	mkdir platform/${PLATFORM}
	mkdir platform/${PLATFORM}/components
	cp components/libtraybiff.so platform/${PLATFORM}/components/

subdirs: ${SUBDIRS}

${SUBDIRS}:
	# Build with apbuild, if possible
	if which apgcc > /dev/null && which apg++ > /dev/null; then \
		APBUILD_STATIC_LIBGCC=1 CC=apgcc CXX=apg++ ${MAKE} -C $@; \
	else \
		${MAKE} -C $@; \
	fi

install.rdf: install.rdf.in
	./make-install.rdf

${XPI_FILENAME}: subdirs install.rdf
	rm -rf $@; \
	zip $@ -r \
		${PLATFORM_FILES} \
		components/libtraybiff.xpt \
		chrome/tray-biff.jar \
		defaults/preferences/tray-biff.js \
		$(XPI_EXTRA_FILES)
