#pragma once	
#include "sdk.h"
#include "iostream"
#include "algorithm"


struct vec {
	float x, y, z;
	vec operator-(const vec& other) const {
		return { x - other.x, y - other.y, z - other.z };
	}
	vec operator/(float val) const {
		return { x / val, y / val, z / val };
	}
	float length() const {
		return sqrtf(x * x + y * y + z * z);
	}
};

struct extentity;

namespace hooks
{
	
	//void SDL_GL_SwapWindow(SDL_Window * window);

	using TSDLSwapWindow = void(*)(void*);
	inline bool is_initialized{};
	inline void* p_sdl_swap_window{};
	inline TSDLSwapWindow p_original_swap_window;

	using raycube_t = float(__fastcall*)(const vec* from, const vec* to, float radius, int32_t mode, int32_t size, extentity* t);


	
	inline raycube_t raycube{};  //sauerbraten.exe + 0x1142C0
	bool install();
	void remove();

	bool IsVisible(const vec& from, const vec& to);

	void SwapWindow(void* p_window);
}




