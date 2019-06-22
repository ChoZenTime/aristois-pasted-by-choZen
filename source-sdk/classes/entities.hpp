#pragma once
#include "../../dependencies/math/math.hpp"
#include <array>
#include <vector>
#include "collideable.hpp"
#include "client_class.hpp"
#include "../../dependencies/utilities/netvar_manager.hpp"

enum move_type {
	movetype_none = 0,
	movetype_isometric,
	movetype_walk,
	movetype_step,
	movetype_fly,
	movetype_flygravity,
	movetype_vphysics,
	movetype_push,
	movetype_noclip,
	movetype_ladder,
	movetype_observer,
	movetype_custom,
	movetype_last = movetype_custom,
	movetype_max_bits = 4,
	max_movetype
};

enum entity_flags {
	fl_onground = (1 << 0),
	fl_ducking = (1 << 1),
	fl_waterjump = (1 << 2),
	fl_ontrain = (1 << 3),
	fl_inrain = (1 << 4),
	fl_frozen = (1 << 5),
	fl_atcontrols = (1 << 6),
	fl_client = (1 << 7),
	fl_fakeclient = (1 << 8),
	fl_inwater = (1 << 9),
	fl_fly = (1 << 10),
	fl_swim = (1 << 11),
	fl_conveyor = (1 << 12),
	fl_npc = (1 << 13),
	fl_godmode = (1 << 14),
	fl_notarget = (1 << 15),
	fl_aimtarget = (1 << 16),
	fl_partialground = (1 << 17),
	fl_staticprop = (1 << 18),
	fl_graphed = (1 << 19),
	fl_grenade = (1 << 20),
	fl_stepmovement = (1 << 21),
	fl_donttouch = (1 << 22),
	fl_basevelocity = (1 << 23),
	fl_worldbrush = (1 << 24),
	fl_object = (1 << 25),
	fl_killme = (1 << 26),
	fl_onfire = (1 << 27),
	fl_dissolving = (1 << 28),
	fl_transragdoll = (1 << 29),
	fl_unblockable_by_player = (1 << 30)
};

enum item_definition_indexes {
	WEAPON_NONE = 0,
	WEAPON_DEAGLE,
	WEAPON_ELITE,
	WEAPON_FIVESEVEN,
	WEAPON_GLOCK,
	WEAPON_AK47 = 7,
	WEAPON_AUG,
	WEAPON_AWP,
	WEAPON_FAMAS,
	WEAPON_G3SG1,
	WEAPON_GALILAR = 13,
	WEAPON_M249,
	WEAPON_M4A1 = 16,
	WEAPON_MAC10,
	WEAPON_P90 = 19,
	WEAPON_MP5SD = 23,
	WEAPON_UMP45,
	WEAPON_XM1014,
	WEAPON_BIZON,
	WEAPON_MAG7,
	WEAPON_NEGEV,
	WEAPON_SAWEDOFF,
	WEAPON_TEC9,
	WEAPON_TASER,
	WEAPON_HKP2000,
	WEAPON_MP7,
	WEAPON_MP9,
	WEAPON_NOVA,
	WEAPON_P250,
	WEAPON_SHIELD,
	WEAPON_SCAR20,
	WEAPON_SG556,
	WEAPON_SSG08,
	WEAPON_KNIFEGG,
	WEAPON_KNIFE,
	WEAPON_FLASHBANG,
	WEAPON_HEGRENADE,
	WEAPON_SMOKEGRENADE,
	WEAPON_MOLOTOV,
	WEAPON_DECOY,
	WEAPON_INCGRENADE,
	WEAPON_C4,
	WEAPON_HEALTHSHOT = 57,
	WEAPON_KNIFE_T = 59,
	WEAPON_M4A1_SILENCER,
	WEAPON_USP_SILENCER,
	WEAPON_CZ75A = 63,
	WEAPON_REVOLVER,
	WEAPON_TAGRENADE = 68,
	WEAPON_FISTS,
	WEAPON_BREACHCHARGE,
	WEAPON_TABLET = 72,
	WEAPON_MELEE = 74,
	WEAPON_AXE,
	WEAPON_HAMMER,
	WEAPON_SPANNER = 78,
	WEAPON_KNIFE_GHOST = 80,
	WEAPON_FIREBOMB,
	WEAPON_DIVERSION,
	WEAPON_FRAG_GRENADE,
	WEAPON_SNOWBALL,
	WEAPON_BUMPMINE,
	WEAPON_BAYONET = 500,
	WEAPON_KNIFE_FLIP = 505,
	WEAPON_KNIFE_GUT,
	WEAPON_KNIFE_KARAMBIT,
	WEAPON_KNIFE_M9_BAYONET,
	WEAPON_KNIFE_TACTICAL,
	WEAPON_KNIFE_FALCHION = 512,
	WEAPON_KNIFE_SURVIVAL_BOWIE = 514,
	WEAPON_KNIFE_BUTTERFLY,
	WEAPON_KNIFE_PUSH,
	WEAPON_KNIFE_URSUS = 519,
	WEAPON_KNIFE_GYPSY_JACKKNIFE,
	WEAPON_KNIFE_STILETTO = 522,
	WEAPON_KNIFE_WIDOWMAKER,
	GLOVE_STUDDED_BLOODHOUND = 5027,
	GLOVE_T_SIDE = 5028,
	GLOVE_CT_SIDE = 5029,
	GLOVE_SPORTY = 5030,
	GLOVE_SLICK = 5031,
	GLOVE_LEATHER_WRAP = 5032,
	GLOVE_MOTORCYCLE = 5033,
	GLOVE_SPECIALIST = 5034,
	GLOVE_HYDRA = 5035
};

enum weapon_type : int
{
	WT_Invalid,
	WT_Grenade,
	WT_Knife,
	WT_Misc,
	WT_Pistol,
	WT_Submg,
	WT_Rifle,
	WT_Sniper,
	WT_Shotgun,
	WT_Machinegun,
	WT_Max
};

enum data_update_type {
	DATA_UPDATE_CREATED = 0,
	DATA_UPDATE_DATATABLE_CHANGED,
};

class entity_t {
public:
	void* animating() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x4);
	}
	void* networkable() {
		return reinterpret_cast<void*>(uintptr_t(this) + 0x8);
	}
	collideable_t* collideable() {
		using original_fn = collideable_t * (__thiscall*)(void*);
		return (*(original_fn**)this)[3](this);
	}
	c_client_class* client_class() {
		using original_fn = c_client_class * (__thiscall*)(void*);
		return (*(original_fn**)networkable())[2](networkable());
	}

	int index() {
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn**)networkable())[10](networkable());
	}
	bool is_player() {
		using original_fn = bool(__thiscall*)(entity_t*);
		return (*(original_fn**)this)[155](this);
	}
	bool is_weapon() {
		using original_fn = bool(__thiscall*)(entity_t*);
		return (*(original_fn**)this)[163](this);
	}
	vec3_t get_absolute_origin() {
		__asm {
			MOV ECX, this
			MOV EAX, DWORD PTR DS : [ECX]
			CALL DWORD PTR DS : [EAX + 0x28]
		}
	}
	bool setup_bones(matrix_t* out, int max_bones, int mask, float time) {
		if (!this) {
			return false;
		}

		using original_fn = bool(__thiscall*)(void*, matrix_t*, int, int, float);
		return (*(original_fn**)animating())[13](animating(), out, max_bones, mask, time);
	}
	model_t* model() {
		using original_fn = model_t * (__thiscall*)(void*);
		return (*(original_fn**)animating())[8](animating());
	}
	void update() {
		using original_fn = void(__thiscall*)(entity_t*);
		(*(original_fn**)this)[218](this);
	}
	int draw_model(int flags, uint8_t alpha) {
		using original_fn = int(__thiscall*)(void*, int, uint8_t);
		return (*(original_fn**)animating())[9](animating(), flags, alpha);
	}
	void set_angles(vec3_t angles) {
		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn set_angles_fn = (original_fn)((uintptr_t)utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 83 EC 64 53 56 57 8B F1"));
		set_angles_fn(this, angles);
	}
	void set_position(vec3_t position) {
		using original_fn = void(__thiscall*)(void*, const vec3_t&);
		static original_fn set_position_fn = (original_fn)((uintptr_t)utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "55 8B EC 83 E4 F8 51 53 56 57 8B F1 E8"));
		set_position_fn(this, position);
	}
	vec3_t &get_world_space_center() {
		vec3_t vec_origin = origin();

		vec3_t min = this->collideable()->mins() + vec_origin;
		vec3_t max = this->collideable()->maxs() + vec_origin;

		vec3_t size = max - min;
		size /= 2.f;
		size += min;

		return size;
	}
	NETVAR("DT_CSPlayer", "m_fFlags", flags, int);
	OFFSET(bool, dormant, 0xED);
	NETVAR("DT_BaseEntity", "m_hOwnerEntity", owner_handle, unsigned long);
	NETVAR("DT_CSPlayer", "m_flSimulationTime", simulation_time, float);
	NETVAR("DT_BasePlayer", "m_vecOrigin", origin, vec3_t);
	NETVAR("DT_BasePlayer", "m_vecViewOffset[0]", view_offset, vec3_t);
	NETVAR("DT_CSPlayer", "m_iTeamNum", team, int);
	NETVAR("DT_BaseEntity", "m_bSpotted", spotted, bool);
};

class econ_view_item_t {
public:
	NETVAR("DT_ScriptCreatedItem", "m_bInitialized", is_initialized, bool);
	NETVAR("DT_ScriptCreatedItem", "m_iEntityLevel", entity_level, int);
	NETVAR("DT_ScriptCreatedItem", "m_iAccountID", account_id, int);
	NETVAR("DT_ScriptCreatedItem", "m_iItemIDLow", item_id_low, int);

};

class base_view_model : public entity_t {
public:
	NETVAR("DT_BaseViewModel", "m_nModelIndex", model_index, int);
	NETVAR("DT_BaseViewModel", "m_nViewModelIndex", view_model_index, int);
	NETVAR("DT_BaseViewModel", "m_hWeapon", m_hweapon, int);
	NETVAR("DT_BaseViewModel", "m_hOwner", m_howner, int);	
};

class attributable_item_t : public entity_t {
public:
	NETVAR("DT_BaseViewModel", "m_nModelIndex", model_index, int);
	NETVAR("DT_BaseAttributableItem", "m_OriginalOwnerXuidLow", original_owner_xuid_low, int);
	NETVAR("DT_BaseAttributableItem", "m_OriginalOwnerXuidHigh", original_owner_xuid_high, int);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackStatTrak", fallback_stattrak, int);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackPaintKit", fallback_paint_kit, int);
	NETVAR("DT_BaseAttributableItem", "m_nFallbackSeed", fallback_seed, int);
	NETVAR("DT_BaseAttributableItem", "m_iEntityQuality", entity_quality, int);
	NETVAR("DT_BaseAttributableItem", "m_flFallbackWear", fallback_wear, float);
	NETVAR("DT_BaseCombatWeapon", "m_hWeaponWorldModel", world_model_handle, unsigned long);
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", item_definition_index, short);
	NETVAR("DT_BaseAttributableItem", "m_iItemIDHigh", item_id_high, int); //jakby crash to wez offset z hazedumpera
	NETVAR("DT_BaseAttributableItem", "m_iAccountID", acc_id, int);

	void net_pre_data_update(int update_type){
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn * *)networkable())[6](networkable(), update_type);
	}
	
	void net_release() {
		using original_fn = void(__thiscall*)(void*);
		return (*(original_fn * *)networkable())[1](networkable());
	}

	int net_set_destroyed_on_recreate_entities() {
		using original_fn = int(__thiscall*)(void*);
		return (*(original_fn * *)networkable())[13](networkable());
	}

	void set_model_index(int index) {
		using original_fn = void(__thiscall*)(void*, int);
		return (*(original_fn * *)this)[75](this, index);
	}

	base_view_model* get_view_model(){
		return (base_view_model*)(DWORD)this;
	}

	econ_view_item_t& item() {
		return *(econ_view_item_t*)this;
	}
};

class weapon_t : public entity_t {
public:
	NETVAR("DT_BaseCombatWeapon", "m_flNextPrimaryAttack", next_primary_attack, float);
	NETVAR("DT_BaseCombatWeapon", "m_flNextSecondaryAttack", next_secondary_attack, float);
	NETVAR("DT_BaseCombatWeapon", "m_iClip1", clip1_count, int);
	NETVAR("DT_BaseCombatWeapon", "m_iClip2", clip2_count, int);
	NETVAR("DT_BaseCombatWeapon", "m_iPrimaryReserveAmmoCount", primary_reserve_ammo_acount, int);
	NETVAR("DT_WeaponCSBase", "m_flRecoilIndex", recoil_index, float);
	NETVAR("DT_WeaponCSBaseGun", "m_zoomLevel", zoom_level, float);
	NETVAR("DT_BaseAttributableItem", "m_iItemDefinitionIndex", item_definition_index, short);
	NETVAR("DT_BaseCombatWeapon", "m_iEntityQuality", entity_quality, int);

	float get_innacuracy() {
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn**)this)[476](this);
	}

	float get_spread() {
		using original_fn = float(__thiscall*)(void*);
		return (*(original_fn**)this)[446](this);
	}

	void update_accuracy_penalty() {
		using original_fn = void(__thiscall*)(void*);
		(*(original_fn**)this)[477](this);
	}
	weapon_info_t* get_weapon_data() {
		using original_fn = weapon_info_t * (__thiscall*)(void*);
		return (*(original_fn**)this)[454](this); //skinchanger crash
	}

	std::string weapon_name_definition() {
		if (!this)
			return ("");
		int id = this->item_definition_index();
		switch (id) {
		case WEAPON_DEAGLE:
			return ("deagle");
		case WEAPON_AUG:
			return ("aug");
		case WEAPON_G3SG1:
			return ("g3sg1");
		case WEAPON_MAC10:
			return ("mac10");
		case WEAPON_P90:
			return ("p90");
		case WEAPON_SSG08:
			return ("ssg08");
		case WEAPON_SCAR20:
			return ("scar20");
		case WEAPON_UMP45:
			return ("ump45");
		case WEAPON_ELITE:
			return ("elite");
		case WEAPON_FAMAS:
			return ("famas");
		case WEAPON_FIVESEVEN:
			return ("fiveseven");
		case WEAPON_GALILAR:
			return ("galilar");
		case WEAPON_M4A1_SILENCER:
			return ("m4a1_s");
		case WEAPON_M4A1:
			return ("m4a4");
		case WEAPON_P250:
			return ("p250");
		case WEAPON_M249:
			return ("m249");
		case WEAPON_XM1014:
			return ("xm1014");
		case WEAPON_GLOCK:
			return ("glock");
		case WEAPON_USP_SILENCER:
			return ("usp_s");
		case WEAPON_HKP2000:
			return ("p2000");
		case WEAPON_AK47:
			return ("ak47");
		case WEAPON_AWP:
			return ("awp");
		case WEAPON_BIZON:
			return ("bizon");
		case WEAPON_MAG7:
			return ("mag7");
		case WEAPON_NEGEV:
			return ("negev");
		case WEAPON_SAWEDOFF:
			return ("sawedoff");
		case WEAPON_TEC9:
			return ("tec9");
		case WEAPON_TASER:
			return ("zeus");
		case WEAPON_NOVA:
			return ("nova");
		case WEAPON_CZ75A:
			return ("cz75");
		case WEAPON_SG556:
			return ("sg553");
		case WEAPON_REVOLVER:
			return ("revolver");
		case WEAPON_MP7:
			return ("mp7");
		case WEAPON_MP9:
			return ("mp9");
		case WEAPON_MP5SD:  //same icon as ump
			return ("mp5");
		case WEAPON_C4:
			return ("c4");
		case WEAPON_FRAG_GRENADE:
			return ("grenade");
		case WEAPON_SMOKEGRENADE:
			return ("smoke");
		case WEAPON_MOLOTOV:
			return ("fire_molo");
		case WEAPON_INCGRENADE:
			return ("fire_inc");
		case WEAPON_FLASHBANG:
			return ("flash");
		case WEAPON_DECOY:
			return ("decoy");
		default:
			return ("knife");
		}
		return ("");
	}

	std::string weapon_icon_definition() {
		if (!this)
			return ("");
		int id = this->item_definition_index();
		switch (id) {
		case WEAPON_KNIFE_T:
			return ("[");
		case WEAPON_DEAGLE:
			return ("A");
		case WEAPON_AUG:
			return ("U");
		case WEAPON_G3SG1:
			return ("X");
		case WEAPON_MAC10:
			return ("K");
		case WEAPON_P90:
			return ("P");
		case WEAPON_SSG08:
			return ("a");
		case WEAPON_SCAR20:
			return ("Y");
		case WEAPON_UMP45:
			return ("L");
		case WEAPON_ELITE:
			return ("B");
		case WEAPON_FAMAS:
			return ("R");
		case WEAPON_FIVESEVEN:
			return ("C");
		case WEAPON_GALILAR:
			return ("Q");
		case WEAPON_M4A1_SILENCER:
			return ("T");
		case WEAPON_M4A1:
			return ("S");
		case WEAPON_P250:
			return ("F");
		case WEAPON_M249:
			return ("g");
		case WEAPON_XM1014:
			return ("b");
		case WEAPON_GLOCK:
			return ("D");
		case WEAPON_USP_SILENCER:
			return ("G");
		case WEAPON_HKP2000:
			return ("E");
		case WEAPON_AK47:
			return ("W");
		case WEAPON_AWP:
			return ("Z");
		case WEAPON_BIZON:
			return ("M");
		case WEAPON_MAG7:
			return ("d");
		case WEAPON_NEGEV:
			return ("f");
		case WEAPON_SAWEDOFF:
			return ("c");
		case WEAPON_TEC9:
			return ("H");
		case WEAPON_TASER:
			return ("h");
		case WEAPON_NOVA:
			return ("e");
		case WEAPON_CZ75A:
			return ("I");
		case WEAPON_SG556:
			return ("V");
		case WEAPON_REVOLVER:
			return ("J");
		case WEAPON_MP7:
			return ("N");
		case WEAPON_MP9:
			return ("O");
		case WEAPON_MP5SD:  //same icon as ump
			return ("L");
		case WEAPON_C4:
			return ("o");
		case WEAPON_FRAG_GRENADE:
			return ("j");
		case WEAPON_SMOKEGRENADE:
			return ("k");
		case WEAPON_MOLOTOV:
			return ("l");
		case WEAPON_INCGRENADE:
			return ("n");
		case WEAPON_FLASHBANG:
			return ("i");
		case WEAPON_DECOY:
			return ("m");
		default:
			return ("]");
		}
		return ("");
	}
	
	int get_type(){
		if (!this) return WT_Invalid;
		int ID = this->item_definition_index();
		switch (ID){
		case WEAPON_DEAGLE:
		case WEAPON_P250:
		case WEAPON_USP_SILENCER:
		case WEAPON_HKP2000:
		case WEAPON_GLOCK:
		case WEAPON_FIVESEVEN:
		case WEAPON_TEC9:
		case WEAPON_ELITE:
		case WEAPON_REVOLVER:
		case WEAPON_CZ75A:
			return WT_Pistol;
			break;
		case WEAPON_MP9:
		case WEAPON_MP7:
		case WEAPON_UMP45:
		case WEAPON_BIZON:
		case WEAPON_P90:
		case WEAPON_MAC10:
			return WT_Submg;
			break;
		case WEAPON_BAYONET:
		case WEAPON_KNIFE_SURVIVAL_BOWIE:
		case WEAPON_KNIFE_BUTTERFLY:
		case WEAPON_KNIFE:
		case WEAPON_KNIFE_FALCHION:
		case WEAPON_KNIFE_FLIP:
		case WEAPON_KNIFE_GUT:
		case WEAPON_KNIFE_KARAMBIT:
		case WEAPON_KNIFE_M9_BAYONET:
		case WEAPON_KNIFE_PUSH:
		case WEAPON_KNIFE_TACTICAL:
		case WEAPON_KNIFE_T:
			return WT_Knife;
			break;
		case WEAPON_SAWEDOFF:
		case WEAPON_XM1014:
		case WEAPON_MAG7:
		case WEAPON_NOVA:
			return WT_Shotgun;
		case WEAPON_M249:
		case WEAPON_NEGEV:
			return WT_Machinegun;
		case WEAPON_TASER:
		case WEAPON_C4:
			return WT_Misc;
			break;
		case WEAPON_HEGRENADE:
		case WEAPON_FLASHBANG:
		case WEAPON_DECOY:
		case WEAPON_SMOKEGRENADE:
		case WEAPON_INCGRENADE:
		case WEAPON_MOLOTOV:
			return WT_Grenade;
			break;
		case WEAPON_AK47:
		case WEAPON_M4A1:
		case WEAPON_M4A1_SILENCER:
		case WEAPON_GALILAR:
		case WEAPON_FAMAS:
		case WEAPON_AUG:
		case WEAPON_SG556:
			return WT_Rifle;
			break;
		case WEAPON_SCAR20:
		case WEAPON_G3SG1:
		case WEAPON_SSG08:
		case WEAPON_AWP:
			return WT_Sniper;
			break;
		default:
			return WT_Knife;
		}
		return WT_Invalid;
	}

	bool is_empty()
	{
		int clip = *(int*)((DWORD)this + (netvar_manager::get_net_var(netvar_manager::fnv::hash("DT_BaseCombatWeapon"), netvar_manager::fnv::hash("m_iClip1"))));
	
		return clip == 0;
	}

	bool is_reloading()
	{
		if (!this)
			return false;

		auto type = get_type();

		if (type == WT_Invalid || type == WT_Misc || type == WT_Grenade || type == WT_Knife)
			return false;

		if (!is_empty())
			return false;

		return true;
	}
};

class player_t : public entity_t {
private:
	template <typename T>
	T& read(uintptr_t offset) {
		return *reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset);
	}

	template <typename T>
	void write(uintptr_t offset, T data) {
		*reinterpret_cast<T*>(reinterpret_cast<uintptr_t>(this) + offset) = data;
	}

public:
	NETVAR("DT_BasePlayer", "m_hViewModel[0]", view_model, int); //tutaj
	NETVAR("DT_BasePlayer", "m_viewPunchAngle", punch_angle, vec3_t);
	NETVAR("DT_BasePlayer", "m_aimPunchAngle", aim_punch_angle, vec3_t);
	NETVAR("DT_BasePlayer", "m_vecVelocity[0]", velocity, vec3_t);
	NETVAR("DT_BasePlayer", "m_flMaxspeed", max_speed, float);
	NETVAR("DT_BaseEntity", "m_flShadowCastDistance", m_flFOVTime, float);
	NETVAR("DT_BasePlayer", "m_iObserverMode", observer_mode, int);
	NETVAR("DT_BasePlayer", "m_hObserverTarget", observer_target, int);
	NETVAR("DT_BasePlayer", "m_nHitboxSet", hitbox_set, int);

	NETVAR("DT_CSPlayer", "m_bHasDefuser", has_defuser, bool);
	NETVAR("DT_CSPlayer", "m_bGunGameImmunity", has_gun_game_immunity, bool);
	NETVAR("DT_CSPlayer", "m_iShotsFired", shots_fired, int);
	NETVAR("DT_CSPlayer", "m_angEyeAngles", eye_angles, vec3_t); //"DT_CSPlayer->m_angEyeAngles[0]");
	NETVAR("DT_CSPlayer", "m_ArmorValue", armor, int);
	NETVAR("DT_CSPlayer", "m_bHasHelmet", has_helmet, bool);
	NETVAR("DT_CSPlayer", "m_bIsScoped", is_scoped, bool);
	NETVAR("DT_CSPlayer", "m_bIsDefusing", is_defusing, bool);
	NETVAR("DT_CSPlayer", "m_iAccount", money, int);
	NETVAR("DT_CSPlayer", "m_flLowerBodyYawTarget", lower_body_yaw, float);
	NETVAR("DT_CSPlayer", "m_flNextAttack", next_attack, float);
	NETVAR("DT_CSPlayer", "m_flFlashDuration", flash_duration, float);
	NETVAR("DT_CSPlayer", "m_flFlashMaxAlpha", flash_alpha, float);
	NETVAR("DT_CSPlayer", "m_bHasNightVision", m_bHasNightVision, float);
	NETVAR("DT_CSPlayer", "m_bNightVisionOn", m_bNightVisionOn, float);
	NETVAR("DT_CSPlayer", "m_iHealth", health, int);
	NETVAR("DT_CSPlayer", "m_lifeState", life_state, int);
	NETVAR("DT_CSPlayer", "m_fFlags", flags, int);
	NETVAR("DT_CSPlayer", "m_flDuckAmount", duck_amount, float);
	NETVAR("DT_CSPlayer", "m_bHasHeavyArmor", has_heavy_armor, bool);
	NETVAR("DT_CSPlayer", "m_nTickBase", get_tick_base, int);

	NETVAR("DT_PlantedC4", "m_bBombTicking", c4_is_ticking, bool);
	NETVAR("DT_PlantedC4", "m_bBombDefused", c4_is_defused, bool);
	NETVAR("DT_PlantedC4", "m_hBombDefuser", c4_gets_defused, float);
	NETVAR("DT_PlantedC4", "m_flC4Blow", c4_blow_time, float);
	NETVAR("DT_PlantedC4", "m_flDefuseCountDown", c4_defuse_countdown, float);

	NETVAR("DT_SmokeGrenadeProjectile", "m_nSmokeEffectTickBegin", smoke_grenade_tick_begin, int);
	
	weapon_t* active_weapon() {
		auto active_weapon = read<uintptr_t>(netvar_manager::get_net_var(netvar_manager::fnv::hash("DT_CSPlayer"), netvar_manager::fnv::hash("m_hActiveWeapon"))) & 0xFFF;
		return reinterpret_cast<weapon_t*>(interfaces::entity_list->get_client_entity(active_weapon));
	}
	
	char* get_callout()
	{
		return (char*)((uintptr_t)this + (netvar_manager::get_net_var(netvar_manager::fnv::hash("DT_BasePlayer"), netvar_manager::fnv::hash("m_szLastPlaceName"))));
	}
	
	uintptr_t* get_wearables() {
		return (uintptr_t*)((uintptr_t)this + (netvar_manager::get_net_var(netvar_manager::fnv::hash("DT_CSPlayer"), netvar_manager::fnv::hash("m_hMyWearables"))));
	}

	bool has_c4() {
		static auto ret = reinterpret_cast<bool(__thiscall*)(void*)>(utilities::pattern_scan(GetModuleHandleA("client_panorama.dll"), "56 8B F1 85 F6 74 31"));
		return ret(this);
	}

	vec3_t get_eye_pos() {
		return origin() + view_offset(); //hazedumper
	}

	bool can_see_player_pos(player_t* player, const vec3_t& pos) {
		trace_t tr;
		ray_t ray;
		trace_filter filter;
		filter.skip = this;

		auto start = get_eye_pos();
		auto dir = (pos - start).normalized();

		ray.initialize(start, pos);
		interfaces::trace_ray->trace_ray(ray, MASK_SHOT | CONTENTS_GRATE, &filter, &tr);

		return tr.entity == player || tr.flFraction > 0.97f;
	}

	vec3_t get_bone_position(int bone) {
		matrix_t bone_matrices[128];
		if (setup_bones(bone_matrices, 128, 256, 0.0f))
			return vec3_t{ bone_matrices[bone][0][3], bone_matrices[bone][1][3], bone_matrices[bone][2][3] };
		else
			return vec3_t{ };
	}

	vec3_t get_hitbox_position(player_t* entity, int hitbox_id) {
		matrix_t bone_matrix[MAXSTUDIOBONES];

		if (entity->setup_bones(bone_matrix, MAXSTUDIOBONES, BONE_USED_BY_HITBOX, 0.0f)) {
			auto studio_model = interfaces::model_info->get_studio_model(entity->model());

			if (studio_model) {
				auto hitbox = studio_model->hitbox_set(0)->hitbox(hitbox_id);

				if (hitbox) {
					auto min = vec3_t{}, max = vec3_t{};

					math.transform_vector(hitbox->mins, bone_matrix[hitbox->bone], min);
					math.transform_vector(hitbox->maxs, bone_matrix[hitbox->bone], max);

					return vec3_t((min.x + max.x) * 0.5f, (min.y + max.y) * 0.5f, (min.z + max.z) * 0.5f);
				}
			}
		}
		return vec3_t{};
	}
	vec3_t get_hitbox(int hitbox) {
		matrix_t matrix[128];

		if (this->setup_bones(matrix, 128, 256, interfaces::globals->cur_time)) {
			studio_hdr_t* hdr = interfaces::model_info->get_studio_model(this->model());
			studio_hitbox_set_t* set = hdr->hitbox_set(0);
			studio_box_t* box = set->hitbox(hitbox);

			if (box) {
				vec3_t min, max, vec_center, screen_center;
				math.transform_vector(box->mins, matrix[box->bone], min);
				math.transform_vector(box->maxs, matrix[box->bone], max);
				vec_center = (min + max) * 0.5;

				return vec_center;
			}
		}

		return vec3_t(0, 0, 0);
	}
	
	vec3_t get_head_pos()
	{
		return this->get_bone_position(6);
	}

	bool is_enemy() {
		static auto danger_zone = interfaces::console->get_convar("game_type");

		if (!is_in_local_team() || danger_zone->get_int() == 6)
			return true;
		else
			return false;
	}

	bool is_in_local_team() {
		return utilities::call_virtual<bool(__thiscall*)(void*)>(this, 92)(this);
	}

	bool is_alive() {
		return life_state() == 0;
	}

	bool is_moving() {
		if (this->velocity().length() > 0.1f)
			return true;

		return false;
	}

	bool is_in_air() {
		if (this->flags() & fl_onground)
			return false;

		return true;
	}

	bool is_flashed() {
		if (this->flash_duration() > 0.0f)
			return true;

		return false;
	}

	void update_client_side_animations() {
		using original_fn = void(__thiscall*)(void*);
		(*(original_fn**)this)[221](this);
	}

	vec3_t & abs_origin() {
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn**)this)[10](this);;
	}
	vec3_t & abs_angles() {
		using original_fn = vec3_t & (__thiscall*)(void*);
		return (*(original_fn**)this)[11](this);;
	}
	int	move_type() {
		return *reinterpret_cast<int*> (reinterpret_cast<uintptr_t>(this) + 0x25C); //hazedumper
	}
	vec3_t eye_pos() {
		vec3_t ret;
		utilities::call_virtual<void(__thiscall*)(void*, vec3_t&)>(this, 281)(this, ret); // this is the real eye pos
		return ret;
	}

	int* weapons() { //tu jesli skinchanger
		return reinterpret_cast<int*> (uintptr_t(this) + 0x2DF8);
	} 
};
