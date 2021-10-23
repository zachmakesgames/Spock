#pragma once

//This disables the annoying "Prefer enum class" warning that Vulkan causes
#pragma warning( disable : 26812 )


#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers


#include <cstdint>
#include <vector>
#include <string>

#include "vulkan/vulkan.h"
#include "vulkan/vk_sdk_platform.h"


//Forward declaration so that the material factory can be defined in other
//libraries or applications like ColdStorage for shader compilation
class c_material_description_factory;

namespace MaterialDescription {
	//Material description version definitions
	enum class MaterialDescriptionVersion {
		MD_VERSION_1_0 = 0
	};



	//Enum values defined along bit boundaries for easy masking
	//Can actually use the vulkan stage definitions now that the vulkan
	//libraries are being included
	enum class ShaderStage {
		SHADER_STAGE_VERTEX= 0x0001,
		SHADER_STAGE_TES_CONTROL= 0x0002,
		SHADER_STAGE_TES_EVAL= 0x0004,
		SHADER_STAGE_GEOMETRY= 0x0008,
		SHADER_STAGE_FRAGMENT= 0x0010,
		SHADER_STAGE_MAX_ENUM= 0x1111
	};

	//Used to combine multiple ShaderStage values to denote which stages are being used in a material
	typedef uint8_t ShaderStageFlags;

	class c_vertex_input_attribute {
		uint32_t m_vertex_input_attribute_location= 0;
		uint32_t m_vertex_input_attribute_binding= 0;

		VkFormat m_vertex_input_attribute_format= VK_FORMAT_R32G32B32_SFLOAT; //RGB or vec 3

		uint32_t m_vertex_input_attribute_offset= 0;
	};

	class c_uniform_block {
		uint32_t m_set= 0;
		uint32_t m_binding= 0;
		uint64_t m_size= 0;	//Size of the uniform block in *bytes*
		ShaderStageFlags m_stage_flags= 0;
	};

	class c_uniform_sampler {
		uint32_t m_set= 0;
		uint32_t m_binding= 0;
		uint32_t m_image_size= 0; //Size of sample image in *bytes* to include all mip levels as well
		ShaderStageFlags m_stage_flags= 0;
	};



	class c_material_description {
	public:

		//Copy constructor. 
		c_material_description(const c_material_description &old_material_description)
		{
			this->m_version= old_material_description.m_version;

			this->m_shader_stages= old_material_description.m_shader_stages;

			
			for (int i = 0; i < 5; ++i) {
				this->m_shader_binaries[i].insert(this->m_shader_binaries[i].end(), old_material_description.m_shader_binaries[i].begin(), old_material_description.m_shader_binaries[i].end());
			}
			
			

			this->m_vertex_input_binding= old_material_description.m_vertex_input_binding;
			this->m_vertex_input_stride= old_material_description.m_vertex_input_stride;
			this->m_vertex_input_rate= old_material_description.m_vertex_input_rate;

			
			this->m_vertex_input_attributes.insert(this->m_vertex_input_attributes.end(), old_material_description.m_vertex_input_attributes.begin(), old_material_description.m_vertex_input_attributes.end());
			this->m_uniform_blocks.insert(this->m_uniform_blocks.end(), old_material_description.m_uniform_blocks.begin(), old_material_description.m_uniform_blocks.end());
			this->m_uniform_samplers.insert(this->m_uniform_samplers.end(), old_material_description.m_uniform_samplers.begin(), old_material_description.m_uniform_samplers.end());

			this->m_vertex_input_assembly_topology= old_material_description.m_vertex_input_assembly_topology;

			this->m_cull_mode_flags= old_material_description.m_cull_mode_flags;
			this->m_polygon_mode= old_material_description.m_polygon_mode;
			this->m_front_face= old_material_description.m_front_face;

			this->m_line_width= old_material_description.m_line_width;
		
			this->m_name= old_material_description.m_name;
		
		}


	private:

		//empty constructor so factory can create instances
		c_material_description(){}

		//The version of this material description for compatibility
		MaterialDescriptionVersion m_version= MaterialDescriptionVersion::MD_VERSION_1_0;


		//The pipeline stages defined for this material
		ShaderStageFlags m_shader_stages= 0;

		//Array to hold binary SPIR-V data for each shader stage
		//Stages are at the following index
		// 0 - vertex
		// 1 - tessellation control
		// 2 - tessellation evaluation
		// 3 - geometry
		// 4 - fragment
		std::vector<unsigned char> m_shader_binaries[5];


		uint16_t m_vertex_input_binding= 0;
		uint16_t m_vertex_input_stride= 0;
		VkVertexInputRate m_vertex_input_rate= VK_VERTEX_INPUT_RATE_VERTEX;

		std::vector<c_vertex_input_attribute> m_vertex_input_attributes;
		std::vector<c_uniform_block> m_uniform_blocks;
		std::vector<c_uniform_sampler> m_uniform_samplers;
		
		VkPrimitiveTopology m_vertex_input_assembly_topology= VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;


		VkCullModeFlags m_cull_mode_flags= VK_CULL_MODE_NONE;
		VkPolygonMode m_polygon_mode= VK_POLYGON_MODE_FILL;
		VkFrontFace m_front_face= VK_FRONT_FACE_COUNTER_CLOCKWISE;

		float m_line_width= 1.0f;


		std::string m_name= "";


		//Declare the friend in the global namespace. This with the
		//forward declaration makes it possible to implement a class outside
		//of this library that can create material descriptions without
		//the need to carry a factory with this class.
		friend class ::c_material_description_factory;
	};

}