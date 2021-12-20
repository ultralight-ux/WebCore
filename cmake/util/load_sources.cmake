# Get list of all directories within 'dir', including 'dir'
MACRO(DIRLIST_RECURSE result dir)
    FILE(GLOB_RECURSE file_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${dir}/*)
    SET(dir_list "")
    FOREACH(file_path ${file_list})
	  GET_FILENAME_COMPONENT(file_dir ${file_path} DIRECTORY)
      LIST(APPEND dir_list ${file_dir})
    ENDFOREACH()
	LIST(APPEND dir_list ${dir})
	LIST(REMOVE_DUPLICATES dir_list)
    SET(${result} ${dir_list})
ENDMACRO()

# Add all CPP and GLSL source files recursively in a certain directory
# while also generating source_groups to maintain the
# original directory structure in Visual Studio.
MACRO(LOAD_SOURCES result dir)
  IF(NOT DEFINED ${result})
    set(${result} "")
  ENDIF()
  DIRLIST_RECURSE(dir_list ${dir})
  FOREACH(subdir ${dir_list})
    file(GLOB file_list RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} ${subdir}/*.h ${subdir}/*.c ${subdir}/*.cc ${subdir}/*.cpp ${subdir}/*.vert ${subdir}/*.frag ${subdir}/*.hlsl ${subdir}/*.fx)
    list(APPEND ${result} ${file_list})
	string(REGEX REPLACE "/" "\\\\\\\\" _sourcegroup "${subdir}")
    source_group("${_sourcegroup}" FILES ${file_list})
  ENDFOREACH()
ENDMACRO()