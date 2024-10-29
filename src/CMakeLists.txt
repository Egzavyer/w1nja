set(SOURCE
    main.cpp
    peer/peer.cpp
    peer/connectionManager.cpp
    peer/fileHandler.cpp
    peer/requestHandler.cpp
)

add_executable(n1nja ${SOURCE})

target_link_libraries(n1nja PRIVATE ws2_32)