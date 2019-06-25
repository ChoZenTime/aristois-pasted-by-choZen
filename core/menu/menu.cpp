#include "menu.hpp"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "../features/misc/logs.hpp"
#include "../features/skinchanger/parser.hpp"
#include "../features/skinchanger/skinchanger.hpp"
#include "../features/skinchanger/glovechanger.hpp"

c_menu menu;

#define UNLEN 256
IDirect3DStateBlock9 *state_block;
bool reverse = false;
int offset = 0;
bool show_popup = false;
bool save_config = false;
bool load_config = false;

namespace ImGui {
	long get_mils() {
		auto duration = std::chrono::system_clock::now().time_since_epoch();
		return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	}

	void begin_popup(const char* text, int on_screen_mils, bool* done) {
		if (!done)
			show_popup = true;

		ImGuiIO &io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();
		int width = io.DisplaySize.x;
		static long old_time = -1;
		ImGui::SetNextWindowPos(ImVec2(width - offset, 100));
		style.WindowMinSize = ImVec2(100.f, 20.f);
		ImGui::Begin("##PopUpWindow", &show_popup, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImVec2 p = ImGui::GetCursorScreenPos();

		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 15, p.y - 13), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		long current_time_ms = get_mils();

		ImVec2 text_size = ImGui::CalcTextSize(text);
		ImGui::SetCursorPosY(ImGui::GetWindowHeight() / 2 - text_size.y / 2);
		ImGui::Text(text);

		if (!reverse) {
			if (offset < ImGui::GetWindowWidth())
				offset += (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);

			if (offset >= ImGui::GetWindowWidth() && old_time == -1) {
				old_time = current_time_ms;
			}
		}

		if (current_time_ms - old_time >= on_screen_mils && old_time != -1)
			reverse = true;

		if (reverse) {
			if (offset > 0)
				offset -= (ImGui::GetWindowWidth() + 5) * ((1000.0f / ImGui::GetIO().Framerate) / 100);
			if (offset <= 0) {
				offset = 0;
				reverse = false;
				*done = true;
				old_time = -1;
				show_popup = false;
			}
		}

		ImGui::End();
	}
}

void c_menu::run() {
	static int page = 0;

	if (opened) {
		ImGui::GetStyle().Colors[ImGuiCol_CheckMark] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrab] = ImVec4(167 / 255.f, 24 / 255.f, 71 / 255.f, 1.f);
		ImGui::GetStyle().Colors[ImGuiCol_SliderGrabActive] = ImVec4(167 / 255.f, 24 / 71, 247 / 255.f, 1.f);

		ImGui::SetNextWindowSize(ImVec2(600, 600), ImGuiSetCond_FirstUseEver);
		ImGui::Begin("aristois", &opened, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
			ImVec2 p = ImGui::GetCursorScreenPos();
			ImColor c = ImColor(32, 114, 247);

			ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x, p.y + 30), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 3), ImColor(30, 30, 39));
			ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x, p.y + 32), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + +30), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));
			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 7); 
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 7); 
			ImGui::Text("aristois.me");
			ImGui::SameLine();

			ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9);
			if (ImGui::ButtonT("legit", ImVec2(40, 30), page, 0, false, false)) page = 0; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("visuals", ImVec2(40, 30), page, 1, false, false)) page = 1; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("misc", ImVec2(40, 30), page, 2, false, false)) page = 2; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("skins", ImVec2(40, 30), page, 3, false, false)) page = 3; ImGui::SameLine(0, 0);
			if (ImGui::ButtonT("config", ImVec2(40, 30), page, 4, false, false)) page = 4; ImGui::SameLine(0, 0);
			ImGui::PopFont();

			ImGui::PushFont(font_menu);
			ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 45);
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 222); 

			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
			ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

			if (config_system.item.visuals_preview) {
				run_visuals_preview();
			}

			ImGui::PopStyleColor();
			ImGui::PopStyleColor();
			ImGui::PopStyleVar();

			static int test = 0;
			switch (page) {
			case 0:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				//push window color for child
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				//push border color for child
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("aimbot", ImVec2(279, 268), true); {
					ImGui::Checkbox("active", &config_system.item.aim_enabled);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("mode", &config_system.item.aim_mode, "hitbox\0nearest hitbox"); //todo add custom bone selection - designer
					ImGui::PopStyleColor();
					ImGui::Checkbox("dynamic fov", &config_system.item.aim_distance_based_fov);
					ImGui::Checkbox("silent aim", &config_system.item.aim_silent);
					ImGui::Checkbox("scope aim", &config_system.item.scope_aim);
					ImGui::Checkbox("smoke aim", &config_system.item.smoke_check);
					ImGui::Checkbox("friendly fire", &config_system.item.aim_team_check);
					ImGui::Checkbox("auto pistol", &config_system.item.aimbot_auto_pistol);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderInt("kill delay", &config_system.item.aimbot_delay_after_kill, 0, 350);
					ImGui::PopStyleColor();

				} ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("extras", ImVec2(279, 267), true); {
					ImGui::Checkbox("- backtrack -", &config_system.item.backtrack);
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.backtrack ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (config_system.item.backtrack) {
						ImGui::SliderFloat(" records (ms)", &config_system.item.backtrack_ms, 1.0f, 200.0f, "%.2f");
					}
					ImGui::Checkbox("- standalone rcs -", &config_system.item.rcs_standalone);
					if (config_system.item.rcs_standalone) {
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						ImGui::SliderFloat(" rcs amount x", &config_system.item.rcs_standalone_x, 0.0f, 1.0f, "%.2f");
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						ImGui::SliderFloat(" rcs amount y", &config_system.item.rcs_standalone_y, 0.0f, 1.0f, "%.2f");
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
					}
					ImGui::Checkbox("- triggerbot -", &config_system.item.trigger_enable);
					if (config_system.item.trigger_enable) {
						ImGui::Hotkey("##triger key", &config_system.item.trigger_key, ImVec2(100, 20));
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						if (ImGui::BeginCombo("hitbox", "...", ImVec2(0, 105))) {
							ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("head"), &config_system.item.trigger_hitbox_head, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("body"), &config_system.item.trigger_hitbox_body, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("arms"), &config_system.item.trigger_hitbox_arms, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
							ImGui::Selectable(("legs"), &config_system.item.trigger_hitbox_legs, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
							ImGui::EndCombo();
							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						}
						ImGui::SliderInt(" trigger delay", &config_system.item.trigger_delay, 1, 50);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
						ImGui::Checkbox(" trigger recoil", &config_system.item.trigger_recoil);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 10);
					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				
				ImGui::NextColumn();

				ImGui::BeginChild("settings", ImVec2(279, 543), true); {
					if (ImGui::ButtonT("pistol", ImVec2(50, 30), test, 0, false, ImColor(0, 0, 0))) test = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("rifle", ImVec2(50, 30), test, 1, false, false)) test = 1; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("sniper", ImVec2(50, 30), test, 2, false, false)) test = 2; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("smg", ImVec2(50, 30), test, 3, false, false)) test = 3; ImGui::SameLine(0, 0); 
					if (ImGui::ButtonT("heavy", ImVec2(50, 30), test, 4, false, false)) test = 4; 

					ImGui::PushFont(font_menu);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.aim_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					switch (test) {
					case 0:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo("pistol hitbox", &config_system.item.aim_bone_pistol, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("pistol fov", &config_system.item.aim_fov_pistol, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("pistol smooth", &config_system.item.aim_smooth_pistol, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("pistol rcs x", &config_system.item.rcs_x_pistol, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("pistol rcs y", &config_system.item.rcs_y_pistol, 0.0f, 1.0f, "%.2f");
						break;
					case 1:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo("rifle hitbox", &config_system.item.aim_bone_rifle, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("rifle fov", &config_system.item.aim_fov_rifle, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("rifle smooth", &config_system.item.aim_smooth_rifle, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("rifle rcs x", &config_system.item.rcs_x_rifle, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("rifle rcs y", &config_system.item.rcs_y_rifle, 0.0f, 1.0f, "%.2f");
						break;
					case 2:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo("sniper hitbox", &config_system.item.aim_bone_sniper, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("sniper fov", &config_system.item.aim_fov_sniper, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("sniper smooth", &config_system.item.aim_smooth_sniper, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("sniper rcs x", &config_system.item.rcs_x_sniper, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("sniper rcs y", &config_system.item.rcs_y_sniper, 0.0f, 1.0f, "%.2f");
						break;
					case 3:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo("smg hitbox", &config_system.item.aim_bone_smg, "head\0neck\0chest\0stomach\0pelvis");
						}

						ImGui::SliderFloat("smg fov", &config_system.item.aim_fov_smg, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("smg smooth", &config_system.item.aim_smooth_smg, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("smg rcs x", &config_system.item.rcs_x_smg, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("smg rcs y", &config_system.item.rcs_y_smg, 0.0f, 1.0f, "%.2f");
						break;
					case 4:
						if (config_system.item.aim_mode == 0) {
							ImGui::Combo("heavy hitbox", &config_system.item.aim_bone_heavy, "head\0neck\0chest\0stomach\0pelvis");
						}
						ImGui::SliderFloat("heavy fov", &config_system.item.aim_fov_heavy, 0.0f, 180.0f, "%.2f");
						ImGui::SliderFloat("heavy smooth", &config_system.item.aim_smooth_heavy, 1.f, 10.f, "%.2f");
						ImGui::SliderFloat("heavy rcs x", &config_system.item.rcs_x_heavy, 0.0f, 1.0f, "%.2f");
						ImGui::SliderFloat("heavy rcs y", &config_system.item.rcs_y_heavy, 0.0f, 1.0f, "%.2f");
						break;
					}
					ImGui::PopStyleColor();

					ImGui::PopFont();
				} ImGui::EndChild(true);
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::PopStyleVar();

				break;
			case 1:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("player", ImVec2(279, 268), true); {
					ImGui::Checkbox("active", &config_system.item.visuals_enabled);
					
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("esp mode", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("enemy"), &config_system.item.visuals_enemy_check, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("teammate"), &config_system.item.visuals_team_check, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("visible only"), &config_system.item.visuals_visible_only, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("visible on key"), &config_system.item.visuals_on_key, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();
					
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("weapon", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("weapon name"), &config_system.item.player_weapon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("weapon icon"), &config_system.item.player_weapon_icon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::ColorEdit4("weapon color", config_system.item.clr_weapon, ImGuiColorEditFlags_NoInputs);
					ImGui::PopStyleColor();
					
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("flags", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("bombcarrier"), &config_system.item.player_flags_c4, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("is flashed"), &config_system.item.player_flags_flashed, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("is reloading"), &config_system.item.player_flags_reloading, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("is scoped"), &config_system.item.player_flags_scoped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("is defusing"), &config_system.item.player_flags_defuse, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("position callout"), &config_system.item.player_flags_pos, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("defusekit"), &config_system.item.player_flags_kit, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("armor"), &config_system.item.player_flags_armor, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("money $$"), &config_system.item.player_flags_money, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();
	
					ImGui::Checkbox("name", &config_system.item.player_name);
					ImGui::ColorEdit4("name color", config_system.item.clr_name, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("box", &config_system.item.player_box);
					ImGui::ColorEdit4("box color", config_system.item.clr_box, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("health", &config_system.item.player_health);	
					ImGui::Checkbox("footstep", &config_system.item.sound_footstep);
					ImGui::ColorEdit4("footstep color", config_system.item.clr_footstep, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("skeleton", &config_system.item.skeleton);
					//ImGui::Checkbox("backtrack skeleton", &config_system.item.backtrack_skeleton);					
					
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("effects", ImVec2(279, 267), true); {
					ImGui::Checkbox("force crosshair", &config_system.item.force_crosshair);
					
					ImGui::Checkbox("ambient light", &config_system.item.ambient);
					ImGui::ColorEdit4("ambient light color", config_system.item.clr_ambient, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("- night mode -", &config_system.item.nightmode);
					ImGui::ColorEdit4("sky color", config_system.item.clr_sky, ImGuiColorEditFlags_NoInputs);
					if (config_system.item.nightmode) {
						ImGui::SliderInt(" brightness", &config_system.item.nightmode_brightness, 0, 100);
					}

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("world", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("planted bomb"), &config_system.item.bomb_planted, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("dropped weapons name"), &config_system.item.dropped_weapons, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("dropped weapons icon"), &config_system.item.dropped_weapons_icon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("projectiles name"), &config_system.item.projectiles, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("projectiles icon"), &config_system.item.projectiles_icon, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("misc entities"), &config_system.item.entity_esp, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("danger zone"), &config_system.item.danger_zone_dropped, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("visible on key "), &config_system.item.entity_visuals_on_key, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("visible only (lower fps)"), &config_system.item.entity_visible_only, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);

						ImGui::EndCombo();
					}
					ImGui::ColorEdit4("weapon drop color", config_system.item.clr_weapon_drop, ImGuiColorEditFlags_NoInputs);

					if (ImGui::BeginCombo("removals", "...", ImVec2(0, 105))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("fog"), &config_system.item.remove_fog, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("smoke"), &config_system.item.remove_smoke, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("flash"), &config_system.item.reduce_flash, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("sleeves"), &config_system.item.remove_sleeves, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("hands"), &config_system.item.remove_hands, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("scope overlay"), &config_system.item.remove_scope, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}					
					ImGui::PopStyleColor();

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SliderInt("viewmodel field of view", &config_system.item.viewmodel_fov, 0, 135);
					ImGui::SliderInt("field of view", &config_system.item.fov, 0, 60);
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("helper", ImVec2(279, 100), true); {

					ImGui::Checkbox("preview visuals", &config_system.item.visuals_preview);
					ImGui::Checkbox("grenade predition", &config_system.item.nade_pred);

					ImGui::Checkbox("damage indicator", &config_system.item.damage_indicator);
					ImGui::ColorEdit4("damage indicator color ", config_system.item.clr_damage_indicator, ImGuiColorEditFlags_NoInputs);
				}
				ImGui::EndChild(true);
				
				ImGui::BeginChild("glow", ImVec2(279, 160), true); {
					ImGui::Checkbox("active", &config_system.item.visuals_glow);
					ImGui::Checkbox("enemy", &config_system.item.visuals_glow_enemy);
					ImGui::ColorEdit4("glow color", config_system.item.clr_glow, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("teammate", &config_system.item.visuals_glow_team);
					ImGui::ColorEdit4("glow color team", config_system.item.clr_glow_team, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("planted bomb", &config_system.item.visuals_glow_planted);
					ImGui::ColorEdit4("glow color planted", config_system.item.clr_glow_planted, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("dropped weapons", &config_system.item.visuals_glow_weapons);
					ImGui::ColorEdit4("glow color weapons", config_system.item.clr_glow_dropped, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("dropped nades", &config_system.item.visuals_glow_nades);
					ImGui::ColorEdit4("glow color nades", config_system.item.clr_glow_dropped_nade, ImGuiColorEditFlags_NoInputs);

				}
				ImGui::EndChild(true);

				ImGui::Spacing();

				ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);

				ImGui::BeginChild("chams", ImVec2(279, 267), true); {
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.visuals_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("chams type", &config_system.item.vis_chams_type, "textured\0flat\0metallic\0pulsating");
					ImGui::PopStyleColor();
					ImGui::Spacing();
					ImGui::Checkbox("health based color", & config_system.item.health_chams_type);
					ImGui::Checkbox("smoke check", &config_system.item.vis_chams_smoke_check);
					ImGui::Spacing();
					ImGui::Checkbox("enemy", &config_system.item.vis_chams_vis);
					ImGui::ColorEdit4("enemy color", config_system.item.clr_chams_vis, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("enemy (behind wall)", &config_system.item.vis_chams_invis);
					ImGui::ColorEdit4("enemy (behind wall) color", config_system.item.clr_chams_invis, ImGuiColorEditFlags_NoInputs);
					
					ImGui::Checkbox("teammate", &config_system.item.vis_chams_vis_teammate);
					ImGui::ColorEdit4("teammate color", config_system.item.clr_chams_vis_teammate, ImGuiColorEditFlags_NoInputs);
					ImGui::Checkbox("teammate (behind wall)", &config_system.item.vis_chams_invis_teammate);
					ImGui::ColorEdit4("teammate (behind wall) color", config_system.item.clr_chams_invis_teammate, ImGuiColorEditFlags_NoInputs);
					ImGui::Spacing();
					ImGui::Checkbox("backtrack", &config_system.item.backtrack_visualize);
					ImGui::Spacing();
					ImGui::Checkbox("- dropped items -", &config_system.item.dropped_items_chams);
					if (config_system.item.dropped_items_chams) {

						ImGui::Checkbox(" dropped weapon", &config_system.item.dropped_weapon_chams);
						ImGui::ColorEdit4("dropped weapon color", config_system.item.clr_dropped_weapon_chams, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox(" dropped weapon (behind wall)", &config_system.item.dropped_weapon_chams_xqz);
						ImGui::ColorEdit4("dropped weapon (behind wall) color", config_system.item.clr_dropped_weapon_chams_xqz, ImGuiColorEditFlags_NoInputs);

						ImGui::Checkbox(" dropped grenade", &config_system.item.dropped_grenade_chams);
						ImGui::ColorEdit4("dropped grenade color", config_system.item.clr_dropped_grenade_chams, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox(" dropped grenade (behind wall)", &config_system.item.dropped_grenade_chams_xqz);
						ImGui::ColorEdit4("dropped grenade (behind wall) color", config_system.item.clr_dropped_grenade_chams_xqz, ImGuiColorEditFlags_NoInputs);

						ImGui::Checkbox(" planted c4", &config_system.item.planted_c4_chams);
						ImGui::ColorEdit4("planted c4 color", config_system.item.clr_planted_c4_chams, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox(" planted c4 (behind wall)", &config_system.item.planted_c4_chams_xqz);
						ImGui::ColorEdit4("planted c4 (behind wall) color", config_system.item.clr_planted_c4_chams_xqz, ImGuiColorEditFlags_NoInputs);

					}
					
					ImGui::Checkbox("- viewmodels -", &config_system.item.viewmodel_chams);
					if (config_system.item.viewmodel_chams) {
						ImGui::Checkbox(" weapon viewmodel", &config_system.item.weapon_chams);
						ImGui::ColorEdit4("weapon viewmodel color", config_system.item.clr_weapon_chams, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox(" hand viewmodel", &config_system.item.hand_chams);
						ImGui::ColorEdit4("hand viewmodel color", config_system.item.clr_hand_chams, ImGuiColorEditFlags_NoInputs);
						ImGui::Checkbox(" sleeve viewmodel", &config_system.item.sleeve_chams);
						ImGui::ColorEdit4("sleeve viewmodel color", config_system.item.clr_sleeve_chams, ImGuiColorEditFlags_NoInputs);
					}							
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 2:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

				ImGui::BeginChild("misc", ImVec2(279, 543), true); {
					ImGui::Checkbox("active", &config_system.item.misc_enabled);
					ImGui::Checkbox("clantag spammer", &config_system.item.clan_tag);
					ImGui::Checkbox("engine radar", &config_system.item.radar);

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					if (ImGui::BeginCombo("logs", "...", ImVec2(0, 65))) {
						ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 8);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player hurt"), &config_system.item.logs_player_hurt, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("player bought"), &config_system.item.logs_player_bought, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 4);
						ImGui::Selectable(("config system"), &config_system.item.logs_config_system, ImGuiSelectableFlags_::ImGuiSelectableFlags_DontClosePopups);
						ImGui::EndCombo();
					}
					ImGui::PopStyleColor();

					ImGui::Checkbox("hitmarker", &config_system.item.hitmarker);
					if (config_system.item.hitmarker) {
						ImGui::Combo("hitmarker sound", &config_system.item.hitmarker_sound, "none\0one\0two\0three");
					}
					ImGui::Checkbox("anti screenshot", &config_system.item.anti_screenshot);
					ImGui::Checkbox("spectators", &config_system.item.spectators_list);
					ImGui::Checkbox("watermark", &config_system.item.watermark);
					ImGui::Checkbox("disable post processing", &config_system.item.disable_post_processing);
					ImGui::Checkbox("recoil crosshair", &config_system.item.recoil_crosshair);
					ImGui::Checkbox("rank reveal", &config_system.item.rank_reveal);

					ImGui::Checkbox("viewmodel offset", &config_system.item.viewmodel_offset);
					if (config_system.item.viewmodel_offset) {
						ImGui::SliderInt("viewmodel x", &config_system.item.viewmodel_x, -10, 10);
						ImGui::SliderInt("viewmodel y", &config_system.item.viewmodel_y, -10, 10);
						ImGui::SliderInt("viewmodel z", &config_system.item.viewmodel_z, -10, 10);

					}

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.misc_enabled ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("dump steam id", ImVec2(84, 18))) {
						utilities::dump_steam_id();
					}

					ImGui::SameLine();
					if (ImGui::Button("hide name", ImVec2(84, 18))) {
						utilities::change_name("\n\xAD\xAD\xAD");
					}
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);
				ImGui::NextColumn();

				ImGui::BeginChild("movement", ImVec2(279, 543), true); {
					ImGui::Checkbox("- bunny hop -", &config_system.item.bunny_hop);
					if (config_system.item.bunny_hop) {
						ImGui::SliderInt("hitchance", &config_system.item.bunny_hop_hitchance, 0, 100);
						ImGui::SliderInt("minimum hops", &config_system.item.bunny_hop_minimum_value, 0, 20);
						ImGui::SliderInt("maximum hops", &config_system.item.bunny_hop_maximum_value, 0, 20);

						ImGui::Checkbox("auto strafer", &config_system.item.strafer);
					}


					ImGui::Checkbox("- edge jump -", &config_system.item.edge_jump);
					if (config_system.item.edge_jump) {
						ImGui::Checkbox("duck in air", &config_system.item.edge_jump_duck_in_air);
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			case 3:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("knife", ImVec2(279, 268), true); {
					ImGui::Checkbox("enable", &config_system.item.skinchanger_enable);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("typ", &config_system.item.knife_model, "default\0bayonet\0m9\0karambit\0bowie\0butterfly\0falchion\0flip\0gut\0huntsman\0shaddow-daggers\0navaja\0stiletto\0talon\0ursus");	
					ImGui::Combo(("skin"), &config_system.item.paint_kit_vector_index_knife, [](void* data, int idx, const char** out_text) {
						*out_text = parser_skins[idx].name.c_str();
						return true;
					}, nullptr, parser_skins.size(), 10);
					config_system.item.paint_kit_index_knife = parser_skins[config_system.item.paint_kit_vector_index_knife].id;
					ImGui::Spacing();
					ImGui::Combo("condition", &config_system.item.knife_wear, "factory-new\0minimal-wear\0field-tested\0well-worn\0battle-scarred");
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					ImGui::Spacing();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						utilities::force_update();
					}			
					ImGui::PopStyleColor();

				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("gloves", ImVec2(279, 267), true); {
					ImGui::Checkbox("enable", &config_system.item.glovechanger_enable);
					ImGui::Spacing();
					ImGui::Spacing();
					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.glovechanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					ImGui::Combo("typ", &config_system.item.glove_model, "default\0blood\0sport\0slick\0leather\0moto\0speci\0hydra");
					ImGui::Combo(("skin"), &config_system.item.paint_kit_vector_index_glove, [](void* data, int idx, const char** out_text) {
						*out_text = parser_gloves[idx].name.c_str();
						return true;
						}, nullptr, parser_gloves.size(), 10);
					config_system.item.paint_kit_index_glove = parser_gloves[config_system.item.paint_kit_vector_index_glove].id;
					ImGui::Spacing();
					ImGui::Combo("condition", &config_system.item.glove_wear, "factory-new\0minimal-wear\0field-tested\0well-worn\0battle-scarred");
					ImGui::Spacing();
					ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
					if (ImGui::Button("force update", ImVec2(84, 18))) {
						utilities::force_update();
					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("weapons", ImVec2(279, 543), true); {
					static int weapons_page = 0;
					if (ImGui::ButtonT("pistol", ImVec2(50, 30), weapons_page, 0, false, ImColor(0, 0, 0))) weapons_page = 0; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("rifle", ImVec2(50, 30), weapons_page, 1, false, false)) weapons_page = 1; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("sniper", ImVec2(50, 30), weapons_page, 2, false, false)) weapons_page = 2; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("smg", ImVec2(50, 30), weapons_page, 3, false, false)) weapons_page = 3; ImGui::SameLine(0, 0);
					if (ImGui::ButtonT("heavy", ImVec2(50, 30), weapons_page, 4, false, false)) weapons_page = 4;

					ImGui::PushStyleColor(ImGuiCol_Text, config_system.item.skinchanger_enable ? ImVec4(1.f, 1.f, 1.f, 1) : ImVec4(.6f, .6f, .6f, 1));
					switch (weapons_page) {
					case 0:
						ImGui::Combo(("p2000"), &config_system.item.paint_kit_vector_index_p2000, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p2000 = parser_skins[config_system.item.paint_kit_vector_index_p2000].id;


						ImGui::Combo(("usp-s"), &config_system.item.paint_kit_vector_index_usp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_usp = parser_skins[config_system.item.paint_kit_vector_index_usp].id;

						ImGui::Combo(("glock"), &config_system.item.paint_kit_vector_index_glock, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_glock = parser_skins[config_system.item.paint_kit_vector_index_glock].id;

						ImGui::Combo(("p250"), &config_system.item.paint_kit_vector_index_p250, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p250 = parser_skins[config_system.item.paint_kit_vector_index_p250].id;

						ImGui::Combo(("five-seven"), &config_system.item.paint_kit_vector_index_fiveseven, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_fiveseven = parser_skins[config_system.item.paint_kit_vector_index_fiveseven].id;

						ImGui::Combo(("tec9"), &config_system.item.paint_kit_vector_index_tec, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_tec = parser_skins[config_system.item.paint_kit_vector_index_tec].id;

						ImGui::Combo(("cz75a"), &config_system.item.paint_kit_vector_index_cz, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_cz = parser_skins[config_system.item.paint_kit_vector_index_cz].id;

						ImGui::Combo(("duals"), &config_system.item.paint_kit_vector_index_duals, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_duals = parser_skins[config_system.item.paint_kit_vector_index_duals].id;

						ImGui::Combo(("deagle"), &config_system.item.paint_kit_vector_index_deagle, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_deagle = parser_skins[config_system.item.paint_kit_vector_index_deagle].id;

						ImGui::Combo(("revolver"), &config_system.item.paint_kit_vector_index_revolver, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_revolver = parser_skins[config_system.item.paint_kit_vector_index_revolver].id;

						break;
					case 1:
						ImGui::Combo(("famas"), &config_system.item.paint_kit_vector_index_famas, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_famas = parser_skins[config_system.item.paint_kit_vector_index_famas].id;

						ImGui::Combo(("galil"), &config_system.item.paint_kit_vector_index_galil, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_galil = parser_skins[config_system.item.paint_kit_vector_index_galil].id;

						ImGui::Combo(("m4a4"), &config_system.item.paint_kit_vector_index_m4a4, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a4 = parser_skins[config_system.item.paint_kit_vector_index_m4a4].id;

						ImGui::Combo(("m4a1"), &config_system.item.paint_kit_vector_index_m4a1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m4a1 = parser_skins[config_system.item.paint_kit_vector_index_m4a1].id;

						ImGui::Combo(("ak47"), &config_system.item.paint_kit_vector_index_ak47, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ak47 = parser_skins[config_system.item.paint_kit_vector_index_ak47].id;

						ImGui::Combo(("sg 553"), &config_system.item.paint_kit_vector_index_sg553, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_sg553 = parser_skins[config_system.item.paint_kit_vector_index_sg553].id;

						ImGui::Combo(("aug"), &config_system.item.paint_kit_vector_index_aug, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_aug = parser_skins[config_system.item.paint_kit_vector_index_aug].id;

						break;
					case 2:
						ImGui::Combo(("ssg08"), &config_system.item.paint_kit_vector_index_ssg08, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ssg08 = parser_skins[config_system.item.paint_kit_vector_index_ssg08].id;

						ImGui::Combo(("awp"), &config_system.item.paint_kit_vector_index_awp, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_awp = parser_skins[config_system.item.paint_kit_vector_index_awp].id;

						ImGui::Combo(("scar20"), &config_system.item.paint_kit_vector_index_scar, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_scar = parser_skins[config_system.item.paint_kit_vector_index_scar].id;

						ImGui::Combo(("g3sg1"), &config_system.item.paint_kit_vector_index_g3sg1, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
						}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_g3sg1 = parser_skins[config_system.item.paint_kit_vector_index_g3sg1].id;

						break;
					case 3:
						ImGui::Combo(("bizon"), &config_system.item.paint_kit_vector_index_bizon, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_bizon = parser_skins[config_system.item.paint_kit_vector_index_bizon].id;

						ImGui::Combo(("mp5"), &config_system.item.paint_kit_vector_index_mp5sd, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp5sd = parser_skins[config_system.item.paint_kit_vector_index_mp5sd].id;

						ImGui::Combo(("mp7"), &config_system.item.paint_kit_vector_index_mp7, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp7 = parser_skins[config_system.item.paint_kit_vector_index_mp7].id;

						ImGui::Combo(("mp9"), &config_system.item.paint_kit_vector_index_mp9, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mp9 = parser_skins[config_system.item.paint_kit_vector_index_mp9].id;

						ImGui::Combo(("mac10"), &config_system.item.paint_kit_vector_index_mac10, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mac10 = parser_skins[config_system.item.paint_kit_vector_index_mac10].id;

						ImGui::Combo(("p90"), &config_system.item.paint_kit_vector_index_p90, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_p90 = parser_skins[config_system.item.paint_kit_vector_index_p90].id;

						ImGui::Combo(("ump45"), &config_system.item.paint_kit_vector_index_ump45, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_ump45 = parser_skins[config_system.item.paint_kit_vector_index_ump45].id;


						break;
					case 4:
						ImGui::Combo(("sawoff"), &config_system.item.paint_kit_vector_index_sawoff, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_sawoff = parser_skins[config_system.item.paint_kit_vector_index_sawoff].id;

						ImGui::Combo(("m249"), &config_system.item.paint_kit_vector_index_m249, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_m249 = parser_skins[config_system.item.paint_kit_vector_index_m249].id;

						ImGui::Combo(("negev"), &config_system.item.paint_kit_vector_index_negev, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_negev = parser_skins[config_system.item.paint_kit_vector_index_negev].id;

						ImGui::Combo(("mag7"), &config_system.item.paint_kit_vector_index_mag7, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_mag7 = parser_skins[config_system.item.paint_kit_vector_index_mag7].id;

						ImGui::Combo(("xm1014"), &config_system.item.paint_kit_vector_index_xm1014, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_xm1014 = parser_skins[config_system.item.paint_kit_vector_index_xm1014].id;

						ImGui::Combo(("nova"), &config_system.item.paint_kit_vector_index_nova, [](void* data, int idx, const char** out_text) {
							*out_text = parser_skins[idx].name.c_str();
							return true;
							}, nullptr, parser_skins.size(), 10);
						config_system.item.paint_kit_index_nova = parser_skins[config_system.item.paint_kit_vector_index_nova].id;

						break;

					}
					ImGui::PopStyleColor();
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;

			case 4:
				ImGui::Columns(2, NULL, false);
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
				ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
				ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));
				ImGui::BeginChild("config", ImVec2(279, 268), true); {
					constexpr auto& config_items = config_system.get_configs();
					static int current_config = -1;

					if (static_cast<size_t>(current_config) >= config_items.size())
						current_config = -1;

					static char buffer[16];

					if (ImGui::ListBox("", &current_config, [](void* data, int idx, const char** out_text) {
						auto& vector = *static_cast<std::vector<std::string>*>(data);
						*out_text = vector[idx].c_str();
						return true;
						}, & config_items, config_items.size(), 5) && current_config != -1)
						strcpy(buffer, config_items[current_config].c_str());

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						ImGui::PushID(0);
						ImGui::PushItemWidth(178);
						if (ImGui::InputText("", buffer, IM_ARRAYSIZE(buffer), ImGuiInputTextFlags_EnterReturnsTrue)) {
							if (current_config != -1)
								config_system.rename(current_config, buffer);
						}
						ImGui::PopID();
						ImGui::NextColumn();

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (ImGui::Button(("create"), ImVec2(85, 20))) {
							config_system.add(buffer);
						}

						ImGui::SameLine();

						if (ImGui::Button(("reset"), ImVec2(85, 20))) {
							config_system.reset();

						}

						ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
						if (current_config != -1) {
							if (ImGui::Button(("load"), ImVec2(85, 20))) {
								config_system.load(current_config);

								load_config = true;

								if (config_system.item.logs_config_system) {
									utilities::console_warning("[config system] ");
									interfaces::console->console_printf(config_items[current_config].c_str());
									interfaces::console->console_printf(" loaded. \n");
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " loaded.";
									event_logs.add(ss.str(), color(167, 255, 255, 255));
								}
								utilities::force_update();
							}

							ImGui::SameLine();

							if (ImGui::Button(("save"), ImVec2(85, 20))) {
								config_system.save(current_config);

								save_config = true;

								if (config_system.item.logs_config_system) {
									utilities::console_warning("[config system] ");
									interfaces::console->console_printf(config_items[current_config].c_str());
									interfaces::console->console_printf(" saved. \n");
									std::stringstream ss;
									ss << config_items[current_config].c_str() << " saved.";
									event_logs.add(ss.str(), color(167, 255, 255, 255));
								}

							}

							ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 18);
							if (ImGui::Button(("remove"), ImVec2(85, 20))) {
								config_system.remove(current_config);
							}
						}
				}
				ImGui::EndChild(true);

				ImGui::PopStyleVar();
				ImGui::Dummy(ImVec2(0, -2)); ImGui::SameLine();
				ImGui::Dummy(ImVec2(0, 0)); ImGui::SameLine();
				ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));

				ImGui::BeginChild("settings", ImVec2(279, 267), true); {
					ImGui::Combo("keybinds", &config_system.item.keybinds_selection, "edgejump\0aimbot\0visuals\0entity");

					if (config_system.item.keybinds_selection == 0) {
						ImGui::Hotkey("##edge jump key", &config_system.item.edge_jump_key, ImVec2(100, 20));
					}
					else if (config_system.item.keybinds_selection == 1) {
						ImGui::Hotkey("##aimbot key", &config_system.item.aim_key, ImVec2(100, 20));
					}
					else if (config_system.item.keybinds_selection == 2) {
						ImGui::Hotkey("##visuals key", &config_system.item.visuals_key, ImVec2(100, 20));
					}
					else if (config_system.item.keybinds_selection == 3) {
						ImGui::Hotkey("##entity key", &config_system.item.entity_visuals_key, ImVec2(100, 20));
					}
					
				}
				ImGui::EndChild(true);

				ImGui::NextColumn();

				ImGui::BeginChild("info", ImVec2(279, 543), true); {
					char buffer[UNLEN + 1];
					DWORD size;
					size = sizeof(buffer);
					GetUserName(buffer, &size);
					char title[UNLEN];
					char ch1[25] = "welcome, ";
					char *ch = strcat(ch1, buffer);

					ImGui::Text(ch);
					ImGui::Text("build: " __DATE__ " / " __TIME__);
					if (std::strstr(GetCommandLineA(), "-insecure")) {
						ImGui::Text("insecure mode!");
					}
				}
				ImGui::EndChild(true);
				ImGui::PopStyleVar();
				ImGui::PopStyleColor();
				ImGui::PopStyleColor();
				ImGui::Columns();
				break;
			}
		}

		ImGui::PopFont();

		ImGui::End();
	}
}

void c_menu::run_popup() {
	ImGui::PushFont(font_menu);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(16, 16));
	ImGui::PushStyleColor(ImGuiCol_ChildWindowBg, ImVec4(30 / 255.f, 30 / 255.f, 39 / 255.f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0 / 255.f, 0 / 255.f, 0 / 255.f, 0.1f));

	if (save_config) {
		bool done = false;
		ImGui::begin_popup("config saved.", 2000, &done);
		if (done)
			save_config = false;
	}

	if (load_config) {
		bool done = false;
		ImGui::begin_popup("config loaded.", 2000, &done);
		if (done)
			load_config = false; 
	}

	ImGui::PopStyleColor();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar();
	ImGui::PopFont();
}

void c_menu::run_visuals_preview() {
	static std::vector<esp_info_s> info;
	static bool enabled = true;

	ImGui::SetNextWindowSize(ImVec2(235, 400));

	ImGui::Begin("ESP Preview", &enabled, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_ShowBorders | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar); {
		const auto cur_window = ImGui::GetCurrentWindow();
		const ImVec2 w_pos = cur_window->Pos;

		ImVec2 p = ImGui::GetCursorScreenPos();
		ImColor c = ImColor(32, 114, 247);

		//title bar
		ImGui::GetWindowDrawList()->AddRectFilled(ImVec2(p.x - 20, p.y + 15), ImVec2(p.x + ImGui::GetWindowWidth(), p.y - 40), ImColor(30, 30, 39));

		//draw gradient bellow title bar
		ImGui::GetWindowDrawList()->AddRectFilledMultiColor(ImVec2(p.x - 20, p.y + 14), ImVec2(p.x + ImGui::GetWindowWidth(), p.y + 16), ImColor(167, 24, 71, 255), ImColor(58, 31, 87, 255), ImColor(58, 31, 87, 255), ImColor(167, 24, 71, 255));

		ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 9); //góra, dół
		ImGui::SetCursorPosX(ImGui::GetCursorPosX() - 8); //lewo prawo
		//render cheat name
		ImGui::Text("visuals preview");


		if (config_system.item.player_box) {
			cur_window->DrawList->AddRect(ImVec2(w_pos.x + 40, w_pos.y + 60), ImVec2(w_pos.x + 200, w_pos.y + 360), ImGui::GetColorU32(ImGuiCol_Text));
		}

		if (config_system.item.player_health)
			cur_window->DrawList->AddRectFilled(ImVec2(w_pos.x + 34, w_pos.y + 60), ImVec2(w_pos.x + 36, w_pos.y + 360), ImGui::GetColorU32(ImVec4(83 / 255.f, 200 / 255.f, 84 / 255.f, 255 / 255.f)));

		if (config_system.item.player_name)
			info.emplace_back(esp_info_s("name", color(255, 255, 255, 255), CENTER_UP));

		if (config_system.item.player_weapon)
			info.emplace_back(esp_info_s("awp", color(255, 255, 255, 255), CENTER_DOWN));

		if (config_system.item.player_flags_armor)
			info.emplace_back(esp_info_s("hk", color(255, 255, 255, 255), RIGHT));


		for (auto render : info) {
			const auto text_size = ImGui::CalcTextSize(render.f_name.c_str());

			auto pos = ImVec2(w_pos.x + 205, w_pos.y + 60);

			if (render.f_position == CENTER_DOWN) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y + 315 - text_size.y);
			}
			else if (render.f_position == CENTER_UP) {
				pos = ImVec2(w_pos.x + (240 / 2) - text_size.x / 2, pos.y - 5 - text_size.y);
			}

			cur_window->DrawList->AddText(pos, ImGui::GetColorU32(ImVec4(255 / 255.f, 255 / 255.f, 255 / 255.f, 255 / 255.f)), render.f_name.c_str());
		}
	}
	ImGui::End();

	info.clear();
}
















































