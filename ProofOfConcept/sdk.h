#pragma once

#include <cstdint>
#include "offsets.h"
#include "Player.h"
#include "math.h"

namespace sdk
{
	inline bool is_Initialized{};

	inline uintptr_t h_sauerbraten_client{};
	inline Player* p_localplayer{};
	inline uintptr_t base{};
	inline uintptr_t* entityList{};
	
	inline int lobbySize{};
	inline Matrix4x4* viewMatrix{};
	inline Matrix4x4* ProjectionMatrix{};
	inline Matrix4x4* viewProjectionMatrix{};

	
	
	

	bool initialize();
	void drawline(Vector2 from, Vector2 to, float r, float g, float b, float thickness);

}