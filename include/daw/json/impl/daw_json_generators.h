// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link_data_gen
//

#pragma once

#include "../../data_faker/concepts/daw_writable_output.h"

#include <daw/daw_scope_guard.h>
#include <daw/json/daw_json_link.h>

#include <fmt/format.h>
#include <random>
#include <type_traits>

namespace daw::data_gen {
	enum class basic_data_types {
		String,
		Signed,
		Unsigned,
		Real,
		Bool,
		Array,
		Class,
		Nullable
	};

	template<typename>
	inline constexpr daw::string_view valid_string_chars =
	  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789!@#$%^&*()-="
	  "_+[]{}|;':,.<>/? 	";

	template<typename RandomEngine>
	static char gen_random_character( RandomEngine &reng ) {
		static_assert( not valid_string_chars<char>.empty( ) );
		static auto dist = std::uniform_int_distribution<std::size_t>(
		  0, valid_string_chars<char>.size( ) );

		return valid_string_chars<char>.data( )[dist( reng )];
	}

	template<typename T, typename RandomEngine>
	T gen_random_string( RandomEngine &reng ) {
		T result;
		if constexpr( std::is_same_v<T, std::string> ) {
			result.reserve( 4096U );
		}
		char c = gen_random_character( reng );
		while( c != 0 ) {
			put_output( result, c );
			c = gen_random_character( reng );
		}
		return result;
	}
	template<basic_data_types, typename, typename = void>
	struct default_value_generator;

	template<typename T>
	struct default_value_generator<basic_data_types::String, T> {};
} // namespace daw::data_gen

namespace daw::json::json_details {
	template<typename JsonMember, typename = void,
	         typename = std::enable_if_t<is_a_json_type_v<JsonMember>>>
	struct value_generator;

	template<typename JsonMember, JsonParseTypes ExpectedType>
	inline constexpr bool member_is_parse_type_v =
	  JsonMember::expected_type == ExpectedType;

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Real>>> {

		using type = typename JsonMember::parse_to_t;
		static_assert( std::is_floating_point_v<type>,
		               "For non std::is_floating_point types, one needs to "
		               "specialize value_generator" );

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			static auto dist = std::uniform_real_distribution<type>(
			  0, 1.0 /*std::numeric_limits<type>::max( )*/ );
			static auto dsign = std::uniform_int_distribution<int>( 0, 1 );
			auto result = dist( reng );
			if( dsign( reng ) ) {
				return -result;
			}
			return result;
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Signed>>> {

		using type = typename JsonMember::parse_to_t;
		static_assert(
		  std::is_integral_v<type> and std::is_signed_v<type>,
		  "For non std::is_integral/std::is_signed types, one needs to "
		  "specialize value_generator" );
		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			static auto dist = std::uniform_int_distribution<type>(
			  std::numeric_limits<type>::min( ), std::numeric_limits<type>::max( ) );
			return dist( reng );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Unsigned>>> {
		using type = typename JsonMember::parse_to_t;
		static_assert( std::is_unsigned_v<type>,
		               "For non std::is_unsigned types, one needs to specialize "
		               "value_generator" );
		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			static auto dist = std::uniform_int_distribution<type>(
			  0, std::numeric_limits<type>::max( ) );
			return dist( reng );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Bool>>> {
		using type = typename JsonMember::parse_to_t;
		static_assert( std::is_convertible_v<bool, type>,
		               "For types not convertible to bool, one must specialize "
		               "value_generator" );
		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			static auto dist = std::uniform_int_distribution<unsigned>( 0, 1 );

			return static_cast<type>( dist( reng ) );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember,
	                       std::enable_if_t<member_is_parse_type_v<
	                         JsonMember, JsonParseTypes::StringEscaped>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			return data_gen::gen_random_string<type>( reng );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember,
	                       std::enable_if_t<member_is_parse_type_v<
	                         JsonMember, JsonParseTypes::StringRaw>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State const & ) const {
			return data_gen::gen_random_string<type>( reng );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Null>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State &state ) const {
			static auto dist = std::uniform_int_distribution<unsigned>( 0, 5 );
			using constructor_t = typename JsonMember::constructor_t;
			auto const construct_empty = [&] {
				if constexpr( std::is_invocable_v<
				                constructor_t,
				                concepts::construct_nullable_with_empty_t> ) {
					return construct_value(
					  template_args<typename JsonMember::wrapped_type, constructor_t>,
					  state, concepts::construct_nullable_with_empty );
				} else {
					return construct_value(
					  template_args<typename JsonMember::wrapped_type, constructor_t>,
					  state );
				}
			};
			auto const v = dist( reng );
			if( not static_cast<bool>( v ) ) {
				return construct_empty( );
			} else {
				using base_member_type = typename JsonMember::member_type;
				return construct_value(
				  template_args<base_member_type, constructor_t>, state,
				  value_generator<base_member_type>{ }( reng, state ) );
			}
		}
	};

	template<typename>
	inline static constexpr std::size_t max_array_size = 100ULL; // 1'000'000ULL;

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Array>>>;

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Class>>>;

	template<typename JsonMember, typename RandomEngine, typename State>
	struct value_generator_array_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using value_type = typename JsonMember::json_element_parse_to_t;
		using reference = value_type &;
		using pointer = value_type *;
		using difference_type = std::ptrdiff_t;

		std::size_t m_count = 0;
		RandomEngine *m_engine = nullptr;
		State *m_state = nullptr;
		mutable std::optional<value_type> m_last = std::nullopt;

		// Construct start iter
		constexpr explicit value_generator_array_iterator( RandomEngine &reng,
		                                                   State &state )
		  : m_engine( std::addressof( reng ) )
		  , m_state( std::addressof( state ) ){ };

		// Construct end iter
		constexpr explicit value_generator_array_iterator( std::size_t count )
		  : m_count( count ){ };

		constexpr value_generator_array_iterator &operator++( ) {
			m_last.reset( );
			++m_count;
			return *this;
		}

		constexpr value_generator_array_iterator operator++( int ) & {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr value_generator_array_iterator &operator--( ) {
			m_last.reset( );
			--m_count;
			return *this;
		}

		constexpr value_generator_array_iterator operator--( int ) & {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr difference_type
		operator-( value_generator_array_iterator const &rhs ) const {
			return static_cast<difference_type>( m_count ) -
			       static_cast<difference_type>( rhs.m_count );
		}

		constexpr void ensure_last( ) const {
			if( not m_last ) {
				m_last = value_generator<typename JsonMember::json_element_t>{ }(
				  *m_engine, *m_state );
			}
		}

		constexpr value_type const &operator*( ) const {
			ensure_last( );
			return *m_last;
		}

		constexpr value_type &operator*( ) {
			ensure_last( );
			return *m_last;
		}

		constexpr value_type const *operator->( ) const {
			ensure_last( );
			return std::addressof( *m_last );
		}

		constexpr value_type *operator->( ) {
			ensure_last( );
			return std::addressof( *m_last );
		}

		constexpr bool
		operator==( value_generator_array_iterator const &rhs ) const {
			return m_count == rhs.m_count;
		}

		constexpr bool
		operator!=( value_generator_array_iterator const &rhs ) const {
			return m_count != rhs.m_count;
		}

		constexpr bool
		operator<( value_generator_array_iterator const &rhs ) const {
			return m_count < rhs.m_count;
		}

		constexpr bool
		operator>( value_generator_array_iterator const &rhs ) const {
			return m_count > rhs.m_count;
		}

		constexpr bool
		operator<=( value_generator_array_iterator const &rhs ) const {
			return m_count <= rhs.m_count;
		}

		constexpr bool
		operator>=( value_generator_array_iterator const &rhs ) const {
			return m_count >= rhs.m_count;
		}
	};

	template<typename JsonMember, typename RandomEngine, typename State>
	struct value_generator_kv_iterator {
		using iterator_category = std::random_access_iterator_tag;
		using key_type_t = json_link_no_name<typename JsonMember::key_type_t>;
		using value_type_t = json_link_no_name<typename JsonMember::value_type_t>;
		using kv_t = std::pair<typename key_type_t::parse_to_t,
		                       typename value_type_t::parse_to_t>;
		using container_t = typename JsonMember::parse_to_t;
		using constructor_t = typename JsonMember::constructor_t;

		using reference = kv_t &;
		using pointer = kv_t *;
		using difference_type = std::ptrdiff_t;

		std::size_t m_count = 0;
		RandomEngine *m_engine = nullptr;
		State *m_state = nullptr;
		mutable std::optional<kv_t> m_last = std::nullopt;

		// Construct start iter
		constexpr explicit value_generator_kv_iterator( RandomEngine &reng,
		                                                State &state )
		  : m_engine( std::addressof( reng ) )
		  , m_state( std::addressof( state ) ){ };

		// Construct end iter
		constexpr explicit value_generator_kv_iterator( std::size_t count )
		  : m_count( count ){ };

		constexpr value_generator_kv_iterator &operator++( ) {
			m_last.reset( );
			++m_count;
			return *this;
		}

		constexpr value_generator_kv_iterator operator++( int ) & {
			auto result = *this;
			operator++( );
			return result;
		}

		constexpr value_generator_kv_iterator &operator--( ) {
			m_last.reset( );
			--m_count;
			return *this;
		}

		constexpr value_generator_kv_iterator operator--( int ) & {
			auto result = *this;
			operator--( );
			return result;
		}

		constexpr difference_type
		operator-( value_generator_kv_iterator const &rhs ) const {
			return static_cast<difference_type>( m_count ) -
			       static_cast<difference_type>( rhs.m_count );
		}

		constexpr void ensure_last( ) const {
			if( not m_last ) {
				m_last =
				  kv_t{ value_generator<key_type_t>{ }( *m_engine, *m_state ),
				        value_generator<value_type_t>{ }( *m_engine, *m_state ) };
			}
		}

		constexpr kv_t const &operator*( ) const {
			ensure_last( );
			return *m_last;
		}

		constexpr kv_t &operator*( ) {
			ensure_last( );
			return *m_last;
		}

		constexpr kv_t const *operator->( ) const {
			ensure_last( );
			return std::addressof( *m_last );
		}

		constexpr kv_t *operator->( ) {
			ensure_last( );
			return std::addressof( *m_last );
		}

		constexpr bool operator==( value_generator_kv_iterator const &rhs ) const {
			return m_count == rhs.m_count;
		}

		constexpr bool operator!=( value_generator_kv_iterator const &rhs ) const {
			return m_count != rhs.m_count;
		}

		constexpr bool operator<( value_generator_kv_iterator const &rhs ) const {
			return m_count < rhs.m_count;
		}

		constexpr bool operator>( value_generator_kv_iterator const &rhs ) const {
			return m_count > rhs.m_count;
		}

		constexpr bool operator<=( value_generator_kv_iterator const &rhs ) const {
			return m_count <= rhs.m_count;
		}

		constexpr bool operator>=( value_generator_kv_iterator const &rhs ) const {
			return m_count >= rhs.m_count;
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Array>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		DAW_ATTRIB_FLATTEN type operator( )( RandomEngine &reng,
		                                     State &state ) const {
			static auto sz_dist =
			  std::uniform_int_distribution<unsigned>( 0, max_array_size<type> );
			auto const ary_size = sz_dist( reng );
			using it_t =
			  value_generator_array_iterator<JsonMember, RandomEngine, State>;
			auto first = it_t( reng, state );
			auto last = it_t( ary_size );
			using constructor_t = typename JsonMember::constructor_t;
			return construct_value(
			  template_args<json_result<JsonMember>, constructor_t>, state, first,
			  last );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Custom>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		DAW_ATTRIB_FLATTEN type operator( )( RandomEngine &reng,
		                                     State &state ) const {
			/*
			static auto sz_dist =
			  std::uniform_int_distribution<unsigned>( 0, max_array_size<type> );
			auto const ary_size = sz_dist( reng );
			using it_t =
			  value_generator_array_iterator<JsonMember, RandomEngine, State>;
			auto first = it_t( reng, state );
			auto last = it_t( ary_size );
			using constructor_t = typename JsonMember::constructor_t;
			return construct_value(
			  template_args<json_result<JsonMember>, constructor_t>, state, first,
			  last );
			  */
			return { };
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::KeyValue>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		DAW_ATTRIB_FLATTEN type operator( )( RandomEngine &reng,
		                                     State &state ) const {

			static auto sz_dist =
			  std::uniform_int_distribution<unsigned>( 0, max_array_size<type> );
			auto const ary_size = sz_dist( reng );
			using it_t = value_generator_kv_iterator<JsonMember, RandomEngine, State>;
			auto first = it_t( reng, state );
			auto last = it_t( ary_size );
			using constructor_t = typename JsonMember::constructor_t;
			return construct_value(
			  template_args<json_result<JsonMember>, constructor_t>, state, first,
			  last );
		}
	};

	template<typename JsonMember, typename RandomEngine, typename State>
	constexpr auto visit_json_member( RandomEngine &reng, State &state ) {
		/*
		auto old_path = DAW_MOVE( state.path );
		auto const ae = daw::on_exit_success( [&] {
			state.path = DAW_MOVE( old_path );
		} );
		state.path = fmt::format(
		  "{}.{}", old_path, static_cast<std::string_view>( JsonMember::name ) );*/
		return value_generator<json_link_no_name<JsonMember>>{ }( reng, state );
	}

	template<typename, typename>
	struct class_generator;

	template<typename JsonMember, typename... JsonMembers>
	struct class_generator<JsonMember, json_member_list<JsonMembers...>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State &state ) const {
			using constructor_t = typename JsonMember::constructor_t;
			return construct_value(
			  template_args<json_result<JsonMember>, constructor_t>, state,
			  visit_json_member<JsonMembers>( reng, state )... );
		}
	};

	template<typename JsonMember, typename... JsonMembers>
	struct class_generator<JsonMember, json_tuple_member_list<JsonMembers...>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		type operator( )( RandomEngine &reng, State &state ) const {
			using constructor_t = typename JsonMember::constructor_t;

			return construct_value(
			  template_args<json_result<JsonMember>, constructor_t>, state,
			  visit_json_member<JsonMembers>( reng, state )... );
		}
	};

	template<typename JsonMember>
	struct value_generator<JsonMember, std::enable_if_t<member_is_parse_type_v<
	                                     JsonMember, JsonParseTypes::Class>>> {
		using type = typename JsonMember::parse_to_t;

		template<typename RandomEngine, typename State>
		DAW_ATTRIB_FLATTEN type operator( )( RandomEngine &reng,
		                                     State &state ) const {

			return class_generator<JsonMember, json_data_contract_trait_t<
			                                     typename JsonMember::base_type>>{ }(
			  reng, state );
		}
	};
} // namespace daw::json::json_details
