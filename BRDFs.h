#pragma once
#include <cassert>
#include "Math.h"
#include <iostream>

namespace dae
{
	namespace BRDF
	{
		/**
		 * \param kd Diffuse Reflection Coefficient
		 * \param cd Diffuse Color
		 * \return Lambert Diffuse Color
		 */
		static ColorRGB Lambert(float kd, const ColorRGB& cd)
		{			
			return {(kd * cd) / dae::PI};
		}

		static ColorRGB Lambert(const ColorRGB& kd, const ColorRGB& cd)
		{
			//todo: W3

			return {};
		}

		/**
		 * \brief todo
		 * \param ks Specular Reflection Coefficient
		 * \param exp Phong Exponent
		 * \param l Incoming (incident) Light Direction
		 * \param v View Direction
		 * \param n Normal of the Surface
		 * \return Phong Specular Color
		 */
		static ColorRGB Phong(float ks, float exp, const Vector3& l, const Vector3& v, const Vector3& n)
		{
			Vector3 reflect{ Vector3::Reflect(l, n).Normalized()};
			//float cosAlp{ (Vector3::Dot(reflect, v) > 0.f)? Vector3::Dot(reflect, v)  : 0.f };
			float cosAlp{ std::max(0.f, Vector3::Dot(reflect, v))};
			float value{ ks * powf(cosAlp , exp) };
			return {value, value, value};
		}

		/**
		 * \brief BRDF Fresnel Function >> Schlick
		 * \param h Normalized Halfvector between View and Light directions
		 * \param v Normalized View direction
		 * \param f0 Base reflectivity of a surface based on IOR (Indices Of Refrection), this is different for Dielectrics (Non-Metal) and Conductors (Metal)
		 * \return
		 */
		static ColorRGB FresnelFunction_Schlick(const Vector3& h, const Vector3& v, const ColorRGB& f0)
		{
			ColorRGB one{ 1.f, 1.f, 1.f };

			return { f0 + (one - f0) * powf(1.f - fmaxf(Vector3::Dot(v,h), 0.f), 5.f) };
		}

		/**
		 * \brief BRDF NormalDistribution >> Trowbridge-Reitz GGX (UE4 implemetation - squared(roughness))
		 * \param n Surface normal
		 * \param h Normalized half vector
		 * \param roughness Roughness of the material
		 * \return BRDF Normal Distribution Term using Trowbridge-Reitz GGX
		 */
		static float NormalDistribution_GGX(const Vector3& n, const Vector3& h, float roughness)
		{
			float alphSqr{ roughness * roughness };
			return { alphSqr / (float(M_PI) * powf(  powf(Vector3::Dot(n, h), 2.f) *( alphSqr - 1.f) +1.f, 2.f  ) )};
		}


		/**
		 * \brief BRDF Geometry Function >> Schlick GGX (Direct Lighting + UE4 implementation - squared(roughness))
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using SchlickGGX
		 */
		static float GeometryFunction_SchlickGGX(const Vector3& n, const Vector3& v, float roughness)
		{
			float dotNV{ fmaxf(Vector3::Dot(n, v), 0.f) };
			float kDirect{ powf(roughness + 1.f, 2) / 8.f };
			return {dotNV/ (dotNV * (1.f - kDirect) + kDirect)};
		}

		/**
		 * \brief BRDF Geometry Function >> Smith (Direct Lighting)
		 * \param n Normal of the surface
		 * \param v Normalized view direction
		 * \param l Normalized light direction
		 * \param roughness Roughness of the material
		 * \return BRDF Geometry Term using Smith (> SchlickGGX(n,v,roughness) * SchlickGGX(n,l,roughness))
		 */
		static float GeometryFunction_Smith(const Vector3& n, const Vector3& v, const Vector3& l, float roughness)
		{
			float dotNL{ fmaxf(Vector3::Dot(n, l), 0.f) };
			float kDirect{ powf(roughness + 1.f, 2) / 8.f };
			return { (dotNL / (dotNL * (1.f - kDirect) + kDirect)) * GeometryFunction_SchlickGGX(n, v, roughness)};
		}

	}
}