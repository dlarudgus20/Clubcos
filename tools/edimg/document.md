edimg
=======

**edimg** is command-line floppy image (*.img) editor.

*written by* H.Kawai (http://sourceforge.jp/projects/osask/scm/svn/tree/183/28GO/edimg/trunk/)

*modified by* dlarudgus20 (https://github.com/dlarudgus20)

===========

Files
=======

edimg.c : the main source.

autodec.asm : NASM Assembly version of "autodec/autodec.ask"

autodec/autodec.ask : tek compress/decompress algorithm - I hope I can remove it sometime

autodec/aska.exe, autodec/naskcnv0.exe : tools for convert "autodec/autodec.ask" to "autodec.asm"

=======

How to use
=======

## Command line

```
# Examples
edimg [command1, command2, ...] # it's okay that there's many dizzy script files.

# copy guide.txt into root of disk image and write image to floppy.img (@: means root of disk image)
edimg imgin:floppy_origin.img copy from:guide.txt to:@: imgout:floppy.img

# extract guide.txt from asdf directory of disk image
edimg imgin:floppy_origin.img copy from:@:/asdf/guide.txt to:guide.txt

edimg @script0.txt
edimg imgin:floppy_origin.img @script1.txt
```

Notice that edimg *cannot* understand names which has space.

Command is formed like this:

```
cmd_name par_name1:param1 par_name2:param2 ...
```

`cmd_name` can be skipped, if it's `opt` command.

### `opt` command

`opt` command has only one parameter. Its behavior varies greatly depending on the parameter.
If you don't give any parameters, `opt` command do nothing.

Also, command name of `opt` is can be skipped.
That is, you can use either `opt imgin:file` or just `imgin:file`.

#### Parameter List

##### `imgin` parameter

Usage: `[opt] imgin:<filename>`

Behavior: Load disk image. If disk has partition, edimg access the first partition.

##### `imgout` parameter

Usage: `[opt] imgout:<filename>`

Behavior: Save disk image.

##### `vsiz` parameter

Usage: `[opt] vsiz:<image size>`

Behavior: Change disk size. It's used for SF16.

##### `binin` parameter

Usage: `[opt] binin:<filename>`

Behavior: Load *binary* image file. In that case, you can use only `wbinimg`, `writedata` and `exe2bin` commands.

##### `binout` parameter

Usage: `[opt] binout:<filename>`

Behavior: Save binary image.

##### `_path` parameter

Usage: `[opt] _path:<base path>`

Behavior: Replace `_:` path with base path. You cannot set base path with `@:`.

#### `bias` parameter

Usage: `[opt] bias:<value>`

Behavior: Specify timestamp bias parameter

### `copy` command

Usage: `copy [nocmp:] from:<path> to:<path>`

`copy` command has two parameters, `from` and `to`, and it copies `from` file to `to` file. The order of parameters must be `from` - `to`. In case of `to` file, the end of path can be `:` or `/` so that the filename of `from` file is used instead.

If the drive name of path is `@`, it means the working disk image. By using this you can copy files inside disk image.

If `nocmp:` option is specified, `from` file is uncompressed before copying if it is compressed.

Path cannot contain wildcards.

### `ovrcopy` command

Usage: `ovrcopy [nocmp:] from:<path> to:<path>`

Equal to `copy` commands if `to` file does not exist.

If `to` file exists, it is carefully overwritten not to change the cluster location of `to` file.

### `create` comamnd

Usage: `create file:<path> size:<size> [begin:<cluster index>]`

Make a zero-filled file.

### `ovrcreate` command

Usage: `ovrcreate file:<path> size:<size> [begin:<cluster index>]`

Equal to `create` command if file does not exists.

If the specified file exists, the size of the file would not change even if the specified size is bigger than the size of the existing file.

### `setattr` command

Usage: `setattr file:<path> attr:<attribute>`

Change the attribute of a file. The disk name of path must be `@` which means the working disk image.

### `delete` command

Usage: `delete file:<path>`

Delete a file.

### `wbinimg` command

Usage: `wbinimg src:<path> len:<count> from:<offset> to:<offset>`

Write a binary file to the specific region in the disk image.

This command can be used to write bootsector image.

### `release` command

Usage: `release [mini:] [zerofill:] [efat:] [nofrag:] [minibpb:]`

`mini:` omits the trailing empty sectors of disk.

`zerofill:` zero-fills the contents of deleted files.

`efat:` makes the filesystem of disk image eFAT. Only valid if SF16.

`nofrag:` checks fragmentation of files. Only valid if SF16.

`minibpb:` omits the trailing empty sectors of disk and updates BPB infomation. Only valid if SF16.

### `writedata` command

Usage: `writedata offset:<offset> [byte:<data,data,...,data>] [word:<data,data,...,data>] [dwrd:<data,data,...,data>]`

Write a binary file to the specific region in the disk image.

In-line-data version of `wbinimg` command.

### `list` command

List directory contents.

### `copyall` command

Usage: `copyall from:@: to:<directory>`

Copy all files of the disk image to the specific directory.

The path of the directory must be ended with `/`.

### `exe2bin` command

Usage: `exe2bin seg:<segment>`

Convert 16-bit EXE file into binary COM file.

### `/*` command and `*/` command

These commands is used for commentation. They are treated as commands, so space is needed before and after them and `/**` or `**/` would not be recognized.

Examples
===

## #1

Standard OSASK image.

```
opt imgin:fdimg0at.tek /* 1440 KB empty disk image */
wbinimg src:osaskbs1.bin len:512 from:0 to:0
create file:@:osask.sys size:108k begin:5
ovrcopy from:osask.sys to:@:
setattr file:@:osask.sys attr:0x06 /* system+hidden */
copy from:osask0.psf to:@: /* this should be first */
/* order does not matter below */
copy from:! guide!.txt to:@:
copy from:! guide2!.txt to:@:
    /* .. and so on */
copy from:videomod.txt to:@:
/* applications are copied. */
release mini:
opt imgout:testimg.bin
```

## #2

  OSASK.SYS는 디폴트에서 기동시에 FD를 읽으러 갑니다만, 이것을 ATA나 CF로 할 수 있습니다.

```
/* sys_cf0.txt */
opt binin:osask.sys
writedata offset:2 word:0x8200
```

그래서 파일을 만들어 놓고,

```
prompt>edimg @sys_cf0.txt binout:osaskcf0.sys
```

으로 하면 CF용으로 교체됩니다.

## #3

  같은 것을 OSASK.EXE에 대해서 해 보겠습니다.

```
/* exe_cf0.txt */
opt binin:osask.exe
writedata offset:0x202 word:0x8200
```

그래서 파일을 만들어 놓고,

```
prompt>edimg @exe_cf0.txt binout:osaskcf0.exe
```

로 하면, CF용으로 교체됩니다.

## #4

  같은 것을 OSAIMGAT.BIN에 대해서 해 보겠습니다.

```
/* img_cf0.txt */
opt imgin:osaimgat.bin
writedata offset:0x4802 word:0x8200
```

그래서 파일을 만들어 놓고,

```
prompt>edimg @img_cf0.txt binout:osaskcf0.bin
```

로 하면, CF용으로 교체됩니다.

## #5

  sf16_40 s.tek를 사용하여 디스크 이미지를 아카이브(archive) 대신에 해 보겠습니다.

```
opt imgin:sf16_40s.tek /* SF16_40 simplest image */
opt vsiz:8m
copy from:document.txt to:@:
copy from:edimg.exe    to:@:
copy from:fdimg0at.tek to:@:
copy from:fdimg0tw.tek to:@:
copy from:sf16_40s.tek to:@:
copy from:edimg.c      to:@:
copy from:Makefile     to:@:
copy from:make.bat     to:@:
copy from:! cons_9x.bat to:@:
copy from:! cons_nt.bat to:@:
release minibpb: nofrag: efat:
opt imgout:imgarc.bin
```

## #6

  같은 sf16_40 s.tek를 사용하여 OSASK용의 아카이브(archive)를 만드는 예입니다.

```
opt imgin:sf16_40s.tek /* SF16_40 simplest image */
opt vsiz:8m
copy from:teditarc.txt to:@:ARCINFO0.TXT
copy nocmp: from:teditc02.bin to:@:
copy nocmp: from:teditc02.buf to:@:
copy nocmp: from:teditc02.ini to:@:
copy nocmp: from:te_alph0.bin to:@:
copy nocmp: from:te_roma0.bin to:@:
release minibpb: nofrag: efat:
opt imgout:teditc02.org
```

## #7

  OSASK용의 아카이브(archive)를 커런트 디렉토리에 전개해 보겠습니다(상기의 teditc02.org
를 bim2bin로 teditc02.tk로 했음. OSASK로 사용할 때는 아마 bin으로 하고 있다고
생각합니다만, bin인 채로 전개하면 내부의 teditc02.bin와 이름이 충돌되어 덮어쓰기가 되어
버립니다).

```
prompt>edimg imgin:teditc02.tk copyall from:@: to:. /
```

## #8

  시시한 사용법으로, 단순한 tek 해동 툴로 bim2bin의 대신을 시키는 것도
일단 할 수 있습니다. 다만 rjc가 걸려있는 것은 rjc가 붙은 상태로 전개됩니다.
OSASK ver.4.6이후라면 rjc가 걸려있어도 문제 없이 실행할 수 있으므로 기본적으로 문제는
없습니다.

```
prompt>edimg copy nocmp: from:helloc4.bin to:helloc4.org
```
