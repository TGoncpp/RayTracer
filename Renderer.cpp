//External includes
#include "SDL.h"
#include "SDL_surface.h"

//Project includes
#include "Renderer.h"
#include "Math.h"
#include "Matrix.h"
#include "Material.h"
#include "Scene.h"
#include "Utils.h"

using namespace dae;

Renderer::Renderer(SDL_Window * pWindow) :
	m_pWindow(pWindow),
	m_pBuffer(SDL_GetWindowSurface(pWindow))
{
	//Initialize
	SDL_GetWindowSize(pWindow, &m_Width, &m_Height);
	m_pBufferPixels = static_cast<uint32_t*>(m_pBuffer->pixels);
}

void Renderer::Render(Scene* pScene) const
{
	Camera& camera = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights = pScene->GetLights();


	//Vector3 direction{ 0,0,1 };
	//Ray ray{ {0,0,0}, direction };
	//HitRecord hitRecord{};
	//float ar = static_cast<float>(m_Width) / static_cast<float>(m_Height);

	//Sphere sphere;
	////sphere.origin = { m_Width / 2.0f, m_Height / 2.0f, 0.0f };
	//sphere.origin = { 0.0f, 0.0f, 100.0f };
	//sphere.radius = 50.0f;
	//ColorRGB finalColor{  };

	//for (int px{}; px < m_Width; ++px)
	//{

	//	for (int py{}; py < m_Height; ++py)
	//	{

	//		float pxc{ (float)px + 0.5f };
	//		direction.x = ( (2 * pxc) / static_cast<float>(m_Width) - 1 ) * ar;

	//		direction.y = 1 - ((2 * (static_cast<float>(py) + 0.5f)) / static_cast<float>(m_Height) );

	//		direction.Normalize();
	//		ray.direction = direction;

	//		if (GeometryUtils::HitTest_Sphere(sphere, ray, hitRecord))
	//		{
	//			const float scale_t{ (hitRecord.t - 50.0f) / 40.0f };
	//			ColorRGB Color{ scale_t,scale_t, scale_t };
	//			finalColor = Color;
	//		}

	//		else
	//		{
	//			ColorRGB Color{ 0,0, 0 };
	//			finalColor = Color;
	//		}


	//		//Update Color in Buffer
	//		finalColor.MaxToOne();

	//		m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
	//			static_cast<uint8_t>(finalColor.r * 255),
	//			static_cast<uint8_t>(finalColor.g * 255),
	//			static_cast<uint8_t>(finalColor.b * 255));
	//	}
	//}

	////@END
	////Update SDL Surface
	//SDL_UpdateWindowSurface(m_pWindow);



	float x{}, y{};
	float ar = float(m_Width) / float(m_Height);
	Ray ray{ pScene->GetCamera().origin, {} };
	HitRecord hitRecord{};
	ColorRGB finalColor{  };


	for (int px{}; px < m_Width; ++px)
	{
			float pxc = float(px) + .5f;
			x = (2 * pxc / float(m_Width)-1) * ar;

		for (int py{}; py < m_Height; ++py)
		{
			// create ray direction vector
			float pyc = float(py) + .5f;
			y = 1 - 2 * pyc / float(m_Height);
			ray.direction.x = x;
			ray.direction.y = y;
			ray.direction.z = 1.0f;
			ray.direction.Normalize();

			//raytrace scene
			pScene->GetClosestHit(ray, hitRecord);

			//render
			if (hitRecord.didHit)
			{
				finalColor = materials[hitRecord.materialIndex]->Shade();
				hitRecord.didHit = false;
			}
			else
			{
				finalColor = {};
			}

			/////////////////////////////////////////////////////////////////////////////////////////
			////check for correct raydirection 
			//ColorRGB finalColor{ ray.direction.x ,ray.direction.y, ray.direction.z };

			////////////////////////////////////////////////////////////////////////////////////////
			
			////check plane test
			//Plane testPlane{ {0.0f,-50.0f, 0.0f }, {.0f, 1.0f, .0f}, 0 };
			//if (GeometryUtils::HitTest_Plane(testPlane, ray, hitRecord))
			//{
			//	const float test_t{ hitRecord.t / 500.f };
			//	//finalColor = materials[hitRecord.materialIndex]->Shade();
			//	ColorRGB color{ test_t, test_t, test_t };
			//	finalColor = color;
			//}
			//else
			//{
			//	finalColor = {};
			//}

			//////////////////////////////////////////////////////////////////////////////////////////
			//Update Color in Buffer
			finalColor.MaxToOne();

			m_pBufferPixels[px + (py * m_Width)] = SDL_MapRGB(m_pBuffer->format,
				static_cast<uint8_t>(finalColor.r * 255),
				static_cast<uint8_t>(finalColor.g * 255),
				static_cast<uint8_t>(finalColor.b * 255));
		}
	}

	//@END
	//Update SDL Surface
	SDL_UpdateWindowSurface(m_pWindow);

}


bool Renderer::SaveBufferToImage() const
{
	return SDL_SaveBMP(m_pBuffer, "RayTracing_Buffer.bmp");
}
