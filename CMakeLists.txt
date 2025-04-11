cmake_minimum_required(VERSION 3.8)
project(ApogeeSS C ASM_MASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ASM_NASM_OBJECT_FORMAT coff)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS ${CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS} asm ASM)

enable_language(ASM_NASM)

add_library(ass_usrhooks OBJECT
    include/usrhooks/usrhooks.c
    include/usrhooks/usrhooks.h
)

add_library(ass_audio STATIC
    source/adlibfx.c    source/adlibfx.h
    source/_al_midi.h
    source/al_midi.c    source/al_midi.h
    source/a_assert.h
    source/awe32.c      source/awe32.h
    source/_blaster.h
    source/blaster.c
    source/blaster.h
    source/blastold.c
    source/ctaweapi.h
    source/ctaweapi.inc
    source/debugio.c
    source/debugio.h
    source/dma.c        source/dma.h
    source/a_dpmi.c     source/a_dpmi.h
    source/fx_man.c     include/fx_man.h
    source/gmtemp.c
    source/gmtimbre.c
    source/gmtmbold.c
    source/gus.c
    source/gusmidi.c    source/gusmidi.h
    source/gusmidi2.c   source/gusmidi2.h   source/_guswave.h
    source/guswave.c    source/guswave.h
    source/irq.c        source/irq.h    source/interrup.h
    source/leeold.c
    source/leetimb1.c
    source/leetimbr.c
    source/linklist.h
    source/ll_man.c     source/ll_man.h
    source/memcheck.h
    source/_midi.h
    source/midi.c       source/midi.h
    source/mpu401.c     source/mpu401.h
    source/_multivc.h
    source/multivoc.c   source/multivoc.h
    source/music.c      include/music.h
    source/mv1.c
    source/myprint.c
    source/myprint.h
    source/newgf1.h
    source/oldtimbr.c
    source/_pas16.h
    source/pas16.c
    source/pas16.h
    source/pcfx.c
    source/pcfx.h
    source/pitch.c
    source/pitch.h
    include/sndcards.h
    source/_sndscap.h
    source/sndscape.c
    source/sndscape.h
    source/sndsrc.c
    source/sndsrc.h
    source/standard.h
    source/task_man.c
    include/task_man.h
    source/user.c
    source/user.h

# Assembly sources
    source/mv_mix.asm
    source/mv_mix1.asm
    source/mv_mix2.asm
    source/mv_mix3.asm
    source/mv_mix4.asm
    source/mv_mix5.asm
    source/mv_mix6.asm
    source/mv_mix16.asm
    source/mvreverb.asm
)

target_include_directories(ass_audio PUBLIC include)
target_include_directories(ass_audio PRIVATE include/usrhooks)
target_link_libraries(ass_audio PUBLIC ass_usrhooks)
