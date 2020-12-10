#include "GameObject.h"
using namespace GameObjectLib::Library;

std::vector<Primitive> instanceHolder;
Primitive *tmpInstance = new Primitive;

int GameObject::newUID = 0;

GameObject::GameObject() : uid(newUID++) { }

GameObject::GameObject(Vector3 pos) : uid(newUID++) {
	tmpInstance->SetPosition(pos);
	instanceHolder.push_back(*tmpInstance);
}

GameObject::GameObject(float x, float y, float z) : uid(newUID++) {
	tmpInstance->SetPosition(x, y, z);
	instanceHolder.push_back(*tmpInstance);
}

GameObject::~GameObject() { }

void GameObject::UpdateGameObject() {

}

void GameObject::DrawPrimitive(const PrimitiveType *primitive) {
	for(int i = 0; i < instanceHolder.size(); i++) {
		if(i == this->uid) {
			instanceHolder[i].DrawPrimitive(primitive);
			//if(this->showCollider) instanceHolder[i].DrawCollider();
		}
	}
}

void GameObject::Destroy(GameObject instance) {
	for(int i = 0; i < instanceHolder.size(); i++){
		if(i == instance.uid)
			instanceHolder.erase(instanceHolder.begin() + i);
	}
}

void GameObject::MoveUp(float speed) {
	instanceHolder[this->uid].MoveUp(speed);
}

void GameObject::MoveDown(float speed) {
	instanceHolder[this->uid].MoveDown(speed);
}

void GameObject::MoveRight(float speed) {
	instanceHolder[this->uid].MoveRight(speed);
}

void GameObject::MoveLeft(float speed) {
	instanceHolder[this->uid].MoveLeft(speed);
}

void GameObject::MoveForward(float speed) {
	instanceHolder[this->uid].MoveForward(speed);
}

void GameObject::MoveBack(float speed) {
	instanceHolder[this->uid].MoveBack(speed);
}