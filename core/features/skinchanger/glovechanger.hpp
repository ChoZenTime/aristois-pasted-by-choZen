#pragma once
#include "../../../dependencies/common_includes.hpp"
class c_glovechanger {
public:
	bool apply_glove_model(attributable_item_t* glove, const char* model) noexcept;
	bool apply_glove_skin(attributable_item_t* glove, int item_definition_index, int paint_kit, int model_index, int entity_quality, float fallback_wear) noexcept;
	void run() noexcept;
}; 
extern c_glovechanger glove_changer;