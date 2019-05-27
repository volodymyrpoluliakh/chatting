cmake_minimum_required(VERSION 2.8)

project(server)

add_executable(${PROJECT_NAME}
    main.cpp
    connection.cpp 
    connection_manager.cpp 
    connection_manager.cpp
    )

target_link_libraries(${PROJECT_NAME} boost_system boost_program_options pthread)
