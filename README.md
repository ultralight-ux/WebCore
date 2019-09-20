# Ultralight WebCore Port

This is the full source and build toolchain of the custom WebCore module for Ultralight.

## Useful Links

| Link                   | URL                                 |
| ---------------------- | ----------------------------------- |
| __Main Website__       | <https://ultralig.ht>               |
| __Slack Channel__      | <https://chat.ultralig.ht>          |

## Downloading pre-built binaries

Pre-built binares are uploaded to the following S3 buckets every commit:
 * Debug Bins: https://webcore-bin-dbg.sfo2.digitaloceanspaces.com/
 * Release Bins: https://webcore-bin.sfo2.digitaloceanspaces.com/

Click any of the bucket links to get an XML file listing. Download a package by appending the filename to the bucket URL.

## You'll need the following:

 * CMake 3.2+ (https://cmake.org/)
 * Ninja (https://ninja-build.org/)
 * Recent GCC/Clang on macOS/Linux
 * Visual Studio 2015 on Windows
 * Perl 5.10+
 * Python 2.7+
 * Ruby 1.9+
 * Gperf 3.0.1+
 
### Setting up on Windows

1. Install Visual Studio 2015 (Free Community Edition works fine). Remember to select the C++ packages.
2. Install Chocolatey (https://chocolatey.org).
3. Run the following from command line: 

```
choco install cmake ninja strawberryperl python2 ruby1.9 gperf
```

### Setting up on macOS

1. Install Xcode 9.4.1 (Older versions of Xcode can be found here: https://developer.apple.com/download/more/)
2. Install Homebrew (https://brew.sh/)
3. Homebrew may overwrite the command line tools, if so run: `sudo xcode-select --switch /Applications/Xcode.app/Contents/Developer`
3. Run the following from command line:

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

To build for Windows / 64-bit:

```
make release x64
```

OR

To build for Windows / 32-bit:

```
make release x86
```

### Building on macOS and Linux

To build for macOS/Linux:

```
./make
```

## Build products

Build products will be in `<build_dir>/out`.

If your checkout is up-to-date, a 7zip release archive will also be generated in `<build_dir>`.
