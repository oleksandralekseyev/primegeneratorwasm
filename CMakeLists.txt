cmake_minimum_required(VERSION 3.24)

project(PrimeGeneratorWasm VERSION 0.1.0 LANGUAGES CXX)

if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(PRIMEGENERATOR_BUILD_SMOKE_TEST "Build the native smoke test" ON)

add_library(prime_generator_core STATIC
  src/PrimeGenerator.cpp
)

target_include_directories(prime_generator_core
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(MSVC)
  target_compile_options(prime_generator_core PRIVATE /W4 /EHs-c-)
else()
  target_compile_options(prime_generator_core PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions)
endif()

if(EMSCRIPTEN)
  add_executable(prime_generator_wasm
    src/PrimeGeneratorBindings.cpp
  )

  target_link_libraries(prime_generator_wasm PRIVATE prime_generator_core)

  target_link_options(prime_generator_wasm PRIVATE
    "--bind"
    "--no-entry"
    "-sASYNCIFY"
    "-sALLOW_MEMORY_GROWTH=1"
    "-sDISABLE_EXCEPTION_CATCHING=1"
    "-sENVIRONMENT=web,node"
    "-sEXPORT_ES6=1"
    "-sEXPORT_NAME=createPrimeGeneratorModule"
    "-sFILESYSTEM=0"
    "-sMODULARIZE=1"
    "-sSTRICT=1"
  )

  set_target_properties(prime_generator_wasm PROPERTIES
    OUTPUT_NAME "prime_generator"
    SUFFIX ".js"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/dist"
  )

  add_custom_command(TARGET prime_generator_wasm POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/dist"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/web/index.html"
      "${CMAKE_BINARY_DIR}/dist/index.html"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/web/app.js"
      "${CMAKE_BINARY_DIR}/dist/app.js"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/web/PrimeGeneratorClient.js"
      "${CMAKE_BINARY_DIR}/dist/PrimeGeneratorClient.js"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
      "${CMAKE_CURRENT_SOURCE_DIR}/web/styles.css"
      "${CMAKE_BINARY_DIR}/dist/styles.css"
    VERBATIM
  )
else()
  if(PRIMEGENERATOR_BUILD_SMOKE_TEST)
    enable_testing()

    add_executable(prime_generator_smoke
      tests/PrimeGeneratorSmoke.cpp
    )

    target_link_libraries(prime_generator_smoke PRIVATE prime_generator_core)

    if(MSVC)
      target_compile_options(prime_generator_smoke PRIVATE /W4 /EHs-c-)
    else()
      target_compile_options(prime_generator_smoke PRIVATE -Wall -Wextra -Wpedantic -fno-exceptions)
    endif()

    add_test(NAME prime_generator_smoke COMMAND prime_generator_smoke)
  endif()
endif()
