#include <algorithm>
#include <cstdlib>

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <tiny_gltf.h>

#include "geometry.h"

const float Geometry::unit = 0.0625f;

void Geometry::to_gltf(tinygltf::Model& m_out, tinygltf::Primitive& prim_out)
{
	tinygltf::Buffer& buf_out = m_out.buffers[0];

	int num_pos = (int)positions.size();
	int num_face = (int)faces.size();

	size_t offset = 0;
	size_t length = 0;
	size_t view_id = 0;
	size_t acc_id = 0;

	offset = buf_out.data.size();
	length = sizeof(glm::ivec3) * num_face;
	buf_out.data.resize(offset + length);
	memcpy(buf_out.data.data() + offset, faces.data(), length);

	view_id = m_out.bufferViews.size();
	{
		tinygltf::BufferView view;
		view.buffer = 0;
		view.byteOffset = offset;
		view.byteLength = length;
		view.target = TINYGLTF_TARGET_ELEMENT_ARRAY_BUFFER;
		m_out.bufferViews.push_back(view);
	}

	acc_id = m_out.accessors.size();
	{
		tinygltf::Accessor acc;
		acc.bufferView = view_id;
		acc.byteOffset = 0;
		acc.componentType = TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT;
		acc.count = (size_t)(num_face * 3);
		acc.type = TINYGLTF_TYPE_SCALAR;
		acc.maxValues = { (double)(num_pos - 1) };
		acc.minValues = { 0 };
		m_out.accessors.push_back(acc);
	}

	prim_out.indices = acc_id;

	glm::vec3 min_pos = { FLT_MAX, FLT_MAX, FLT_MAX };
	glm::vec3 max_pos = { -FLT_MAX, -FLT_MAX, -FLT_MAX };

	for (int k = 0; k < num_pos; k++)
	{
		glm::vec3 pos = positions[k];
		if (pos.x < min_pos.x) min_pos.x = pos.x;
		if (pos.x > max_pos.x) max_pos.x = pos.x;
		if (pos.y < min_pos.y) min_pos.y = pos.y;
		if (pos.y > max_pos.y) max_pos.y = pos.y;
		if (pos.z < min_pos.z) min_pos.z = pos.z;
		if (pos.z > max_pos.z) max_pos.z = pos.z;
	}

	offset = buf_out.data.size();
	length = sizeof(glm::vec3) * num_pos;
	buf_out.data.resize(offset + length);
	memcpy(buf_out.data.data() + offset, positions.data(), length);

	view_id = m_out.bufferViews.size();
	{
		tinygltf::BufferView view;
		view.buffer = 0;
		view.byteOffset = offset;
		view.byteLength = length;
		view.target = TINYGLTF_TARGET_ARRAY_BUFFER;
		m_out.bufferViews.push_back(view);
	}

	acc_id = m_out.accessors.size();
	{
		tinygltf::Accessor acc;
		acc.bufferView = view_id;
		acc.byteOffset = 0;
		acc.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
		acc.count = (size_t)(num_pos);
		acc.type = TINYGLTF_TYPE_VEC3;
		acc.maxValues = { max_pos.x, max_pos.y, max_pos.z };
		acc.minValues = { min_pos.x, min_pos.y, min_pos.z };
		m_out.accessors.push_back(acc);
	}

	prim_out.attributes["POSITION"] = acc_id;

	offset = buf_out.data.size();
	length = sizeof(glm::vec3) * num_pos;
	buf_out.data.resize(offset + length);
	memcpy(buf_out.data.data() + offset, normals.data(), length);

	view_id = m_out.bufferViews.size();
	{
		tinygltf::BufferView view;
		view.buffer = 0;
		view.byteOffset = offset;
		view.byteLength = length;
		view.target = TINYGLTF_TARGET_ARRAY_BUFFER;
		m_out.bufferViews.push_back(view);
	}

	acc_id = m_out.accessors.size();
	{
		tinygltf::Accessor acc;
		acc.bufferView = view_id;
		acc.byteOffset = 0;
		acc.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
		acc.count = (size_t)(num_pos);
		acc.type = TINYGLTF_TYPE_VEC3;
		m_out.accessors.push_back(acc);
	}

	prim_out.attributes["NORMAL"] = acc_id;	

	offset = buf_out.data.size();
	length = sizeof(glm::vec2) * num_pos;
	buf_out.data.resize(offset + length);
	memcpy(buf_out.data.data() + offset, texcoords.data(), length);

	view_id = m_out.bufferViews.size();
	{
		tinygltf::BufferView view;
		view.buffer = 0;
		view.byteOffset = offset;
		view.byteLength = length;
		view.target = TINYGLTF_TARGET_ARRAY_BUFFER;
		m_out.bufferViews.push_back(view);
	}

	acc_id = m_out.accessors.size();
	{
		tinygltf::Accessor acc;
		acc.bufferView = view_id;
		acc.byteOffset = 0;
		acc.componentType = TINYGLTF_COMPONENT_TYPE_FLOAT;
		acc.count = (size_t)(num_pos);
		acc.type = TINYGLTF_TYPE_VEC2;
		m_out.accessors.push_back(acc);
	}

	prim_out.attributes["TEXCOORD_0"] = acc_id;

}

void Geometry::generate_ground(int x_units, int z_units, int offset_x, int offset_y, int offset_z)
{
	int idx = (int)positions.size();

	float y = (float)offset_y * unit;
	float x0 = (float)offset_x * unit;
	float x1 = x0 + x_units * unit;
	float z0 = -(float)offset_z * unit;
	float z1 = z0 - z_units * unit;

	float u0 = 0.0f;
	float v0 = 1.0f;
	float u1 = 1.0f;
	float v1 = 0.0f;

	glm::vec3 norm = { 0.0f, 1.0f, 0.0f };

	positions.push_back({ x0, y, z0 });
	normals.push_back(norm);	
	texcoords.push_back({ u0, v0 });

	positions.push_back({ x1, y, z0 });
	normals.push_back(norm);	
	texcoords.push_back({ u1, v0 });

	positions.push_back({ x1, y, z1 });
	normals.push_back(norm);	
	texcoords.push_back({ u1, v1 });

	positions.push_back({ x0, y, z1 });
	normals.push_back(norm);	
	texcoords.push_back({ u0, v1 });

	faces.push_back({ idx + 0, idx + 1, idx + 3 });
	faces.push_back({ idx + 1, idx + 2, idx + 3 });

}

void Geometry::generate_pillar(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z)
{
	// -x
	{
		int idx = (int)positions.size();

		float x = (float)offset_x * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z1 = -(float)offset_z * unit;
		float z0 = z1 - z_units * unit;

		float u0 = 0.25f;
		float v0 = 13.0f / 17.0f;
		float u1 = 0.5f;
		float v1 = 0.0f;
		
		glm::vec3 norm = { -1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// +x
	{
		int idx = (int)positions.size();

		float x = (float)(offset_x + x_units) * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;
		
		float u0 = 0.0f;
		float v0 = 13.0f / 17.0f;
		float u1 = 0.25f;
		float v1 = 0.0f;		

		glm::vec3 norm = { 1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// -y
	{
		int idx = (int)positions.size();

		float y = (float)offset_y * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float z1 = -(float)offset_z * unit;
		float z0 = z1 - z_units * unit;

		float u0 = 0.25f;
		float v0 = 1.0f;
		float u1 = 0.5f;
		float v1 = 13.0f / 17.0f;

		glm::vec3 norm = { 0.0f, -1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// y
	{
		int idx = (int)positions.size();

		float y = (float)(offset_y + y_units) * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 1.0f;
		float u1 = 0.25f;
		float v1 = 13.0f / 17.0f;

		glm::vec3 norm = { 0.0f, 1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// -z
	{
		int idx = (int)positions.size();

		float z = -(float)(offset_z + z_units) * unit;
		float x1 = (float)offset_x * unit;
		float x0 = x1 + x_units * unit;

		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		
		float u0 = 0.75f;
		float v0 = 13.0f / 17.0f;
		float u1 = 1.0f;
		float v1 = 0.0f;

		glm::vec3 norm = { 0.0f, 0.0f, -1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// z
	{
		int idx = (int)positions.size();

		float z = -(float)offset_z * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
	
		float u0 = 0.5f;
		float v0 = 13.0f / 17.0f;
		float u1 = 0.75f;
		float v1 = 0.0f;
	
		glm::vec3 norm = { 0.0f, 0.0f, 1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);	
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);	
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);		
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);		
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

}

void Geometry::generate_wall_x(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z)
{
	// -x
	{
		int idx = (int)positions.size();

		float x = (float)offset_x * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z1 = -(float)offset_z * unit;
		float z0 = z1 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 96.0f/120.0f;
		float u1 = 96.0f/108.0f;
		float v1 = 48.0f/120.0f;

		glm::vec3 norm = { -1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// +x
	{
		int idx = (int)positions.size();

		float x = (float)(offset_x + x_units) * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 48.0f / 120.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 0.0f;

		glm::vec3 norm = { 1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// -y
	{
		int idx = (int)positions.size();

		float y = (float)offset_y * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 1.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 108.0f / 120.0f;

		glm::vec3 norm = { 0.0f, -1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// y
	{
		int idx = (int)positions.size();

		float y = (float)(offset_y + y_units) * unit;
		float x1 = (float)offset_x * unit;
		float x0 = x1 + x_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 108.0f / 120.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 96.0f / 120.0f;

		glm::vec3 norm = { 0.0f, 1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// -z
	{
		int idx = (int)positions.size();

		float z = -(float)(offset_z + z_units) * unit;
		float x1 = (float)offset_x * unit;
		float x0 = x1 + x_units * unit;

		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;

		float u0 = 96.0f / 108.0f;
		float v0 = 96.0f / 120.0f;
		float u1 = 1.0f;
		float v1 = 48.0f/120.0f;

		glm::vec3 norm = { 0.0f, 0.0f, -1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// z
	{
		int idx = (int)positions.size();

		float z = -(float)offset_z * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;

		float u0 = 96.0f / 108.0f;
		float v0 = 48.0f / 120.0f;
		float u1 = 1.0f;
		float v1 = 0.0f;

		glm::vec3 norm = { 0.0f, 0.0f, 1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}


}

void Geometry::generate_wall_z(int x_units, int y_units, int z_units, int offset_x, int offset_y, int offset_z)
{
	// -x
	{
		int idx = (int)positions.size();

		float x = (float)offset_x * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z1 = -(float)offset_z * unit;
		float z0 = z1 - z_units * unit;

		float u0 = 96.0f / 108.0f;
		float v0 = 96.0f / 120.0f;
		float u1 = 1.0f;
		float v1 = 48.0f / 120.0f;

		glm::vec3 norm = { -1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// +x
	{
		int idx = (int)positions.size();

		float x = (float)(offset_x + x_units) * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 96.0f / 108.0f;
		float v0 = 48.0f / 120.0f;
		float u1 = 1.0f;
		float v1 = 0.0f;

		glm::vec3 norm = { 1.0f, 0.0f, 0.0f };

		positions.push_back({ x, y0, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x, y0, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x, y1, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x, y1, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });
	}

	// -y
	{
		int idx = (int)positions.size();

		float y = (float)offset_y * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float z1 = -(float)offset_z * unit;
		float z0 = z1 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 1.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 108.0f / 120.0f;

		glm::vec3 norm = { 0.0f, -1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// y
	{
		int idx = (int)positions.size();

		float y = (float)(offset_y + y_units) * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float z0 = -(float)offset_z * unit;
		float z1 = z0 - z_units * unit;

		float u0 = 0.0f;
		float v0 = 108.0f / 120.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 96.0f / 120.0f;

		glm::vec3 norm = { 0.0f, 1.0f, 0.0f };

		positions.push_back({ x0, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y, z0 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y, z1 });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// -z
	{
		int idx = (int)positions.size();

		float z = -(float)(offset_z + z_units) * unit;
		float x1 = (float)offset_x * unit;
		float x0 = x1 + x_units * unit;

		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;

		float u0 = 0.0f;
		float v0 = 96.0f / 120.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 48.0f / 120.0f;

		glm::vec3 norm = { 0.0f, 0.0f, -1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

	// z
	{
		int idx = (int)positions.size();

		float z = -(float)offset_z * unit;
		float x0 = (float)offset_x * unit;
		float x1 = x0 + x_units * unit;
		float y0 = (float)offset_y * unit;
		float y1 = y0 + y_units * unit;

		float u0 = 0.0f;
		float v0 = 48.0f / 120.0f;
		float u1 = 96.0f / 108.0f;
		float v1 = 0.0f;

		glm::vec3 norm = { 0.0f, 0.0f, 1.0f };

		positions.push_back({ x0, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v0 });

		positions.push_back({ x1, y0, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v0 });

		positions.push_back({ x1, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u1, v1 });

		positions.push_back({ x0, y1, z });
		normals.push_back(norm);
		texcoords.push_back({ u0, v1 });

		faces.push_back({ idx + 0, idx + 1, idx + 3 });
		faces.push_back({ idx + 1, idx + 2, idx + 3 });

	}

}