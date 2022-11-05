// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#pragma once

#include "impl/daw_json_generators_fwd.h"

#include <daw/json/daw_json_link.h>

#include <random>

namespace daw::json {
	template<typename T>
	inline auto generate_data_for( ) {
		using json_member = json_details::json_deduced_type<T>;
		auto r = std::random_device( );
		auto eng = std::default_random_engine( r( ) );
		using State = BasicParsePolicy<options::parse_flags_t<>::value>;
		auto state = State{ };
		return json_details::value_generator<json_member>{ }( eng, state );
	}
} // namespace daw::json
