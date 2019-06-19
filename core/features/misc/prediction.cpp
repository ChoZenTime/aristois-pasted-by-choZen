#include "prediction.hpp"
#include "../../../dependencies/utilities/md5.hpp"
#include "../../../dependencies/interfaces/interfaces.hpp"
#include <iostream>
#include <fstream>

c_prediction engine_prediction;

void c_prediction::start_prediction(c_usercmd* command) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player) {
		static bool initialized = false;
		if (!initialized) {
			prediction_random_seed = *(int**)(utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "8B 0D ? ? ? ? BA ? ? ? ? E8 ? ? ? ? 83 C4 04") + 2);
			initialized = true;
		}

		*prediction_random_seed = utilities::md5::pseduo_random(command->command_number) & 0x7FFFFFFF;

		old_cur_time = interfaces::globals->cur_time;
		old_frame_time = interfaces::globals->frame_time;

		interfaces::globals->cur_time = local_player->get_tick_base() * interfaces::globals->interval_per_tick; //tick_base_test
		interfaces::globals->frame_time = interfaces::globals->interval_per_tick;

		interfaces::game_movement->start_track_prediction_errors(local_player);

		memset(&move_data, 0, sizeof(move_data));
		interfaces::move_helper->set_host(local_player);
		interfaces::prediction->setup_move(local_player, command, interfaces::move_helper, &move_data);
		interfaces::game_movement->process_movement(local_player, &move_data);
		interfaces::prediction->finish_move(local_player, command, &move_data);
	}
}

void c_prediction::end_prediction() noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!local_player)
		return;

	if (local_player) {
		interfaces::game_movement->finish_track_prediction_errors(local_player);
		interfaces::move_helper->set_host(0);

		*prediction_random_seed = -1;

		interfaces::globals->cur_time = old_cur_time;
		interfaces::globals->frame_time = old_cur_time;
	}
}
