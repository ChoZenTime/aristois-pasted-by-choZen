#pragma once
#include "../../../dependencies/common_includes.hpp"

struct c_damageindicator_t {
	int di_damage;
	const char* di_damage_group;
	bool di_initialized;
	float di_erase_time;
	float di_last_update;
	player_t* di_player;
	vec3_t di_position;
};

class c_damageindicator {
public:
	std::vector<c_damageindicator_t> data;
	void draw() noexcept;
};

extern c_damageindicator damage_indicator;
