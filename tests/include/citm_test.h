// Copyright (c) Darrell Wright
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/beached/daw_json_link
//

#pragma once

#include <daw/json/daw_from_json_fwd.h>

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace daw::citm {
	struct events_value_t {
		std::int64_t id;
		std::optional<std::string> logo;
		std::string name;
		std::vector<std::int64_t> subTopicIds;
		std::vector<std::int64_t> topicIds;
	}; // events_value_t

	struct prices_element_t {
		std::int64_t amount;
		std::int64_t audienceSubCategoryId;
		std::int64_t seatCategoryId;
	}; // prices_element_t

	struct areas_element_t {
		std::int64_t areaId;
	}; // areas_element_t

	struct seatCategories_element_t {
		std::vector<areas_element_t> areas;
		std::int64_t seatCategoryId;
	}; // seatCategories_element_t

	struct performances_element_t {
		std::int64_t eventId;
		std::int64_t id;
		std::optional<std::string> logo;
		std::vector<prices_element_t> prices;
		std::vector<seatCategories_element_t> seatCategories;
		std::int64_t start;
		std::string venueCode;
	}; // performances_element_t

	struct venueNames_t {
		std::string pleyel_pleyel;
	}; // venueNames_t

	struct citm_object_t {
		std::unordered_map<std::int64_t, std::string> areaNames;
		std::unordered_map<std::int64_t, std::string> audienceSubCategoryNames;
		std::unordered_map<std::int64_t, events_value_t> events;
		std::vector<performances_element_t> performances;
		std::unordered_map<std::string, std::string> seatCategoryNames;
		std::unordered_map<std::string, std::string> subTopicNames;
		std::unordered_map<std::string, std::string> topicNames;
		std::unordered_map<std::string, std::vector<std::int64_t>> topicSubTopics;
		std::optional<venueNames_t> venueNames;
	}; // citm_object_t

	citm_object_t parse_citm( std::string json_doc );
} // namespace daw::citm
