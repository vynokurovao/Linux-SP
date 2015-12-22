project(Lab7)

set(CMAKE_CXX_FLAGS "-std=c++0x")

add_executable(manager main.cpp mqueue.cpp)
add_executable(proc1 proc1.cpp mqueue.cpp)
add_executable(proc2 proc2.cpp mqueue.cpp)

target_link_libraries(manager pthread)
