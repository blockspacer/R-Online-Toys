cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

INCLUDE_DIRECTORIES(
    SYSTEM ${PROTOBUF_INCLUDE_DIR}
    SYSTEM ${ASIO_INCLUDE_DIR}
    SYSTEM ${EXTERNAL_INCLUDE_DIR}
    SYSTEM ${MYSQL_INCLUDE_DIRS}
    SYSTEM ${MYSQLpp_INCLUDE_DIR}
)

LINK_DIRECTORIES(${EXTERNAL_BINARY_DIR})

SET(PROJECT LoginServer)

project(${PROJECT})

IF(ENABLE_MYSQL_ONE_INSTANCE)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_MYSQL_ONE_INSTANCE")
ENDIF()

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DASIO_STANDALONE")
set(GCC_LINK_FLAGS "")
set(Clang_LINK_FLAGS "")
set(MSVC_LINK_FLAGS " /NODEFAULTLIB:MSVCRTD /NODEFAULTLIB:MSVCRT")

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
ELSE()
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Werror")
ENDIF()

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-packed-bitfield-compat -Wl,-Map=${PROJECT}.map")
ELSEIF( ${CMAKE_CXX_COMPILER_ID} STREQUAL Clang )
	SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-keyword-compat")
ENDIF()

# Our Stuff 
INCLUDE_DIRECTORIES(
    ${CMAKE_SOURCE_DIR}/src/core/include
    ${CMAKE_SOURCE_DIR}/src/rosecommon/include
    ${CMAKE_SOURCE_DIR}/src/proto/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

SET( SOURCES_MAIN
  src/main.cpp )
  
SET( SOURCES_HEADER
  include/cloginserver
  include/cloginclient
  include/cloginisc )

SET( SOURCES_BASE
  src/cloginserver
  src/cloginclient
  src/cloginisc )

find_package( Threads REQUIRED )
add_executable(${PROJECT} ${SOURCES_MAIN} ${SOURCES_BASE} ${SOURCES_HEADER})

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
	ADD_DEPENDENCIES(${PROJECT} protoGen)
	ADD_DEPENDENCIES(${PROJECT} mysqlpp)
ENDIF()

SET_TARGET_PROPERTIES(${PROJECT} PROPERTIES LINK_FLAGS
                "${${CMAKE_CXX_COMPILER_ID}_LINK_FLAGS}")

target_link_libraries(${PROJECT} 
	Core 
	RoseCommon
	protoGen
	${CMAKE_THREAD_LIBS_INIT}
)

IF (${CMAKE_CXX_COMPILER_ID} STREQUAL MSVC)
        TARGET_LINK_LIBRARIES(${PROJECT} ${PROTOBUF_LIBRARY}
                ${MYSQL_LIBRARIES} 
                wsock32.lib
                mysqlpp
        )
ELSE()
        TARGET_LINK_LIBRARIES(${PROJECT}
                ${PROTOBUF_BINARY_DIR}/libprotobuf.a
                mysqlpp
                mysqlclient
                )
ENDIF()

INSTALL(
    TARGETS ${PROJECT}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib/static
    CONFIGURATIONS Release)