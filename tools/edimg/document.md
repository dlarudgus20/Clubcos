﻿edimg
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

*(Working...)*

  bias 파라미터：
    서식： [opt] bias:시차
      디스크 이미지내의 파일의 타임 스탬프 보정을 위한 파라미터입니다.
    일본에서는 9를 지정합니다.

·copy 커맨드

  서식： copy [nocmp:] from:파일명 to:파일명

  copy 커맨드는 2개의 파라미터를 가지며 from에서 to로 파일을 카피합니다. from
과 to의 순서를 반대로 쓸 수는 없습니다. to의 파일명에 한하여 파일명의 최후를
":", "/" 그리고 중지하는 것이 허락됩니다. 이 경우, from의 파일명이 보완됩니다. 파
일명이 아니고 디렉토리명을 쓸 때는 반드시 마지막/를 잊지 말아 주세요.

  파일명에 대해 드라이브 네임에 "@"을 사용하면, 그것은 디스크 이미지를 의미합니다.
디스크 이미지 내의 파일을 디스크 이미지 내에 카피할 수도 있고,
보통 파일 카피도 가능합니다.

  옵션의 nocmp:를 사용하면, from으로 지정된 파일이 압축되고 있었을 경우 전개한
결과를 to에 write합니다. 압축되어 있지 않았던 경우는 그대로 write합니다.

  와일드 카드는 사용할 수 없습니다.

·ovrcopy 커맨드

  서식： ovrcopy [nocmp:] from:파일명 to:파일명

  over-write 카피 커맨드입니다. to의 파일이 이미 존재하고 있으면, 클러스터
의 위치가 변함없이 주의 깊게 카피됩니다. 파일이 존재하고 있지 않으면,
카피 커맨드와 전혀 바뀌지 않습니다. 그 다른 것도 카피 커맨드와 같습니다.

·create 커맨드

  서식： create file:파일명 size:사이즈 [begin:개시 클러스터 번호]

  내용이 0x00으로 채워진 파일을 만듭니다.

·ovrcreate 커맨드

  서식： ovrcreate file:파일명 size:사이즈 [begin:개시 클러스터 번호]

  기본적으로 create 커맨드와 같습니다만, 만약 파일이 이미 존재하고 있고 게다가 지정
사이즈보다 크거나 동일하면 파일 사이즈를 변경하지 않고 사이즈분 만큼
0x00로 다 채웁니다.

·setattr 커맨드

  서식： setattr file:파일명(@: ~가 아니면 안 된다) attr:애트리뷰트(attribute)값

  디스크 이미지 내의 파일 속성치를 변경합니다.

·delete 커맨드

  서식： delete file:파일명

  파일을 삭제합니다.

·wbinimg 커맨드

  서식： wbinimg src:바이너리 필드명 len:전송 바이트수
             from:read 개시 오프셋(offset) to:기입 개시 오프셋(offset)

  이것은 즉, boot sector를 write하기 위한 커맨드입니다. from는 바이너리 필드
안의 오프셋(offset)이고 to는 파티션의 선두부터의 오프셋(offset)입니다.

·release 커맨드

  서식： release [mini:] [zerofill:] [efat:] [nofrag:] [minibpb:]

  이것은 주로 imgout 직전에 실행하는 커맨드입니다. 디폴트라면 1440KB용의 디스크
이미지가 1440KB로 나옵니다만(비록 읽어들일 때 뒤가 잘려 있어도),
mini:를 지정하면 말미의 빈 섹터를 출력하지 않게 됩니다. 또 zerofill:를 하면,
delete한 부분에 남아 있던 파일의 잔해가 제로로 클리어 됩니다.

  edimg0d부터는 3개의 파라미터가 추가되었습니다. 이것들은 SF16 때만 유효하고,
efat:는 FAT를 확장 FAT형식으로 출력시킬 것, nofrag:는 모든 파일이 단편화
하고 있지 않는 것을 확인한 뒤 검사필 플래그를 세울 것, 그리고 minibpb:는 mini:와
같은 방법으로 잘라 채운 뒤에 거기에 맞춰서 BPB정보를 갱신할 것, 을 각각 의미
합니다.

·writedata 커맨드

  서식： writedata offset:기입 개시 오프셋(offset) [byte:데이터,...,데이터]
    [word:데이터, 데이터,...,데이터] [dwrd:데이터, 데이터,...,데이터]

  write 개시 오프셋(offset)으로부터, 지정된 데이터를 지정된 데이터 폭으로 write
. wbinimg 커맨드의, write하는 데이터를 스크립트상에서 지정하는 것, 이라고 하는 느낌입니다.

·list 커맨드

  서식： list

  디스크 이미지의 디렉토리 일람을 출력합니다.

·copyall 커맨드

  서식： copyall from:@: to:디렉토리명

  디스크 이미지의 파일을 모두 카피합니다. 주로 아카이브(archive)의 전개에 사용합니다. 
디렉토리명은 /으로 끝날 필요가 있으므로, 예를 들어 경향 패스에 전개하고 싶을 때에는
to: ./이라고 쓰게 됩니다.

·exe2bin 커맨드

  서식： exe2bin seg:이미지가 로드 되는 세그먼트(segment)

  16 bit-EXE 파일을 바이너리 이미지로 합니다. imgtol의 s커맨드의 대용입니다.

·/*커맨드

  서식： /*

  리마크부 개시 커맨드입니다. 커맨드이므로 "/**" 등으로 하면 인식하지 못하고 에러가
됩니다. 리마크 상자도 가능합니다.

·*/커맨드

  서식： */

  리마크부 종료 커맨드입니다. 커맨드이므로 "**/"등으로 하면 인식하지 못하고 에러가
됩니다.

[예 - 1]

  OSASK.SYS의 크기와 위치는 정해져 있으므로 다음과 같이 하는 것이 보통입니다(AT호환기종의 경우). 
처음 2행을 바꾸면, TOWNS나 98용의 이미지가 생깁니다.

opt imgin:fdimg0at.tek /* 1440 KB공 디스크 이미지 */
wbinimg src:osaskbs1.bin len:512 from:0 to:0
create file:@:osask.sys size:108k begin:5
ovrcopy from:osask.sys to:@:
setattr file:@:osask.sys attr:0x06 /* system+hidden */
copy from:osask0.psf to:@: /* 이것은 제일 처음으로 */
/* 이하 무순서 */
copy from:! guide!.txt to:@:
copy from:! guide2!.txt to:@:
    /* 중략 */
copy from:videomod.txt to:@:
/* 여기까지 어플리케이션의 전송 */
release mini:
opt imgout:testimg.bin

[예 - 2]

  OSASK.SYS는 디폴트에서 기동시에 FD를 읽으러 갑니다만, 이것을 ATA나 CF로 할 수 있습니다.

"sys_cf0.txt"

opt binin:osask.sys
writedata offset:2 word:0x8200

그래서 파일을 만들어 놓고,

prompt>edimg @sys_cf0.txt binout:osaskcf0.sys

으로 하면 CF용으로 교체됩니다.

[예 - 3]

  같은 것을 OSASK.EXE에 대해서 해 보겠습니다.

"exe_cf0.txt"

opt binin:osask.exe
writedata offset:0x202 word:0x8200

그래서 파일을 만들어 놓고,

prompt>edimg @exe_cf0.txt binout:osaskcf0.exe

로 하면, CF용으로 교체됩니다.

[예 - 4]

  같은 것을 OSAIMGAT.BIN에 대해서 해 보겠습니다.

"img_cf0.txt"

opt imgin:osaimgat.bin
writedata offset:0x4802 word:0x8200

그래서 파일을 만들어 놓고,

prompt>edimg @img_cf0.txt binout:osaskcf0.bin

로 하면, CF용으로 교체됩니다.

[예 - 5]

  sf16_40 s.tek를 사용하여 디스크 이미지를 아카이브(archive) 대신에 해 보겠습니다.

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

[예 - 6]

  같은 sf16_40 s.tek를 사용하여 OSASK용의 아카이브(archive)를 만드는 예입니다.

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

[예 - 7]

  OSASK용의 아카이브(archive)를 커런트 디렉토리에 전개해 보겠습니다(상기의 teditc02.org
를 bim2bin로 teditc02.tk로 했음. OSASK로 사용할 때는 아마 bin으로 하고 있다고
생각합니다만, bin인 채로 전개하면 내부의 teditc02.bin와 이름이 충돌되어 덮어쓰기가 되어 
버립니다).

prompt>edimg imgin:teditc02.tk copyall from:@: to:. /

[예 - 8]

  시시한 사용법으로, 단순한 tek 해동 툴로 bim2bin의 대신을 시키는 것도
일단 할 수 있습니다. 다만 rjc가 걸려있는 것은 rjc가 붙은 상태로 전개됩니다.
OSASK ver.4.6이후라면 rjc가 걸려있어도 문제 없이 실행할 수 있으므로 기본적으로 문제는
없습니다.

prompt>edimg copy nocmp: from:helloc4.bin to:helloc4.org
