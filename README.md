# Ultralight WebCore Port

Ultralight maintains its own port of WebCore (the core layout engine of WebKit). This module statically
links against JavaScriptCore and uses our own custom, lightweight renderer (UltralightCore) for all drawing.

We've streamlined the build process to use CMake/Ninja on all platforms. Pre-built dependencies will be
automatically fetched for your platform during build.

Building typically takes 20 minutes or less on most modern machines.

## Useful Links

| Link                   | URL                                 |
| ---------------------- | ----------------------------------- |
| __Main Website__       | <https://ultralig.ht>               |
| __Chat on Discord!__   | <https://chat.ultralig.ht>          |

## Downloading pre-built binaries

Pre-built binares are uploaded to the following S3 buckets every commit:
 * Release Bins: https://webcore-bin.sfo2.digitaloceanspaces.com/

Click any of the bucket links to get an XML file listing. Download a package by appending the filename to the bucket URL.

## You'll need the following:

 * CMake 3.2+ (https://cmake.org/)
 * Ninja (https://ninja-build.org/)
 * Recent GCC/Clang on macOS/Linux
 * Visual Studio 2019 on Windows
 * Perl 5.10+
 * Python 2.7+
 * Ruby 1.9+
 * Gperf 3.0.1+
 
### Setting up on Windows

1. Install Visual Studio 2019 (Free Community Edition works fine). Remember to select the C++ packages.
2. Install Chocolatey (https://chocolatey.org).
3. Run the following from command line: 

```
choco install cmake ninja activeperl python2 ruby1.9 gperf
```

### Setting up on macOS

1. Install Xcode Command Line Tools (or the full Xcode package).
2. Download the macOS 10.14 Platform SDK from https://github.com/phracker/MacOSX-SDKs/releases and
   extract it to `/Library/Developer/CommandLineTools/SDKs/MacOSX10.14.sdk`.
3. Install Homebrew (https://brew.sh/)
4. Run the following from the Terminal to setup the needed homebrew packages:

```
brew install cmake ninja
```
 
### Setting up on Linux (Debian 9.5+)
 
To get everything on Linux, just type the following:
 
```
sudo apt install cmake ninja-build gperf ruby clang libgcrypt20 libgcrypt11-dev lld-4.0
```
    
## Building from source
 
### Building on Windows

To build WebCore for Windows / 64-bit:

```
make release x64
```

#### Building Debug on Windows

To build non-optimized WebCore with Debug PDBs:

```
make debug x64
```

#### Building on Windows with locally-built dependencies

By default, the build scripts in this repo fetch pre-built dependencies from our servers.

To build WebCore using locally-built dependencies in the `/deps` folder (really only useful
if you need to build against modified modules/deps):

```
make release x64 local
```

### Building on macOS and Linux

To build for macOS/Linux:

```
./make
```

## Build products

Build products will be in `<build_dir>/out`.

If your checkout is up-to-date, a 7zip release archive will also be generated in `<build_dir>`.
