#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_trigger {
private:
	int trigger_delay{0};
public:	
	void trigger(c_usercmd* user_cmd);
	
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
		static const std::vector<int> v = { WEAPON_FRAG_GRENADE, WEAPON_HEGRENADE, WEAPON_INCGRENADE, WEAPON_SMOKEGRENADE, WEAPON_TAGRENADE, WEAPON_MOLOTOV , WEAPON_DECOY };
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
};

extern c_trigger trigger;