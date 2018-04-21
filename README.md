DeforaOS libDesktop
===================

About libDesktop
----------------

libDesktop is a collection of compatibility and portability routines to help
with the development of desktop applications using the Gtk+ toolkit.


Compiling libDesktop
--------------------

With GCC, this should be enough:

    $ make

To install libDesktop in a dedicated directory, like `/path/to/libDesktop`:

    $ make PREFIX="/path/to/libDesktop" install


Using Gtk+ version 3
--------------------

libDesktop defaults to the version 2 of the Gtk+ toolkit. Alternatively, it is
possible to re-generate the Makefiles for the Gtk+ 3 toolkit with `configure(1)`
from the DeforaOS configure project, found at
<https://www.defora.org/os/project/16/configure>. The procedure is then as
follows:

    $ $EDITOR data/libDesktop.pc.in
    [replace "gtk+-2.0" with "gtk+-3.0"]
    $ $EDITOR src/project.conf tests/project.conf tools/project.conf
    [comment lines in and out as described]
    $ make distclean
    $ configure
    $ make

Please refer to the documentation of DeforaOS configure for further
instructions.
