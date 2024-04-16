cmake_minimum_required(VERSION 3.28)
project (LAPendulumModel)

set(SOURCES
    src/main.cpp
    src/Models.cpp    
)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)
