
# sources
SET(APP dlopes_bot)
SET(APP_SRC dlopes_bot.c cJSON.c)


# Add a option to enable/disable tests
option(RUN_TESTING "Enable test routines" OFF)

# If RUN_TESTING is ON, define it as a preprocessor macro
if(RUN_TESTING STREQUAL "ON")
    add_definitions(-DRUN_TEST)
    message(STATUS "RUN_TEST is turned on...")
else()
    message(STATUS "RUN_TEST is turned off")
endif()

# executable
ADD_EXECUTABLE(${APP} ${APP_SRC})

# includes
TARGET_INCLUDE_DIRECTORIES(${APP} PRIVATE ${CMAKE_SOURCE_DIR})


# Adding the build of the Rust lib
SET(RUST_LIB_DIR "${CMAKE_SOURCE_DIR}/rust_lib/open_meteo_wrapper")
SET(RUST_RELEASE_DIR "${RUST_LIB_DIR}/target/release")
SET(RUST_LIB_NAME "libopen_meteo_wrapper.so")
ADD_CUSTOM_COMMAND(
    OUTPUT ${RUST_RELEASE_DIR}/${RUST_LIB_NAME}
    COMMAND cargo build --release
    WORKING_DIRECTORY ${RUST_LIB_DIR}
    COMMENT "Building Rust library..."
)

ADD_CUSTOM_TARGET(
    rust_lib ALL
    DEPENDS ${RUST_RELEASE_DIR}/${RUST_LIB_NAME}
)

# Our project depends on rust_lib
ADD_DEPENDENCIES(${APP} rust_lib)


TARGET_LINK_LIBRARIES(${APP} ${PKGS_LDFLAGS} ${PROJECT_NAME} ${RUST_RELEASE_DIR}/${RUST_LIB_NAME} pthread)


# Customized clean to also clear the rust library
ADD_CUSTOM_TARGET(
    clean-rust
    COMMAND cargo clean
    WORKING_DIRECTORY ${RUST_LIB_DIR}
    COMMENT "Cleaning Rust build..."
)

# The clean-all
ADD_CUSTOM_TARGET(
    clean-all
    COMMAND ${CMAKE_MAKE_PROGRAM} clean
    DEPENDS clean-rust
    COMMENT "Cleaning all build outputs, including Rust library..."
)



#EOF
