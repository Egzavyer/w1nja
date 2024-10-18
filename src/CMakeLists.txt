set(SOURCES
    main.cpp
    peer.cpp
)

add_executable(n1nja ${SOURCES})

target_link_libraries(n1nja PRIVATE ws2_32)