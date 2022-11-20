// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_data_gen
//

#pragma once

#include "daw_container_traits_fwd.h"

#include <daw/cpp_17.h>

#include <array>
#include <cstddef>
#include <iterator>
#include <type_traits>

namespace daw::data_gen::concepts {
	namespace container_detect {
		template<typename T>
		using is_container_test =
		  decltype( (void)( std::begin( std::declval<T &>( ) ) ),
		            (void)( std::end( std::declval<T &>( ) ) ),
		            (void)( std::declval<typename T::value_type>( ) ),
		            (void)( std::declval<T &>( ).insert(
		              std::end( std::declval<T &>( ) ),
		              std::declval<typename T::value_type>( ) ) ) );
	} // namespace container_detect

	template<typename T>
	struct container_traits<T, std::enable_if_t<daw::is_detected_v<
	                             container_detect::is_container_test, T>>>
	  : std::true_type {};

	template<typename T, std::size_t N>
	struct container_traits<std::array<T, N>> : std::true_type {
		static constexpr bool has_custom_constructor = true;

		/*
		template<typename Iterator>
		static constexpr std::array<T, N> construct( Iterator first, Iterator
		last ) {

		}*/
	};

	/// @brief Is the type deduced or specialized as a container
	template<typename T>
	inline constexpr bool is_container_v = container_traits<T>::value;
} // namespace daw::data_gen::concepts
