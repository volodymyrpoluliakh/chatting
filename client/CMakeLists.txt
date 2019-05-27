cmake_minimum_required(VERSION 2.8)

project(client)

add_executable(${PROJECT_NAME}
    main.cpp
    client.cpp
    )

target_link_libraries(${PROJECT_NAME} PRIVATE
    boost_system
    boost_program_options
    pthread)
