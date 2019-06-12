#pragma once
#include "../../../dependencies/common_includes.hpp"
#include "../../../dependencies/math/math.hpp"

struct stored_records {
	vec3_t head;
	float simulation_time;
	matrix_t matrix[128];
};

struct convars {
	convar* update_rate;
	convar* max_update_rate;
	convar* interp;
	convar* interp_ratio;
	convar* min_interp_ratio;
	convar* max_interp_ratio;
	convar* max_unlag;
};

extern std::deque<stored_records> records[65];
extern convars cvars;

class c_backtrack  {
public:
	void update() noexcept;
	void run(c_usercmd*) noexcept;
	float get_lerp_time() noexcept;
	int time_to_ticks(float time) noexcept;
	bool valid_tick(float simtime) noexcept;
	static void init() {
		records->clear();

		cvars.update_rate = interfaces::console->get_convar("cl_updaterate");
		cvars.max_update_rate = interfaces::console->get_convar("sv_maxupdaterate");
		cvars.interp = interfaces::console->get_convar("cl_interp");
		cvars.interp_ratio = interfaces::console->get_convar("cl_interp_ratio");
		cvars.min_interp_ratio = interfaces::console->get_convar("sv_client_min_interp_ratio");
		cvars.max_interp_ratio = interfaces::console->get_convar("sv_client_max_interp_ratio");
		cvars.max_unlag = interfaces::console->get_convar("sv_maxunlag");
	}
};

extern c_backtrack backtrack;