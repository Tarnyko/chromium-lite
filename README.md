### Chromium-Lite build system

![Content-Shell screenshot](http://www.tarnyko.net/repo/chromium-lite.png)

An attempt to cut down Chromium, and make it more comprehensive,
by doing the following :

- extracting Linux-only sources (removing MacOS, Win32, Android...
                                 source files);
- linking with system libraries (system libnss instead of local
                                 Google BoringSSL e.g.);
- configuring with CMake instead of Gyp ;
- compiling with GCC instead of Clang/LLVM ;
- disabling unneeded optimizations ;
- making debugging easier.


## How to clone the source

To grab the whole Chromium source I am using (it is around 12 GB,
thus schedule accordingly) :

   $ git clone https://chromium.googlesource.com/chromium/tools/depot_tools.git
   $ export PATH="$PWD/depot_tools:$PATH"
   $ gclient config https://github.com/Tarnyko/chromium-lite.git \
 --name=chromium-lite --deps-file DEPS
   $ gclient sync --nohooks

## How to build

 Relatively to the cloning root tree, here is the root folder of each package :
01a-base) base
01b-skia) third_party/skia
01c-v8) v8
02a-url) url
02b-crypto) crypto
02c-ipc) ipc
02d-sdch) sdch
02e-gpu_command_buffer) gpu/command_buffer
03a-net) net
03d-ui) ui/gfx
04a-ozone) ozone
04d-ui_events) ui/events
05a-media) media
05b-ui_base) ui/base
06b-ui_gl) ui/gl
07b-gpu) gpu
08a-cc) cc
09a-blink) third_party/WebKit/Source

 For each "recipe", please copy it this folder the "CMakeList.txt" file,
and any .cmake and .patch file present.
 Then, move to it, and apply the patches with :
    $ patch -p0 < <patchfile>
 Finally, build the package :
    $ mkdir Build ; cd Build
    $ cmake ..
 (for Skia :
    $ cmake .. -DBACKEND=GLX -DCMAKE_TOOLCHAIN_FILE=../Toolchain-x86_64.cmake
 for others :
    $ cmake .. -DBACKEND=X11 -DCMAKE_TOOLCHAIN_FILE=../Toolchain-x86_64.cmake
