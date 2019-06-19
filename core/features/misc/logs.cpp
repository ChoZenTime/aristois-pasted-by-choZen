#include "logs.hpp"

c_event_logs event_logs;

void c_event_logs::run() noexcept {
	if (logs.empty())
		return;

	auto last_y = 6;

	for (unsigned int i = 0; i < logs.size(); i++) {
		auto & log = logs.at(i);

		if (utilities::epoch_time() - log.log_time > 2700) {
			float factor = (log.log_time + 3100) - utilities::epoch_time();
			factor /= 1000;

			auto opacity = int(255 * factor);

			if (opacity < 2) {
				logs.erase(logs.begin() + i);
				continue;
			}

			log.y += 1 * (factor * 1.25);
		}

		const auto text = log.message.c_str();
		render.draw_text(log.x, last_y + log.y, render.name_font, text, false, color(255, 255, 255, 255));

		last_y += 14;
	}
}

void c_event_logs::event_item_purchase(i_game_event* event) noexcept {
	if (!config_system.item.logs_player_bought)
		return;

	auto userid = event->get_int("userid");
	if (!userid)
		return;

	auto engine_userid = interfaces::engine->get_player_for_user_id(userid);
	if (!engine_userid)
		return;

	player_info_t info;
	interfaces::engine->get_player_info(engine_userid, &info);

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(engine_userid));

	if (!local_player || !entity)
		return;

	if (entity->team() == local_player->team())
		return;

	std::string player_name = info.name;
	std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

	std::stringstream ss;
	ss << player_name << " purchased a(n) " << event->get_string("weapon");

	utilities::console_warning("[player purchase] ");
	interfaces::console->console_printf("%s purchased a(n) %s.     \n", player_name.c_str(), event->get_string("weapon"));

	add(ss.str(), color(255, 255, 255, 255));

}

void c_event_logs::event_player_hurt(i_game_event* event) noexcept {
	if (!config_system.item.logs_player_hurt && !config_system.item.damage_indicator)
		return;
	
	if (!event)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto victim = interfaces::engine->get_player_for_user_id(event->get_int("userid"));
	if (!victim)
		return;

	auto attacker = interfaces::entity_list->get_client_entity(interfaces::engine->get_player_for_user_id(event->get_int("attacker")));
	if (!attacker)
		return;

	if (attacker == local_player) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(victim));
		if (!entity) {
			return;
		}

		player_info_t info;
		interfaces::engine->get_player_info(entity->index(), &info);

		auto hitbox = event->get_int("hitgroup");
		if (!hitbox)
			return;

		auto damage = event->get_int("dmg_health");
		if (!damage)
			return;

		auto health = event->get_int("health");
		if (!health && health != 0)
			return;

		auto hitgroup = utilities::hitgroup_name(hitbox);
		
		if (config_system.item.logs_player_hurt) {
			std::string player_name = info.name;
			std::transform(player_name.begin(), player_name.end(), player_name.begin(), ::tolower);

			std::stringstream ss;
			ss << "hit " << player_name.c_str() << " in the " << hitgroup << " for " << damage << " damage";
			ss << " (" << health << " health remaining).";

			utilities::console_warning("[player hurt] ");
			interfaces::console->console_printf("hit %s in the %s for %d damage (%d health remaining). \n", player_name.c_str(), hitgroup, damage, health);
			event_logs.add(ss.str(), color(255, 255, 255, 255));

			utilities::console_warning("[player info] ");
			interfaces::console->console_printf("moving: %s", entity->is_moving() ? "yes, " : "no, ");
			interfaces::console->console_printf("in air: %s", entity->is_in_air() ? "yes, " : "no, ");
			interfaces::console->console_printf("flashed: %s", entity->is_flashed() ? "yes \n" : "no \n");
		}
		if (config_system.item.damage_indicator) {
			c_damageindicator_t damage_in;
			damage_in.di_damage = damage;
			damage_in.di_damage_group = hitgroup;
			damage_in.di_player = entity;
			damage_in.di_erase_time = local_player->get_tick_base() * interfaces::globals->interval_per_tick + 3.f;
			damage_in.di_initialized = false;
			damage_indicator.data.push_back(damage_in);
		}
	}
}

void c_event_logs::add(std::string text, color colors) noexcept {
	logs.push_front(loginfo_t(utilities::epoch_time(), text, colors));
}
