Clubcos
=======

Clubcos - Clubc Operating System

=======

License
=======

Clubcos is under The BSD (2-Clause) License.

tools/edimg is under KL-01 License. (tools/edimg/license.txt - it's Japanese)

=======

Prerequisites
=======

Unix-like shell (I use Cygwin)
make
binutils, **which is cross-compiled for i686-pc-elf**
gcc upper 4.8.x, **which is cross-compiled for i686-pc-elf**
nasm upper 2.11

qemu (for `make run`)
bochsdbg (for `make run_bochs`)

MinGW (if you're in windows - to build *excpp*)

=======

How to build
=======

First, clone this repository:

    git clone https://github.com/dlarudgus20/Clubcos.git

*Notice:* Kernel is in `bin/<CONFIG>/Clubcos.sys`. Bootable image is in `img/<CONFIG>/floopy.img`.
If you want to just use it, not to build, you don't need to build it. Just use it!

> Windows, with Cygwin & MinGW
  You can build & run it as follows:
        make tools
        make CONFIG=Release
        make run CONFIG=Release

=======

Troubleshooting
=======

> **can't input arrow keys**
  qemu has an bug of transform arrow keys to 2/4/6/8 keys of keypad. Turn off NumLock, and use 2/4/6/8 of keypad.

