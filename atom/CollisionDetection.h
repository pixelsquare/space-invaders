#ifndef __ATOM_ENGINE_COLLISION_DETECTION_LIBRARY_H__
#define __ATOM_ENGINE_COLLISION_DETECTION_LIBRARY_H__

/* Connected to BasicPrimitives Library header */
/* Connected to ModelLoader Library header */

#pragma region Comments
//
// This section is all about Collision Detection in 3-dimension
// CollisionDetection declaration for this section is found at CollisionDetection.cpp
//
#pragma endregion

#include "glut.h"

#include "BasicTransformation.h"
using namespace BasicTransformationLib::Library;

namespace CollisionDetectionLib {
	namespace Library {
		class Collision : public BasicTransformation {
		public:
			Collision();
			~Collision();

			void ApplyCollider(bool acquireCollider);
			void ApplyCollider(bool acquireCollider, bool showCollider);
			void SetColliderSize(float x, float y, float z);
			void SetColliderSize(Vector3 size);
			bool HasCollidedWith(Collision *object);
			bool HasCollidedWith(Collision &object);

			Vector3 GetColliderOffset();
			bool GetAcquireCollider();
			bool GetShowCollider();
		private:
			Vector3 colliderOffset; 
			bool acquireCollider;
		protected:
			bool showCollider;
			void DrawCollider();
			
		};
	}
}

#endif