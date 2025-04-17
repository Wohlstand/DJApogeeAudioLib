.OPTIMIZE

lib_dir           = ../..
prg_dir           = .
obj_dir           = $(prg_dir)/obj
main_dir          = $(prg_dir)
header_dir        = $(main_dir);$(lib_dir)/include
#tasm_include_dir  = /i$(main_dir) /i$(lib_dir)/source

lib_files         = $(lib_dir)/public/audio_wf.lib

.h   :              $(header_dir)
.c   :              $(header_dir)
#.asm :              $(header_dir)
.o :                $(obj_dir)

object_files        = ps.o usrhooks.o

#tasm_options        = /zi /p $(tasm_include_dir)
#wcc_options         = /w4 /d2 /i=$(header_dir)
wcc_options         = -q -w4 -d1 -omaxnet -zp4 -5r -i="$(header_dir)"
wlink_options       =

ps.exe: $(object_files) $(lib_files) Makefile
   %create temp.lnk
   %append temp.lnk option quiet
   %append temp.lnk option stack=128k
   %append temp.lnk system dos4g
   %append temp.lnk name $^&
   %append temp.lnk debug all
   %append temp.lnk libfile $(lib_files)
   for %i in ($(object_files)) do %append temp.lnk file $(obj_dir)/%i
   wlink $(wlink_options) @temp.lnk
   #wc2dbg ps.exe
   rm temp.lnk

#.asm.obj :
#   tasm $(tasm_options) $[* $(obj_dir)\$^&

.c.o :
   wcc386 $(wcc_options) $[* -fo="$(obj_dir)/$^&"

ps.o          : ps.c ../../include/fx_man.h ../../include/sndcards.h

usrhooks.o    : usrhooks.c ../../include/usrhooks/usrhooks.h
