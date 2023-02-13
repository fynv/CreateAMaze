#include <ctime>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <glm.hpp>

#include <tiny_gltf.h>

#include "maze.h"
#include "geometry.h"


int main()
{
	srand(time(nullptr));

	int maze_w = 21;
	int maze_h = 21;
	Maze maze(maze_w, maze_h);
	std::vector<Maze::CellLocation> farthests;
	maze.analyze(farthests);

	for (size_t i = 0; i < farthests.size(); i++)
	{
		printf("%d %d\n", farthests[i].x, farthests[i].y);
	}

	tinygltf::Model m_out;
	m_out.scenes.resize(1);
	tinygltf::Scene& scene_out = m_out.scenes[0];
	scene_out.name = "Scene";

	m_out.asset.version = "2.0";
	m_out.asset.generator = "tinygltf";

	m_out.buffers.resize(1);
	tinygltf::Buffer& buf_out = m_out.buffers[0];

	size_t offset = 0;
	size_t length = 0;
	size_t view_id = 0;
	size_t acc_id = 0;

	// sampler
	m_out.samplers.resize(1);
	tinygltf::Sampler& sampler = m_out.samplers[0];
	sampler.minFilter = TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR;
	sampler.magFilter = TINYGLTF_TEXTURE_FILTER_LINEAR;

	// texture
	struct TexInfo
	{
		std::string path;
		int width;
		int height;
	};

	TexInfo tex_info[3] = {
		{
			"textures/ground.jpg",
			1024, 1024
		},		
		{
			"textures/pillar.jpg",
			964, 1024
		},		
		{
			"textures/wall.jpg",
			922, 1024
		},		
	};

	m_out.images.resize(3);
	m_out.textures.resize(3);
	
	for (int i=0;i<3;i++)
	{
		TexInfo& info = tex_info[i];
		tinygltf::Image& img_out = m_out.images[i];
		tinygltf::Texture& tex_out = m_out.textures[i];
		img_out.width = info.width;
		img_out.height = info.height;
		img_out.component = 4;
		img_out.bits = 8;
		img_out.pixel_type = TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE;
		
		/*img_out.mimeType = "image/jpeg";
		{
			FILE* fp = fopen(info.path.c_str(), "rb");
			fseek(fp, 0, SEEK_END);
			size_t len = (size_t)ftell(fp);
			fseek(fp, 0, SEEK_SET);
			offset = buf_out.data.size();
			length = (len + 3) / 4 * 4;
			buf_out.data.resize(offset + length);
			fread(buf_out.data.data() + offset, 1, length, fp);
			fclose(fp);
		}

		view_id = m_out.bufferViews.size();
		{
			tinygltf::BufferView view;
			view.buffer = 0;
			view.byteOffset = offset;
			view.byteLength = length;
			m_out.bufferViews.push_back(view);
		}
		img_out.bufferView = view_id;*/

		img_out.uri = info.path;


		tex_out.sampler = 0;
		tex_out.source = i;
	}

	// material
	m_out.materials.resize(3);
	{
		tinygltf::Material& material_out = m_out.materials[0];
		material_out.name = "ground";
		material_out.pbrMetallicRoughness.baseColorTexture.index = 0;
		material_out.pbrMetallicRoughness.metallicFactor = 0.2;
		material_out.pbrMetallicRoughness.roughnessFactor = 0.1;		
	}
	{
		tinygltf::Material& material_out = m_out.materials[1];
		material_out.name = "pillar";
		material_out.pbrMetallicRoughness.baseColorTexture.index = 1;
		material_out.pbrMetallicRoughness.metallicFactor = 0.0;
		material_out.pbrMetallicRoughness.roughnessFactor = 1.0;		
	}
	{
		tinygltf::Material& material_out = m_out.materials[2];
		material_out.name = "wall";
		material_out.pbrMetallicRoughness.baseColorTexture.index = 2;
		material_out.pbrMetallicRoughness.metallicFactor = 0.0;
		material_out.pbrMetallicRoughness.roughnessFactor = 1.0;		
	}

	// node
	m_out.nodes.resize(1);
	tinygltf::Node& node_out = m_out.nodes[0];
	scene_out.nodes.push_back(0);

	// mesh
	m_out.meshes.resize(1);
	tinygltf::Mesh& mesh_out = m_out.meshes[0];
	node_out.mesh = 0;

	// ground
	int origin_x = -maze_w * 48 / 2;
	int origin_y = -maze_h * 48 / 2;
	for (int y = 0; y < maze_h; y++)
	{
		for (int x = 0; x < maze_w; x++)
		{
			tinygltf::Primitive prim_out;
			prim_out.material = 0;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;
			Geometry ground;
			ground.generate_ground(48, 48, origin_x + x * 48, 0, origin_y + y * 48);
			ground.to_gltf(m_out, prim_out);
			mesh_out.primitives.emplace_back(prim_out);
		}
	}

	// pillars
	for (int y = 0; y < maze_h + 1; y++)
	{
		for (int x = 0; x < maze_w + 1; x++)
		{
			tinygltf::Primitive prim_out;
			prim_out.material = 1;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;
			Geometry wall;
			wall.generate_pillar(8, 26, 8, origin_x + x * 48 - 4, 0, origin_y + y * 48 - 4);
			wall.to_gltf(m_out, prim_out);
			mesh_out.primitives.emplace_back(prim_out);
		}
	}

	// outer walls
	for (int x = 0; x < maze_w; x++)
	{
		{
			tinygltf::Primitive prim_out;
			prim_out.material = 2;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;

			Geometry wall;
			wall.generate_wall_z(48, 24, 6, origin_x + x * 48, 0, origin_y - 3);
			wall.to_gltf(m_out, prim_out);

			mesh_out.primitives.emplace_back(prim_out);
		}

		{
			tinygltf::Primitive prim_out;
			prim_out.material = 2;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;

			Geometry wall;
			wall.generate_wall_z(48, 24, 6, origin_x + x * 48, 0, origin_y + maze_h * 48 - 3);
			wall.to_gltf(m_out, prim_out);

			mesh_out.primitives.emplace_back(prim_out);
		}
	}

	for (int y = 0; y < maze_h; y++)
	{
		{
			tinygltf::Primitive prim_out;
			prim_out.material = 2;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;

			Geometry wall;
			wall.generate_wall_x(6, 24, 48, origin_x - 3, 0, origin_y + y * 48);
			wall.to_gltf(m_out, prim_out);

			mesh_out.primitives.emplace_back(prim_out);
		}

		{
			tinygltf::Primitive prim_out;
			prim_out.material = 2;
			prim_out.mode = TINYGLTF_MODE_TRIANGLES;

			Geometry wall;
			wall.generate_wall_x(6, 24, 48, origin_x + maze_w * 48 - 3, 0, origin_y + y * 48);
			wall.to_gltf(m_out, prim_out);

			mesh_out.primitives.emplace_back(prim_out);
		}
	}

	// maze walls
	for (int y = 0; y < maze_h; y++)
	{
		for (int x = 0; x < maze_w - 1; x++)
		{
			if (maze.x_walls[x + y * (maze_w - 1)])
			{
				tinygltf::Primitive prim_out;
				prim_out.material = 2;
				prim_out.mode = TINYGLTF_MODE_TRIANGLES;

				Geometry wall;
				wall.generate_wall_x(6, 24, 48, origin_x + (x + 1) * 48 - 3, 0, origin_y + y * 48);
				wall.to_gltf(m_out, prim_out);

				mesh_out.primitives.emplace_back(prim_out);
			}
		}
	}
	
	for (int y = 0; y < maze_h - 1; y++)
	{
		for (int x = 0; x < maze_w; x++)
		{
			if (maze.y_walls[x + y * maze_w])
			{
				tinygltf::Primitive prim_out;
				prim_out.material = 2;
				prim_out.mode = TINYGLTF_MODE_TRIANGLES;

				Geometry wall;
				wall.generate_wall_z(48, 24, 6, origin_x + x * 48, 0, origin_y + (y + 1) * 48 - 3);
				wall.to_gltf(m_out, prim_out);

				mesh_out.primitives.emplace_back(prim_out);
			}
		}
	}


	tinygltf::TinyGLTF gltf;
	gltf.WriteGltfSceneToFile(&m_out, "maze.glb", true, true, false, true);

	return 0;
}
