#pragma once
#include "../../../dependencies/common_includes.hpp"

class c_misc {
public:
	std::vector<int> get_observervators(int playerid) noexcept;
	color cl_player_obs;
	void remove_smoke() noexcept;
	void remove_flash() noexcept;
	void rank_reveal() noexcept;
	void remove_scope() noexcept;	
	void spectators() noexcept;
	void watermark() noexcept;
	void clantag_spammer() noexcept;
	void viewmodel_offset() noexcept;
	void disable_post_processing() noexcept;
	void recoil_crosshair() noexcept;
	void force_crosshair() noexcept;
private:
};

extern c_misc misc;
