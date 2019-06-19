#include "nightmode.hpp"

c_nightmode night_mode;

static convar* old_sky_name;
bool executed = false;

void c_nightmode::run() noexcept {
	if (!config_system.item.visuals_enabled)
		return;

	config_system.item.nightmode ? night_mode.apply() : night_mode.remove();

	static auto r_drawspecificstaticprop = interfaces::console->get_convar("r_DrawSpecificStaticProp");
	r_drawspecificstaticprop->set_value(config_system.item.nightmode ? 0 : 1);

	static auto r_3dsky = interfaces::console->get_convar("r_3dsky");
	r_3dsky->set_value(config_system.item.nightmode ? 0 : 1);
}

void c_nightmode::apply() noexcept {
	if (executed) {
		return;
	}

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	old_sky_name = interfaces::console->get_convar("sv_skyname");
	float brightness = config_system.item.nightmode_brightness / 100.f;

	for (MaterialHandle_t i = interfaces::material_system->first_material(); i != interfaces::material_system->invalid_material_handle(); i = interfaces::material_system->next_material(i)) {
		auto material = interfaces::material_system->get_material(i);

		if (!material)
			continue;

		if (strstr(material->GetTextureGroupName(), "World")) {
			material->color_modulate(brightness, brightness, brightness);
		}
		else if (strstr(material->GetTextureGroupName(), "StaticProp")) {
			material->color_modulate(brightness + 0.25f, brightness + 0.25f, brightness + 0.25f);
		}
		if (strstr(material->GetTextureGroupName(), ("SkyBox"))) {
			material->color_modulate(config_system.item.clr_sky[0], config_system.item.clr_sky[1], config_system.item.clr_sky[2]);
		}
	}

	utilities::load_named_sky("sky_csgo_night02");
	executed = true;
}

void c_nightmode::remove() noexcept {
	if (!executed) {
		return;
	}

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	for (MaterialHandle_t i = interfaces::material_system->first_material(); i != interfaces::material_system->invalid_material_handle(); i = interfaces::material_system->next_material(i)) {
		auto material = interfaces::material_system->get_material(i);

		if (!material)
			continue;

		if (strstr(material->GetTextureGroupName(), "World")) {
			material->color_modulate(1.f, 1.f, 1.f);
		}
		else if (strstr(material->GetTextureGroupName(), "StaticProp")) {
			material->color_modulate(1.f, 1.f, 1.f);
		}
		if (strstr(material->GetTextureGroupName(), ("SkyBox"))) {
			material->color_modulate(1.f, 1.f, 1.f);
		}
	}

	if (old_sky_name)
		utilities::load_named_sky(old_sky_name->string);

	executed = false;
}

void c_nightmode::ambient_light() noexcept
{
	if (!config_system.item.ambient)
		return;

	static auto red = interfaces::console->get_convar("mat_ambient_light_r");
	static auto green = interfaces::console->get_convar("mat_ambient_light_g");
	static auto blue = interfaces::console->get_convar("mat_ambient_light_b");

	red->set_value(config_system.item.clr_ambient[0]);
	green->set_value(config_system.item.clr_ambient[1]);
	blue->set_value(config_system.item.clr_ambient[2]);
}
