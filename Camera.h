#pragma once
#include <cassert>
#include <SDL_keyboard.h>
#include <SDL_mouse.h>

#include "Math.h"
#include "Timer.h"

namespace dae
{
	struct Camera
	{
		Camera() = default;
		Camera(const Vector3& _origin, float _fovAngle):

			origin{_origin},
			fovAngle{_fovAngle}
		{
		}


		Vector3 origin{};
		float fovAngle{90.f};

		Vector3 forward{Vector3::UnitZ};
		Vector3 up{Vector3::UnitY};
		Vector3 right{Vector3::UnitX};

		float totalPitch{0.f};
		float totalYawn{0.f};

		Matrix cameraToWorld{};



		Matrix CalculateCameraToWorld()
		{
			Matrix newRotion{
					Matrix::CreateRotation({totalPitch, totalYawn, 0.f}) };
			forward = newRotion.TransformVector(Vector3::UnitZ);
			forward.Normalize();

			Vector3 worldUp{0.f, 1.f, 0.f};
			//forward.Normalize();
			right = Vector3::Cross(worldUp, forward).Normalized();
			up    = Vector3::Cross(forward, right).Normalized();

			return
			{
					{		right   },
					{		up		},
					{		forward },
					{		origin	}

			};
		}

		void Update(Timer* pTimer)
		{
			const float deltaTime = pTimer->GetElapsed();
			float 
				scrollSpeed{ 7.f },
				swipeSpeed{ 0.25f },
				rotationSpeed{ 0.05f };

			//Keyboard Input
			//*********************************
#pragma region KeyboardInput
			//camera movement

			const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
			if (pKeyboardState[SDL_SCANCODE_W])
			{
				origin += forward * scrollSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_S])
			{
				origin -= forward * scrollSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_D])
			{
				origin += right * scrollSpeed * deltaTime;
			}
			if (pKeyboardState[SDL_SCANCODE_A])
			{
				origin -= right * scrollSpeed * deltaTime;
			}

			

#pragma endregion

#pragma region MouseInput

			//Mouse Input
			//move forward/sideways
			int mouseX{}, mouseY{};
			const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);

			if (SDL_BUTTON(1) == mouseState )
			{
				origin -= forward * swipeSpeed * deltaTime * float(mouseY);
				origin -= right   * swipeSpeed * deltaTime * float(mouseX);
			}

			//Rotation
			if ( SDL_BUTTON(3) == mouseState)
			{
				totalPitch += float(mouseY) * deltaTime * rotationSpeed;
				totalYawn  += float(mouseX) * deltaTime * rotationSpeed;
				/*Matrix newRotion{
					Matrix::CreateRotation({totalPitch, totalYawn, 0.f}) };
				forward = newRotion.TransformVector(Vector3::UnitZ);
				forward.Normalize();*/
				cameraToWorld = CalculateCameraToWorld();
				
			}


#pragma endregion

		}
	};
}
