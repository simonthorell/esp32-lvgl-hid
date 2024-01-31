# check_and_copy.cmake
set(source_file "${CMAKE_ARGV3}")
set(destination_file "${CMAKE_ARGV4}")

message(STATUS "Source file path: ${source_file}")
message(STATUS "Destination file path: ${destination_file}")

# Check if the source file exists
if(EXISTS "${source_file}")
    # The file exists, check if the destination file is different from the source
    execute_process(
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${source_file}" "${destination_file}"
        RESULT_VARIABLE copy_result
    )

    if(copy_result EQUAL 0)
        message(STATUS "File ${source_file} was copied to ${destination_file}")
    else()
        message(WARNING "File ${source_file} could not be copied to ${destination_file}. Error code: ${copy_result}")
    endif()
else()
    # The file doesn't exist, print a message
    message(STATUS "Source file ${source_file} does not exist. Copy not performed.")
endif()