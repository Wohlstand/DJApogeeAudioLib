
add_executable(maketmb
    maketmb.c
)
set_target_properties(maketmb PROPERTIES SUFFIX ".exe")

install(TARGETS maketmb
        RUNTIME DESTINATION "util"
)

install(FILES
        samples/genmidi.ibk
        samples/gmtimbre.ini
        samples/percus.ibk
        samples/timbres.zip
        DESTINATION "samples"
)
