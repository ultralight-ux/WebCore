# Ultralight WebCore Port

## You'll need the following:

 * CMake 3.2+ (https://cmake.org/)
 * Ninja (https://ninja-build.org/)
 * Recent GCC/Clang on macOS/Linux
 * Visual Studio 2015 on Windows
 * Perl 5.10+
 * Python 2.7+
 * Ruby 1.9+
 * Gperf 3.0.1+
 
### Settings up on Windows

To get everything on Windows, first install Chocolatey (https://chocolatey.org), then type the following:

```
choco install cmake ninja strawberryperl python2 ruby1.9 gperf
```

You'll still need to install Visual Studio 2015 (Community Edition works fine, get the C++ packages).
 
### Setting up on Linux (Debian 9.5+)
 
 To get everything on Linux, just type the following:
 
```
sudo apt install cmake ninja-build gperf ruby clang libgcrypt20 libgcrypt11-dev lld-4.0
```
    
## Building from source
 
 To build on Windows:
 
```
make release x64
```

To build on macOS/Linux:

```
./make
```

## Build products

Build products will be in `<build_dir>/out`.

If your checkout is up-to-date, a 7zip release archive will also be generated in `<build_dir>`.
