# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(embedoors ${MY_SOURCES})

# Make us of the HAL library
target_link_libraries(embedoors LINK_PRIVATE hal m)

# Copy executable to final location (change `embedoors` to project name as needed)
add_custom_command(TARGET embedoors POST_BUILD 
  COMMAND "${CMAKE_COMMAND}" -E copy 
     "$<TARGET_FILE:embedoors>"
     "~/cmpt433/public/myApps/embedoors" 
  COMMENT "Copying ARM executable to public NFS directory")

  add_custom_command(TARGET embedoors POST_BUILD
    COMMAND "${CMAKE_COMMAND}" -E copy_directory
    "${CMAKE_SOURCE_DIR}/server"
    "~/cmpt433/public/myApps/server_copy"
    COMMENT "Copying server files to public NFS directory")

add_custom_command(TARGET embedoors POST_BUILD
    COMMAND npm install 
    WORKING_DIRECTORY ~/cmpt433/public/myApps/server_copy
    COMMENT "npm installed!")
  