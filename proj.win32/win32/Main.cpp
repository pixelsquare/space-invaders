#include "../../atom/AtomEngine.h"
using namespace Atom::Engine;

#pragma region Comments
//
// Developer's Build
//
// A Simulation made out of Atom Engine
//
// Author: Anthony Ganzon
//
// Atom Engine Authors:
//		Anthony Ganzon
//		Dhenz Zerrudo
//
// NOTE!
// Please inform the author if you want to borrow
// this code/s (script/s) for authorization and copyright.
//
#pragma endregion

#define Title "Space Invaders using Atom Engine v1.0"						// Title of GLUT WINDOW

enum GameState { StartGame, MainGame, EndGame };
GameState gameState = StartGame;

// Set to false to run models in Executable
// Set to true to run models in Debug Mode
bool isDebugging = false;

bool enableSound = true;
int score = 0;

class Bullet {
public:
	~Bullet() {
		AtomEngine::Clean(this->bulletInstance);
		AtomEngine::Clean(this->direction);
		AtomEngine::Clean(this->speed);
		AtomEngine::Clean(this->yMinLimit);
		AtomEngine::Clean(this->yMaxLimit);
	}

	Primitive *bulletInstance;
	const PrimitiveType *primitiveType;
	Vector3 direction;
	float speed;
	float yMinLimit;
	float yMaxLimit;
private:
protected:
};

class ParticleAttribute {
public:
	~ParticleAttribute() {
		AtomEngine::Clean(this->size);
		AtomEngine::Clean(this->max_energy);
		AtomEngine::Clean(this->speed);
		AtomEngine::Clean(this->time);
		AtomEngine::Clean(this->particle);
		AtomEngine::Clean(this->direction);
	}
	int size;
	float max_energy;
	float speed;
	Time *time;
	std::vector<Primitive*> particle;
	std::vector<Vector3*> direction;
private:
protected:
};

class Particle {
public:
	Particle() { 
		this->maxEnergy = 0.5f;
	}

	~Particle() { 
		AtomEngine::Clean(this->maxEnergy);
	}

	void Initialize(Vector3 initialPos, int particleSize, float speed) {
		ParticleAttribute *tmpInstance = new ParticleAttribute;
		tmpInstance->size = particleSize;

		for(int i = 0; i < tmpInstance->size; i++) {
			tmpInstance->max_energy = this->maxEnergy;
			tmpInstance->speed = speed;

			Vector3 *tmpDir = new Vector3;
			tmpDir = this->GetRandomDirection();
			tmpInstance->direction.push_back(tmpDir);

			tmpInstance->time = new Time;

			Primitive *tmpParticle = new Primitive;
			tmpParticle->SetColor(*this->GetRandomColor());
			tmpParticle->SetPosition(initialPos);
			tmpParticle->SetScale(Vector3::One * 0.3f);
			tmpInstance->particle.push_back(tmpParticle);
		}

		this->vecParticle.push_back(tmpInstance);
	}

	void DrawParticle() {
		if(!this->vecParticle.empty()) {
			for(int i = 0; i < this->vecParticle.size(); i++) {
				
				if(!this->vecParticle[i]->particle.empty()) {
					for(int j = 0; j < this->vecParticle[i]->particle.size(); j++) {
						this->vecParticle[i]->particle[j]->DrawPrimitive(PrimitiveType::Plane);
						this->vecParticle[i]->particle[j]->Move(this->vecParticle[i]->speed, this->vecParticle[i]->direction[j]);
					}
				}

				if(this->vecParticle[i]->time->LocalTimeElapsed() > this->vecParticle[i]->max_energy) {
					this->vecParticle[i]->particle.clear();
					this->vecParticle[i]->direction.clear();
					this->vecParticle.erase(this->vecParticle.begin() + i);
				}
			}
		}
	}

	void SetMaxEnergy(float num) {
		this->maxEnergy = num;
	}

	Vector3 *GetRandomDirection() {
		Vector3 *dir = new Vector3;
		dir->X = (rand() % 100 + 1);
		dir->Y = (rand() % 100 + 1);
		dir->Z = (rand() % 100 + 1);

		int randX = rand() % 2;
		if(randX == 0) dir->X = -dir->X;

		int randY = rand() % 2;
		if(randY == 0) dir->Y = -dir->Y;

		int randZ = rand() % 2;
		if(randZ == 0) dir->Z = -dir->Z;

		return dir;
	}

	Color *GetRandomColor() {
		Color *color = new Color;
		color->R = (rand() % 100 + 1) / 100.0f;
		color->G = (rand() % 100 + 1) / 100.0f;;
		color->B = (rand() % 100 + 1) / 100.0f;;

		return color;
	}
private:
	float maxEnergy;
	std::vector<ParticleAttribute*> vecParticle;
protected:
};

class EnemyAttribute{
public:
	~EnemyAttribute() {
		AtomEngine::Clean(this->instance);
		AtomEngine::Clean(this->life);
	}

	int life;
	ModelLoad* instance;
	std::vector<Bullet*> bullet;
private:
protected:
};

class GalagaShip {
public:
	GalagaShip() { 
		this->instance1 = new ModelLoad;
		this->instance2 = new ModelLoad;
		this->instance3 = new ModelLoad;
		this->dt = 0.02f;
		this->colorOffset = 1.0f;
		this->maxLife = 3;
		this->life = this->maxLife;
		this->position = Vector3::Zero;
		this->objectHit = new ModelLoad;
		this->shipExplosion = new Particle;
		this->alive = true;
	}

	~GalagaShip() { 
		AtomEngine::Clean(this->maxLife);
		AtomEngine::Clean(this->life);
		AtomEngine::Clean(this->colorOffset);
		AtomEngine::Clean(this->dt);
		AtomEngine::Clean(this->position);
	}

	void Initialize(const char* objName1, const char* objName2, const char* objName3) {
		this->instance1->LoadObj(objName1);
		this->instance1->ApplyCollider(true, true);
		this->instance1->SetColliderSize(Vector3(3.5f, 3.5f, 1.0f));

		this->instance2->LoadObj(objName2);
		this->instance3->LoadObj(objName3);

		this->SyncMovement();
	}

	void DrawShip() {
		if(gameState == StartGame) return;

		if(this->alive) {
			this->instance1->RenderModel();
			this->instance1->SetColor(Color::White * this->colorOffset);

			this->instance2->RenderModel();
			this->instance2->SetColor(Color::Red * this->colorOffset);

			this->instance3->RenderModel();
			this->instance3->SetColor(Color::Blue * this->colorOffset);

			this->SyncMovement();
			this->DrawBullet();

			if(gameState == EndGame)
				this->DestroyShip();
		}

		this->shipExplosion->DrawParticle();
	}

	void SyncMovement() {
		this->instance1->SetPosition(this->position);
		this->instance2->SetPosition(this->position);
		this->instance3->SetPosition(this->position);
	}

	void ShipMoveRight(float speed) {
		if(gameState == StartGame) return;
		this->position += Vector3::Right * speed * this->dt;
	}

	void ShipMoveLeft(float speed) {
		if(gameState == StartGame) return;
		this->position += Vector3::Left * speed * this->dt;
	}

	void FireBullet(const PrimitiveType *primitive, float spd, Vector3 dir) {
		if(gameState == StartGame) return;
		if(!this->alive) return;
		Sound::Play("assets\\Shoot.wav");

		Bullet *tmpInstance = new Bullet;
		tmpInstance->bulletInstance = new Primitive;
		tmpInstance->direction = dir;
		tmpInstance->speed = spd;
		tmpInstance->yMinLimit = -40.0f;
		tmpInstance->yMaxLimit = 40.0f;
		tmpInstance->primitiveType = primitive;

		tmpInstance->bulletInstance->SetPosition(this->position);
		tmpInstance->bulletInstance->SetScale(Vector3::One * 0.5f);
		tmpInstance->bulletInstance->ApplyCollider(true, true);
		tmpInstance->bulletInstance->SetColliderSize(Vector3(0.5f, 0.5f, 1.0f));
		vecBullet.push_back(tmpInstance);
	}

	void DrawBullet() {
		if(!this->vecBullet.empty()) {
			for(int i = 0; i < this->vecBullet.size(); i++) {
				this->vecBullet[i]->bulletInstance->Move(this->vecBullet[i]->speed, this->vecBullet[i]->direction);
				this->vecBullet[i]->bulletInstance->DrawPrimitive(this->vecBullet[i]->primitiveType);

				if(this->vecBullet[i]->bulletInstance->GetPosition().Y > this->vecBullet[i]->yMaxLimit)
					this->vecBullet.erase(this->vecBullet.begin() + i);
			}
		}
	}

	bool BulletHasCollidedWith(std::vector<EnemyAttribute*> object) {
		if(!this->vecBullet.empty() || !object.empty()) {
			for(int i = 0; i < this->vecBullet.size(); i++) {
				for(int j = 0; j < object.size(); j++) {
					if(this->vecBullet[i]->bulletInstance->HasCollidedWith(object[j]->instance)) {
						this->vecBullet.erase(this->vecBullet.begin() + i);
						this->objectHit = object[j]->instance;
						return true;
					}
				}
			}
		}
		return false;
	}

	void SetPosition(float x, float y, float z) {
		this->position = Vector3(x, y, z);
	}

	void SetShipScale(Vector3 scale) {
		this->instance1->SetScale(scale);
		this->instance2->SetScale(scale);
		this->instance3->SetScale(scale);
	}

	void DestroyShip() {
		AtomEngine::Clean(this->instance1);
		AtomEngine::Clean(this->instance2);
		AtomEngine::Clean(this->instance3);
		this->alive = false;
	}

	void SubtractLife() {
		if(this->life > 0)
			this->life--;
		
		if(this->life == 0) {
			Sound::Play("assets\\Explosion.wav");
			this->DestroyShip();
			this->shipExplosion->SetMaxEnergy(1.5f);
			this->shipExplosion->Initialize(this->position, 100, 40);
		}

		if(this->colorOffset > 0.0f)
			this->colorOffset -= (1.0f / this->maxLife);
	}

	bool isAlive() {
		return this->alive;
	}

	int GetLife() {
		return this->life;
	}

	Vector3 GetPosition() {
		return this->position;
	}

	ModelLoad *GetMainInstance() {
		return this->instance1;
	}

	ModelLoad *GetObjectHit() {
		return this->objectHit;
	}

private:
	ModelLoad *instance1;
	ModelLoad *instance2;
	ModelLoad *instance3;
	ModelLoad *objectHit;

	int maxLife;
	int life;
	float colorOffset;
	float dt;
	bool alive;
	Vector3 position;
	Particle *shipExplosion;
	std::vector<Bullet*> vecBullet;
protected:
};

class Enemy {
public:
	Enemy() { 
		this->color = Color::White;
		this->position = Vector3::Zero;
		this->tmpLife = 0;
		this->rateOfFire = 2.5f;
		this->enemyParticle = new Particle;
		this->fireTime = new Time;
		this->hasFired = false;
		this->bulletSpeed = 100;
	}

	~Enemy() { 
		AtomEngine::Clean(this->position);
		AtomEngine::Clean(this->tmpLife);
		this->color.~Color();
		this->enemyContainer.clear();
	}

	void Initialize(const char* object, float minPos, float maxPos, int size, float spacing) {
		for(float i = minPos; i < maxPos; i += spacing) {
			EnemyAttribute *tmpEnemy = new EnemyAttribute;
			tmpEnemy->life = this->tmpLife;

			tmpEnemy->instance = new ModelLoad;
			tmpEnemy->instance->SetSineTime(0);
			tmpEnemy->instance->SetScale(Vector3::One * 0.6f);
			tmpEnemy->instance->SetPosition(this->position.X + (i * spacing), this->position.Y, this->position.Z);
			tmpEnemy->instance->SetColor(this->color);
			tmpEnemy->instance->ApplyCollider(true, true);
			tmpEnemy->instance->SetColliderSize(Vector3(1.5f, 1.5f, 1.0f));
			tmpEnemy->instance->LoadObj(object);

			this->enemyContainer.push_back(tmpEnemy);
		}
	}

	void Render() {
		if(gameState == StartGame) {
			this->fireTime->LocalReset();
			return;
		}

		for(int i = 0; i < this->enemyContainer.size(); i++) {
			if(i == this->enemyContainer.size())
				return;

			this->enemyContainer[i]->instance->MoveSine(25.0f, Vector3::Right);
			this->enemyContainer[i]->instance->RenderModel();
		}

		this->DrawBullet();
		this->FireBullet();
		this->enemyParticle->DrawParticle();
	}

	int PickIndex() {
		return Mathf::RandomRange(0, this->enemyContainer.size());
	}

	void InitializeBullet(int indx) {
		Bullet *tmpInstance = new Bullet;
		tmpInstance->bulletInstance = new Primitive;

		Vector3 targetPos = this->target->GetPosition() - this->enemyContainer[indx]->instance->GetPosition();
		targetPos.Normalize();
		tmpInstance->direction = targetPos;

		tmpInstance->speed = this->bulletSpeed;
		tmpInstance->yMinLimit = -40.0f;
		tmpInstance->yMaxLimit = 40.0f;
		tmpInstance->primitiveType = PrimitiveType::Sphere;

		tmpInstance->bulletInstance->SetColor(Color::Red);
		tmpInstance->bulletInstance->ApplyCollider(true, true);
		tmpInstance->bulletInstance->SetColliderSize(Vector3(0.5f, 0.5f, 1.0f));
		tmpInstance->bulletInstance->SetPosition(this->enemyContainer[indx]->instance->GetPosition());
		tmpInstance->bulletInstance->SetScale(Vector3::One * 0.5f);
		this->enemyContainer[indx]->bullet.push_back(tmpInstance);
	}

	void FireBullet() {
		if(!this->target->isAlive()) return;

		if(!this->enemyContainer.empty()) {
			if(this->fireTime->LocalTimeElapsed() > this->rateOfFire) {
				if(!this->hasFired) {
					this->InitializeBullet(this->PickIndex());
					this->hasFired = true;
				}
			}

			if(this->hasFired) {
				this->fireTime->LocalReset();
				this->hasFired = false;
			}
		}
	}

	void DrawBullet() {
		for(int i = 0; i < this->enemyContainer.size(); i++) {

			if(!this->enemyContainer[i]->bullet.empty()) {
				for(int j = 0; j < this->enemyContainer[i]->bullet.size(); j++) {
					this->enemyContainer[i]->bullet[j]->bulletInstance->Move(this->enemyContainer[i]->bullet[j]->speed, this->enemyContainer[i]->bullet[j]->direction);
					this->enemyContainer[i]->bullet[j]->bulletInstance->DrawPrimitive(this->enemyContainer[i]->bullet[j]->primitiveType);

					if(this->enemyContainer[i]->bullet[j]->bulletInstance->GetPosition().Y < this->enemyContainer[i]->bullet[j]->yMinLimit)
						this->enemyContainer[i]->bullet.erase(this->enemyContainer[i]->bullet.begin() + j);
				}
			}
		}
	}

	bool BulletHasCollidedWith(ModelLoad* ship) {
		for(int i = 0; i < this->enemyContainer.size(); i++) {
			for(int j = 0; j < this->enemyContainer[i]->bullet.size(); j++) {
				if(ship->HasCollidedWith(this->enemyContainer[i]->bullet[j]->bulletInstance)) {
					this->enemyContainer[i]->bullet.erase(this->enemyContainer[i]->bullet.begin() + j);
					return true;
				}
			}
		}
		return false;
	}

	void LocateTarget(GalagaShip *object) {
		this->target = object;
	}

	void SetColor(Color col) {
		this->color = col;
	}

	void SetPosition(float x, float y, float z) {
		this->position = Vector3(x, y, z);
	}

	void SetLife(int num) {
		this->tmpLife = num;
	}

	void SetBulletSpeed(float num) {
		this->bulletSpeed = num;
	}

	void SubtractLife(ModelLoad *object) {
		EnemyAttribute *tmpObj = new EnemyAttribute;
		tmpObj->instance = new ModelLoad;
		int index = 0;

		for(int i = 0; i < this->enemyContainer.size(); i++) {
			if(this->enemyContainer[i]->instance == object) {
				tmpObj = this->enemyContainer[i];
				index = i;
			}
		}

		this->ChangeColor(tmpObj);

		if(tmpObj->life != 0)
			tmpObj->life--;

		if(tmpObj->life == 0) {
			score += Mathf::RandomRange(5, 10);
			Sound::Play("assets\\Explosion1.wav");
			this->enemyParticle->SetMaxEnergy(0.5f);
			this->enemyParticle->Initialize(tmpObj->instance->GetPosition(), 30, 30);

			AtomEngine::Clean(tmpObj->instance);
			this->enemyContainer.erase(this->enemyContainer.begin() + index);
		}
	}

	void ChangeColor(EnemyAttribute *&object) {
		if(object->life == 3)
			object->instance->SetColor(Color::Yellow);
		if(object->life == 2)
			object->instance->SetColor(Color::Red);
	}

	void CleanEnemy() {
		for(int i = 0; i < this->enemyContainer.size(); i++) {
			if(this->enemyContainer[i]->life != 0)
				this->enemyContainer[i]->life--;

			if(this->enemyContainer[i]->life == 0) {
				this->enemyParticle->SetMaxEnergy(0.5f);
				this->enemyParticle->Initialize(this->enemyContainer[i]->instance->GetPosition(), 30, 30);

				this->enemyContainer.erase(this->enemyContainer.begin() + i);
			}
		}
		this->enemyContainer.clear();
	}

	std::vector<EnemyAttribute*> GetEnemyContainer() {
		return this->enemyContainer;
	}

private:
	int tmpLife;
	int rateOfFire;
	float bulletSpeed;
	bool hasFired;
	Color color;
	Time *fireTime;
	Vector3 position;
	GalagaShip *target;
	Particle *enemyParticle;
	std::vector<EnemyAttribute*> enemyContainer;
protected:
};

GalagaShip *Ship = new GalagaShip;

Enemy *easyEnemy = new Enemy;
Enemy *medEnemy = new Enemy;
Enemy *hardEnemy = new Enemy;
int totalEnemySize = 0;

bool gameHasStarted = false;
int fireRate = 2.5f;
Time *bulletFireTimer = new Time;

void RandomizeMovement() {
	for(int i = 0; i < easyEnemy->GetEnemyContainer().size(); i++) {
		Vector3 pos = easyEnemy->GetEnemyContainer()[i]->instance->GetPosition();
		if(easyEnemy->GetEnemyContainer()[i]->instance->GetPosition().X > 0)
			pos = pos - Vector3(50.0f, 0.0f, 0.0f);
		easyEnemy->GetEnemyContainer()[i]->instance->SetPosition(pos);
		easyEnemy->GetEnemyContainer()[i]->instance->SetSineTime(Mathf::RandomRange(-50, 50));
	}

	for(int i = 0; i < medEnemy->GetEnemyContainer().size(); i++) {
		Vector3 pos = medEnemy->GetEnemyContainer()[i]->instance->GetPosition();
		if(medEnemy->GetEnemyContainer()[i]->instance->GetPosition().X > 0)
			pos = pos - Vector3(50.0f, 0.0f, 0.0f);
		medEnemy->GetEnemyContainer()[i]->instance->SetPosition(pos);
		medEnemy->GetEnemyContainer()[i]->instance->SetSineTime(Mathf::RandomRange(-50, 50));
	}

	for(int i = 0; i < hardEnemy->GetEnemyContainer().size(); i++) {
		Vector3 pos = hardEnemy->GetEnemyContainer()[i]->instance->GetPosition();
		if(hardEnemy->GetEnemyContainer()[i]->instance->GetPosition().X > 0)
			pos = pos - Vector3(50.0f, 0.0f, 0.0f);
		hardEnemy->GetEnemyContainer()[i]->instance->SetPosition(pos);
		hardEnemy->GetEnemyContainer()[i]->instance->SetSineTime(Mathf::RandomRange(-50, 50));
	}
}

void AtomEngine::Start() {
	AtomEngine::SetProjectName(Title);
	Camera::SetCameraEye(Vector3(0.0f, 0.0f, 100.0f));

	Ship->SetPosition(0.0f, -35.0f, 0.0f);
	Ship->SetShipScale(Vector3::One * 0.5f);
	if(!isDebugging) {
		Ship->Initialize("assets\\Spaceshipbody.obj", 
						"assets\\Spaceshipred.obj", 
						"assets\\Spaceshipblue.obj");
	}
	else {
		Ship->Initialize("Spaceshipbody.obj", 
						"Spaceshipred.obj", 
						"Spaceshipblue.obj");
	}

	easyEnemy->SetLife(1);
	easyEnemy->SetColor(Color::White);
	easyEnemy->SetPosition(-25.0f, 15.0f, 0.0f);
	if(!isDebugging)
		easyEnemy->Initialize("assets\\Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);
	else
		easyEnemy->Initialize("Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);

	medEnemy->SetLife(2);
	medEnemy->SetColor(Color::Cyan);
	medEnemy->SetPosition(-25.0f, 20.0f, 0.0f);
	if(!isDebugging)
		medEnemy->Initialize("assets\\Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);
	else
		medEnemy->Initialize("Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);

	hardEnemy->SetLife(3);
	hardEnemy->SetColor(Color::Magenta);
	hardEnemy->SetPosition(-25.0f, 25.0f, 0.0f);
	if(!isDebugging)
		hardEnemy->Initialize("assets\\Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);
	else
		hardEnemy->Initialize("Crab_2OBJ.obj", -10.0f, 10.0f, 5, 2.5f);

	gameHasStarted = true;
}

bool randomOnce = false;
void AtomEngine::Update() {
	if(gameHasStarted) {
		Sound::PlayLoop("assets\\Theme.wav");
		gameHasStarted = false;
	}

	Ship->DrawShip();
	easyEnemy->LocateTarget(Ship);
	easyEnemy->Render();

	medEnemy->LocateTarget(Ship);
	medEnemy->Render();

	hardEnemy->LocateTarget(Ship);
	hardEnemy->Render();

	totalEnemySize = easyEnemy->GetEnemyContainer().size() + medEnemy->GetEnemyContainer().size() + hardEnemy->GetEnemyContainer().size();

	if(!randomOnce) {
		if(totalEnemySize == 12) {
			RandomizeMovement();

			easyEnemy->SetBulletSpeed(150);
			medEnemy->SetBulletSpeed(150);
			hardEnemy->SetBulletSpeed(150);

			randomOnce = true;
		}
	}

	if(!Ship->isAlive() || totalEnemySize == 0) {
		easyEnemy->CleanEnemy();
		medEnemy->CleanEnemy();
		hardEnemy->CleanEnemy();
		gameState = EndGame;
	}
}

void AtomEngine::CollisionDetection() {
	if(Ship->BulletHasCollidedWith(easyEnemy->GetEnemyContainer()))
		easyEnemy->SubtractLife(Ship->GetObjectHit());

	if(Ship->BulletHasCollidedWith(medEnemy->GetEnemyContainer()))
		medEnemy->SubtractLife(Ship->GetObjectHit());

	if(Ship->BulletHasCollidedWith(hardEnemy->GetEnemyContainer())) 
		hardEnemy->SubtractLife(Ship->GetObjectHit());

	if(easyEnemy->BulletHasCollidedWith(Ship->GetMainInstance()) ||
		medEnemy->BulletHasCollidedWith(Ship->GetMainInstance()) ||
		hardEnemy->BulletHasCollidedWith(Ship->GetMainInstance()) ) {
		Ship->SubtractLife();
	}
}

Time *textTime = new Time;
Time *changeSound = new Time;
Time *introBlinkTime = new Time;
bool introBlink = true;
bool gameHasEnded = false;
int changeText = 0;
void AtomEngine::TextRender() {
	Text::AddText("FPS: %.1f", Time::GetFPS(), -55.0f, 40.0f, 0.0f);

	switch(gameState) {
	case StartGame:
		Text::AddText("Space Invaders", -0.5f, 2.0f, 0.0f);

		if(introBlinkTime->LocalTimeElapsed() > 0.5f)
			introBlink = !introBlink;

		if(introBlinkTime->LocalTimeElapsed() > 0.5f && introBlinkTime->LocalTimeElapsed() < 0.6f)
			introBlinkTime->LocalReset();

		if(introBlink)
			Text::AddText("press SPACEBAR to Start!", -4.0f, 0.0f, 0.0f);
		break;

	case MainGame:
		char text[100];
		sprintf(text, "Ship Position: [%.1f, %.1f, %.1f]", Ship->GetPosition().X, Ship->GetPosition().Y, Ship->GetPosition().Z);
		Text::AddText(-53.0f, -30.0f, 0.0f, text);

		Text::AddText("Ship Life: %i", Ship->GetLife(), -53.0f, -28.0f, 0.0f);
		Text::AddText("Score: %i", score, -40.0f, -28.0f, 0.0f);

		Text::AddText("Controls", -53.0f, -32.0f, 0.0f);
		Text::AddText("A - Move Left", -50.0f, -34.0f, 0.0f);
		Text::AddText("D - Move Right", -50.0f, -36.0f, 0.0f);

		if(enableSound) {
			Text::AddText("Sound: On", -53.0f, -38.0f, 0.0f);
			Sound::Mute(false);
		}
		else {
			Text::AddText("Sound: Off", -53.0f, -38.0f, 0.0f);
			Sound::Mute(true);
		}

		textTime->LocalReset();
		changeSound->LocalReset();
	break;
	
	case EndGame:
		Sound::Mute(false);

		if(!gameHasEnded) {
			if(changeSound->LocalTimeElapsed() > 3.0f) {
				Sound::PlayLoop("assets\\Theme.wav");
				gameHasEnded = true;
			}
		}

		if(textTime->LocalTimeElapsed() > 3.0f) {
			changeText++;

			if(changeText == 4)
				changeText = 0;
		}

		if(textTime->LocalTimeElapsed() > 3.0f && textTime->LocalTimeElapsed() < 3.1f)
			textTime->LocalReset();

		switch(changeText){
		case 0:
			Text::AddText("GAME OVER!", 0.0f, 0.0f, 0.0f);
			break;

		case 1:
			Text::AddText("Final Score: %i", score, -0.5f, 0.0f, 0.0f);
			Text::AddText("Press Esc to Exit", -1.0f, -2.0f, 0.0f);
			break;

		case 2:
			Text::AddText("Space Invaders", -0.5f, 2.0f, 0.0f);
			Text::AddText("Created by: Anthony Ganzon", -5.0f, 0.0f, 0.0f);
			break;

		case 3:
			Text::AddText("Thank you for playing!", -2.0f, 0.0f, 0.0f);
			Text::AddText("Press Esc to Exit", -1.0f, -2.0f, 0.0f);
			break;
		}
		break;
	}
}

void AtomEngine::CleanUp() {
	Ship->~GalagaShip();
	easyEnemy->~Enemy();
	medEnemy->~Enemy();
	hardEnemy->~Enemy();

	AtomEngine::Clean(bulletFireTimer);
	AtomEngine::Clean(textTime);
	AtomEngine::Clean(changeSound);
	AtomEngine::Clean(introBlinkTime);
	AtomEngine::Clean(changeText);
	AtomEngine::Clean(score);
}

void Input::KeyHold() {
	if(Input::KeyDown['a']) {
		if(Ship->GetPosition().X > -50.0f)
			Ship->ShipMoveLeft(20);
	}
	if(Input::KeyDown['d']) {
		if(Ship->GetPosition().X < 50.0f)
			Ship->ShipMoveRight(20);
	}
}

void Input::KeyUp(unsigned char key, int x, int y) {
	Input::KeyDown[key] = true;
	switch(key) {
	case ' ':
		if(!Ship->isAlive()) return;
			Ship->FireBullet(PrimitiveType::Sphere, 150, Vector3::Up);

		if(gameState != MainGame)
			gameState = MainGame;
		break;

	case 's':
		if(gameState != EndGame && gameState != StartGame)
			enableSound = !enableSound;
		break;

	case 27:
		Sound::Stop();
		AtomEngine::ShutDown();
		exit(0);
		break;
	}
}