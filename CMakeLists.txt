cmake_minimum_required(VERSION 3.8)
project(ApogeeSS C ASM_MASM)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ASM_NASM_OBJECT_FORMAT coff)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS ${CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS} asm ASM)

enable_language(ASM_NASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration -Werror=pointer-sign")

add_library(ass_usrhooks OBJECT
    include/usrhooks/usrhooks.c
    include/usrhooks/usrhooks.h
)

add_library(ass_audio STATIC
    source/adlibfx.c    source/adlibfx.h
    source/_al_midi.h
    source/al_midi.c    source/al_midi.h
    source/a_assert.h
    # source/awe32.c      source/awe32.h    # Missing external libraries
    source/blaster.c    source/blaster.h    source/_blaster.h
    source/debugio.c    source/debugio.h
    source/dma.c        source/dma.h
    source/a_dpmi.c     source/a_dpmi.h
    source/fx_man.c     include/fx_man.h
    source/gmtimbre.c
    # source/gus.c      # Missing external libraries for GUS support
    # source/gusmidi.c    source/gusmidi.h    source/_guswave.h
    # source/guswave.c    source/guswave.h
    # source/newgf1.h
    # ------- Junk
    # source/gusmidi2.c   source/gusmidi2.h
    # ------- Junk
    source/irq.c        source/irq.h        source/interrup.h
    source/ll_man.c     source/ll_man.h     source/linklist.h
    source/_midi.h
    source/midi.c       source/midi.h
    source/mpu401.c     source/mpu401.h
    source/multivoc.c   source/multivoc.h   source/_multivc.h
    source/music.c      include/music.h
    source/pas16.c      source/pas16.h      source/_pas16.h
    source/pcfx.c       source/pcfx.h
    source/pitch.c      source/pitch.h
    include/sndcards.h
    source/sndscape.c   source/sndscape.h   source/_sndscap.h
    source/sndsrc.c     source/sndsrc.h
    source/standard.h
    source/task_man.c   include/task_man.h
    source/user.c       source/user.h
    source/djconfig.h

# Assembly sources
    source/mv_mix.asm
    # source/mv_mix1.asm
    # source/mv_mix2.asm
    # source/mv_mix3.asm
    # source/mv_mix4.asm
    # source/mv_mix5.asm
    # source/mv_mix6.asm
    source/mv_mix16.asm
    source/mvreverb.asm
    source/multvcas.c
)

target_compile_definitions(ass_audio PRIVATE -DULTRASOUND_OFF -DAWE32_OFF)

target_include_directories(ass_audio PUBLIC include)
target_include_directories(ass_audio PRIVATE include/usrhooks)
target_link_libraries(ass_audio PUBLIC ass_usrhooks)

option(ASS_BUILD_TOOLS "Build tools" ON)
if(ASS_BUILD_TOOLS)
    add_subdirectory(util/pm)
    add_subdirectory(util/ps)
    add_subdirectory(util/timer)
endif()
