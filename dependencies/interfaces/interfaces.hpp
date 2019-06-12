#pragma once
#include "i_base_client_dll.hpp"
#include "i_client_entity_list.hpp"
#include "iv_engine_client.hpp"
#include "i_client_mode.hpp"
#include "i_client_state.hpp"
#include "i_panel.hpp"
#include "i_surface.hpp"
#include "c_global_vars_base.hpp"
#include "i_material_system.hpp"
#include "iv_model_info.hpp"
#include "iv_model_render.hpp"
#include "iv_debug_overlay.hpp"
#include "i_console.hpp"
#include "i_localize.hpp"
#include "i_game_event_manager.hpp"
#include "i_input.hpp"
#include "iv_effects.hpp"
#include "i_input_system.hpp"
#include "engine_sounds.hpp"
#include "i_trace.hpp"
#include "i_render_view.hpp"
#include "glow_manager.hpp"
#include "i_player_movement.hpp"
#include "iv_render_beams.hpp"

namespace interfaces {
	extern i_base_client_dll* client;
	extern i_input* input;
	extern i_client_entity_list* entity_list;
	extern iv_engine_client* engine;
	extern i_client_mode* clientmode;
	extern i_client_state* clientstate;
	extern i_panel* panel;
	extern i_surface* surface;
	extern c_global_vars_base* globals;
	extern i_material_system* material_system;
	extern iv_model_info* model_info;
	extern iv_model_render* model_render;
	extern i_render_view* render_view;
	extern iv_effects* effects;
	extern iv_debug_overlay* debug_overlay;
	extern i_console* console;
	extern i_localize* localize;
	extern i_game_event_manager2* event_manager;
	extern i_inputsytem* inputsystem;
	extern IDirect3DDevice9* directx;
	extern iv_engine_sound* engine_sound;
	extern trace *trace_ray;
	extern glow_manager_t *glow_manager;
	extern player_game_movement *game_movement;
	extern player_prediction *prediction;
	extern player_move_helper *move_helper;
	extern i_view_render_beams *render_beams;

	void initialize();
}