#include "damage_indicator.hpp"
#include "../misc/logs.hpp"

c_damageindicator damage_indicator;

void c_damageindicator::draw() noexcept {
	if (!config_system.item.damage_indicator)
		return;
	
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player || !local_player->is_alive())
		return;

	float current_time = local_player->get_tick_base() * interfaces::globals->interval_per_tick;

	for (int i = 0; i < data.size(); i++) {
		if (data[i].di_erase_time < current_time) {
			data.erase(data.begin() + i);
			continue;
		}

		if (!data[i].di_initialized) {
			data[i].di_position = data[i].di_player->get_head_pos();
			data[i].di_initialized = true;
		}

		if (current_time - data[i].di_last_update > 0.0001f) {
			data[i].di_position.z -= (0.1f * (current_time - data[i].di_erase_time));
			data[i].di_last_update = current_time;
		}

		vec3_t screen_pos;
		auto red = config_system.item.clr_damage_indicator[0] * 255;
		auto green = config_system.item.clr_damage_indicator[1] * 255;
		auto blue = config_system.item.clr_damage_indicator[2] * 255;
		auto alpha = config_system.item.clr_damage_indicator[3] * 255;

		if (!math.world_to_screen(data[i].di_position, screen_pos))
			return;

		render.draw_text(screen_pos.x - 5, screen_pos.y + 10, render.name_font, std::to_string(data[i].di_damage).c_str(), false, color(red, green, blue, alpha));
		render.draw_text(screen_pos.x + 5, screen_pos.y - 10, render.name_font_big, std::string(data[i].di_damage_group).c_str(), false, color(red, green, blue, alpha));
	}
}
