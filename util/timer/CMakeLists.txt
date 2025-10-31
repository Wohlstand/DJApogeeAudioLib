
add_executable(timer
    timer.c
)

set_target_properties(timer PROPERTIES SUFFIX ".exe")

target_link_libraries(timer PRIVATE ass_audio)

install(TARGETS timer
        RUNTIME DESTINATION "util"
)
