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


Using Gtk+ version 2
--------------------

libDesktop defaults to the version 3 of the Gtk+ toolkit. Alternatively, it is
possible to re-generate the Makefiles for the Gtk+ 2 toolkit with `configure(1)`
from the DeforaOS configure project, found at
<https://www.defora.org/os/project/16/configure>. The procedure is then as
follows:

    $ configure -M gtk2-debug
    [or "gtk2-release"]
    $ make distclean
    $ make

Please refer to the documentation of DeforaOS configure for further
instructions.
