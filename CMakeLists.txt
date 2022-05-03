cmake_minimum_required(VERSION 3.15)
project(libevent_cpp)

set(CMAKE_CXX_STANDARD 17)

include_directories(/opt/homebrew/include)
link_directories(/opt/homebrew/lib)

add_library(libevent_cpp SHARED EventBase.cpp EventBase.h Timer.cpp Timer.h TCPServer.cpp TCPServer.h TCPClient.cpp TCPClient.h EvBuffer.cpp EvBuffer.h)
target_link_libraries(libevent_cpp
        event
        )

add_executable(timer-test
        sample-test/timer_test.cpp)
target_link_libraries(timer-test
        libevent_cpp
        )

add_executable(client
        sample-test/client_test.cpp)
target_link_libraries(client
        libevent_cpp
        )

add_executable(server
        sample-test/server_test.cpp)
target_link_libraries(server
        libevent_cpp
        )