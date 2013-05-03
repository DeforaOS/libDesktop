PREFIX	= /usr/local
DESTDIR	=
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install
INCLUDEDIR= $(PREFIX)/include


all:

clean:

distclean: clean

install:
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 about.h $(DESTDIR)$(INCLUDEDIR)/Desktop/about.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 accel.h $(DESTDIR)$(INCLUDEDIR)/Desktop/accel.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 compat.h $(DESTDIR)$(INCLUDEDIR)/Desktop/compat.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 help.h $(DESTDIR)$(INCLUDEDIR)/Desktop/help.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 menubar.h $(DESTDIR)$(INCLUDEDIR)/Desktop/menubar.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 message.h $(DESTDIR)$(INCLUDEDIR)/Desktop/message.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 mime.h $(DESTDIR)$(INCLUDEDIR)/Desktop/mime.h
	$(MKDIR) $(DESTDIR)$(INCLUDEDIR)/Desktop
	$(INSTALL) -m 0644 toolbar.h $(DESTDIR)$(INCLUDEDIR)/Desktop/toolbar.h

uninstall:
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/about.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/accel.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/compat.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/help.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/menubar.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/message.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/mime.h
	$(RM) -- $(DESTDIR)$(INCLUDEDIR)/Desktop/toolbar.h

.PHONY: all clean distclean install uninstall
