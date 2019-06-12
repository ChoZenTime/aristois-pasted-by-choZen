#pragma once
#include "../../../dependencies/common_includes.hpp"
#include "../../../dependencies/interfaces/i_player_movement.hpp"

class c_prediction {
public:
	void start_prediction(c_usercmd* command) noexcept;
	void end_prediction() noexcept;
	float old_cur_time;
	float old_frame_time;
	int* prediction_random_seed;
	player_move_data move_data;
};

extern c_prediction engine_prediction;