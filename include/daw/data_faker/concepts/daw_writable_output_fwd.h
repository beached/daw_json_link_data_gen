// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_data_gen
//

#pragma once

#include <daw/daw_traits.h>

namespace daw::data_gen::concepts {
	/// @brief Writable output models write/putc methods to allow efficient
	/// output to buffers/FILE streams/ostreams/and containers with less
	/// allocation/size checks Specializations must have static T write( T,
	/// StringViews... ), static T put( T, char ), and static bool value.
	/// StringViews work will have a .size( ) and .data( ) member function,
	/// and have a character element type
	template<typename, typename = void>
	struct writable_output_trait : std::false_type {};

	template<typename T>
	inline constexpr bool is_writable_output_type_v =
	  writable_output_trait<T>::value;
} // namespace daw::data_gen::concepts

namespace daw::data_gen {
	template<typename WritableOutput, typename... StringViews>
	WritableOutput &write_output( WritableOutput &out,
	                              StringViews const &...svs ) {
		concepts::writable_output_trait<WritableOutput>::write( out, svs... );
		return out;
	}

	template<typename WritableOutput, typename CharT>
	WritableOutput &put_output( WritableOutput &out, CharT c ) {
		concepts::writable_output_trait<WritableOutput>::put( out, c );
		return out;
	}
} // namespace daw::data_gen
