// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#include "citm_test_json.h"
#include "geojson_json.h"
#include "twitter_test_json.h"

#include <daw/daw_do_not_optimize.h>
#include <daw/json/daw_json_link_data_gen.h>

#include <fstream>
#include <optional>
#include <ostream>
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

		static auto to_json_data( Foo const &f ) {
			return std::forward_as_tuple( f.x, f.y );
		}
	};
} // namespace daw::json

struct Bar {
	std::optional<signed> osig;
	signed sig;
	unsigned unsig;
	double real;
	bool b;
	std::string str;
	std::vector<int> v;
	Foo c;
	std::vector<Foo> cv;
};

namespace daw::json {
	template<>
	struct json_data_contract<Bar> {
		static constexpr char const osig[] = "osig";
		static constexpr char const sig[] = "sig";
		static constexpr char const unsig[] = "unsig";
		static constexpr char const real[] = "real";
		static constexpr char const b[] = "b";
		static constexpr char const str[] = "str";
		static constexpr char const v[] = "v";
		static constexpr char const c[] = "c";
		static constexpr char const cv[] = "cv";

		using type =
		  json_member_list<json_number_null<osig, std::optional<signed>>,
		                   json_number<sig, signed>, json_number<unsig, unsigned>,
		                   json_number<real>, json_bool<b>, json_string<str>,
		                   json_array<v, int>, json_class<c, Foo>,
		                   json_array<cv, Foo>>;

		static auto to_json_data( Bar const &b ) {
			return std::forward_as_tuple( b.osig, b.sig, b.unsig, b.real, b.b, b.str,
			                              b.v, b.c, b.cv );
		}
	};
} // namespace daw::json


int main( ) {
	using namespace daw::json;
	using namespace daw::data_gen;

	auto osig = generate_data_for<std::optional<int>>( );
	auto sig = generate_data_for<signed>( );
	auto unsig = generate_data_for<unsigned>( );
	auto real = generate_data_for<double>( );
	auto b = generate_data_for<bool>( );
	auto str = generate_data_for<std::string>( );
	auto v = generate_data_for<std::vector<int>>( );
	auto c = generate_data_for<Foo>( );
	auto cv = generate_data_for<std::vector<Foo>>( );
	auto bar = generate_data_for<Bar>( );
	auto bar_str =
	  to_json( bar, options::output_flags<options::SerializationFormat::Pretty> );
	(void)bar_str;
	auto geo = generate_data_for<daw::geojson::FeatureCollection>( );
	std::string geo_str = to_json( geo );
	auto geo2 = from_json<daw::geojson::FeatureCollection>( geo_str );
	(void)geo2;
	{
	  auto geo_outf = std::ofstream( "geojson.json" );
	  assert( geo_outf );
	  to_json( geo, geo_outf,
	           options::output_flags<options::SerializationFormat::Pretty> );
	}
	auto twit = generate_data_for<daw::twitter::twitter_object_t>( );
	std::string twit_str = to_json( twit );
	auto twit2 = from_json<daw::twitter::twitter_object_t>( twit_str );
	(void)twit2;
	{
	  auto twit_outf = std::ofstream( "twitter.json" );
	  assert( twit_outf );
	  to_json( twit, twit_outf,
	           options::output_flags<options::SerializationFormat::Pretty> );
	}
	auto citm = generate_data_for<daw::citm::citm_object_t>( );
	std::string citm_str = to_json( citm );
	auto citm2 = from_json<daw::citm::citm_object_t>( citm_str );
	(void)citm2;
	{
	  auto citm_outf = std::ofstream( "citm.json" );
	  assert( citm_outf );
	  to_json( citm, citm_outf,
	           options::output_flags<options::SerializationFormat::Pretty> );
	}
	return 0;
}
