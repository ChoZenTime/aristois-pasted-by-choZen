#include <cmath>
#include "trigger_simple.hpp"
#include "../../../source-sdk/sdk.hpp"
#include "../../../source-sdk/math/vector2d.hpp"
#include "../../../dependencies/common_includes.hpp"
#define _USE_MATH_DEFINES
#define	HITGROUP_GENERIC	0
#define	HITGROUP_HEAD		1
#define	HITGROUP_CHEST		2
#define	HITGROUP_STOMACH	3
#define HITGROUP_LEFTARM	4
#define HITGROUP_RIGHTARM	5
#define HITGROUP_LEFTLEG	6
#define HITGROUP_RIGHTLEG	7
#define HITGROUP_GEAR		10

c_trigger trigger;

void c_trigger::trigger(c_usercmd* user_cmd) {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	if (!config_system.item.trigger_enable && !GetAsyncKeyState(config_system.item.trigger_key))
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player && !local_player->is_alive())
		return;

	auto weapon = local_player->active_weapon();

	if (!weapon || !weapon->clip1_count() || is_bomb(weapon) || is_knife(weapon) || is_grenade(weapon))
		return;

	vec3_t src, dst, forward;
	trace_t tr;
	ray_t ray;
	trace_filter filter;

	math.angle_vectors(user_cmd->viewangles, forward);

	forward *= weapon->get_weapon_data()->range;
	filter.skip = local_player;
	src = local_player->get_eye_pos();
	dst = src + forward;
	ray.initialize(src, dst);
	interfaces::trace_ray->trace_ray(ray, 0x46004003, &filter, &tr);

	vec3_t viewangles = user_cmd->viewangles;
	if (config_system.item.trigger_recoil)
		viewangles += local_player->aim_punch_angle() * 2.0f;

	vec3_t crosshair_forward;
	math.angle_vectors(viewangles, crosshair_forward);
	crosshair_forward *= 8000.f;


	if (tr.DidHitWorld())
		return;
	if (!tr.DidHitNonWorldEntity())
		return;

	if (local_player->team() == tr.entity->team())
		return;

	int hitgroup = tr.hitGroup;
	bool didhit = false;

	if (config_system.item.trigger_hitbox_head)
	{
		if (hitgroup == HITGROUP_HEAD) didhit = true;
	}
	if (config_system.item.trigger_hitbox_body)
	{
		if (hitgroup == HITGROUP_CHEST || hitgroup == HITGROUP_STOMACH) didhit = true;
	}
	if (config_system.item.trigger_hitbox_arms)
	{
		if (hitgroup == HITGROUP_LEFTARM || hitgroup == HITGROUP_RIGHTARM) didhit = true;
	}
	if (config_system.item.trigger_hitbox_legs)
	{
		if (hitgroup == HITGROUP_LEFTLEG || hitgroup == HITGROUP_RIGHTLEG) didhit = true;
	}

	if (trigger_delay >= 2 + config_system.item.trigger_delay && didhit)
	{
		trigger_delay = 0;
		user_cmd->buttons |= in_attack;
	}
	trigger_delay++;
}