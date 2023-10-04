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
#include <iostream>

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
	
	
	// create ray direction vector
	/////////////////////////////////////////
	for (int px{}; px < m_Width; ++px)
	{
			float pxc = float(px) + .5f;
			x = (2 * pxc / float(m_Width)-1) * ar * fov;

		for (int py{}; py < m_Height; ++py)
		{
			ColorRGB finalColor{};
			float pyc = float(py) + .5f;
			y = (1 - 2 * pyc / float(m_Height)) * fov;
			ray.direction.x = x;
			ray.direction.y = y;
			ray.direction.z = 1.0f;
			ray.direction.Normalize();
			ray.direction = camera.cameraToWorld.TransformVector(ray.direction).Normalized();

			//raytrace scene
			pScene->GetClosestHit(ray, hitRecord);

			//Check what the camera is seeing
			if (hitRecord.didHit)
			{
				//Check lighting on the location spotted by camera
				for (int i{}; i < lights.size(); ++i)
				{
					Vector3 lightDir{ LightUtils::GetDirectionToLight(lights[i], hitRecord.origin) };
					Vector3 lightDirNrm{ lightDir.Normalized()};
					Ray LightRay{ {hitRecord.origin + hitRecord.normal * 0.001f} ,lightDirNrm };
					LightRay.max = (lights[i].origin - hitRecord.origin).Magnitude();
					LightRay.min = 0.0001f;

					//if shadows enabled ,put shadow
					if (m_ShadowEnabled && pScene->DoesHit(LightRay) )
					{
						finalColor += {};
					}
					else
					{
						switch (m_LightingMode)
						{
						case LightingMode::ObservedArea:
							finalColor.r += (CalculateObservedArea(lightDir, hitRecord.normal));
							finalColor.g += (CalculateObservedArea(lightDir, hitRecord.normal));
							finalColor.b += (CalculateObservedArea(lightDir, hitRecord.normal));
							break;

						case LightingMode::Radiance:
							finalColor += LightUtils::GetRadiance(lights[i], hitRecord.origin);
							break;

						case LightingMode::BRDF:
							finalColor += materials[hitRecord.materialIndex]->Shade(hitRecord, -lightDirNrm, -ray.direction);
							break;

						case LightingMode::Combined:
							finalColor +=
								LightUtils::GetRadiance(lights[i], hitRecord.origin) *
								materials[hitRecord.materialIndex]->Shade(hitRecord, -lightDirNrm, -ray.direction) *
								CalculateObservedArea(lightDir, hitRecord.normal);
							break;
						default:
							break;
						}

					}
					
				}
				//hitrecord set false to continue search for hits with smaller t-value (closer objects)
				hitRecord.didHit = false;
			}
			//if no hit on an object in world --> finalcolor = 0
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

void Renderer::Input()
{
const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	if (pKeyboardState[SDL_SCANCODE_F2])
	{
		ToggleShadows();
		//std::cout << "F2\n";
	}
	if (pKeyboardState[SDL_SCANCODE_F3])
	{
		CyclelightingMode();
		//std::cout << "F3\n";
	}
}

//cosine Law
float Renderer::CalculateObservedArea(const Vector3& light, const Vector3& normal)const
{
	float value{ Vector3::Dot(light, normal) / light.Magnitude() };
	value = (value > 0.f) ? value : 0.f;
	return value;
}

void dae::Renderer::CyclelightingMode()
{
	m_LightingMode = static_cast<LightingMode>((int(m_LightingMode) + 1) % 4);
	//std::cout << int(m_LightingMode) << "\n";
}


