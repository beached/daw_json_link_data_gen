// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_data_gen
//

#pragma once

#include "../impl/version.h"

#include <daw/cpp_17.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::data_gen::concepts {
	/// @brief Concept to help deduce container types.
	template<typename, typename = void>
	struct container_traits : std::false_type {};
} // namespace daw::data_gen::concepts
