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

struct arg_t {
	std::string channel;
	std::string instId;
}; // arg_t

struct price_t {
	double price;
	double quantity;
	double dummy;
	double orderCount;
};

#if __has_include( <boost/container/small_vector.hpp>)
#	define DAW_USE_SMALL_VEC
#	include <boost/container/small_vector.hpp>

inline static constexpr int64_t kMaxBookLevels = 100;
using price_level_vec_t =
  boost::container::small_vector<price_t, kMaxBookLevels>;

struct price_level_vec_ctor_t {
	using T = price_level_vec_t;
	[[nodiscard]] T operator( )( ) const
	  noexcept( std::is_nothrow_default_constructible_v<T> ) {
		return T{ };
	}

	template<typename... Args,
	         std::enable_if_t<(sizeof...( Args ) > 0 and
	                           std::is_constructible_v<T, Args...>),
	                          std::nullptr_t> = nullptr>
	[[nodiscard]] T operator( )( Args &&...args ) const {

		return T( DAW_FWD( args )... );
	}
};

struct data_element_t {
	price_level_vec_t asks;
	price_level_vec_t bids;
};
#else
struct data_element_t {
	std::vector<price_t> asks;
	std::vector<price_t> bids;
}; // data_element_t
#endif

struct root_object_t {
	arg_t arg;
	std::string action;
	std::vector<data_element_t> data;
}; // root_object_t

namespace daw::json {
	template<>
	struct json_data_contract<arg_t> {
		static constexpr char const mem_channel[] = "channel";
		static constexpr char const mem_instId[] = "instId";
		using type =
		  json_member_list<json_string<mem_channel>, json_string<mem_instId>>;

		static inline auto to_json_data( arg_t const &value ) {
			return std::forward_as_tuple( value.channel, value.instId );
		}
	};

	template<>
	struct json_data_contract<price_t> {
		using PriceStr =
		  json_number_no_name<double, options::number_opt(
		                                options::LiteralAsStringOpt::Always )>;
		using QuantityStr =
		  json_number_no_name<double, options::number_opt(
		                                options::LiteralAsStringOpt::Always )>;
		using type =
		  json_tuple_member_list<PriceStr, QuantityStr, QuantityStr, QuantityStr>;

		static auto to_json_data( price_t const &p ) {
			return std::forward_as_tuple( p.price, p.quantity, p.dummy,
			                              p.orderCount );
		}
	};

	template<>
	struct json_data_contract<data_element_t> {
		static constexpr char const mem_asks[] = "asks";
		static constexpr char const mem_bids[] = "bids";
		static constexpr char const mem_ts[] = "ts";
		static constexpr char const mem_checksum[] = "checksum";
#ifdef DAW_USE_SMALL_VEC
		using type = json_member_list<
		  json_array<mem_asks, price_t, price_level_vec_t, price_level_vec_ctor_t>,
		  json_array<mem_bids, price_t, price_level_vec_t, price_level_vec_ctor_t>>;
#else
		using type = json_member_list<json_array<mem_asks, price_t>,
		                              json_array<mem_bids, price_t>>;
#endif
		static inline auto to_json_data( data_element_t const &value ) {
			return std::forward_as_tuple( value.asks, value.bids );
		}
	};

	template<>
	struct json_data_contract<root_object_t> {
		static constexpr char const mem_arg[] = "arg";
		static constexpr char const mem_action[] = "action";
		static constexpr char const mem_data[] = "data";
		using type =
		  json_member_list<json_class<mem_arg, arg_t>, json_string<mem_action>,
		                   json_array<mem_data, json_class_no_name<data_element_t>,
		                              std::vector<data_element_t>>>;

		static inline auto to_json_data( root_object_t const &value ) {
			return std::forward_as_tuple( value.arg, value.action, value.data );
		}
	};
} // namespace daw::json

int main( ) {
	using namespace daw::json;
	using namespace daw::data_gen;
	/*
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
	}*/
	std::string json_str;
	for( std::size_t n = 0; n < 1000; ++n ) {
		auto d = generate_data_for<root_object_t>( );
		daw::do_not_optimize( d );
		to_json( d, json_str );
		daw::do_not_optimize( json_str );
		auto d2 = from_json<root_object_t>( json_str );
		daw::do_not_optimize( d2 );
	}
	daw::do_not_optimize( json_str );
	return 0;
}
