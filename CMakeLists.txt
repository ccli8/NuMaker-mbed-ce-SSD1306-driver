# Copyright (c) 2020 ARM Limited. All rights reserved.
# SPDX-License-Identifier: Apache-2.0

add_library(nu-ssd1306 STATIC EXCLUDE_FROM_ALL)

target_include_directories(nu-ssd1306
    PUBLIC
        .
)

target_sources(nu-ssd1306
    PRIVATE
        draw2D.cpp
        ssd1306.cpp
)

target_link_libraries(nu-ssd1306 PUBLIC mbed-core-flags)
