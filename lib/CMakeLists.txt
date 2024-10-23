cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(argon
  GIT_REPOSITORY https://github.com/stellar-aria/argon
  GIT_TAG main
)
FetchContent_MakeAvailable(argon)

add_compile_definitions(NDEBUG)
set(NE10_ASM_OPTIMIZATION ON)
set(NE10_TARGET_ARCH armv7)

FetchContent_Declare(Ne10
  GIT_REPOSITORY https://github.com/stellar-aria/Ne10
  GIT_TAG bare_metal
  GIT_SHALLOW 1
)
FetchContent_MakeAvailable(Ne10)
target_include_directories(NE10 BEFORE PUBLIC NE10)
