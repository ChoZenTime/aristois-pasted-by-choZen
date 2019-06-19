#include "nade_pred.hpp"

c_nade_prediction nade_pred;

void c_nade_prediction::predict(c_usercmd* user_cmd) noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	//	readability
	constexpr float restitution = 0.45f;
	constexpr float power[] = { 1.0f, 1.0f, 0.5f, 0.0f };
	constexpr float velocity = 403.0f * 0.9f;

	float step, gravity, new_velocity, unk01;
	int index{}, grenade_act{ 1 };
	vec3_t pos, thrown_direction, start, eye_origin;
	vec3_t angles, thrown;

	//	first time setup
	static auto sv_gravity = interfaces::console->get_convar("sv_gravity");

	//	calculate step and actual gravity value
	gravity = sv_gravity->get_float() / 8.0f;
	step = interfaces::globals->interval_per_tick;

	//	get local view and eye origin
	eye_origin = local_player->eye_pos();
	angles = user_cmd->viewangles;

	//	copy current angles and normalise pitch
	thrown = angles;

	if (thrown.x < 0) {
		thrown.x = -10 + thrown.x * ((90 - 10) / 90.0f);
	}
	else {
		thrown.x = -10 + thrown.x * ((90 + 10) / 90.0f);
	}

	//	find out how we're throwing the grenade
	auto primary_attack = user_cmd->buttons & in_attack;
	auto secondary_attack = user_cmd->buttons & in_attack2;

	if (primary_attack && secondary_attack) {
		grenade_act = ACT_LOB;
	}
	else if (secondary_attack) {
		grenade_act = ACT_DROP;
	}

	//	apply 'magic' and modulate by velocity
	unk01 = power[grenade_act];

	unk01 = unk01 * 0.7f;
	unk01 = unk01 + 0.3f;

	new_velocity = velocity * unk01;

	//	here's where the fun begins
	math.angle_vectors(thrown, thrown_direction);

	start = eye_origin + thrown_direction * 16.0f;
	thrown_direction = (thrown_direction * new_velocity) + local_player->velocity();

	//	let's go ahead and predict
	for (auto time = 0.0f; index < 500; time += step) {

		pos = start + thrown_direction * step;
		trace_t trace;
		ray_t ray;
		trace_filter_skip_one_entity filter(local_player);

		ray.initialize(start, pos);
		interfaces::trace_ray->trace_ray(ray, MASK_SOLID, &filter, &trace);

		//	modify path if we have hit something
		if (trace.flFraction != 1.0f) {
			thrown_direction = trace.plane.normal * -2.0f * thrown_direction.dot(trace.plane.normal) + thrown_direction;

			thrown_direction *= restitution;

			pos = start + thrown_direction * trace.flFraction * step;

			time += (step * (1.0f - trace.flFraction));
		}

		//	check for detonation
		auto detonate = detonated(local_player->active_weapon(), time, trace);

		//	emplace nade point
		_points.at(index++) = c_nadepoint(start, pos, trace.flFraction != 1.0f, true, trace.plane.normal, detonate);
		start = pos;

		//	apply gravity modifier
		thrown_direction.z -= gravity * trace.flFraction * step;

		if (detonate) {
			break;
		}
	}

	//	invalidate all empty points and finish prediction
	for (auto n = index; n < 500; ++n) {
		_points.at(n).m_valid = false;
	}

	_predicted = true;
}

bool c_nade_prediction::detonated(weapon_t * weapon, float time, trace_t & trace)noexcept {
	
	if (!weapon) {
		return true;
	}

	//	get weapon item index
	const auto index = weapon->item_definition_index();

	switch (index) {
		//	flash and HE grenades only live up to 2.5s after thrown
	case WEAPON_HEGRENADE:
	case WEAPON_FLASHBANG:
		if (time > 2.5f) {
			return true;
		}
		break;

		//	fire grenades detonate on ground hit, or 3.5s after thrown
	case WEAPON_MOLOTOV:
	case WEAPON_INCGRENADE:
		if (trace.flFraction != 1.0f && trace.plane.normal.z > 0.7f || time > 3.5f) {
			return true;
		}
		break;

		//	decoy and smoke grenades were buggy in prediction, so i used this ghetto work-around
	case WEAPON_DECOY:
	case WEAPON_SMOKEGRENADE:
		if (time > 5.0f) {
			return true;
		}
		break;
	}

	return false;
}

void c_nade_prediction::trace(c_usercmd * user_cmd) noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.item.nade_pred) {
		return;
	}

	if (!(user_cmd->buttons & in_attack) && !(user_cmd->buttons & in_attack2)) {
		_predicted = false;
		return;
	}

	const static std::vector< int > nades{
		WEAPON_FLASHBANG,
		WEAPON_SMOKEGRENADE,
		WEAPON_HEGRENADE,
		WEAPON_MOLOTOV,
		WEAPON_DECOY,
		WEAPON_INCGRENADE
	};

	//	grab local weapon
	auto weapon = local_player->active_weapon();

	if (!weapon) {
		return;
	}

	if (std::find(nades.begin(), nades.end(), weapon->item_definition_index()) != nades.end()) {
		return predict(user_cmd);
	}

	_predicted = false;
}

void c_nade_prediction::draw() noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player) {
		return;
	}
	if (!config_system.item.nade_pred) {
		return;
	}

	if (!interfaces::engine->is_in_game()) {
		return;
	}

	if (!local_player->is_alive()) {
		return;
	}

	auto draw_3d_dotted_circle = [](vec3_t position, float points, float radius) {
		float step = 3.141592654f * 2.0f / points;
		for (float a = 0; a < 3.141592654f * 2.0f; a += step) {
			vec3_t start(radius * cosf(a) + position.x, radius * sinf(a) + position.y, position.z);

			vec3_t start2d;
			if (interfaces::debug_overlay->world_to_screen(start, start2d))
				render.draw_line(start2d.x, start2d.y, start2d.x + 1, start2d.y + 1, color(255, 255, 255));
		}
	};

	vec3_t start, end;

	//	draw nade path
	if (_predicted) {
		for (auto& p : _points) {
			if (!p.m_valid) {
				break;
			}

			if (math.world_to_screen(p.m_start, start) && math.world_to_screen(p.m_end, end)) {

				render.draw_line(start.x, start.y, end.x, end.y, color(0, 125, 255));

				if (p.m_detonate || p.m_plane) {
					;
					render.draw_rect(start.x - 2, start.y - 2, 5, 5, p.m_detonate ? color(255, 0, 0) : color(255, 255, 255));
				}
				if (p.m_detonate)
					draw_3d_dotted_circle(p.m_end, 100, 150);
			}
		}
	}
}
