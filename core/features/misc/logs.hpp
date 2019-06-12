#pragma once
#include <deque>
#include "../../../dependencies/common_includes.hpp"

#include "damage_indicator.hpp"

class c_event_logs {
public:
	void run() noexcept;
	void event_item_purchase(i_game_event * event) noexcept;
	void event_player_hurt(i_game_event * event) noexcept;
	void add(std::string text, color colors) noexcept;

private:
	struct loginfo_t {
		loginfo_t(const float log_time, std::string message, const color colors) {
			this->log_time = log_time;
			this->message = message;
			this->colors = colors;

			this->x = 6.f;
			this->y = 0.f;
		}

		float log_time;
		std::string message;
		color colors;
		float x, y;
	};

	std::deque< loginfo_t > logs;
};

extern c_event_logs event_logs;