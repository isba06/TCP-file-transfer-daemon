cmake_minimum_required (VERSION 3.16.3)

project("TCP-file-transfer-daemon" CXX)

SET(CMAKE_CXX_STANDARD 17)
SET(CMAKE_CXX_STANDARD_REQUIRED ON)
SET(Boost_USE_MULTITHREAD ON)
FIND_PACKAGE(Boost 1.58.0 REQUIRED COMPONENTS log_setup log system thread)

add_executable(client Client/main.cpp Client/client.cpp Log/logger.cpp)

target_include_directories(client
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/Client"
    "${CMAKE_CURRENT_SOURCE_DIR}/Log"
)

target_link_libraries(client
  PUBLIC
    Boost::log_setup
    Boost::log
    Boost::system
    Boost::thread
    Boost::filesystem
)

target_compile_options(client
  PUBLIC
    -DBOOST_LOG_DYN_LINK
  )

add_executable(server Server/main.cpp Server/server.cpp Log/logger.cpp)

target_include_directories(server
  PUBLIC
    ${Boost_INCLUDE_DIRS}
    "${CMAKE_CURRENT_SOURCE_DIR}/Server"
    "${CMAKE_CURRENT_SOURCE_DIR}/Log"
  )

target_link_libraries(server
  PUBLIC
    Boost::log_setup
    Boost::log
    Boost::system
    Boost::thread
    Boost::filesystem
  )

target_compile_options(server
  PUBLIC
    -DBOOST_LOG_DYN_LINK
  )
