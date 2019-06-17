#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_aimbot {
private:
	int find_target(c_usercmd * user_cmd) noexcept;
	int get_nearest_bone(player_t * entity, c_usercmd* user_cmd) noexcept;
	void weapon_settings(weapon_t* weapon) noexcept;
	float aim_smooth;
	float aim_fov;
	float rcs_x;
	float rcs_y;
	int hitbox_id;
	vec3_t angle;
	int kill_delay;
	int trigger_delay{00000};
public:
	void run(c_usercmd*) noexcept;
	void event_player_death(i_game_event* event) noexcept;
	void auto_pistol(c_usercmd* user_cmd);
	void rcs_standalone(c_usercmd* user_cmd) noexcept;

	bool is_pistol(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_GLOCK, WEAPON_ELITE, WEAPON_P250, WEAPON_TEC9, WEAPON_CZ75A, WEAPON_DEAGLE, WEAPON_REVOLVER, WEAPON_USP_SILENCER, WEAPON_HKP2000, WEAPON_FIVESEVEN };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_sniper(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_AWP, WEAPON_SSG08, WEAPON_G3SG1, WEAPON_SCAR20 };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_smg(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_MAC10, WEAPON_MP7, WEAPON_UMP45, WEAPON_P90, WEAPON_BIZON, WEAPON_MP9, WEAPON_MP5SD };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_heavy(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_NOVA, WEAPON_XM1014, WEAPON_SAWEDOFF, WEAPON_M249, WEAPON_NEGEV, WEAPON_MAG7 };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_knife(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_KNIFE, WEAPON_KNIFEGG, WEAPON_KNIFE_BUTTERFLY, WEAPON_KNIFE_FALCHION, WEAPON_KNIFE_FLIP, WEAPON_KNIFE_GHOST, WEAPON_KNIFE_GUT, WEAPON_KNIFE_GYPSY_JACKKNIFE, WEAPON_KNIFE_KARAMBIT, WEAPON_KNIFE_M9_BAYONET, WEAPON_KNIFE_PUSH, WEAPON_KNIFE_STILETTO, WEAPON_KNIFE_T, WEAPON_KNIFE_TACTICAL, WEAPON_KNIFE_URSUS, WEAPON_KNIFE_WIDOWMAKER, WEAPON_BAYONET };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_grenade(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_FRAG_GRENADE, WEAPON_HEGRENADE, WEAPON_INCGRENADE, WEAPON_SMOKEGRENADE, WEAPON_TAGRENADE, WEAPON_MOLOTOV , WEAPON_DECOY};
		return (std::find(v.begin(), v.end(), id) != v.end());
	}
	
	bool is_bomb(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_C4 };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}

	bool is_rifle(void* weapon) {
		if (!weapon)
			return false;

		auto current_weapon = reinterpret_cast<weapon_t*>(weapon);
		int id = current_weapon->item_definition_index();
		static const std::vector<int> v = { WEAPON_AK47,WEAPON_AUG,WEAPON_FAMAS,WEAPON_GALILAR,WEAPON_M249,WEAPON_M4A1,WEAPON_M4A1_SILENCER,WEAPON_NEGEV,WEAPON_SG556 };
		return (std::find(v.begin(), v.end(), id) != v.end());
	}
};

extern c_aimbot aimbot;