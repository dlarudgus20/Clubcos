Clubcos
=======

Clubcos - Clubc Operating System

#### Table of Contents

[How to use](#use)

[License](#license)

[Prerequisites](##prerequisites)

[How to build](#build)

[How to build cross-compiler](#cross-compiler)

[Troubleshooting](#troubleshooting)


=======

<a name="use" />
How to use
=======

To just use, maybe you easily find floppy disk image with *grub4dos* (img/<config>/floopy.img) and kernel image (bin/<config>/Clubcos.sys)

=======

<a name="license" />
License
=======

Clubcos is under The BSD (2-Clause) License.

=======

<a name="prerequisites" />
Prerequisites
=======

Unix-like shell (I use Cygwin)

make

binutils, **which is cross-compiled for i686-pc-elf**

gcc upper 4.8.x, **which is cross-compiled for i686-pc-elf**

nasm upper 2.11

<hr/>

qemu (for `make run`)

bochsdbg (for `make run_bochs`)

<hr/>

MinGW (if you're in windows - to build *excpp*)

=======

<a name="build" />
How to build
=======

**Notice:** Before you try to build, you must build [cross-compiler](#cross-compiler)

And you also need qemu for `make run`, and bochsdbg for `make run_bochs`

<hr/>

First, clone this repository:

    git clone https://github.com/dlarudgus20/Clubcos.git

Second, just make it!

    make

Notice that default configuration is Debug mode, so you should specify configuration if you want to build Release mode.

    make CONFIG=Release

If the cross-compiler is not in PATH environmental variable, you should specify its path.

    # Notice that it MUST contain target triplet prefix (e.g. i686-pc-elf-)
    make TARGET_PREFIX="/usr/local/cross/bin/i686-pc-elf-"

=======

<a name="cross-compiler" />
How to build cross compiler
=======

Before building, you need **cross compiler** for i686-pc-elf to build Clubcos.

First, install prerequisites for gcc and binutils. [links](https://gcc.gnu.org/install/prerequisites.html)

Especially, you need these:

```
bison
flex
libgmp-dev
libmpfr-dev
libmpc-dev

# optional
isl
cloog
```

Download source of [gcc](https://gcc.gnu.org/) and [binutils](http://www.gnu.org/software/binutils/). (The latest, the better.)
And, follow this script to build.

(In my example script, I assume that both gcc and binutils are in `/usr/src`.)

```
export TARGET=i686-pc-elf
export PREFIX=<where-to-install-cross-compiler>
export PATH="$PREFIX/bin:$PATH"

cd /usr/src
mkdir build-binutils
cd build-binutikls

../binutils-x.y.z/configure --prefix="$PREFIX" --target=$TARGET --disable-nls --disable-shared
make
make install

cd ..
mkdir build-gcc
cd build-gcc

../gcc-x.y.z/configure --prefix="$PREFIX" --target=$TARGET --disable-nls --enable-language=c --without-headers --disable-shared
make all-gcc
make all-target-libgcc
make install-gcc
make install-target-libgcc
```

=======

<a name="troubleshooting" />
Troubleshooting
=======

> **can't input arrow keys**

  qemu has an bug of transform arrow keys to 2/4/6/8 keys of keypad. Turn off NumLock, and use 2/4/6/8 of keypad.

