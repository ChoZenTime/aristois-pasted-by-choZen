#include "visuals.hpp"
#include "../../../dependencies/common_includes.hpp"
#include "../backtrack/backtrack.hpp"
#define TIME_TO_TICKS(dt) ((int)( 0.5f + (float)(dt) / interfaces::globals->interval_per_tick))

c_visuals visuals;

void c_visuals::run() noexcept {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (!config_system.item.visuals_enabled)
		return;

	if (config_system.item.anti_screenshot && interfaces::engine->is_taking_screenshot())
		return;

	if (!local_player)
		return;

	//player drawing loop
	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (!entity)
			continue;

		if (entity == local_player)
			continue;

		if (entity->health() <= 0)
			continue;

		if (config_system.item.radar)
			entity->spotted() = true;

		if (entity->team() == local_player->team() && !config_system.item.visuals_team_check)
			continue;

		if (!local_player->can_see_player_pos(entity, entity->get_eye_pos()) && config_system.item.visuals_visible_only)
			continue;

		if (config_system.item.visuals_on_key && !GetAsyncKeyState(config_system.item.visuals_key))
			continue;

		const int fade = (int)((6.66666666667f * interfaces::globals->frame_time) * 255);

		auto new_alpha = alpha[i];
		new_alpha += entity->dormant() ? -fade : fade;

		if (new_alpha > (entity->has_gun_game_immunity() ? 130 : 210))
			new_alpha = (entity->has_gun_game_immunity() ? 130 : 210);
		if (new_alpha < config_system.item.player_dormant ? 50 : 0)
			new_alpha = config_system.item.player_dormant ? 50 : 0;

		alpha[i] = new_alpha;

		player_rendering(entity);
		skeleton(entity);
		last_dormant[i] = entity->dormant();
	}

	//non player drawing loop
	for (int i = 0; i < interfaces::entity_list->get_highest_index(); i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (entity && entity != local_player) {
			auto client_class = entity->client_class();
			auto model_name = interfaces::model_info->get_model_name(entity->model());

			if (client_class->class_id == class_ids::cplantedc4) { //this should be fixed in better ways than this - designer
				bomb_esp(entity);
				bomb_defuse_esp(entity);
			}

			entity_esp(entity);
			dropped_weapons(entity);
			projectiles(entity);
		}
	}
}

void c_visuals::entity_esp(player_t* entity) noexcept {
	if (!config_system.item.entity_esp)
		return;

	if (!entity)
		return;

	if (entity->dormant())
		return;

	auto model_name = interfaces::model_info->get_model_name(entity->model());
	vec3_t entity_position, entity_origin;
	entity_origin = entity->origin();
	auto client_class = entity->client_class();

	if (!math.world_to_screen(entity_origin, entity_position))
		return;

	if (client_class->class_id == class_ids::cchicken) {
		render.draw_text(entity_position.x, entity_position.y, render.name_font, "chicken", true, color(255, 255, 255));
	}

	else if (strstr(model_name, "dust_soccer_ball001")) {
		render.draw_text(entity_position.x, entity_position.y, render.name_font, "soccer ball", true, color(255, 255, 255));
	}

	else if (client_class->class_id == class_ids::chostage) {
		render.draw_text(entity_position.x, entity_position.y, render.name_font, "hostage", true, color(255, 255, 255));
	}
}

void c_visuals::player_rendering(player_t* entity) noexcept {
	if ((entity->dormant() && alpha[entity->index()] == 0) && !config_system.item.player_dormant)
		return;

	player_info_t info;
	interfaces::engine->get_player_info(entity->index(), &info);

	box bbox;
	if (!get_playerbox(entity, bbox))
		return;

	if (config_system.item.player_box) {
		auto red = config_system.item.clr_box[0] * 255;
		auto green = config_system.item.clr_box[1] * 255;
		auto blue = config_system.item.clr_box[2] * 255;

		render.draw_outline(bbox.x - 1, bbox.y - 1, bbox.w + 2, bbox.h + 2, color(0, 0, 0, 255 + alpha[entity->index()]));
		render.draw_rect(bbox.x, bbox.y, bbox.w, bbox.h, color(red, green, blue, alpha[entity->index()]));
		render.draw_outline(bbox.x + 1, bbox.y + 1, bbox.w - 2, bbox.h - 2, color(0, 0, 0, 255 + alpha[entity->index()]));
	}
	if (config_system.item.player_health) {
		box temp(bbox.x - 5, bbox.y + (bbox.h - bbox.h * (utilities::math::clamp_value<int>(entity->health(), 0, 100.f) / 100.f)), 1, bbox.h * (utilities::math::clamp_value<int>(entity->health(), 0, 100) / 100.f) - (entity->health() >= 100 ? 0 : -1));
		box temp_bg(bbox.x - 5, bbox.y, 1, bbox.h);

		// change the color depending on the entity health
		auto health_color = color( ( 255 - entity->health() * 2.55 ), ( entity->health() * 2.55 ), 0, alpha[entity->index()] );

		// clamp health (custom maps, danger zone, etc)
		if ( entity->health() > 100 )
		    	health_color = color( 0, 255, 0 );

		//draw actual dynamic hp bar
		render.draw_filled_rect(temp_bg.x - 1, temp_bg.y - 1, temp_bg.w + 2, temp_bg.h + 2, color(0, 0, 0, 25 + alpha[entity->index()]));
		render.draw_filled_rect(temp.x, temp.y, temp.w, temp.h, color(health_color));
	}
	if (config_system.item.player_name) {
		auto red = config_system.item.clr_name[0] * 255;
		auto green = config_system.item.clr_name[1] * 255;
		auto blue = config_system.item.clr_name[2] * 255;

		std::string print(info.fakeplayer ? std::string("bot ").append(info.name).c_str() : info.name);
		std::transform(print.begin(), print.end(), print.begin(), ::tolower);

		render.draw_text(bbox.x + (bbox.w / 2), bbox.y - 13, render.name_font, print, true, color(red, green, blue, alpha[entity->index()]));
	}
	{
		std::vector<std::pair<std::string, color>> flags;
		std::string posi = (entity->get_callout());
		std::transform(posi.begin(), posi.end(), posi.begin(), ::tolower);

		if (config_system.item.player_flags_armor && entity->has_helmet() && entity->armor() > 0)
			flags.push_back(std::pair<std::string, color>("hk", color(255, 255, 255, alpha[entity->index()])));
		else if (config_system.item.player_flags_armor && !entity->has_helmet() && entity->armor() > 0)
			flags.push_back(std::pair<std::string, color>("k", color(255, 255, 255, alpha[entity->index()])));

		if (config_system.item.player_flags_money && entity->money())
			flags.push_back(std::pair<std::string, color>(std::string("$").append(std::to_string(entity->money())), color(120, 180, 10, alpha[entity->index()])));

		if (config_system.item.player_flags_scoped && entity->is_scoped())
			flags.push_back(std::pair<std::string, color>(std::string("zoom"), color(80, 160, 200, alpha[entity->index()])));

		if (config_system.item.player_flags_c4 && entity->has_c4())
			flags.push_back(std::pair<std::string, color>(std::string("bomb"), color(255, 127, 36, alpha[entity->index()])));

		if (config_system.item.player_flags_kit && entity->has_defuser())
			flags.push_back(std::pair<std::string, color>(std::string("kit"), color(28, 134, 238, alpha[entity->index()])));

		if (config_system.item.player_flags_flashed && entity->is_flashed())
			flags.push_back(std::pair<std::string, color>(std::string("flashed"), color(255, 255, 0, alpha[entity->index()])));
		
		if (config_system.item.player_flags_defuse && entity->is_defusing() && !entity->has_defuser())
			flags.push_back(std::pair<std::string, color>(std::string("defusing"), color(0, 191, 255, alpha[entity->index()])));

		if (config_system.item.player_flags_defuse && entity->is_defusing() && entity->has_defuser())
			flags.push_back(std::pair<std::string, color>(std::string("defusing"), color(122, 103, 238, alpha[entity->index()])));

		if (config_system.item.player_flags_pos && entity->get_callout())
			//flags.push_back(std::pair<std::string, color>(std::string(clean_item_name(posi)), color(0, 190, 90, alpha[entity->index()])));
			flags.push_back(std::pair<std::string, color>(std::string(posi), color(0, 190, 90, alpha[entity->index()])));
		
		auto position = 0;
		for (auto text : flags) {
			render.draw_text(bbox.x + bbox.w + 3, bbox.y + position - 2, render.name_font, text.first, false, text.second);
			position += 10;
		}
	}
	/*if (config_system.item.player_weapon) {
auto red = config_system.item.clr_weapon[0] * 255;
auto green = config_system.item.clr_weapon[1] * 255;
auto blue = config_system.item.clr_weapon[2] * 255;
auto weapon = entity->active_weapon();

if (!weapon)
	return;

std::string names;
names = clean_item_name(weapon->get_weapon_data()->weapon_name);

render.draw_text(bbox.x + (bbox.w / 2), bbox.h + bbox.y + 2, render.name_font, names, true, color(red, green, blue, alpha[entity->index()]));
}*/

	if (config_system.item.player_weapon) {

		auto red = config_system.item.clr_weapon[0] * 255;
		auto green = config_system.item.clr_weapon[1] * 255;
		auto blue = config_system.item.clr_weapon[2] * 255;
		auto weapon = entity->active_weapon();

		if (!weapon)
			return;

		auto weapon_data = weapon->get_weapon_data();

		auto item_definition_index = weapon->item_definition_index();

		if (!weapon_data)
			return;

		std::string weapon_name = weapon_data->hud_name;
		weapon_name.erase(0, 13);

		std::string search = "M4A1";
		std::string replace = "m4a4";
		std::string search1 = "M4_SILENCER";
		std::string replace1 = "m4a1_s";
		std::string search2 = "AK47";
		std::string replace2 = "ak47";
		std::string search3 = "Elites";
		std::string replace3 = "elite";
		std::string search4 = "DesertEagle";
		std::string replace4 = "deagle";
		std::string search5 = "Aug";
		std::string replace5 = "aug";
		std::string search6 = "AWP";
		std::string replace6 = "awp";
		std::string search7 = "Famas";
		std::string replace7 = "famas";
		std::string search8 = "FiveSeven";
		std::string replace8 = "fiveseven";
		std::string search9 = "G3SG1";
		std::string replace9 = "g3sg1";
		std::string search10 = "GalilAR";
		std::string replace10 = "galilar";
		std::string search11 = "Glock18";
		std::string replace11 = "glock18";
		std::string search12 = "M249";
		std::string replace12 = "m249";
		std::string search13 = "MAC10";
		std::string replace13 = "mac10";
		std::string search14 = "P90";
		std::string replace14 = "p90";
		std::string search15 = "UMP45";
		std::string replace15 = "ump45";
		std::string search16 = "mx1014";
		std::string replace16 = "xm1014";
		std::string search17 = "Bizon";
		std::string replace17 = "bizon";
		std::string search18 = "Mag7";
		std::string replace18 = "mag7";
		std::string search19 = "Negev";
		std::string replace19 = "negev";
		std::string search20 = "Sawedoff";
		std::string replace20 = "sawedoff";
		std::string search21 = "Tec9";
		std::string replace21 = "tec9";
		std::string search22 = "Taser";
		std::string replace22 = "zeus";
		std::string search23 = "HKP2000";
		std::string replace23 = "p2000";
		std::string search24 = "MP7";
		std::string replace24 = "mp7";
		std::string search25 = "Nova";
		std::string replace25 = "nova";
		std::string search26 = "P250";
		std::string replace26 = "p250";
		std::string search27 = "CZ75";
		std::string replace27 = "cz75";
		std::string search28 = "SCAR20";
		std::string replace28 = "scar20";
		std::string search29 = "SG556";
		std::string replace29 = "sg553";
		std::string search30 = "SSG08";
		std::string replace30 = "ssG08";
		std::string search31 = "REVOLVER";
		std::string replace31 = "revolver";
		std::string search51 = "USP_SILENCER";
		std::string replace51 = "usp_s";
		std::string search52 = "MP9";
		std::string replace52 = "mp9";
		std::string search53 = "MP5SD";
		std::string replace53 = "mp5";
		//grenades
		std::string search32 = "HE_Grenade";
		std::string replace32 = "grenade";
		std::string search33 = "Smoke_Grenade";
		std::string replace33 = "smoke";
		std::string search34 = "C4";
		std::string replace34 = "c4";
		std::string search35 = "MOLOTOV";
		std::string replace35 = "fire_molo";
		std::string search36 = "IncGrenade";
		std::string replace36 = "fire_inc";
		std::string search37 = "FLASHBANG";
		std::string replace37 = "flash";
		std::string search38 = "DECOY";
		std::string replace38 = "decoy";
		//knifes
		std::string search39 = "KnifeBayonet";
		std::string replace39 = "bayonet";
		std::string search40 = "KnifeFlip";
		std::string replace40 = "flip";
		std::string search41 = "KnifeGut";
		std::string replace41 = "gut";
		std::string search42 = "KnifeM9";
		std::string replace42 = "m9";
		std::string search43 = "KnifeKaram";
		std::string replace43 = "karambit";
		std::string search44 = "KnifeTactical";
		std::string replace44 = "huntsman";
		std::string search45 = "Knife_Butterfly";
		std::string replace45 = "butterfly";
		std::string search46 = "knife_falchion_advanced";
		std::string replace46 = "falchion";
		std::string search47 = "knife_push";
		std::string replace47 = "shadow";
		std::string search48 = "knife_survival_bowie";
		std::string replace48 = "bowie";
		std::string search49 = "Knife_T";
		std::string replace49 = "knife";
		std::string search50 = "Knife";
		std::string replace50 = "knife";


		//weapons
		for (int i = weapon_name.find(search); i >= 0; i = weapon_name.find(search))
			weapon_name.replace(i, search.size(), replace);
		for (int i = weapon_name.find(search1); i >= 0; i = weapon_name.find(search1))
			weapon_name.replace(i, search1.size(), replace1);
		for (int i = weapon_name.find(search2); i >= 0; i = weapon_name.find(search2))
			weapon_name.replace(i, search2.size(), replace2);
		for (int i = weapon_name.find(search3); i >= 0; i = weapon_name.find(search3))
			weapon_name.replace(i, search3.size(), replace3);
		for (int i = weapon_name.find(search4); i >= 0; i = weapon_name.find(search4))
			weapon_name.replace(i, search4.size(), replace4);
		for (int i = weapon_name.find(search5); i >= 0; i = weapon_name.find(search5))
			weapon_name.replace(i, search5.size(), replace5);
		for (int i = weapon_name.find(search6); i >= 0; i = weapon_name.find(search6))
			weapon_name.replace(i, search6.size(), replace6);
		for (int i = weapon_name.find(search7); i >= 0; i = weapon_name.find(search7))
			weapon_name.replace(i, search7.size(), replace7);
		for (int i = weapon_name.find(search8); i >= 0; i = weapon_name.find(search8))
			weapon_name.replace(i, search8.size(), replace8);
		for (int i = weapon_name.find(search9); i >= 0; i = weapon_name.find(search9))
			weapon_name.replace(i, search9.size(), replace9);
		for (int i = weapon_name.find(search10); i >= 0; i = weapon_name.find(search10))
			weapon_name.replace(i, search10.size(), replace10);
		for (int i = weapon_name.find(search11); i >= 0; i = weapon_name.find(search11))
			weapon_name.replace(i, search11.size(), replace11);
		for (int i = weapon_name.find(search12); i >= 0; i = weapon_name.find(search12))
			weapon_name.replace(i, search12.size(), replace12);
		for (int i = weapon_name.find(search13); i >= 0; i = weapon_name.find(search13))
			weapon_name.replace(i, search13.size(), replace13);
		for (int i = weapon_name.find(search14); i >= 0; i = weapon_name.find(search14))
			weapon_name.replace(i, search14.size(), replace14);
		for (int i = weapon_name.find(search15); i >= 0; i = weapon_name.find(search15))
			weapon_name.replace(i, search15.size(), replace15);
		for (int i = weapon_name.find(search16); i >= 0; i = weapon_name.find(search16))
			weapon_name.replace(i, search16.size(), replace16);
		for (int i = weapon_name.find(search17); i >= 0; i = weapon_name.find(search17))
			weapon_name.replace(i, search17.size(), replace17);
		for (int i = weapon_name.find(search18); i >= 0; i = weapon_name.find(search18))
			weapon_name.replace(i, search18.size(), replace18);
		for (int i = weapon_name.find(search19); i >= 0; i = weapon_name.find(search19))
			weapon_name.replace(i, search19.size(), replace19);
		for (int i = weapon_name.find(search20); i >= 0; i = weapon_name.find(search20))
			weapon_name.replace(i, search20.size(), replace20);
		for (int i = weapon_name.find(search21); i >= 0; i = weapon_name.find(search21))
			weapon_name.replace(i, search21.size(), replace21);
		for (int i = weapon_name.find(search22); i >= 0; i = weapon_name.find(search22))
			weapon_name.replace(i, search22.size(), replace22);
		for (int i = weapon_name.find(search23); i >= 0; i = weapon_name.find(search23))
			weapon_name.replace(i, search23.size(), replace23);
		for (int i = weapon_name.find(search24); i >= 0; i = weapon_name.find(search24))
			weapon_name.replace(i, search24.size(), replace24);
		for (int i = weapon_name.find(search25); i >= 0; i = weapon_name.find(search25))
			weapon_name.replace(i, search25.size(), replace25);
		for (int i = weapon_name.find(search26); i >= 0; i = weapon_name.find(search26))
			weapon_name.replace(i, search26.size(), replace26);
		for (int i = weapon_name.find(search27); i >= 0; i = weapon_name.find(search27))
			weapon_name.replace(i, search27.size(), replace27);
		for (int i = weapon_name.find(search28); i >= 0; i = weapon_name.find(search28))
			weapon_name.replace(i, search28.size(), replace28);
		for (int i = weapon_name.find(search29); i >= 0; i = weapon_name.find(search29))
			weapon_name.replace(i, search29.size(), replace29);
		for (int i = weapon_name.find(search30); i >= 0; i = weapon_name.find(search30))
			weapon_name.replace(i, search30.size(), replace30);
		for (int i = weapon_name.find(search31); i >= 0; i = weapon_name.find(search31))
			weapon_name.replace(i, search31.size(), replace31);
		for (int i = weapon_name.find(search32); i >= 0; i = weapon_name.find(search32))
			weapon_name.replace(i, search32.size(), replace32);
		for (int i = weapon_name.find(search33); i >= 0; i = weapon_name.find(search33))
			weapon_name.replace(i, search33.size(), replace33);
		for (int i = weapon_name.find(search34); i >= 0; i = weapon_name.find(search34))
			weapon_name.replace(i, search34.size(), replace34);
		for (int i = weapon_name.find(search35); i >= 0; i = weapon_name.find(search35))
			weapon_name.replace(i, search35.size(), replace35);
		for (int i = weapon_name.find(search36); i >= 0; i = weapon_name.find(search36))
			weapon_name.replace(i, search36.size(), replace36);
		for (int i = weapon_name.find(search37); i >= 0; i = weapon_name.find(search37))
			weapon_name.replace(i, search37.size(), replace37);
		for (int i = weapon_name.find(search38); i >= 0; i = weapon_name.find(search38))
			weapon_name.replace(i, search38.size(), replace38);
		for (int i = weapon_name.find(search39); i >= 0; i = weapon_name.find(search39))
			weapon_name.replace(i, search39.size(), replace39);
		for (int i = weapon_name.find(search40); i >= 0; i = weapon_name.find(search40))
			weapon_name.replace(i, search40.size(), replace40);
		for (int i = weapon_name.find(search41); i >= 0; i = weapon_name.find(search41))
			weapon_name.replace(i, search41.size(), replace41);
		for (int i = weapon_name.find(search42); i >= 0; i = weapon_name.find(search42))
			weapon_name.replace(i, search42.size(), replace42);
		for (int i = weapon_name.find(search43); i >= 0; i = weapon_name.find(search43))
			weapon_name.replace(i, search43.size(), replace43);
		for (int i = weapon_name.find(search44); i >= 0; i = weapon_name.find(search44))
			weapon_name.replace(i, search44.size(), replace44);
		for (int i = weapon_name.find(search45); i >= 0; i = weapon_name.find(search45))
			weapon_name.replace(i, search45.size(), replace45);
		for (int i = weapon_name.find(search46); i >= 0; i = weapon_name.find(search46))
			weapon_name.replace(i, search46.size(), replace46);
		for (int i = weapon_name.find(search47); i >= 0; i = weapon_name.find(search47))
			weapon_name.replace(i, search47.size(), replace47);
		for (int i = weapon_name.find(search48); i >= 0; i = weapon_name.find(search48))
			weapon_name.replace(i, search48.size(), replace48);
		for (int i = weapon_name.find(search49); i >= 0; i = weapon_name.find(search49))
			weapon_name.replace(i, search49.size(), replace49);
		for (int i = weapon_name.find(search50); i >= 0; i = weapon_name.find(search50))
			weapon_name.replace(i, search50.size(), replace50);
		for (int i = weapon_name.find(search51); i >= 0; i = weapon_name.find(search51))
			weapon_name.replace(i, search51.size(), replace51);
		for (int i = weapon_name.find(search52); i >= 0; i = weapon_name.find(search52))
			weapon_name.replace(i, search52.size(), replace52);
		for (int i = weapon_name.find(search53); i >= 0; i = weapon_name.find(search53))
			weapon_name.replace(i, search53.size(), replace53);

		render.draw_text(bbox.x + (bbox.w / 2), bbox.h + bbox.y + 2, render.name_font, weapon_name.c_str(), true, color(red, green, blue, alpha[entity->index()]));

	}

	if (config_system.item.player_weapon_icon) {

		auto red = config_system.item.clr_weapon_icon[0] * 255;
		auto green = config_system.item.clr_weapon_icon[1] * 255;
		auto blue = config_system.item.clr_weapon_icon[2] * 255;
		auto weapon = entity->active_weapon();

		if (!weapon)
			return;

		auto weapon_data = weapon->get_weapon_data();

		auto item_definition_index = weapon->item_definition_index();

		if (!weapon_data)
			return;

		std::string weapon_icon = weapon_data->hud_name;
		weapon_icon.erase(0, 13);

		std::string search = "M4A1";
		std::string replace = "S";
		std::string search1 = "M4_SILENCER";
		std::string replace1 = "T";
		std::string search2 = "AK47";
		std::string replace2 = "W";
		std::string search3 = "Elites";
		std::string replace3 = "B";
		std::string search4 = "DesertEagle";
		std::string replace4 = "A";
		std::string search5 = "Aug";
		std::string replace5 = "U";
		std::string search6 = "AWP";
		std::string replace6 = "Z";
		std::string search7 = "Famas";
		std::string replace7 = "R";
		std::string search8 = "FiveSeven";
		std::string replace8 = "C";
		std::string search9 = "G3SG1";
		std::string replace9 = "X";
		std::string search10 = "GalilAR";
		std::string replace10 = "Q";
		std::string search11 = "Glock18";
		std::string replace11 = "D";
		std::string search12 = "M249";
		std::string replace12 = "g";
		std::string search13 = "MAC10";
		std::string replace13 = "K";
		std::string search14 = "P90";
		std::string replace14 = "P";
		std::string search15 = "UMP45";
		std::string replace15 = "L";
		std::string search16 = "xm1014";
		std::string replace16 = "b";
		std::string search17 = "Bizon";
		std::string replace17 = "M";
		std::string search18 = "Mag7";
		std::string replace18 = "d";
		std::string search19 = "Negev";
		std::string replace19 = "f";
		std::string search20 = "Sawedoff";
		std::string replace20 = "c";
		std::string search21 = "Tec9";
		std::string replace21 = "H";
		std::string search22 = "Taser";
		std::string replace22 = "h";
		std::string search23 = "HKP2000";
		std::string replace23 = "E";
		std::string search24 = "MP7";
		std::string replace24 = "N";
		std::string search25 = "Nova";
		std::string replace25 = "e";
		std::string search26 = "P250";
		std::string replace26 = "F";
		std::string search27 = "CZ75";
		std::string replace27 = "I";
		std::string search28 = "SCAR20";
		std::string replace28 = "Y";
		std::string search29 = "SG556";
		std::string replace29 = "V";
		std::string search30 = "SSG08";
		std::string replace30 = "a";
		std::string search31 = "REVOLVER";
		std::string replace31 = "J";
		std::string search51 = "USP_SILENCER";
		std::string replace51 = "G";
		std::string search52 = "MP9";
		std::string replace52 = "O";
		std::string search53 = "MP5SD";
		std::string replace53 = "N";
		//grenades
		std::string search32 = "HE_Grenade";
		std::string replace32 = "j";
		std::string search33 = "Smoke_Grenade";
		std::string replace33 = "k";
		std::string search34 = "C4";
		std::string replace34 = "o";
		std::string search35 = "MOLOTOV";
		std::string replace35 = "l";
		std::string search36 = "IncGrenade";
		std::string replace36 = "n";
		std::string search37 = "FLASHBANG";
		std::string replace37 = "i";
		std::string search38 = "DECOY";
		std::string replace38 = "m";
		//knifes
		std::string search39 = "KnifeBayonet";
		std::string replace39 = "1";
		std::string search40 = "KnifeFlip";
		std::string replace40 = "2";
		std::string search41 = "KnifeGut";
		std::string replace41 = "3";
		std::string search42 = "KnifeM9";
		std::string replace42 = "5";
		std::string search43 = "KnifeKaram";
		std::string replace43 = "4";
		std::string search44 = "KnifeTactical";
		std::string replace44 = "6";
		std::string search45 = "Knife_Butterfly";
		std::string replace45 = "8";
		std::string search46 = "knife_falchion_advanced";
		std::string replace46 = "0";
		std::string search47 = "knife_push";
		std::string replace47 = "9";
		std::string search48 = "knife_survival_bowie";
		std::string replace48 = "7";
		std::string search49 = "Knife_T";
		std::string replace49 = "[";
		std::string search50 = "Knife";
		std::string replace50 = "]";


		//weapons
		for (int i = weapon_icon.find(search); i >= 0; i = weapon_icon.find(search))
			weapon_icon.replace(i, search.size(), replace);
		for (int i = weapon_icon.find(search1); i >= 0; i = weapon_icon.find(search1))
			weapon_icon.replace(i, search1.size(), replace1);
		for (int i = weapon_icon.find(search2); i >= 0; i = weapon_icon.find(search2))
			weapon_icon.replace(i, search2.size(), replace2);
		for (int i = weapon_icon.find(search3); i >= 0; i = weapon_icon.find(search3))
			weapon_icon.replace(i, search3.size(), replace3);
		for (int i = weapon_icon.find(search4); i >= 0; i = weapon_icon.find(search4))
			weapon_icon.replace(i, search4.size(), replace4);
		for (int i = weapon_icon.find(search5); i >= 0; i = weapon_icon.find(search5))
			weapon_icon.replace(i, search5.size(), replace5);
		for (int i = weapon_icon.find(search6); i >= 0; i = weapon_icon.find(search6))
			weapon_icon.replace(i, search6.size(), replace6);
		for (int i = weapon_icon.find(search7); i >= 0; i = weapon_icon.find(search7))
			weapon_icon.replace(i, search7.size(), replace7);
		for (int i = weapon_icon.find(search8); i >= 0; i = weapon_icon.find(search8))
			weapon_icon.replace(i, search8.size(), replace8);
		for (int i = weapon_icon.find(search9); i >= 0; i = weapon_icon.find(search9))
			weapon_icon.replace(i, search9.size(), replace9);
		for (int i = weapon_icon.find(search10); i >= 0; i = weapon_icon.find(search10))
			weapon_icon.replace(i, search10.size(), replace10);
		for (int i = weapon_icon.find(search11); i >= 0; i = weapon_icon.find(search11))
			weapon_icon.replace(i, search11.size(), replace11);
		for (int i = weapon_icon.find(search12); i >= 0; i = weapon_icon.find(search12))
			weapon_icon.replace(i, search12.size(), replace12);
		for (int i = weapon_icon.find(search13); i >= 0; i = weapon_icon.find(search13))
			weapon_icon.replace(i, search13.size(), replace13);
		for (int i = weapon_icon.find(search14); i >= 0; i = weapon_icon.find(search14))
			weapon_icon.replace(i, search14.size(), replace14);
		for (int i = weapon_icon.find(search15); i >= 0; i = weapon_icon.find(search15))
			weapon_icon.replace(i, search15.size(), replace15);
		for (int i = weapon_icon.find(search16); i >= 0; i = weapon_icon.find(search16))
			weapon_icon.replace(i, search16.size(), replace16);
		for (int i = weapon_icon.find(search17); i >= 0; i = weapon_icon.find(search17))
			weapon_icon.replace(i, search17.size(), replace17);
		for (int i = weapon_icon.find(search18); i >= 0; i = weapon_icon.find(search18))
			weapon_icon.replace(i, search18.size(), replace18);
		for (int i = weapon_icon.find(search19); i >= 0; i = weapon_icon.find(search19))
			weapon_icon.replace(i, search19.size(), replace19);
		for (int i = weapon_icon.find(search20); i >= 0; i = weapon_icon.find(search20))
			weapon_icon.replace(i, search20.size(), replace20);
		for (int i = weapon_icon.find(search21); i >= 0; i = weapon_icon.find(search21))
			weapon_icon.replace(i, search21.size(), replace21);
		for (int i = weapon_icon.find(search22); i >= 0; i = weapon_icon.find(search22))
			weapon_icon.replace(i, search22.size(), replace22);
		for (int i = weapon_icon.find(search23); i >= 0; i = weapon_icon.find(search23))
			weapon_icon.replace(i, search23.size(), replace23);
		for (int i = weapon_icon.find(search24); i >= 0; i = weapon_icon.find(search24))
			weapon_icon.replace(i, search24.size(), replace24);
		for (int i = weapon_icon.find(search25); i >= 0; i = weapon_icon.find(search25))
			weapon_icon.replace(i, search25.size(), replace25);
		for (int i = weapon_icon.find(search26); i >= 0; i = weapon_icon.find(search26))
			weapon_icon.replace(i, search26.size(), replace26);
		for (int i = weapon_icon.find(search27); i >= 0; i = weapon_icon.find(search27))
			weapon_icon.replace(i, search27.size(), replace27);
		for (int i = weapon_icon.find(search28); i >= 0; i = weapon_icon.find(search28))
			weapon_icon.replace(i, search28.size(), replace28);
		for (int i = weapon_icon.find(search29); i >= 0; i = weapon_icon.find(search29))
			weapon_icon.replace(i, search29.size(), replace29);
		for (int i = weapon_icon.find(search30); i >= 0; i = weapon_icon.find(search30))
			weapon_icon.replace(i, search30.size(), replace30);
		for (int i = weapon_icon.find(search31); i >= 0; i = weapon_icon.find(search31))
			weapon_icon.replace(i, search31.size(), replace31);
		for (int i = weapon_icon.find(search32); i >= 0; i = weapon_icon.find(search32))
			weapon_icon.replace(i, search32.size(), replace32);
		for (int i = weapon_icon.find(search33); i >= 0; i = weapon_icon.find(search33))
			weapon_icon.replace(i, search33.size(), replace33);
		for (int i = weapon_icon.find(search34); i >= 0; i = weapon_icon.find(search34))
			weapon_icon.replace(i, search34.size(), replace34);
		for (int i = weapon_icon.find(search35); i >= 0; i = weapon_icon.find(search35))
			weapon_icon.replace(i, search35.size(), replace35);
		for (int i = weapon_icon.find(search36); i >= 0; i = weapon_icon.find(search36))
			weapon_icon.replace(i, search36.size(), replace36);
		for (int i = weapon_icon.find(search37); i >= 0; i = weapon_icon.find(search37))
			weapon_icon.replace(i, search37.size(), replace37);
		for (int i = weapon_icon.find(search38); i >= 0; i = weapon_icon.find(search38))
			weapon_icon.replace(i, search38.size(), replace38);
		for (int i = weapon_icon.find(search39); i >= 0; i = weapon_icon.find(search39))
			weapon_icon.replace(i, search39.size(), replace39);
		for (int i = weapon_icon.find(search40); i >= 0; i = weapon_icon.find(search40))
			weapon_icon.replace(i, search40.size(), replace40);
		for (int i = weapon_icon.find(search41); i >= 0; i = weapon_icon.find(search41))
			weapon_icon.replace(i, search41.size(), replace41);
		for (int i = weapon_icon.find(search42); i >= 0; i = weapon_icon.find(search42))
			weapon_icon.replace(i, search42.size(), replace42);
		for (int i = weapon_icon.find(search43); i >= 0; i = weapon_icon.find(search43))
			weapon_icon.replace(i, search43.size(), replace43);
		for (int i = weapon_icon.find(search44); i >= 0; i = weapon_icon.find(search44))
			weapon_icon.replace(i, search44.size(), replace44);
		for (int i = weapon_icon.find(search45); i >= 0; i = weapon_icon.find(search45))
			weapon_icon.replace(i, search45.size(), replace45);
		for (int i = weapon_icon.find(search46); i >= 0; i = weapon_icon.find(search46))
			weapon_icon.replace(i, search46.size(), replace46);
		for (int i = weapon_icon.find(search47); i >= 0; i = weapon_icon.find(search47))
			weapon_icon.replace(i, search47.size(), replace47);
		for (int i = weapon_icon.find(search48); i >= 0; i = weapon_icon.find(search48))
			weapon_icon.replace(i, search48.size(), replace48);
		for (int i = weapon_icon.find(search49); i >= 0; i = weapon_icon.find(search49))
			weapon_icon.replace(i, search49.size(), replace49);
		for (int i = weapon_icon.find(search50); i >= 0; i = weapon_icon.find(search50))
			weapon_icon.replace(i, search50.size(), replace50);
		for (int i = weapon_icon.find(search51); i >= 0; i = weapon_icon.find(search51))
			weapon_icon.replace(i, search51.size(), replace51);
		for (int i = weapon_icon.find(search52); i >= 0; i = weapon_icon.find(search52))
			weapon_icon.replace(i, search52.size(), replace52);
		for (int i = weapon_icon.find(search53); i >= 0; i = weapon_icon.find(search53))
			weapon_icon.replace(i, search53.size(), replace53);

		render.draw_text(bbox.x + (bbox.w / 2), bbox.h + bbox.y + 2 +9, render.icon_font, weapon_icon.c_str(), true, color(red, green, blue, alpha[entity->index()]));
	}
}

void c_visuals::dropped_weapons(player_t* entity) noexcept {
	auto class_id = entity->client_class()->class_id;
	auto model_name = interfaces::model_info->get_model_name(entity->model());
	auto weapon = entity;

	if (!entity)
		return;

	if (!weapon)
		return;

	vec3_t dropped_weapon_position, dropped_weapon_origin;

	dropped_weapon_origin = weapon->origin();

	if (!math.world_to_screen(dropped_weapon_origin, dropped_weapon_position))
		return;

	if (!(entity->origin().x == 0 && entity->origin().y == 0 && entity->origin().z == 0)) { //ghetto fix sorry - designer
		if (config_system.item.dropped_weapons) {
			
			if (class_id == class_ids::cc4)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "c4", true, color(255, 127, 36));

			if (strstr(model_name, "w_defuser"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "kit", true, color(30, 144, 255));
		}

		if (config_system.item.dropped_weapons && !strstr(model_name, "models/weapons/w_eq_")
			&& !strstr(model_name, "models/weapons/w_ied"))
		{
			if (strstr(model_name, "models/weapons/w_") && strstr(model_name, "_dropped.mdl"))
			{
				std::string WeaponName = model_name + 17;

				WeaponName[WeaponName.size() - 12] = '\0';

				if (strstr(model_name, "models/weapons/w_rif") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName.erase(0, 4);
				}
				else if (strstr(model_name, "models/weapons/w_pist") && strstr(model_name, "_dropped.mdl") && !strstr(model_name, "models/weapons/w_pist_223"))
				{
					WeaponName.erase(0, 5);
				}
				else if (strstr(model_name, "models/weapons/w_smg") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName.erase(0, 4);
				}
				else if (strstr(model_name, "models/weapons/w_mach") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName.erase(0, 5);
				}
				else if (strstr(model_name, "models/weapons/w_shot") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName.erase(0, 5);
				}
				else if (strstr(model_name, "models/weapons/w_snip") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName.erase(0, 5);
				}
				if (strstr(model_name, "models/weapons/w_pist_223") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "usp_s";
				}
				if (strstr(model_name, "models/weapons/w_pist_hkp2000") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "p2000";
				}
				if (strstr(model_name, "models/weapons/w_pist_cz_75") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "cz75";
				}
				if (strstr(model_name, "models/weapons/w_rif_m4a1") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "m4a4";
				}
				if (strstr(model_name, "models/weapons/w_rif_m4a1_s") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "m4a1_s";
				}
				if (strstr(model_name, "models/weapons/w_rif_sg556") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "sg553";
				}
				if (strstr(model_name, "models/weapons/w_smg_mp5sd") && strstr(model_name, "_dropped.mdl"))
				{
					WeaponName = "mp5";
				}

				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, WeaponName.c_str(), true, color(255, 255, 255));

			}
		}

		if (config_system.item.danger_zone_dropped) { 	//no need to create separate func for danger zone shit - designer (also use switch instead of else if)
			if (strstr(model_name, "case_pistol"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "pistol case", true, color(255, 152, 56));

			else if (strstr(model_name, "case_light_weapon"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "light case", true, color(255, 152, 56));

			else if (strstr(model_name, "case_heavy_weapon"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "heavy case", true, color(255, 152, 56));

			else if (strstr(model_name, "case_explosive"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "explosive case", true, color(255, 152, 56));

			else if (strstr(model_name, "case_tools"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "tools case", true, color(255, 152, 56));

			else if (strstr(model_name, "random"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "airdrop", true, color(255, 255, 255));

			else if (strstr(model_name, "dz_armor_helmet"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "armor, helmet", true, color(66, 89, 244));

			else if (strstr(model_name, "dz_helmet"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "helmet", true, color(66, 89, 244));

			else if (strstr(model_name, "dz_armor"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "armor", true, color(66, 89, 244));

			else if (strstr(model_name, "upgrade_tablet"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "tablet upgrade", true, color(255, 255, 255));

			else if (strstr(model_name, "briefcase"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "briefcase", true, color(255, 255, 255));

			else if (strstr(model_name, "parachutepack"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "parachutepack", true, color(255, 255, 255));

			else if (strstr(model_name, "dufflebag"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "cash bag", true, color(120, 180, 10));

			else if (strstr(model_name, "ammobox"))
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "ammobox", true, color(158, 48, 255));

			else if (class_id == class_ids::cdrone)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "drone", true, color(255, 255, 255));

			else if (class_id == class_ids::cphyspropradarjammer)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "radar jammer", true, color(255, 255, 255));

			else if (class_id == class_ids::cdronegun)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "sentry turret", true, color(255, 30, 30));

			else if (class_id == class_ids::cknife)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "knife", true, color(255, 255, 255));

			else if (class_id == class_ids::cmelee)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "melee", true, color(255, 255, 255));

			else if (class_id == class_ids::citemcash)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "cash", true, color(120, 180, 10));

			else if (class_id == class_ids::citem_healthshot)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "health shot", true, color(66, 89, 244));

			else if (class_id == class_ids::ctablet)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "tablet", true, color(255, 255, 255));

			else if (class_id == class_ids::chostage)
				render.draw_text(dropped_weapon_position.x, dropped_weapon_position.y, render.name_font, "hostage", true, color(255, 255, 255));
		}
	}
}

void c_visuals::projectiles(player_t* entity) noexcept {
	if (!config_system.item.projectiles)
		return;

	if (!entity)
		return;

	auto client_class = entity->client_class();
	auto model = entity->model();

	if (!model)
		return;

	if (model) {
		vec3_t grenade_position, grenade_origin;

		auto model = interfaces::model_info->get_studio_model(entity->model());

		if (!model || !strstr(model->name_char_array, "thrown") && !strstr(model->name_char_array, "dropped"))
			return;

		std::string name = model->name_char_array;
		grenade_origin = entity->origin();

		if (!math.world_to_screen(grenade_origin, grenade_position))
			return;

		if (name.find("flashbang") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "flash", true, color(255, 255, 10));
		}

		else if (name.find("smokegrenade") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "smoke", true, color(0, 55, 255));

			auto time = interfaces::globals->interval_per_tick * (interfaces::globals->tick_count - entity->smoke_grenade_tick_begin());

			if (!(18 - time < 0)) {
				render.draw_filled_rect(grenade_position.x - 18, grenade_position.y + 13, 36, 3, color(10, 10, 10, 180));
				render.draw_filled_rect(grenade_position.x - 18, grenade_position.y + 13, time * 2, 3, color(167, 24, 71, 255));
			}
		}

		else if (name.find("incendiarygrenade") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "fire_inc", true, color(255, 0, 255));
		}

		else if (name.find("molotov") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "fire_molo", true, color(255, 0, 255));
		}

		else if (name.find("fraggrenade") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "grenade", true, color(255, 0, 0));
		}

		else if (name.find("decoy") != std::string::npos) {
			render.draw_text(grenade_position.x, grenade_position.y, render.name_font, "decoy", true, color(255, 255, 255));
		}
	}
}

void c_visuals::bomb_esp(player_t* entity) noexcept {

	if (!config_system.item.bomb_planted)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto explode_time = entity->c4_blow_time();

	auto remaining_time = explode_time - (interfaces::globals->interval_per_tick * local_player->get_tick_base());
	if (remaining_time < 0)
		return;
	if (remaining_time == 0)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);

	vec3_t bomb_origin, bomb_position;

	bomb_origin = entity->origin();

	explode_time -= interfaces::globals->interval_per_tick * local_player->get_tick_base();
	if (explode_time <= 0)
		explode_time = 0;

	char buffer[64];
	sprintf_s(buffer, "%.1f", explode_time);

	auto c4_timer = interfaces::console->get_convar("mp_c4timer")->get_int();
	auto value = (explode_time * height) / c4_timer;

	//bomb damage indicator calculations, credits casual_hacker
	float damage;
	auto distance = local_player->get_eye_pos().distance_to(entity->get_eye_pos());
	auto a = 450.7f;
	auto b = 75.68f;
	auto c = 789.2f;
	auto d = ((distance - b) / c);
	auto fl_damage = a * exp(-d * d);
	damage = float((std::max)((int)ceilf(utilities::csgo_armor(fl_damage, local_player->armor())), 0));

	//convert damage to string
	std::string damage_text;
	damage_text += "-";
	damage_text += std::to_string((int)(damage));
	damage_text += "HP";


	//render on screen bomb bar
	if (explode_time <= 10) {
		render.draw_filled_rect(0, 0, 10, value, color(255, 0, 0, 180));
	}
	else {
		render.draw_filled_rect(0, 0, 10, value, color(0, 255, 0, 180));
	}

	player_t* bomb = nullptr;
	for (int i = 1; i < interfaces::entity_list->get_highest_index(); i++) {
	
		if (entity->client_class()->class_id == class_ids::cplantedc4)
		{
			bomb = (player_t*)entity;
			break;
		}
	}
	
	//render bomb position
	render.draw_text(12, value - 11, render.name_font, bomb->get_callout(), false, color(0, 255, 0));

	//render bomb timer
	render.draw_text(12, value - 21, render.name_font, buffer, false, color(255, 255, 255));

	//render bomb damage
	if (local_player->is_alive()) {
		render.draw_text(12, value - 31, render.name_font, damage_text, false, color(255, 255, 255));
	}

	//render fatal check
	if (local_player->is_alive() && damage >= local_player->health()) {
		render.draw_text(12, value - 41, render.name_font, "FATAL", false, color(255, 0, 0));
	}

	if (!math.world_to_screen(bomb_origin, bomb_position))
		return;

	//render classic world timer + bar
	render.draw_text(bomb_position.x, bomb_position.y, render.name_font, buffer, true, color(255, 255, 255));
	render.draw_text(bomb_position.x, bomb_position.y + 30, render.name_font, bomb->get_callout(), true, color(0,255, 0));
	render.draw_filled_rect(bomb_position.x - c4_timer / 2, bomb_position.y + 13, c4_timer, 3, color(10, 10, 10, 180)); //c4_timer / 2 so it always will be centered
	render.draw_filled_rect(bomb_position.x - c4_timer / 2, bomb_position.y + 13, explode_time, 3, color(167, 24, 71, 255));
}

void c_visuals::bomb_defuse_esp(player_t* entity) noexcept {
	
	if (!config_system.item.bomb_planted)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	int width, height;
	interfaces::engine->get_screen_size(width, height);

	auto explode_time = entity->c4_blow_time();
	auto remaining_time = explode_time - (interfaces::globals->interval_per_tick * local_player->get_tick_base());
	auto countdown = entity->get_c4_defuse_countdown() - (local_player->get_tick_base() * interfaces::globals->interval_per_tick);

	char defuse_time_string[24];
	sprintf_s(defuse_time_string, sizeof(defuse_time_string) - 1, "%.1f", countdown);
	auto defvalue = (countdown * height) / remaining_time;

	vec3_t bomb_origin, bomb_position;
	bomb_origin = entity->origin();

	if (entity->c4_gets_defused() > 0) {
		// on srcreen		
		if (countdown > 0.01f) {
			if (remaining_time > countdown) {
				//loading leftside		
				render.draw_filled_rect(10, 0, 10, defvalue, color(0, 191, 255, 180));
				render.draw_text(12, defvalue - 11, render.name_font, defuse_time_string, false, color(0, 191, 255));
			}
			else {
				//loading leftside no time
				render.draw_filled_rect(10, 0, 10, defvalue, color(255, 0, 0, 180));
				render.draw_text(12, defvalue - 11, render.name_font, "NO TIME", false, color(255, 0, 0));
			}
		}
	}	
	if (entity->c4_gets_defused() > 0) { 
		// on bomb	
		if (!math.world_to_screen(bomb_origin, bomb_position))
			return;
		if (countdown > 0.01f){
			if (remaining_time > countdown){
				//render classic world timer + bar			
				render.draw_text(bomb_position.x, bomb_position.y+15, render.name_font, defuse_time_string, true, color(0,50,255));
				render.draw_filled_rect(bomb_position.x - remaining_time / 3, bomb_position.y + 13+ 15, remaining_time, 3, color(10, 10, 10, 180)); //c4_timer / 2 so it always will be centered
				render.draw_filled_rect(bomb_position.x - remaining_time / 3, bomb_position.y + 13+ 15, countdown, 3, color(0, 50, 255,255));
			}
			else {
				render.draw_text(bomb_position.x , bomb_position.y + 15, render.name_font, "NO TIME", true, color(255, 0, 0));
			}
		}
	}

}

void c_visuals::chams() noexcept {
	if (!config_system.item.visuals_enabled || (!config_system.item.vis_chams_vis && !config_system.item.vis_chams_invis))
		return;

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

		if (!entity || !entity->is_alive() || entity->dormant() || !local_player)
			continue;

		bool is_teammate = entity->team() == local_player->team();
		bool is_enemy = entity->team() != local_player->team();
	
		static i_material* mat = nullptr;
		auto textured = interfaces::material_system->find_material("aristois_material", TEXTURE_GROUP_MODEL, true, nullptr);
		auto metalic = interfaces::material_system->find_material("aristois_reflective", TEXTURE_GROUP_MODEL, true, nullptr);
		auto flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true, nullptr);
		auto dogtag = interfaces::material_system->find_material("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL, true, nullptr);
		textured->increment_reference_count();
		metalic->increment_reference_count();
		flat->increment_reference_count();
		dogtag->increment_reference_count();

		switch (config_system.item.vis_chams_type) {
		case 0:
			mat = textured;
			break;
		case 1:
			mat = flat;
			break;
		case 2:
			mat = metalic;
			break;
		case 3:
			mat = dogtag;
			break;
		}

		if (is_enemy) {
			if (config_system.item.vis_chams_invis) {
				if (utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)) && config_system.item.vis_chams_smoke_check)
					return;
				interfaces::render_view->modulate_color(config_system.item.clr_chams_invis);
				interfaces::render_view->set_blend(config_system.item.clr_chams_invis[3]);
				mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, true);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
			if (config_system.item.vis_chams_vis) {
				if (utilities::is_behind_smoke(local_player->get_eye_pos(), entity->get_hitbox_position(entity, hitbox_head)) && config_system.item.vis_chams_smoke_check)
					return;

				interfaces::render_view->modulate_color(config_system.item.clr_chams_vis);
				interfaces::render_view->set_blend(config_system.item.clr_chams_vis[3]);
				mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
		}

		if (is_teammate) {
			if (config_system.item.vis_chams_invis_teammate) {
				interfaces::render_view->modulate_color(config_system.item.clr_chams_invis_teammate);
				interfaces::render_view->set_blend(config_system.item.clr_chams_invis_teammate[3]);
				mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, true);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
			if (config_system.item.vis_chams_vis_teammate) {
				interfaces::render_view->modulate_color(config_system.item.clr_chams_vis_teammate);
				interfaces::render_view->set_blend(config_system.item.clr_chams_vis_teammate[3]);
				mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);

				interfaces::model_render->override_material(mat);
				entity->draw_model(1, 255);
			}
		}
		interfaces::model_render->override_material(nullptr);
	}
}

void c_visuals::chams_misc(const model_render_info_t& info) noexcept {

	auto model_name = interfaces::model_info->get_model_name((model_t*)info.model);
	static i_material* mat = nullptr;
	auto textured = interfaces::material_system->find_material("aristois_material", TEXTURE_GROUP_MODEL, true, nullptr);
	auto metalic = interfaces::material_system->find_material("aristois_reflective", TEXTURE_GROUP_MODEL, true, nullptr);
	auto flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL, true, nullptr);
	auto dogtag = interfaces::material_system->find_material("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL, true, nullptr);
	textured->increment_reference_count();
	metalic->increment_reference_count();
	flat->increment_reference_count();
	dogtag->increment_reference_count();

	switch (config_system.item.vis_chams_type) {
	case 0:
		mat = textured;
		break;
	case 1:
		mat = flat;
		break;
	case 2:
		mat = metalic;
		break;
	case 3:
		mat = dogtag;
		break;
	}

	if (config_system.item.weapon_chams && strstr(model_name, "models/weapons/v_")) {

		interfaces::render_view->set_blend(config_system.item.clr_weapon_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_weapon_chams);
		mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);
		interfaces::model_render->override_material(mat);
	}
	if (config_system.item.hand_chams && strstr(model_name, "arms"))
	{
		interfaces::render_view->set_blend(config_system.item.clr_hand_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_hand_chams);
		mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);
		interfaces::model_render->override_material(mat);
	}

	if (config_system.item.sleeve_chams && strstr(model_name, "sleeve")) {

		interfaces::render_view->set_blend(config_system.item.clr_sleeve_chams[3]);
		interfaces::render_view->modulate_color(config_system.item.clr_sleeve_chams);
		mat->set_material_var_flag(MATERIAL_VAR_IGNOREZ, false);
		interfaces::model_render->override_material(mat);
	}
}

void c_visuals::glow() noexcept {
	if (!config_system.item.visuals_enabled || !config_system.item.visuals_glow)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	for (size_t i = 0; i < interfaces::glow_manager->size; i++) {
		auto &glow = interfaces::glow_manager->objects[i];

		if (glow.unused())
			continue;

		auto glow_entity = reinterpret_cast<player_t*>(glow.entity);
		auto client_class = glow_entity->client_class();

		if (!glow_entity || glow_entity->dormant())
			continue;

		auto is_enemy = glow_entity->team() != local_player->team();
		auto is_teammate = glow_entity->team() == local_player->team();

		switch (client_class->class_id) {
		case class_ids::ccsplayer:
			if (is_enemy && config_system.item.visuals_glow_enemy) {
				glow.set(config_system.item.clr_glow[0], config_system.item.clr_glow[1], config_system.item.clr_glow[2], config_system.item.clr_glow[3]);
			}

			else if (is_teammate && config_system.item.visuals_glow_team) {
				glow.set(config_system.item.clr_glow_team[0], config_system.item.clr_glow_team[1], config_system.item.clr_glow_team[2], config_system.item.clr_glow_team[3]);
			}
			break;
		case class_ids::cplantedc4:
			if (config_system.item.visuals_glow_planted) {
				glow.set(config_system.item.clr_glow_planted[0], config_system.item.clr_glow_planted[1], config_system.item.clr_glow_planted[2], config_system.item.clr_glow_planted[3]);
			}
			break;
		}	

		if (strstr(client_class->network_name, ("CWeapon")) && config_system.item.visuals_glow_weapons) {
			glow.set(config_system.item.clr_glow_dropped[0], config_system.item.clr_glow_dropped[1], config_system.item.clr_glow_dropped[2], config_system.item.clr_glow_dropped[3]);
		}

		else if (client_class->class_id == class_ids::cak47 && config_system.item.visuals_glow_weapons) {
			glow.set(config_system.item.clr_glow_dropped[0], config_system.item.clr_glow_dropped[1], config_system.item.clr_glow_dropped[2], config_system.item.clr_glow_dropped[3]);
		}

		else if (client_class->class_id == class_ids::cc4 && config_system.item.visuals_glow_weapons) {
			glow.set(config_system.item.clr_glow_dropped[0], config_system.item.clr_glow_dropped[1], config_system.item.clr_glow_dropped[2], config_system.item.clr_glow_dropped[3]);
		}

		else if (client_class->class_id == class_ids::cdeagle && config_system.item.visuals_glow_weapons) {
			glow.set(config_system.item.clr_glow_dropped[0], config_system.item.clr_glow_dropped[1], config_system.item.clr_glow_dropped[2], config_system.item.clr_glow_dropped[3]);
		}
		
		// grendes
		else if (client_class->class_id == class_ids::chegrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cmolotovgrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cmolotovprojectile && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cincendiarygrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cflashbang && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cdecoygrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cdecoyprojectile && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cbasecsgrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cbasecsgrenadeprojectile && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::cbasegrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::csmokegrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::csmokegrenadeprojectile && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}
		else if (client_class->class_id == class_ids::particlesmokegrenade && config_system.item.visuals_glow_nades) {
			glow.set(config_system.item.clr_glow_dropped_nade[0], config_system.item.clr_glow_dropped_nade[1], config_system.item.clr_glow_dropped_nade[2], config_system.item.clr_glow_dropped_nade[3]);
		}

	}
}

void c_visuals::skeleton(player_t* entity) noexcept {
	if (!config_system.item.skeleton)
		return;

	auto p_studio_hdr = interfaces::model_info->get_studio_model(entity->model());

	if (!p_studio_hdr)
		return;

	vec3_t v_parent, v_child, s_parent, s_child;

	for (int i = 0; i < p_studio_hdr->bones_count; i++) {
		studio_bone_t* bone = p_studio_hdr->bone(i);

		if (!bone)
			return;

		if (bone && (bone->flags & BONE_USED_BY_HITBOX) && (bone->parent != -1)) {
			v_child = entity->get_bone_position(i);
			v_parent = entity->get_bone_position(bone->parent);

			if (math.world_to_screen(v_parent, s_parent) && math.world_to_screen(v_child, s_child))
				render.draw_line(s_parent[0], s_parent[1], s_child[0], s_child[1], color(255, 255, 255, alpha[entity->index()]));
		}
	}
}

void c_visuals::backtrack_skeleton(player_t* entity) noexcept {  //normal skeleton need records
	if (!config_system.item.backtrack_skeleton)
		return;

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto p_studio_hdr = interfaces::model_info->get_studio_model(entity->model());
	if (!p_studio_hdr)
		return;

	if (p_studio_hdr) {

		matrix_t matrix[128];

		if (entity->setup_bones(matrix, 128, 0x7FF00, interfaces::globals->cur_time))
		{
			for (int i = 0; i < p_studio_hdr->bones_count; i++){

				studio_bone_t* bone = p_studio_hdr->bone(i);
				if (!bone || !(bone->flags & 256) || bone->parent == -1)
					continue;

				vec3_t vBonePos1;
				if (!math.world_to_screen(vec3_t(matrix[i][0][3], matrix[i][1][3],matrix[i][2][3]), vBonePos1))
					continue;

				vec3_t vBonePos2;
				if (!math.world_to_screen(vec3_t(matrix[bone->parent][0][3],matrix[bone->parent][1][3], matrix[bone->parent][2][3]), vBonePos2))
					continue;

				render.draw_line((int)vBonePos1.x, (int)vBonePos1.y, (int)vBonePos2.x, (int)vBonePos2.y, color(255, 55, 255));
			}
		}
	}
}

void c_visuals::backtrack_chams(IMatRenderContext* ctx, const draw_model_state_t& state, const model_render_info_t& info) {
	if (!config_system.item.backtrack_visualize)
		return;

	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto model_name = interfaces::model_info->get_model_name((model_t*)info.model);

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
	if (!entity)
		return;

	static auto draw_model_execute_fn = reinterpret_cast<hooks::draw_model_execute_fn>(hooks::modelrender_hook->get_original(21));

	if (strstr(model_name, "models/player")) {
		if (entity && entity->is_alive() && !entity->dormant()) {
			int i = entity->index();

			if (local_player && local_player->is_alive() && entity->team() != local_player->team()) {
				auto record = &records[info.entity_index];
				if (!record)
					return;
				
				if (record && record->size() && backtrack.valid_tick(record->front().simulation_time)) {
					vec3_t previous_screenpos;
					vec3_t screen_pos;
					render.draw_line(screen_pos.x, screen_pos.y, previous_screenpos.x, previous_screenpos.y, color(255, 255, 255, 255));
					draw_model_execute_fn(interfaces::model_render, ctx, state, info, record->back().matrix);
					interfaces::model_render->override_material(nullptr);
				}	
			}
		}
	}
}

void c_visuals::viewmodel_modulate(const model_render_info_t& info) {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;

	auto model_name = interfaces::model_info->get_model_name((model_t*)info.model);

	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	if (!local_player->is_alive())
		return;

	if (strstr(model_name, "sleeve")) {
		if (config_system.item.remove_sleeves) {
			interfaces::render_view->set_blend(0.f);
		}
	}

	if (strstr(model_name, "arms")) {
		if (config_system.item.remove_hands) {
			interfaces::render_view->set_blend(0.f);
		}
	}
}








































































































