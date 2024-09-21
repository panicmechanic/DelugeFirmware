cmake_policy(SET CMP0135 NEW)

include(FetchContent)

FetchContent_Declare(argon
  #GIT_REPOSITORY https://github.com/stellar-aria/argon
  #GIT_TAG caa7cd8056351d4fea353c1e572fa62e04a49971
  URL https://github.com/stellar-aria/argon/archive/refs/tags/v0.1.0.tar.gz
  URL_HASH MD5=2cbc9a803e52ded736f1e28eca8b14aa
)
FetchContent_MakeAvailable(argon)

# tinyusb
FetchContent_Declare(tinyusb
  URL https://github.com/hathach/tinyusb/archive/refs/tags/0.17.0.tar.gz
  SOURCE_SUBDIR src
)

add_library(tinyusb_config INTERFACE)
target_include_directories(tinyusb_config INTERFACE tinyusb)

FetchContent_MakeAvailable(tinyusb)

add_library(tinyusb-rusb2)
target_sources(tinyusb-rusb2 PUBLIC
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/dcd_rusb2.c
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/hcd_rusb2.c
    ${tinyusb_SOURCE_DIR}/src/portable/renesas/rusb2/rusb2_common.c
)
target_link_libraries(tinyusb-rusb2 PUBLIC
    tinyusb
    tinyusb_config)
