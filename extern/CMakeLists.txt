# Copyright (c) Darrell Wright
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/beached/daw_json_link_data_gen
#

include( FetchContent )

option( DAW_ENABLE_TESTING "Build unit tests" OFF )
set( DAW_ENABLE_TESTING OFF )

FetchContent_Declare(
        daw_json_link
        GIT_REPOSITORY https://github.com/beached/daw_json_link.git
        GIT_TAG v3
)

FetchContent_Declare(
        fmtlib
        GIT_REPOSITORY https://github.com/fmtlib/fmt.git
        GIT_TAG 9.1.0
)

FetchContent_MakeAvailable( daw_json_link fmtlib )

