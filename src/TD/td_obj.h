#ifndef TERDIMENSION_OBJ_H
#define TERDIMENSION_OBJ_H

// Loads obj meshes using fast_obj.h
// Thanks thisistherk, you are very helpful

#include "td_definitions.h"
#include "td_texture.h"
#include <string.h>

#define FAST_OBJ_IMPLEMENTATION
#include "../fast_obj.h"

// Function to parse and transform a .obj file into a mesh
TD_FUNC bool TD_load_obj(const char* filepath, TD_Mesh* result){
	fastObjMesh* obj = fast_obj_read(filepath);
	if(!obj) return false;
	if(!obj->face_count || !obj->position_count || !obj->index_count){
		fast_obj_destroy(obj);
		return false;
	}
	
	*result = (TD_Mesh){
		(TD_Vec3*) obj->positions,
		NULL,
		(TD_Vec3*) obj->normals,
		(TD_Vec2*) obj->texcoords,
		NULL, NULL,
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
	
	// Load the texture (if it exists)
	if(obj->material_count && obj->materials[0].map_Kd){
		TD_Texture* texture = (TD_Texture*) TD_MALLOC(sizeof(TD_Texture));
		TD_load_texture(obj->textures[obj->materials[0].map_Kd].path, texture);
		result->texture = texture;
	}
	
	// Free all unused memory
	// This is a modified version of fast_obj_destroy()
	fast_obj_destroy_TD(obj);

	return true;
}

TD_FUNC void TD_free_obj(TD_Mesh* m){
	if(!m) return;
	if(m->vertices)
		TD_FREE(_array_header(m->vertices));
	if(m->uvs)
		TD_FREE(_array_header(m->uvs));
	if(m->normals)
		TD_FREE(_array_header(m->normals));
	if(m->faces)
		TD_FREE(m->faces);
	if(m->texture)
		TD_free_texture(m->texture);
}

#endif
