cmake_minimum_required(VERSION 3.8)
project(ApogeeSS C ASM_MASM)

include(GNUInstallDirs)

include(cmake/build_props.cmake)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_ASM_NASM_OBJECT_FORMAT coff)
set(CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS ${CMAKE_ASM_NASM_SOURCE_FILE_EXTENSIONS} asm ASM)

enable_language(ASM_NASM)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror=implicit-function-declaration -Werror=pointer-sign -Wno-comment")

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
    source/irq.h        source/interrup.h   # source/irq.c   # Code of irq.c is unneeded as DJGPP's interrupt installer works flawless for IRQ>8
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

    source/mv_mix2c.c
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
)

target_compile_definitions(ass_audio PRIVATE -DULTRASOUND_OFF -DAWE32_OFF)

target_include_directories(ass_audio PUBLIC include)
target_include_directories(ass_audio PRIVATE include/usrhooks)
target_link_libraries(ass_audio PUBLIC ass_usrhooks)

install(TARGETS ass_audio
        EXPORT ass_audio_targets
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}")

install(FILES
        include/fx_man.h
        include/music.h
        include/sndcards.h
        include/task_man.h
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/djaudio")

install(FILES
        include/usrhooks/usrhooks.h
        include/usrhooks/usrhooks.c
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/djaudio/userhooks")

make_directory(${CMAKE_BINARY_DIR}/docs-dos)

add_custom_command(
    TARGET ass_audio POST_BUILD
    COMMAND iconv -f UTF-8 -t IBM850 -o "${CMAKE_BINARY_DIR}/docs-dos/emidiapi.txt" "${CMAKE_SOURCE_DIR}/public/emidiapi.txt"
    COMMAND iconv -f UTF-8 -t IBM850 -o "${CMAKE_BINARY_DIR}/docs-dos/midi.txt" "${CMAKE_SOURCE_DIR}/public/midi.txt"
    COMMAND iconv -f UTF-8 -t IBM850 -o "${CMAKE_BINARY_DIR}/docs-dos/notes.txt" "${CMAKE_SOURCE_DIR}/public/notes.txt"
    COMMAND iconv -f UTF-8 -t IBM850 -o "${CMAKE_BINARY_DIR}/docs-dos/readme.txt" "${CMAKE_SOURCE_DIR}/public/readme.txt"
    COMMENT "Converting documents to DOS charset..."
)

install(FILES
        public/emidiapi.txt
        public/midi.txt
        public/notes.txt
        public/readme.txt
        DESTINATION "docs-utf8")

install(FILES
        ${CMAKE_BINARY_DIR}/docs-dos/emidiapi.txt
        ${CMAKE_BINARY_DIR}/docs-dos/midi.txt
        ${CMAKE_BINARY_DIR}/docs-dos/notes.txt
        ${CMAKE_BINARY_DIR}/docs-dos/readme.txt
        DESTINATION "docs")

option(ASS_BUILD_TOOLS "Build tools" ON)
if(ASS_BUILD_TOOLS)
    add_subdirectory(util/maketmb)
    add_subdirectory(util/pm)
    add_subdirectory(util/ps)
    add_subdirectory(util/timer)

    install(FILES
            public/CWSDPMI.EXE
            DESTINATION "util")
endif()

set(CPACK_PACKAGE_NAME ApogeeSS)
set(CPACK_PACKAGE_VERSION 1.0.0)
if(NOT CPACK_PACKAGE_FILE_NAME)
    set(CPACK_PACKAGE_FILE_NAME "dj-audio-dos32")
endif()
include(CPack)
