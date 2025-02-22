
# sources
SET(TEST_NAME dlopes_bot)
SET(TEST_SRC dlopes_bot.c cJSON.c)

# executable
ADD_EXECUTABLE(${TEST_NAME} ${TEST_SRC})

# includes
TARGET_INCLUDE_DIRECTORIES(${TEST_NAME} PRIVATE ${CMAKE_SOURCE_DIR})

#  Link the rust api lib
SET(RUST_LIB_PATH "${CMAKE_SOURCE_DIR}/libopen_meteo_wrapper.so")

TARGET_LINK_LIBRARIES(${TEST_NAME} ${PKGS_LDFLAGS} ${PROJECT_NAME} ${RUST_LIB_PATH} pthread)

#EOF
