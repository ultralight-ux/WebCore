# Ultralight WebCore Port

## You'll need the following:

 * CMake 3.2+ (https://cmake.org/)
 * Ninja (https://ninja-build.org/)
 * Recent GCC/Clang on macOS/Linux
 * Visual Studio 2015 on Windows
 
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
