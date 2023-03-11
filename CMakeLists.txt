cmake_minimum_required(VERSION 3.13)
project(specific_grep VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)

set(SRC
    main.cpp
)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=thread")

set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         OFF) # ignore debug libs and 
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs 
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF) 
find_package(Boost 1.30 COMPONENTS program_options REQUIRED)
if(Boost_FOUND)   
    include_directories(${Boost_INCLUDE_DIR})
    add_executable(${PROJECT_NAME} ${SRC})   
    target_link_libraries(${PROJECT_NAME} ${Boost_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT} pthread -fsanitize=thread)
endif()