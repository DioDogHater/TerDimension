#include "td_obj.h"

#define FAST_OBJ_IMPLEMENTATION
#include "fast_obj.h"

// Function to parse and transform a .obj file into a mesh
TD_FUNC bool TD_load_obj(const char* filepath, TD_Mesh* result){
	printf("Loading model %s!\n",filepath);
	fastObjMesh* obj = fast_obj_read(filepath);
	if(!obj) { printf("Failed to load model!\n"); return false; }
	if(!obj->face_count || !obj->position_count || !obj->index_count){
		printf("Failed to load model!\n");
		fast_obj_destroy(obj);
		return false;
	}

	*result = (TD_Mesh){
		(TD_Vec3*) obj->positions,
		NULL,
		(TD_Vec3*) obj->normals,
		(TD_Vec2*) obj->texcoords,
		TD_TextureEMPTY, NULL,
		obj->face_count,
		result->transform
	};

	// Allocate memory for faces
	result->faces = (TD_Face*) TD_MALLOC(obj->face_count * sizeof(TD_Face));

	// Format faces
	fastObjIndex* index = obj->indices;
	for(unsigned int i = 0; i < obj->face_count; i++){
		// Skip face if it's not a triangle
		// Remember to triangulate all faces of your model before importing it
		if(obj->face_vertices[i] != 3)
			result->faces[i] = (TD_Face){0,0,0,0,0,0,0,0,0};
		else{
			result->faces[i] = (TD_Face){
				index[0].p, index[1].p, index[2].p,
				index[0].t, index[1].t, index[2].t,
				index[0].n, index[1].n, index[2].n
			};
		}

		index += obj->face_vertices[i];
	}

	#ifndef TD_NO_TEXTURES
	// Load the texture (if it exists)
	if(obj->material_count && obj->materials[0].map_Kd){
		printf("Loading texture %s!\n",obj->textures[obj->materials[0].map_Kd].path);
		TD_load_texture(obj->textures[obj->materials[0].map_Kd].path, &result->texture);
	}
	#endif

	// Free all unused memory
	// This is a modified version of fast_obj_destroy()
	fast_obj_destroy_TD(obj);

	printf("Successfully loaded model!\n");

	return true;
}

TD_FUNC void TD_free_obj(TD_Mesh* m){
	if(!m) return;
	if(m->vertices)
		TD_FREE(_array_header(m->vertices));
	m->vertices = NULL;
	if(m->uvs)
		TD_FREE(_array_header(m->uvs));
	m->uvs = NULL;
	if(m->normals)
		TD_FREE(_array_header(m->normals));
	m->normals = NULL;
	if(m->faces)
		TD_FREE(m->faces);
	m->faces = NULL;
	#ifndef TD_NO_TEXTURES
	if(m->texture.width && m->texture.height)
		TD_free_texture(&m->texture);
	#endif
}