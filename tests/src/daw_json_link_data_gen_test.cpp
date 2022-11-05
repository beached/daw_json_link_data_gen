// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#include <daw/json/daw_json_link_data_gen.h>

#include <optional>
#include <string>
#include <vector>

struct Foo {
	int x;
	std::string y;
};

namespace daw::json {
	template<>
	struct json_data_contract<Foo> {
		static constexpr char const x[] = "x";
		static constexpr char const y[] = "y";

		using type = json_member_list<json_number<x, int>, json_string<y>>;
	};
} // namespace daw::json

int main( ) {
	auto osig = daw::json::generate_data_for<std::optional<int>>( );
	auto sig = daw::json::generate_data_for<signed>( );
	auto unsig = daw::json::generate_data_for<unsigned>( );
	auto real = daw::json::generate_data_for<double>( );
	auto b = daw::json::generate_data_for<bool>( );
	auto str = daw::json::generate_data_for<std::string>( );
	auto v = daw::json::generate_data_for<std::vector<int>>( );
	auto c = daw::json::generate_data_for<Foo>( );
	auto cv = daw::json::generate_data_for<std::vector<Foo>>( );
	return sig;
}
