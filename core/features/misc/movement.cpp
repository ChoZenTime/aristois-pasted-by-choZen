#include "../../../dependencies/common_includes.hpp"
#include "movement.hpp"

c_movement movement;

auto flags_backup = 0;

void c_movement::bunnyhop(c_usercmd* user_cmd) noexcept {
	if (!config_system.item.bunny_hop || !config_system.item.misc_enabled)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	const int hitchance = config_system.item.bunny_hop_hitchance;
	const int restrict_limit = 12;
	const int hop_limit = config_system.item.bunny_hop_maximum_value;
	const int min_hop = config_system.item.bunny_hop_minimum_value;
	static int hops_restricted = 0;
	static int hops_hit = 0;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (user_cmd->buttons & in_jump && !(local_player->flags() & fl_onground)) {
		user_cmd->buttons &= ~in_jump;
		hops_restricted = 0;
	}

	else if ((rand() % 100 > hitchance && hops_restricted < restrict_limit) || (hop_limit > 0 && hops_hit > hop_limit && min_hop > 0 && hops_hit > min_hop)) {
		user_cmd->buttons &= ~in_jump;
		hops_restricted++;
		hops_hit = 0;
	}
	else {
		hops_hit++;
	}
}

void c_movement::edge_jump_pre_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.item.edge_jump)
		return;

	if (!GetAsyncKeyState(config_system.item.edge_jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	flags_backup = local_player->flags();
}

void c_movement::edge_jump_post_prediction(c_usercmd* user_cmd) noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.item.edge_jump)
		return;

	if (!GetAsyncKeyState(config_system.item.edge_jump_key))
		return;

	if (!local_player)
		return;

	if (local_player->move_type() == movetype_ladder || local_player->move_type() == movetype_noclip)
		return;

	if (flags_backup & fl_onground && !(local_player->flags() & fl_onground))
		user_cmd->buttons |= in_jump;

	if (!(local_player->flags() & fl_onground) && config_system.item.edge_jump_duck_in_air)
		user_cmd->buttons |= in_duck;
}

void c_movement::strafe(c_usercmd* user_cmd)noexcept {
	if (!config_system.item.strafer)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player || !local_player->is_alive())
		return;

	if (local_player->flags() & fl_onground && !(user_cmd->buttons & in_jump))
		return;

	if (local_player->move_type() & (movetype_ladder | movetype_noclip))
		return;

	vec3_t viewangles;
	interfaces::engine->get_view_angles_alternative(viewangles);

	static bool side_switch = false;
	side_switch = !side_switch;

	user_cmd->forwardmove = 0.f;
	user_cmd->sidemove = side_switch ? 450.f : -450.f;

	float velocity_yaw = local_player->velocity().y;

	float rotation = std::clamp< float >(rad_to_deg(std::atan2(15.f, local_player->velocity().Length2D())), 0.f, 90.f);

	float abs_yaw = std::fabs(math.normalize_angle(velocity_yaw - viewangles.y));

	float ideal_yaw_rotation = (side_switch ? rotation : -rotation) + (abs_yaw < 5.f ? velocity_yaw : viewangles.y);

	float cos_rot = std::cos(deg_to_rad(viewangles.y - ideal_yaw_rotation));
	float sin_rot = std::sin(deg_to_rad(viewangles.y - ideal_yaw_rotation));

	user_cmd->forwardmove = (cos_rot * user_cmd->forwardmove) - (sin_rot * user_cmd->sidemove);
	user_cmd->sidemove = (sin_rot * user_cmd->forwardmove) + (cos_rot * user_cmd->sidemove);
}