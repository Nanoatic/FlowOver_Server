cmake_minimum_required(VERSION 3.8)
project(FlowOver_Server)

find_package(Threads)
find_package(OpenMP)
FIND_PACKAGE(PkgConfig REQUIRED)
PKG_CHECK_MODULES(GTK3 REQUIRED gtk+-3.0)

if (OPENMP_FOUND)
    set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

set(CMAKE_CXX_STANDARD 17)

INCLUDE_DIRECTORIES(${GTK3_INCLUDE_DIRS})
LINK_DIRECTORIES(${GTK3_LIBRARY_DIRS})

set(SOURCE_FILES main.cpp constants_and_definitions.h computing_client.cpp computing_client.h message_poller.cpp message_poller.h message_dispatcher.cpp message_dispatcher.h keep_alive.cpp keep_alive.h interrupt_exception.cpp interrupt_exception.h includes.h helper_class.cpp helper_class.h server_things.cpp server_things.h maths_over/mat.h queues_over/sync_queue.h queues_over/file_queue.cpp queues_over/file_queue.h database_singleton.cpp database_singleton.h global_processiong_vars.h global_processiong_vars.cpp)

add_executable(FlowOver_Server ${SOURCE_FILES})

target_link_libraries(FlowOver_Server  ${CMAKE_THREAD_LIBS_INIT} ${GTK3_LIBRARIES} sqlite3)
