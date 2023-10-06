#pragma once

#include <cstdint>

struct SDL_Window;
struct SDL_Surface;

namespace dae
{
	class Scene;
	struct Vector3;

	class Renderer final
	{
	public:
		Renderer(SDL_Window* pWindow);
		~Renderer() = default;

		Renderer(const Renderer&) = delete;
		Renderer(Renderer&&) noexcept = delete;
		Renderer& operator=(const Renderer&) = delete;
		Renderer& operator=(Renderer&&) noexcept = delete;

		void Input();

		void Render(Scene* pScene) const;
		bool SaveBufferToImage() const;

		void CyclelightingMode();
		void ToggleShadows() { m_ShadowEnabled = !m_ShadowEnabled; }

	private:
		SDL_Window* m_pWindow{};

		SDL_Surface* m_pBuffer{};
		uint32_t* m_pBufferPixels{};

		int m_Width{};
		int m_Height{};

		enum class LightingMode
		{
			ObservedArea, //Lambert Cosine Law
			Radiance,     // Incident Radiance
			BRDF,         //Scattering of the light
			Combined      //ObservedArea * Radiance * BRDF
		};

		bool m_ShadowEnabled{ true };
		LightingMode m_LightingMode{ LightingMode::BRDF };

		float CalculateObservedArea(const Vector3& light, const Vector3& normal)const;
	};
}
