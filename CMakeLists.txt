cmake_minimum_required(VERSION 3.5)
project( Dfa VERSION 0.1.1 )






add_library(Dfa STATIC src/Dfa.c)

target_include_directories( Dfa PUBLIC ${PROJECT_SOURCE_DIR}/include )
target_sources( Dfa PRIVATE ${PROJECT_SOURCE_DIR}/src/Dfa )
target_sources( Dfa PUBLIC ${PROJECT_SOURCE_DIR}/include/Dfa )

set_target_properties(Dfa
	PROPERTIES
	ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
	LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/lib"
	RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin"
)

add_subdirectory(${CMAKE_SOURCE_DIR}/ext/HashTable)
target_link_libraries(Dfa HashTable)

add_subdirectory(${CMAKE_SOURCE_DIR}/ext/LinkedList)
target_link_libraries(Dfa LinkedList)
