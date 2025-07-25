#ifndef TERDIMENSION_OBJ_H
#define TERDIMENSION_OBJ_H

// Loads obj meshes using fast_obj.h
// Thanks thisistherk, you are very helpful

#include "terdimension.h"
#include "../fast_obj.h"

// Function to parse and transform a .obj file into a mesh
TD_FUNC TD_Mesh TD_load_obj(const char* filepath){
	fastObjMesh* obj = fast_obj_read(filepath);

}

TD_FUNC void TD_destroy_obj(TD_Mesh* m){
	
}

#endif