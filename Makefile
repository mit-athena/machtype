OS_SCRIPT = machtype_linux.sh
MKDIR_P = mkdir -p
INSTALL = install
lbindir = /bin
mandir = /usr/share/man
ATHENA_MAJOR_VERSION = 10
ATHENA_MINOR_VERSION = 0
SYSNAMES := $(shell ./generate_sysnames.py)
MACHTYPE_ATHENA_SYS := $(word 1, $(SYSNAMES))
MACHTYPE_ATHENA_SYS_COMPAT := $(word 2, $(SYSNAMES))
ifeq ($(MACHTYPE_ATHENA_SYS),)
    $(error MACHTYPE_ATHENA_SYS unset)
endif
ifeq ($(MACHTYPE_ATHENA_SYS_COMPAT),)
    $(error MACHTYPE_ATHENA_SYS_COMPAT unset)
endif

.PHONY: all install clean

all: machtype

machtype.sh: ${OS_SCRIPT}
	rm -f $@
	sed -e 's/@ATHENA_MAJOR_VERSION@/${ATHENA_MAJOR_VERSION}/' \
	    -e 's/@ATHENA_MINOR_VERSION@/${ATHENA_MINOR_VERSION}/' \
	    -e 's/@ATHENA_SYS@/${MACHTYPE_ATHENA_SYS}/' \
	    -e 's/@ATHENA_SYS_COMPAT@/${MACHTYPE_ATHENA_SYS_COMPAT}/' \
	    ${OS_SCRIPT} > $@

install:
	${MKDIR_P} ${DESTDIR}${lbindir}
	${MKDIR_P} ${DESTDIR}${mandir}/man1
	${INSTALL} -m 755 machtype.sh ${DESTDIR}${lbindir}/machtype
	${INSTALL} -m 444 machtype.1 ${DESTDIR}${mandir}/man1

clean:
	rm -f machtype.sh
