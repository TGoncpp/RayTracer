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
	Camera& camera  = pScene->GetCamera();
	auto& materials = pScene->GetMaterials();
	auto& lights    = pScene->GetLights();
	
	float x{}, y{};
	const float 
		ar{ float(m_Width) / float(m_Height) },
		fov{ tanf(dae::TO_RADIANS * camera.fovAngle/2.0f) };
	Ray ray{ camera.origin, {} };
	HitRecord hitRecord{};
	ColorRGB finalColor{};
	
	for (int px{}; px < m_Width; ++px)
	{
			float pxc = float(px) + .5f;
			x = (2 * pxc / float(m_Width)-1) * ar * fov;

		for (int py{}; py < m_Height; ++py)
		{
			// create ray direction vector
			float pyc = float(py) + .5f;
			y = (1 - 2 * pyc / float(m_Height)) * fov;
			ray.direction.x = x;
			ray.direction.y = y;
			ray.direction.z = 1.0f;
			ray.direction.Normalize();
			ray.direction = camera.cameraToWorld.TransformVector(ray.direction).Normalized();

			//raytrace scene
			pScene->GetClosestHit(ray, hitRecord);

			//render
			if (hitRecord.didHit)
			{
				finalColor = materials[hitRecord.materialIndex]->Shade();
				for (int i{}; i < lights.size(); ++i)
				{
					if (pScene->DoesHit({
						hitRecord.origin,														  	  //origin
						(LightUtils::GetDirectionToLight(lights[i], hitRecord.origin)).Normalized() })) // direction
					{
						finalColor *= 0.5f;
					}
				}
				hitRecord.didHit = false;
			}
			else
			{
				finalColor = {};
			}

#pragma region comment
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
#pragma endregion 
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
