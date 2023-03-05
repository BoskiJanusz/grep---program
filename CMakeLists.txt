cmake_minimum_required(VERSION 3.10)
project(specific_grep VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(SRC
    main.cpp
)


set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs 
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF) 
find_package(Boost 1.30 COMPONENTS program_options REQUIRED)
if(Boost_FOUND)   
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SRC})   
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES})
endif()