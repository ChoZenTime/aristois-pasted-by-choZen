#pragma once
#include "../../dependencies/common_includes.hpp"
#include "../features/visuals/visuals.hpp"
#include "../features/misc/movement.hpp"
#include "../features/aimbot/aimbot.hpp"
#include "../features/aimbot/trigger_simple.hpp"
#include "../menu/menu.hpp"
#include "../features/misc/hitmarker.hpp"
#include "../features/misc/damage_indicator.hpp"
#include "../features/backtrack/backtrack.hpp"
#include "../features/misc/prediction.hpp"
#include "../features/misc/misc.hpp"
#include "../features/skinchanger/skinchanger.hpp"
#include "../features/skinchanger/glovechanger.hpp"
#include "../features/skinchanger/knifehook.hpp"
#include "../features/nade_pred/nade_pred.hpp"
#include "../features/misc/logs.hpp"
#include "../features/misc/events.hpp"
#include "../features/visuals/sound.hpp"
#include "../features/skinchanger/parser.hpp"
#include "../features/visuals/nightmode.hpp"

std::unique_ptr<vmt_hook> hooks::client_hook;
std::unique_ptr<vmt_hook> hooks::clientmode_hook;
std::unique_ptr<vmt_hook> hooks::panel_hook;
std::unique_ptr<vmt_hook> hooks::renderview_hook;
std::unique_ptr<vmt_hook> hooks::surface_hook;
std::unique_ptr<vmt_hook> hooks::modelrender_hook;

uint8_t* present_address;
hooks::present_fn original_present;
uint8_t* reset_address;
hooks::reset_fn original_reset;
HWND hooks::window;
WNDPROC hooks::wndproc_original = NULL;

void hooks::initialize() {
	client_hook = std::make_unique<vmt_hook>();
	clientmode_hook = std::make_unique<vmt_hook>();
	panel_hook = std::make_unique<vmt_hook>();
	renderview_hook = std::make_unique<vmt_hook>();
	surface_hook = std::make_unique<vmt_hook>();
	modelrender_hook = std::make_unique<vmt_hook>();

	render.setup_fonts();

	client_hook->setup(interfaces::client);
	client_hook->hook_index(37, reinterpret_cast<void*>(frame_stage_notify));

	clientmode_hook->setup(interfaces::clientmode);
	clientmode_hook->hook_index(18, reinterpret_cast<void*>(override_view));
	clientmode_hook->hook_index(24, reinterpret_cast<void*>(create_move));
	clientmode_hook->hook_index(44, reinterpret_cast<void*>(do_post_screen_effects));
	clientmode_hook->hook_index(35, reinterpret_cast<void*>(viewmodel_fov));

	clientmode_hook->hook_index(17, reinterpret_cast<void*>(should_draw_fog));

	panel_hook->setup(interfaces::panel);
	panel_hook->hook_index(41, reinterpret_cast<void*>(paint_traverse));

	renderview_hook->setup(interfaces::render_view);
	renderview_hook->hook_index(9, reinterpret_cast<void*>(scene_end));

	surface_hook->setup(interfaces::surface);
	surface_hook->hook_index(67, reinterpret_cast<void*>(lock_cursor));
	surface_hook->hook_index(116, reinterpret_cast<void*>(on_screen_size_changed));

	modelrender_hook->setup(interfaces::model_render);
	modelrender_hook->hook_index(21, reinterpret_cast<void*>(draw_model_execute)); //hooked for backtrack chams

	present_address = utilities::pattern_scan(GetModuleHandleW(L"gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 DB") + 0x2;
	reset_address = utilities::pattern_scan(GetModuleHandleW(L"gameoverlayrenderer.dll"), "FF 15 ? ? ? ? 8B F8 85 FF 78 18") + 0x2;

	original_present = **reinterpret_cast<present_fn**>(present_address);
	original_reset = **reinterpret_cast<reset_fn**>(reset_address);

	**reinterpret_cast<void***>(present_address) = reinterpret_cast<void*>(&present);
	**reinterpret_cast<void***>(reset_address) = reinterpret_cast<void*>(&reset);

	window = FindWindow("Valve001", NULL);
	wndproc_original = reinterpret_cast<WNDPROC>(SetWindowLongW(window, GWL_WNDPROC, reinterpret_cast<LONG>(wndproc)));

	interfaces::console->get_convar("crosshair")->set_value(1);
	interfaces::console->get_convar("viewmodel_fov")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_x")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_y")->callbacks.set_size(false);
	interfaces::console->get_convar("viewmodel_offset_z")->callbacks.set_size(false);

	events.setup();
	kit_parser.setup();

	printf("Hooks initialized!\n");
}


void hooks::shutdown() {
	clientmode_hook->release();
	client_hook->release();
	panel_hook->release();
	renderview_hook->release();
	surface_hook->release();
	modelrender_hook->release();

	events.release();

	**reinterpret_cast<void***>(present_address) = reinterpret_cast<void*>(original_present);
	**reinterpret_cast<void***>(reset_address) = reinterpret_cast<void*>(original_reset);

	SetWindowLongW(FindWindowW(L"Valve001", NULL), GWL_WNDPROC, reinterpret_cast<LONG>(wndproc_original));
}

float __stdcall hooks::viewmodel_fov() {
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (local_player && local_player->is_alive()) {
		return 68.f + config_system.item.viewmodel_fov;
	}
	else {
		return 68.f;
	}
}

bool __fastcall hooks::should_draw_fog(uintptr_t ecx, uintptr_t edx) {
	return !config_system.item.remove_fog;
}

void __stdcall hooks::on_screen_size_changed(int old_width, int old_height) {
	static auto original_fn = reinterpret_cast<on_screen_size_changed_fn>(surface_hook->get_original(116));

	original_fn(interfaces::surface, old_width, old_height);

	render.setup_fonts();
}

int __stdcall hooks::do_post_screen_effects(int value) {
	static auto original_fn = reinterpret_cast<do_post_screen_effects_fn>(clientmode_hook->get_original(44));

	visuals.glow();
	night_mode.ambient_light();

	return original_fn(interfaces::clientmode, value);
}

bool __stdcall hooks::create_move(float frame_time, c_usercmd* user_cmd) {
	static auto original_fn = reinterpret_cast<create_move_fn>(clientmode_hook->get_original(24));
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	original_fn(interfaces::clientmode, frame_time, user_cmd); //fixed create move

	if (!user_cmd || !user_cmd->command_number)
		return original_fn;

	if (!interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()))
		return original_fn;

	bool& send_packet = *reinterpret_cast<bool*>(*(static_cast<uintptr_t*>(_AddressOfReturnAddress()) - 1) - 0x1C);

	if (interfaces::engine->is_connected() && interfaces::engine->is_in_game()) {		
		//misc
		movement.bunnyhop(user_cmd);
		movement.strafe(user_cmd);
		misc.clantag_spammer();
		misc.viewmodel_offset();
		misc.disable_post_processing();
		misc.recoil_crosshair();
		misc.force_crosshair();
		misc.rank_reveal();
		nade_pred.trace(user_cmd);

		//legitbot and prediction stuff
		movement.edge_jump_pre_prediction(user_cmd);
		engine_prediction.start_prediction(user_cmd); //small note for prediction, we need to run bhop before prediction otherwise it will be buggy

		aimbot.run(user_cmd);
		backtrack.run(user_cmd);
		if (config_system.item.trigger_enable && GetAsyncKeyState(config_system.item.trigger_key))
			trigger.trigger(user_cmd);

		engine_prediction.end_prediction();
		movement.edge_jump_post_prediction(user_cmd);

		night_mode.run();

		//clamping movement
		auto forward = user_cmd->forwardmove;
		auto right = user_cmd->sidemove;
		auto up = user_cmd->upmove;

		//clamping movement
		user_cmd->forwardmove = std::clamp(user_cmd->forwardmove, -450.0f, 450.0f);
		user_cmd->sidemove = std::clamp(user_cmd->sidemove, -450.0f, 450.0f);
		user_cmd->upmove = std::clamp(user_cmd->upmove, -450.0f, 450.0f);

		// clamping angles
		user_cmd->viewangles.x = std::clamp(user_cmd->viewangles.x, -89.0f, 89.0f);
		user_cmd->viewangles.y = std::clamp(user_cmd->viewangles.y, -180.0f, 180.0f);
		user_cmd->viewangles.z = 0.0f;
	}

	return false;
}

void __fastcall hooks::override_view(void* _this, void* _edx, c_viewsetup* setup) {
	static auto original_fn = reinterpret_cast<override_view_fn>(clientmode_hook->get_original(18));
	auto local_player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (local_player && !local_player->is_scoped() && config_system.item.fov > 0 && config_system.item.visuals_enabled) {
		setup->fov = 90 + config_system.item.fov;
	}

	original_fn(interfaces::clientmode, _this, setup);
}

void __stdcall hooks::draw_model_execute(IMatRenderContext * ctx, const draw_model_state_t & state, const model_render_info_t & info, matrix_t * bone_to_world) {
	static auto original_fn = reinterpret_cast<draw_model_execute_fn>(modelrender_hook->get_original(21));
	
	visuals.backtrack_chams(ctx, state, info);
	visuals.viewmodel_modulate(info);
	visuals.chams_misc(info);

	original_fn(interfaces::model_render, ctx, state, info, bone_to_world);
}

void __stdcall hooks::frame_stage_notify(int frame_stage) {
	static auto original_fn = reinterpret_cast<frame_stage_notify_fn>(client_hook->get_original(37));
	static auto backtrack_init = (backtrack.init(), false);

	if (frame_stage == FRAME_RENDER_START) {
		misc.remove_smoke();
		misc.remove_flash();
	}

	else if (frame_stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		
		skin_changer.run();
		glove_changer.run();
	}

	else if (frame_stage == FRAME_NET_UPDATE_START && interfaces::engine->is_in_game()) {
		sound_esp.draw();
	}

	else if (frame_stage == FRAME_NET_UPDATE_END && interfaces::engine->is_in_game()) {
		backtrack.update();
	}

	original_fn(interfaces::client, frame_stage);
}

void __stdcall hooks::paint_traverse(unsigned int panel, bool force_repaint, bool allow_force) {
	if (strstr(interfaces::panel->get_panel_name(panel), "HudZoom")) {
		if (interfaces::engine->is_connected() && interfaces::engine->is_in_game()) {
			if (config_system.item.remove_scope)
				return;
		}
	}

	reinterpret_cast<paint_traverse_fn>(panel_hook->get_original(41))(interfaces::panel, panel, force_repaint, allow_force);

	if (strstr(interfaces::panel->get_panel_name(panel), "MatSystemTopPanel")) {
		visuals.run();
		hitmarker.run();
		event_logs.run();
		misc.remove_scope();
		misc.watermark();
		misc.spectators();
		nade_pred.draw();	
		damage_indicator.draw();
	}
}

void __stdcall hooks::scene_end() {
	static auto original_fn = reinterpret_cast<scene_end_fn>(renderview_hook->get_original(9));

	visuals.chams();
	
	original_fn(interfaces::render_view);
}

extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

LRESULT __stdcall hooks::wndproc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
	static bool pressed = false;

	if (!pressed && GetAsyncKeyState(VK_INSERT)) {
		pressed = true;
	}
	else if (pressed && !GetAsyncKeyState(VK_INSERT)) {
		pressed = false;

		menu.opened = !menu.opened;
	}

	if (menu.opened) {
		interfaces::inputsystem->enable_input(false);

	}
	else if (!menu.opened) {
		interfaces::inputsystem->enable_input(true);
	}

	if (menu.opened && ImGui_ImplDX9_WndProcHandler(hwnd, message, wparam, lparam))
		return true;

	return CallWindowProcA(wndproc_original, hwnd, message, wparam, lparam);
}

void __stdcall hooks::lock_cursor() {
	static auto original_fn = reinterpret_cast<lock_cursor_fn>(surface_hook->get_original(67));

	if (menu.opened) {
		interfaces::surface->unlock_cursor();
		return;
	}

	original_fn(interfaces::surface);
}

static bool initialized = false;
long __stdcall hooks::present(IDirect3DDevice9* device, RECT* source_rect, RECT* dest_rect, HWND dest_window_override, RGNDATA* dirty_region) {
	if (!initialized) {
		menu.apply_fonts();
		menu.setup_resent(device);
		initialized = true;
	}
	if (initialized) {
		menu.pre_render(device);
		menu.post_render();

		menu.run_popup();
		menu.run();
		menu.end_present(device);
	}

	return original_present(device, source_rect, dest_rect, dest_window_override, dirty_region);
}

long __stdcall hooks::reset(IDirect3DDevice9* device, D3DPRESENT_PARAMETERS* present_parameters) {
	if (!initialized)
		original_reset(device, present_parameters);

	menu.invalidate_objects();
	long hr = original_reset(device, present_parameters);
	menu.create_objects(device);

	return hr;
}

