
add_executable(timer
    timer.c
)

target_link_libraries(timer PRIVATE ass_audio)
