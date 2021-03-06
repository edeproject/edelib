# edelib installation

To build library, you will need the following tools and libraries:
 * C++ compiler (of course)
 * autoconf
 * jam
 * pkg-config
 * FLTK 1.1.x (including FLTK prerequisites)
 * Doxygen (for building API documentation)
 * D-BUS

FLTK library can be obtained from http://www.fltk.org.

As build tool instead a standard make, jam is used. Jam is make-like tool with it's own
syntax and you can find more about at Perforce jam pages (http://www.perforce.com/jam/jam.html).

Since in the wild exists many jam versions and modifications, copy of the version that is used
for development is on our repository where you can download it. Also one of the goals
is to keep current code buildable on other jam versions, so it will work also on:

 * [original jam](http://www.perforce.com/jam/jam.html)
 * [ftjam](http://www.freetype.org/jam.html)

For manual building a Jam tool, please look at the Jam's documentation. After you got the binary,
you can either install it at the usual paths or copy it in edelib root directory.

To build edelib, do:

 1. If you downloaded source from our repository, run `./autogen.sh` first.

 2. Run `./configure`. You can tune building options via parameters given to configure
    script. To see available parameters, run `./configure --help`.

 3. Run `jam` to compile the code.

 4. `jam install` will install library and needed files.


## Compiling under SunStudio compiler

To build under SunStudio compiler, set the following environment variables before running 
configure script, like:

```sh
declare -x CC="[path]/sunstudioXX/bin/cc"
declare -x CXX="[path]/sunstudioXX/bin/CC"
declare -x LDFLAGS="-L[path]/lib"
declare -x LIBS="-lCrun -lCstd"
declare -x STDLIB="$LDFLAGS $LIBS"
```

This is an example how to set variables in bash; your shell maybe uses different methods.

`[path]` is the path to your local SunStudio installation; sunstudioXX is subdirectory containing
version number (e.g. sunstudio12). Of course, paths can be different, so you should locate 
'cc', 'CC' programs and 'libCrun.so' and 'libCstd.so' libraries.

**NOTE**: you have to compile FLTK with SunStudio too or linking would fail!

edelib configure script provides `--with-fltk-path` for this purpose so you can specify
alternative FLTK installation path.
