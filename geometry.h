#pragma once

#include <vector>
#include <glm.hpp>

namespace tinygltf
{
	class Model;
	struct Primitive;
}

class Geometry
{
public:
	static const float unit;

	std::vector<glm::ivec3> faces;

	std::vector<glm::vec3> positions;
	std::vector<glm::vec3> normals;	
	std::vector<glm::vec2> texcoords;

	void to_gltf(tinygltf::Model& m_out, tinygltf::Primitive& prim_out);

	void generate_ground(int x_units, int z_units, int offset_x, int offset_y, int offset_z);
	void generate_pillar(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z);
	void generate_wall_x(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z);
	void generate_wall_z(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z);
};


