
add_executable(pm
    pm.c
)

set_target_properties(pm PROPERTIES SUFFIX ".exe")

target_link_libraries(pm PRIVATE ass_audio)

install(TARGETS pm
        RUNTIME DESTINATION "util"
)
