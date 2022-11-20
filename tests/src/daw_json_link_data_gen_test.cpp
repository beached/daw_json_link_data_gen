// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#include <daw/daw_do_not_optimize.h>
#include <daw/json/daw_json_link_data_gen.h>

#include <boost/container/small_vector.hpp>
#include <fstream>
#include <optional>
#include <ostream>
#include <string>
#include <vector>

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
		using type = json_member_list<
		  json_array<mem_asks, price_t, price_level_vec_t, price_level_vec_ctor_t>,
		  json_array<mem_bids, price_t, price_level_vec_t, price_level_vec_ctor_t>>;

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
