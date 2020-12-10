#ifndef __ATOM_ENGINE_GAMEOBJECT_H__
#define __ATOM_ENGINE_GAMEOBJECT_H__

#include "BasicPrimitives.h"
using namespace BasicPrimitivesLib::Library;

namespace GameObjectLib {
	namespace Library {
		class GameObject : public Collision {
		public:
			GameObject();
			GameObject(Vector3 pos);
			GameObject(float x, float y, float z);
			~GameObject();
			
			void UpdateGameObject();
			void DrawPrimitive(const PrimitiveType *primitive);
			static void Destroy(GameObject instance);

			void MoveUp(float speed);			void MoveDown(float speed);
			void MoveRight(float speed);		void MoveLeft(float speed);
			void MoveForward(float speed);		void MoveBack(float speed);
		private:
			const int uid;
			static int newUID;
		protected:
		};
	}
}

#endif