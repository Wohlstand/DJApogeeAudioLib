
add_executable(ps
    ps.c
)

set_target_properties(ps PROPERTIES SUFFIX ".exe")

target_link_libraries(ps PRIVATE ass_audio)

install(TARGETS ps
        RUNTIME DESTINATION "util"
)
