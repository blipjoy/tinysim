set(SRCS
    fstest.ino
)
arduino_to_cxx(SRCS ${SRCS})

add_executable(fstest ${SRCS})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/fstest)
add_custom_command(
    OUTPUT ${CMAKE_BINARY_DIR}/fstest/smile.tfb
    COMMAND ${CMAKE_SOURCE_DIR}/tools/data2arcade.py -o ${CMAKE_BINARY_DIR}/fstest/smile.tfb ${CMAKE_CURRENT_SOURCE_DIR}/assets/smile.data
    DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/assets/smile.data
)
add_custom_target(
    fstest-assets
    ALL
    DEPENDS ${CMAKE_BINARY_DIR}/fstest/smile.tfb
)

target_link_libraries(fstest tinysim ${SDL2_LIBRARY})
