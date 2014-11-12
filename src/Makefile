TARGETS	= $(OBJDIR)libDesktop.a $(OBJDIR)libDesktop.so.0.0 $(OBJDIR)libDesktop.so.0 $(OBJDIR)libDesktop.so
OBJDIR	=
PREFIX	= /usr/local
DESTDIR	=
LIBDIR	= $(PREFIX)/lib
CC	= cc
CPPFLAGSF= -I ../include
CPPFLAGS=
CFLAGSF	= -W `pkg-config --cflags libSystem gtk+-2.0`
CFLAGS	= -Wall -g -O2 -fPIC
LDFLAGSF= `pkg-config --libs libSystem gtk+-2.0`
AR	= ar
RANLIB	= ranlib
CCSHARED= $(CC) -shared
RM	= rm -f
LN	= ln -f
MKDIR	= mkdir -m 0755 -p
INSTALL	= install


all: $(TARGETS)

libDesktop_OBJS = $(OBJDIR)about.o $(OBJDIR)accel.o $(OBJDIR)compat.o $(OBJDIR)help.o $(OBJDIR)menubar.o $(OBJDIR)message.o $(OBJDIR)mime.o $(OBJDIR)toolbar.o $(OBJDIR)widget.o
libDesktop_CFLAGS = $(CPPFLAGSF) $(CPPFLAGS) $(CFLAGSF) $(CFLAGS)
libDesktop_LDFLAGS = $(LDFLAGSF) $(LDFLAGS)

$(OBJDIR)libDesktop.a: $(libDesktop_OBJS)
	$(AR) -rc $(OBJDIR)libDesktop.a $(libDesktop_OBJS)
	$(RANLIB) $(OBJDIR)libDesktop.a

$(OBJDIR)libDesktop.so.0.0 $(OBJDIR)libDesktop.so.0 $(OBJDIR)libDesktop.so: $(libDesktop_OBJS)
	$(CCSHARED) -o $(OBJDIR)libDesktop.so.0.0 -Wl,-soname,libDesktop.so.0 $(libDesktop_OBJS) $(libDesktop_LDFLAGS)
	$(LN) -s -- libDesktop.so.0.0 $(OBJDIR)libDesktop.so.0
	$(LN) -s -- libDesktop.so.0.0 $(OBJDIR)libDesktop.so

$(OBJDIR)about.o: about.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)about.o -c about.c

$(OBJDIR)accel.o: accel.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)accel.o -c accel.c

$(OBJDIR)compat.o: compat.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)compat.o -c compat.c

$(OBJDIR)help.o: help.c ../config.h
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)help.o -c help.c

$(OBJDIR)menubar.o: menubar.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)menubar.o -c menubar.c

$(OBJDIR)message.o: message.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)message.o -c message.c

$(OBJDIR)mime.o: mime.c ../config.h
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)mime.o -c mime.c

$(OBJDIR)toolbar.o: toolbar.c
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)toolbar.o -c toolbar.c

$(OBJDIR)widget.o: widget.c ../config.h
	$(CC) $(libDesktop_CFLAGS) -o $(OBJDIR)widget.o -c widget.c

clean:
	$(RM) -- $(libDesktop_OBJS)

distclean: clean
	$(RM) -- $(TARGETS)

install: $(TARGETS)
	$(MKDIR) $(DESTDIR)$(PREFIX)/lib
	$(INSTALL) -m 0644 $(OBJDIR)libDesktop.a $(DESTDIR)$(PREFIX)/lib/libDesktop.a
	$(INSTALL) -m 0755 $(OBJDIR)libDesktop.so.0.0 $(DESTDIR)$(PREFIX)/lib/libDesktop.so.0.0
	$(LN) -s -- libDesktop.so.0.0 $(DESTDIR)$(PREFIX)/lib/libDesktop.so.0
	$(LN) -s -- libDesktop.so.0.0 $(DESTDIR)$(PREFIX)/lib/libDesktop.so

uninstall:
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/libDesktop.a
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/libDesktop.so.0.0
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/libDesktop.so.0
	$(RM) -- $(DESTDIR)$(PREFIX)/lib/libDesktop.so

.PHONY: all clean distclean install uninstall
