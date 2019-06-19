#include "skinchanger.hpp"
#define INVALID_EHANDLE_INDEX 0xFFFFFFFF

c_skinchanger skin_changer;

bool c_skinchanger::apply_knife_model(attributable_item_t* weapon, const char* model) noexcept {		
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return false;

	auto viewmodel = reinterpret_cast<base_view_model*>(interfaces::entity_list->get_client_entity_handle(local_player->view_model()));
	if (!viewmodel)
		return false;

	auto h_view_model_weapon = viewmodel->m_hweapon();
	if (!h_view_model_weapon)
		return false;

	auto view_model_weapon = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(h_view_model_weapon));
	if (view_model_weapon != weapon)
		return false;

	viewmodel->model_index() = interfaces::model_info->get_model_index(model);

	return true;
}

bool c_skinchanger::apply_knife_skin(attributable_item_t* weapon, int item_definition_index, int paint_kit, int model_index, int entity_quality, float fallback_wear) noexcept {
	weapon->item_definition_index() = item_definition_index;
	weapon->fallback_paint_kit() = paint_kit;
	weapon->model_index() = model_index;
	weapon->entity_quality() = entity_quality;
	weapon->fallback_wear() = fallback_wear;

	return true;
}

void c_skinchanger::run() noexcept {
	if (!interfaces::engine->is_connected() && !interfaces::engine->is_in_game())
		return;
	
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	if (!local_player)
		return;

	if (!config_system.item.skinchanger_enable) {
		return;
	}

	// skinchanger
	if (config_system.item.skinchanger_enable) {
			
		auto model_bayonet = "models/weapons/v_knife_bayonet.mdl";
		auto model_m9 = "models/weapons/v_knife_m9_bay.mdl";
		auto model_karambit = "models/weapons/v_knife_karam.mdl";
		auto model_bowie = "models/weapons/v_knife_survival_bowie.mdl";
		auto model_butterfly = "models/weapons/v_knife_butterfly.mdl";
		auto model_falchion = "models/weapons/v_knife_falchion_advanced.mdl";
		auto model_flip = "models/weapons/v_knife_flip.mdl";
		auto model_gut = "models/weapons/v_knife_gut.mdl";
		auto model_huntsman = "models/weapons/v_knife_tactical.mdl";
		auto model_shadow_daggers = "models/weapons/v_knife_push.mdl";
		auto model_navaja = "models/weapons/v_knife_gypsy_jackknife.mdl";
		auto model_stiletto = "models/weapons/v_knife_stiletto.mdl";
		auto model_talon = "models/weapons/v_knife_widowmaker.mdl";
		auto model_ursus = "models/weapons/v_knife_ursus.mdl";

		auto index_bayonet = interfaces::model_info->get_model_index("models/weapons/v_knife_bayonet.mdl");
		auto index_m9 = interfaces::model_info->get_model_index("models/weapons/v_knife_m9_bay.mdl");
		auto index_karambit = interfaces::model_info->get_model_index("models/weapons/v_knife_karam.mdl");
		auto index_bowie = interfaces::model_info->get_model_index("models/weapons/v_knife_survival_bowie.mdl");
		auto index_butterfly = interfaces::model_info->get_model_index("models/weapons/v_knife_butterfly.mdl");
		auto index_falchion = interfaces::model_info->get_model_index("models/weapons/v_knife_falchion_advanced.mdl");
		auto index_flip = interfaces::model_info->get_model_index("models/weapons/v_knife_flip.mdl");
		auto index_gut = interfaces::model_info->get_model_index("models/weapons/v_knife_gut.mdl");
		auto index_huntsman = interfaces::model_info->get_model_index("models/weapons/v_knife_tactical.mdl");
		auto index_shadow_daggers = interfaces::model_info->get_model_index("models/weapons/v_knife_push.mdl");
		auto index_navaja = interfaces::model_info->get_model_index("models/weapons/v_knife_gypsy_jackknife.mdl");
		auto index_stiletto = interfaces::model_info->get_model_index("models/weapons/v_knife_stiletto.mdl");
		auto index_talon = interfaces::model_info->get_model_index("models/weapons/v_knife_widowmaker.mdl");
		auto index_ursus = interfaces::model_info->get_model_index("models/weapons/v_knife_ursus.mdl");
		
		auto active_weapon = local_player->active_weapon();
		if (!active_weapon)
			return;

		auto my_weapons = local_player->weapons();
		for (size_t i = 0; my_weapons[i] != INVALID_EHANDLE_INDEX; i++) {
			auto weapon = reinterpret_cast<attributable_item_t*>(interfaces::entity_list->get_client_entity_handle(my_weapons[i]));

			if (!weapon)
				return;

			//knife conditions
			float wear = 0.f;
			switch (config_system.item.knife_wear) {
			case 0:
				wear = 0.0000001f;
				break;
			case 1:
				wear = 0.07f;
				break;
			case 2:
				wear = 0.15f;
				break;
			case 3:
				wear = 0.38f;
				break;
			case 4:
				wear = 0.45f;
				break;
			}

			//apply knife model
			if (active_weapon->client_class()->class_id == class_ids::cknife) {
				switch (config_system.item.knife_model) {
				case 0:
					break;
				case 1:
					apply_knife_model(weapon, model_bayonet);
					break;
				case 2:
					apply_knife_model(weapon, model_m9);
					break;
				case 3:
					apply_knife_model(weapon, model_karambit);
					break;
				case 4:
					apply_knife_model(weapon, model_bowie);
					break;
				case 5:
					apply_knife_model(weapon, model_butterfly);
					break;
				case 6:
					apply_knife_model(weapon, model_falchion);
					break;
				case 7:
					apply_knife_model(weapon, model_flip);
					break;
				case 8:
					apply_knife_model(weapon, model_gut);
					break;
				case 9:
					apply_knife_model(weapon, model_huntsman);
					break;
				case 10:
					apply_knife_model(weapon, model_shadow_daggers);
					break;
				case 11:
					apply_knife_model(weapon, model_navaja);
					break;
				case 12:
					apply_knife_model(weapon, model_stiletto);
					break;
				case 13:
					apply_knife_model(weapon, model_talon);
					break;
				case 14:
					apply_knife_model(weapon, model_ursus);
					break;
				}
			}

			//apply knife skins
			if (weapon->client_class()->class_id == class_ids::cknife) {
				switch (config_system.item.knife_model) {
				case 0:
					break;
				case 1:
					apply_knife_skin(weapon, WEAPON_BAYONET, config_system.item.paint_kit_index_knife, index_bayonet, 3, wear);
					break;
				case 2:
					apply_knife_skin(weapon, WEAPON_KNIFE_M9_BAYONET, config_system.item.paint_kit_index_knife, index_m9, 3, wear);
					break;
				case 3:
					apply_knife_skin(weapon, WEAPON_KNIFE_KARAMBIT, config_system.item.paint_kit_index_knife, index_karambit, 3, wear);
					break;
				case 4:
					apply_knife_skin(weapon, WEAPON_KNIFE_SURVIVAL_BOWIE, config_system.item.paint_kit_index_knife, index_bowie, 3, wear);
					break;
				case 5:
					apply_knife_skin(weapon, WEAPON_KNIFE_BUTTERFLY, config_system.item.paint_kit_index_knife, index_butterfly, 3, wear);
					break;
				case 6:
					apply_knife_skin(weapon, WEAPON_KNIFE_FALCHION, config_system.item.paint_kit_index_knife, index_falchion, 3, wear);
					break;
				case 7:
					apply_knife_skin(weapon, WEAPON_KNIFE_FLIP, config_system.item.paint_kit_index_knife, index_flip, 3, wear);
					break;
				case 8:
					apply_knife_skin(weapon, WEAPON_KNIFE_GUT, config_system.item.paint_kit_index_knife, index_gut, 3, wear);
					break;
				case 9:
					apply_knife_skin(weapon, WEAPON_KNIFE_TACTICAL, config_system.item.paint_kit_index_knife, index_huntsman, 3, wear);
					break;
				case 10:
					apply_knife_skin(weapon, WEAPON_KNIFE_PUSH, config_system.item.paint_kit_index_knife, index_shadow_daggers, 3, wear);
					break;
				case 11:
					apply_knife_skin(weapon, WEAPON_KNIFE_GYPSY_JACKKNIFE, config_system.item.paint_kit_index_knife, index_navaja, 3, wear);
					break;
				case 12:
					apply_knife_skin(weapon, WEAPON_KNIFE_STILETTO, config_system.item.paint_kit_index_knife, index_stiletto, 3, wear);
					break;
				case 13:
					apply_knife_skin(weapon, WEAPON_KNIFE_WIDOWMAKER, config_system.item.paint_kit_index_knife, index_talon, 3, wear);
					break;
				case 14:
					apply_knife_skin(weapon, WEAPON_KNIFE_URSUS, config_system.item.paint_kit_index_knife, index_ursus, 3, wear);
					break;
				}
			}

			//apply weapon skins
			switch (weapon->item_definition_index()) {
			case WEAPON_USP_SILENCER:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_usp, weapon->fallback_wear() = wear;
				break;
			case WEAPON_HKP2000:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_p2000, weapon->fallback_wear() = wear;
				break;
			case WEAPON_GLOCK:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_glock, weapon->fallback_wear() = wear;
				break;
			case WEAPON_P250:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_p250, weapon->fallback_wear() = wear;
				break;
			case WEAPON_FIVESEVEN:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_fiveseven, weapon->fallback_wear() = wear;
				break;
			case WEAPON_TEC9:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_tec, weapon->fallback_wear() = wear;
				break;
			case WEAPON_CZ75A:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_cz, weapon->fallback_wear() = wear;
				break;
			case WEAPON_ELITE:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_duals, weapon->fallback_wear() = wear;
				break;
			case WEAPON_DEAGLE:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_deagle, weapon->fallback_wear() = wear;
				break;
			case WEAPON_REVOLVER:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_revolver, weapon->fallback_wear() = wear;
				break;
			case WEAPON_FAMAS:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_famas, weapon->fallback_wear() = wear;
				break;
			case WEAPON_GALILAR:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_galil, weapon->fallback_wear() = wear;
				break;
			case WEAPON_M4A1:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_m4a4, weapon->fallback_wear() = wear;
				break;
			case WEAPON_M4A1_SILENCER:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_m4a1, weapon->fallback_wear() = wear;
				break;
			case WEAPON_AK47:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_ak47, weapon->fallback_wear() = wear;
				break;
			case WEAPON_SG556:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_sg553, weapon->fallback_wear() = wear;
				break;
			case WEAPON_AUG:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_aug, weapon->fallback_wear() = wear;
				break;
			case WEAPON_SSG08:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_ssg08, weapon->fallback_wear() = wear;
				break;
			case WEAPON_AWP:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_awp, weapon->fallback_wear() = wear;
				break;
			case WEAPON_SCAR20:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_scar, weapon->fallback_wear() = wear;
				break;
			case WEAPON_G3SG1:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_g3sg1, weapon->fallback_wear() = wear;
				break;
			case WEAPON_SAWEDOFF:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_sawoff, weapon->fallback_wear() = wear;
				break;
			case WEAPON_M249:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_m249, weapon->fallback_wear() = wear;
				break;
			case WEAPON_NEGEV:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_negev, weapon->fallback_wear() = wear;
				break;
			case WEAPON_MAG7:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_mag7, weapon->fallback_wear() = wear;
				break;
			case WEAPON_XM1014:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_xm1014, weapon->fallback_wear() = wear;
				break;
			case WEAPON_NOVA:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_nova, weapon->fallback_wear() = wear;
				break;
			case WEAPON_BIZON:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_bizon, weapon->fallback_wear() = wear;
				break;
			case WEAPON_MP5SD:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_mp5sd, weapon->fallback_wear() = wear;
				break;
			case WEAPON_MP7:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_mp7, weapon->fallback_wear() = wear;
				break;
			case WEAPON_MP9:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_mp9, weapon->fallback_wear() = wear;
				break;
			case WEAPON_MAC10:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_mac10, weapon->fallback_wear() = wear;
				break;
			case WEAPON_P90:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_p90, weapon->fallback_wear() = wear;
				break;
			case WEAPON_UMP45:
				weapon->fallback_paint_kit() = config_system.item.paint_kit_index_ump45, weapon->fallback_wear() = wear;
				break;
			}

			weapon->original_owner_xuid_low() = 0;
			weapon->original_owner_xuid_high() = 0;
			weapon->fallback_seed() = 661;
			weapon->item_id_high() = -1;
		}
	}	
}
