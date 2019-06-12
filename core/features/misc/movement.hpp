#pragma once

class c_movement {
public:
	void bunnyhop(c_usercmd* user_cmd) noexcept;
	void edge_jump_pre_prediction(c_usercmd * user_cmd) noexcept;
	void edge_jump_post_prediction(c_usercmd * user_cmd) noexcept;
};

extern c_movement movement;