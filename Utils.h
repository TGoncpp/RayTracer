#pragma once
#include <cassert>
#include <fstream>
#include "Math.h"
#include "DataTypes.h"

namespace dae
{
	namespace GeometryUtils
	{
#pragma region Sphere HitTest
		//SPHERE HIT-TESTS
		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{		
		
			//const float A{ Vector3::Dot(ray.direction, ray.direction) };//always 1.0f!!!!!!!!!!!!!!!
			const float B{ 2.0f * Vector3::Dot(ray.origin - sphere.origin, ray.direction) };
			const float C{ Vector3::Dot(ray.origin - sphere.origin, ray.origin - sphere.origin) - sphere.radius * sphere.radius };
			const float Discrimenant{ (B * B - 4.0f /** A*/ * C) };

			if (Discrimenant < 0 )
				return false;

			float t{ (-B - sqrtf(Discrimenant)) *0.5f };
			//float t{ (-B - sqrtf(Discrimenant)) / (2.0f /** A*/) };

			if (t < 0.f)
				t = (-B + sqrtf(Discrimenant)) *0.5f;
				//t = (-B + sqrtf(Discrimenant)) / (2.0f /** A*/);

			//if t is invallid-> return
			if (t >= ray.max || t < ray.min  )
				return false;

			//fill in hit record on first hit
			if (!hitRecord.didHit )
			{
				hitRecord.t             = t;
				hitRecord.didHit        = true;  
				if (ignoreHitRecord)return true;
				hitRecord.origin        = ray.origin + ray.direction * t;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.normal        = (hitRecord.origin - sphere.origin) / sphere.radius;
				return true;	
			}

			//if hitrecord was already filled, check new t-value
			else if (hitRecord.didHit && t < hitRecord.t)
			{
				hitRecord.t             = t;
				if (ignoreHitRecord)return true;
				hitRecord.origin        = ray.origin + ray.direction * t;
				hitRecord.materialIndex = sphere.materialIndex;
				hitRecord.normal        = (hitRecord.origin - sphere.origin) / sphere.radius;
				return true;
			}
			return false;
		}

		inline bool HitTest_Sphere(const Sphere& sphere, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Sphere(sphere, ray, temp, true);
		}
#pragma endregion
#pragma region Plane HitTest
		//PLANE HIT-TESTS
		inline bool HitTest_Plane(const Plane& plane, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			const float t = Vector3::Dot((plane.origin - ray.origin), plane.normal) / Vector3::Dot(ray.direction, plane.normal);
			
			//if t is invallid-> return
			if (t > ray.max || t < ray.min)return false;
			//check first hit
			if (!hitRecord.didHit) 
			{
				hitRecord.t             = t;
				hitRecord.didHit        = true;
				if (ignoreHitRecord)return true;
				hitRecord.origin        = ray.origin + ray.direction * hitRecord.t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal		= plane.normal;
				return true;

			}
			//check t for smaller value when was already hit
			else if (hitRecord.didHit && t < hitRecord.t )
			{
				hitRecord.t             = t;
				if (ignoreHitRecord)return true;
				hitRecord.origin        = ray.origin + ray.direction * hitRecord.t;
				hitRecord.materialIndex = plane.materialIndex;
				hitRecord.normal        = plane.normal;
				return true;

			}
			return false;
		}

		inline bool HitTest_Plane(const Plane& plane, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Plane(plane, ray, temp, true);
		}
#pragma endregion
#pragma region Triangle HitTest
		//TRIANGLE HIT-TESTS
		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			
			switch(triangle.cullMode)
			{
			case TriangleCullMode::NoCulling:

				if (Vector3::Dot(ray.direction, triangle.normal) == 0 ) return false;
				break;

			case TriangleCullMode::BackFaceCulling:

				if (Vector3::Dot(ray.direction, triangle.normal) > 0 && !ignoreHitRecord) return false;
				if (Vector3::Dot(ray.direction, triangle.normal) < 0 && ignoreHitRecord) return false;
				break;

			case TriangleCullMode::FrontFaceCulling:

				if (Vector3::Dot(ray.direction, triangle.normal ) < 0 && !ignoreHitRecord) return false;
				if (Vector3::Dot(ray.direction, triangle.normal) > 0  &&  ignoreHitRecord) return false;
				break;

			default:
				break;

			}


			const float t = Vector3::Dot((triangle.v0 - ray.origin), triangle.normal) / Vector3::Dot(ray.direction, triangle.normal);
			if (t > ray.max || t < ray.min)return false;
			Vector3 pointOnPlane = ray.origin + ray.direction * t;

			//Check if point is on triangle
			Vector3 
				edge{ triangle.v1 - triangle.v0 },
				edgeToCompare{pointOnPlane - triangle.v0 };
			if (Vector3::Dot(Vector3::Cross(triangle.v1 - triangle.v0, pointOnPlane - triangle.v0), triangle.normal) <= 0.0f) { return false; };
			if (Vector3::Dot(Vector3::Cross(triangle.v2 - triangle.v1, pointOnPlane - triangle.v1), triangle.normal) <= 0.0f) { return false; };
			if (Vector3::Dot(Vector3::Cross(triangle.v0 - triangle.v2, pointOnPlane - triangle.v2), triangle.normal) <= 0.0f) { return false; };


			//check first hit
			if (!hitRecord.didHit)
			{
				hitRecord.t = t;
				hitRecord.didHit = true;
				if (ignoreHitRecord)return true;
				hitRecord.origin = ray.origin + ray.direction * hitRecord.t;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				return true;

			}
			//check t for smaller value when was already hit
			else if (hitRecord.didHit && t < hitRecord.t)
			{
				hitRecord.t = t;
				if (ignoreHitRecord)return true;
				hitRecord.origin = ray.origin + ray.direction * hitRecord.t;
				hitRecord.materialIndex = triangle.materialIndex;
				hitRecord.normal = triangle.normal;
				return true;

			}
			return false;
		}

		inline bool HitTest_Triangle(const Triangle& triangle, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_Triangle(triangle, ray, temp, true);
		}
#pragma endregion
#pragma region TriangeMesh HitTest
		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray, HitRecord& hitRecord, bool ignoreHitRecord = false)
		{
			//for (const Triangle& triangle : mesh.)
		}

		inline bool HitTest_TriangleMesh(const TriangleMesh& mesh, const Ray& ray)
		{
			HitRecord temp{};
			return HitTest_TriangleMesh(mesh, ray, temp, true);
		}
#pragma endregion
	}

	namespace LightUtils
	{
		//Direction from target to light
		inline Vector3 GetDirectionToLight(const Light& light, const Vector3 origin)
		{
			
			return {   light.origin - origin};
		}

		inline ColorRGB GetRadiance(const Light& light, const Vector3& target)
		{
			if (light.type == LightType::Directional)
				return {light.color * light.intensity };
			return {light.color * light.intensity / Vector3::Dot(light.origin - target, light.origin - target) };
		}
	}

	namespace Utils
	{
		//Just parses vertices and indices
#pragma warning(push)
#pragma warning(disable : 4505) //Warning unreferenced local function
		static bool ParseOBJ(const std::string& filename, std::vector<Vector3>& positions, std::vector<Vector3>& normals, std::vector<int>& indices)
		{
			std::ifstream file(filename);
			if (!file)
				return false;

			std::string sCommand;
			// start a while iteration ending when the end of file is reached (ios::eof)
			while (!file.eof())
			{
				//read the first word of the string, use the >> operator (istream::operator>>) 
				file >> sCommand;
				//use conditional statements to process the different commands	
				if (sCommand == "#")
				{
					// Ignore Comment
				}
				else if (sCommand == "v")
				{
					//Vertex
					float x, y, z;
					file >> x >> y >> z;
					positions.push_back({ x, y, z });
				}
				else if (sCommand == "f")
				{
					float i0, i1, i2;
					file >> i0 >> i1 >> i2;

					indices.push_back((int)i0 - 1);
					indices.push_back((int)i1 - 1);
					indices.push_back((int)i2 - 1);
				}
				//read till end of line and ignore all remaining chars
				file.ignore(1000, '\n');

				if (file.eof()) 
					break;
			}

			//Precompute normals
			for (uint64_t index = 0; index < indices.size(); index += 3)
			{
				uint32_t i0 = indices[index];
				uint32_t i1 = indices[index + 1];
				uint32_t i2 = indices[index + 2];

				Vector3 edgeV0V1 = positions[i1] - positions[i0];
				Vector3 edgeV0V2 = positions[i2] - positions[i0];
				Vector3 normal = Vector3::Cross(edgeV0V1, edgeV0V2);

				if(isnan(normal.x))
				{
					int k = 0;
				}

				normal.Normalize();
				if (isnan(normal.x))
				{
					int k = 0;
				}

				normals.push_back(normal);
			}

			return true;
		}
#pragma warning(pop)
	}
}