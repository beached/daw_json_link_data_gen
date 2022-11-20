// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#pragma once

#include "impl/daw_json_generators.h"

#include <daw/json/daw_json_link.h>

#include <random>

namespace daw::json {
	struct state_t : BasicParsePolicy<> {
		std::string path{ };
	};

	struct root_name {
		static constexpr char const value[] = "";
	};

	template<typename T>
	inline auto generate_data_for( ) {
		using json_member_noname = json_details::json_deduced_type<T>;
		using json_member =
		  typename json_member_noname::template with_name<root_name::value>;
		using minstd_rand =
		  std::linear_congruential_engine<unsigned int, 48271, 0, 2147483647>;
		// auto r = std::random_device( );
		auto r = minstd_rand( );
		auto eng = std::default_random_engine( r( ) );
		auto state = state_t{ };
		return json_details::value_generator<json_member>{ }( eng, state );
	}
} // namespace daw::json
