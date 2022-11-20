// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include "defines.h"

#include <daw/json/daw_from_json_fwd.h>

#include <array>
#include <cstdint>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

namespace daw::geojson {
	struct Property {
		std::string name;
	}; // Property

	struct Point {
		double x;
		double y;
	};

	struct Polygon {
		std::string type;
		std::vector<std::vector<Point>> coordinates;

		Polygon( std::string t, std::vector<std::vector<Point>> &&coords )
		  : type( t )
		  , coordinates( std::move( coords ) ) {}
	}; // Polygon

	struct Feature {
		std::string type;
		Property properties;
		Polygon geometry;
	}; // Feature

	struct FeatureCollection {
		std::string type;
		std::vector<Feature> features;
	}; // FeatureCollection
} // namespace daw::geojson
