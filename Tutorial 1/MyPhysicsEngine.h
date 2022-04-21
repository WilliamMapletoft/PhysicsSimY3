#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	///A custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Sphere* sphere;
		StaticBox* box2;
		

	public:
		//Custom scene initialisation
		virtual void CustomInit() 
		{
			GetMaterial()->setDynamicFriction(.5f);

			plane = new Plane();
			Add(plane);

			for (int i = 0; i < 150; i++) {
				box = new Box(PxTransform(PxVec3(i, .5f, 0.f)), PxVec3(.1f,.5f,.3f));
				Add(box);
			}

			sphere = new Sphere(PxTransform(PxVec3(0.f, 2.f, 0.f)), 0.5f);
			Add(sphere);

			/*for (int i = 0; i < 4; i++) {
				for (int j = 0; j < 4; j++) {
					sphere = new Sphere(PxTransform(PxVec3(i, 1.f, j)), 0.5f);
					Add(sphere);
				}
			}*/

			/*box2 = new StaticBox(PxTransform(PxVec3(0.f, 0.5f, 0.f)));
			Add(box2);*/


		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			//get position and velocity of the px_actor
			
			//note that the Get() function returns PxActor* only and therefore
			//to be able to access methods from the inherited classess (e.g. PxRigidDynamic)
			//we need to convert the pointer from PxActor* to PxRigidDynamic*
			
			PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
			PxVec3 position = px_actor->getGlobalPose().p;
			PxVec3 velocity = px_actor->getLinearVelocity();

			
			//'visualise' position and velocity of the box
			cout << setiosflags(ios::fixed) << setprecision(2) << "x=" << position.x << 
				", y=" << position.y << ", z=" << position.z << ",  ";
		}
	};
}
