SUBDIRS = components chrome
.PHONY: all clean subdirs ${SUBDIRS} content locale
VERSION = 1.0.6
PLATFORM := $(shell gcc -dumpmachine)
XPI_NAME = mozTrayBiff
XPI_VERSION := $(VERSION)-$(PLATFORM)
XPI_FILENAME = $(XPI_NAME)-$(XPI_VERSION).xpi

# This can be either 'suite' or 'tbird'
MOZILLA_PLATFORM = 

ifeq ($(MOZILLA_PLATFORM),suite)
REAL_CONFIG = mozilla-config
XPI_VERSION := $(XPI_VERSION)-moz$(shell $(REAL_CONFIG) --version)
XPI_EXTRA_FILES = install.js
endif
ifeq ($(MOZILLA_PLATFORM),tbird)
REAL_CONFIG = mozilla-thunderbird-config
XPI_VERSION := $(XPI_VERSION)-tb$(shell $(REAL_CONFIG) --version)
XPI_EXTRA_FILES = install.rdf
endif
ifeq ($(MOZILLA_PLATFORM),)
$(error You must specify the target Mozilla platform to build for ('tbird' or 'suite'). e.g. make MOZILLA_PLATFORM=tbird)
endif

export DEBUG MOZ_TRUNK MOZILLA_PLATFORM REAL_CONFIG

# all:
# 	echo $(PLATFORM)

all: subdirs ${XPI_FILENAME}

clean:
	[ -e *.xpi ] && rm *.xpi; true
	[ -e xpi/components ] && rm -rf xpi/components; true
	for DIR in ${SUBDIRS}; do \
		${MAKE} -C $$DIR $@; \
	done

subdirs: ${SUBDIRS}

${SUBDIRS}:
	${MAKE} -C $@

${XPI_FILENAME}: subdirs
	rm -rf $@; \
	zip $@ \
		components/libtraybiff.so \
		chrome/tray-biff.jar \
		defaults/preferences/tray-biff.js \
		$(XPI_EXTRA_FILES)
