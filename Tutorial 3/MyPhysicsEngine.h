#pragma once

#include "BasicActors.h"
#include <iostream>
#include <iomanip>

namespace PhysicsEngine
{
	using namespace std;

	//a list of colours: Circus Palette
	static const PxVec3 color_palette[] = {PxVec3(46.f/255.f,9.f/255.f,39.f/255.f),PxVec3(217.f/255.f,0.f/255.f,0.f/255.f),
		PxVec3(255.f/255.f,45.f/255.f,0.f/255.f),PxVec3(255.f/255.f,140.f/255.f,54.f/255.f),PxVec3(4.f/255.f,117.f/255.f,111.f/255.f)};

	//pyramid vertices
	static PxVec3 pyramid_verts[] = {PxVec3(0,1,0), PxVec3(1,0,0), PxVec3(-1,0,0), PxVec3(0,0,1), PxVec3(0,0,-1)};
	//pyramid triangles: a list of three vertices for each triangle e.g. the first triangle consists of vertices 1, 4 and 0
	//vertices have to be specified in a counter-clockwise order to assure the correct shading in rendering
	static PxU32 pyramid_trigs[] = {1, 4, 0, 3, 1, 0, 2, 3, 0, 4, 2, 0, 3, 2, 1, 2, 4, 1};

	class Pyramid : public ConvexMesh
	{
	public:
		Pyramid(PxTransform pose=PxTransform(PxIdentity), PxReal density=1.f) :
			ConvexMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), pose, density)
		{
		}
	};

	class PyramidStatic : public TriangleMesh
	{
	public:
		PyramidStatic(PxTransform pose=PxTransform(PxIdentity)) :
			TriangleMesh(vector<PxVec3>(begin(pyramid_verts),end(pyramid_verts)), vector<PxU32>(begin(pyramid_trigs),end(pyramid_trigs)), pose)
		{
		}
	};

	struct FilterGroup
	{
		enum Enum
		{
			ACTOR0		= (1 << 0),
			ACTOR1		= (1 << 1),
			ACTOR2		= (1 << 2)
			//add more if you need
		};
	};

	///An example class showing the use of springs (distance joints).
	class Trampoline
	{
		vector<DistanceJoint*> springs;
		Box *bottom, *top;

	public:
		Trampoline(const PxVec3& dimensions=PxVec3(1.f,1.f,1.f), PxReal stiffness=1.f, PxReal damping=1.f)
		{
			PxReal thickness = .1f;
			bottom = new Box(PxTransform(PxVec3(0.f,thickness,0.f)),PxVec3(dimensions.x,thickness,dimensions.z));
			top = new Box(PxTransform(PxVec3(0.f,dimensions.y+thickness,0.f)),PxVec3(dimensions.x,thickness,dimensions.z));
			springs.resize(4);
			springs[0] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x,thickness,dimensions.z)), top, PxTransform(PxVec3(dimensions.x,-dimensions.y,dimensions.z)));
			springs[1] = new DistanceJoint(bottom, PxTransform(PxVec3(dimensions.x,thickness,-dimensions.z)), top, PxTransform(PxVec3(dimensions.x,-dimensions.y,-dimensions.z)));
			springs[2] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x,thickness,dimensions.z)), top, PxTransform(PxVec3(-dimensions.x,-dimensions.y,dimensions.z)));
			springs[3] = new DistanceJoint(bottom, PxTransform(PxVec3(-dimensions.x,thickness,-dimensions.z)), top, PxTransform(PxVec3(-dimensions.x,-dimensions.y,-dimensions.z)));

			for (unsigned int i = 0; i < springs.size(); i++)
			{
				springs[i]->Stiffness(stiffness);
				springs[i]->Damping(damping);
			}
		}

		void AddToScene(Scene* scene)
		{
			scene->Add(bottom);
			scene->Add(top);
		}

		~Trampoline()
		{
			for (unsigned int i = 0; i < springs.size(); i++)
				delete springs[i];
		}
	};

	///A customised collision class, implemneting various callbacks
	class MySimulationEventCallback : public PxSimulationEventCallback
	{
	public:
		//an example variable that will be checked in the main simulation loop
		bool trigger;
		Scene* scene;

		MySimulationEventCallback(Scene* sceneI) : trigger(false) {
			scene = sceneI;
		}

		///Method called when the contact with the trigger object is detected.
		virtual void onTrigger(PxTriggerPair* pairs, PxU32 count)
		{
			//you can read the trigger information here
			for (PxU32 i = 0; i < count; i++)
			{
				//filter out contact with the planes
				if (pairs[i].otherShape->getGeometryType() != PxGeometryType::ePLANE && pairs[i].otherShape->getGeometryType() != PxGeometryType::eSPHERE) //Filter out planes and spheres from triggering
				{
					string id = pairs[i].otherActor->getName();
					cout << id << endl;
					if (id == "LastDomino") { // Checking that the object colliding with our trigger has the name "LastDomino"
						cout << "Final domino fallen" << endl;
						scene->CustomUpdate(true); //Pass a bool to the scene's update which will be checked by the visualdebugger.cpp to set the UI to the finish screen
					}
				}
			}
		}

		///Method called when the contact by the filter shader is detected.
		virtual void onContact(const PxContactPairHeader& pairHeader, const PxContactPair* pairs, PxU32 nbPairs)
		{
			cerr << "Contact found between " << pairHeader.actors[0]->getName() << " " << pairHeader.actors[1]->getName() << endl;

			//check all pairs
			for (PxU32 i = 0; i < nbPairs; i++)
			{
				//check eNOTIFY_TOUCH_FOUND
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_FOUND)
				{
					cerr << "onContact::eNOTIFY_TOUCH_FOUND" << endl;
				}
				//check eNOTIFY_TOUCH_LOST
				if (pairs[i].events & PxPairFlag::eNOTIFY_TOUCH_LOST)
				{
					cerr << "onContact::eNOTIFY_TOUCH_LOST" << endl;
				}
			}
		}

		virtual void onConstraintBreak(PxConstraintInfo* constraints, PxU32 count) {}
		virtual void onWake(PxActor** actors, PxU32 count) {}
		virtual void onSleep(PxActor** actors, PxU32 count) {}
#if PX_PHYSICS_VERSION >= 0x304000
		virtual void onAdvance(const PxRigidBody* const* bodyBuffer, const PxTransform* poseBuffer, const PxU32 count) {}
#endif
	};

	//A simple filter shader based on PxDefaultSimulationFilterShader - without group filtering
	static PxFilterFlags CustomFilterShader( PxFilterObjectAttributes attributes0,	PxFilterData filterData0,
		PxFilterObjectAttributes attributes1,	PxFilterData filterData1,
		PxPairFlags& pairFlags,	const void* constantBlock,	PxU32 constantBlockSize)
	{
		// let triggers through
		if(PxFilterObjectIsTrigger(attributes0) || PxFilterObjectIsTrigger(attributes1))
		{
			pairFlags = PxPairFlag::eTRIGGER_DEFAULT;
			return PxFilterFlags();
		}

		pairFlags = PxPairFlag::eCONTACT_DEFAULT;
		//enable continous collision detection
//		pairFlags |= PxPairFlag::eCCD_LINEAR;
		
		
		//customise collision filtering here
		//e.g.

		// trigger the contact callback for pairs (A,B) where 
		// the filtermask of A contains the ID of B and vice versa.
		if((filterData0.word0 & filterData1.word1) && (filterData1.word0 & filterData0.word1))
		{
			//trigger onContact callback for this pair of objects
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_FOUND;
			pairFlags |= PxPairFlag::eNOTIFY_TOUCH_LOST;
//			pairFlags |= PxPairFlag::eNOTIFY_CONTACT_POINTS;
		}

		return PxFilterFlags();
	};

	///Custom scene class
	class MyScene : public Scene
	{
		vector<Actor*> bullets, bullets2;
		Pyramid* pyramid;
		Cloth* cloth;
		Plane* plane;
		Sphere* marble;
		Hammer* hammer;
		Box* box, box2;
		SBox* staticBox;
		MySimulationEventCallback* my_callback;
		PxMaterial* dominoMat;
		RevoluteJoint* hamJoint;
		DistanceJoint* distJoint;
		bool isDone, currentList;
		
	public:
		//specify your custom filter shader here
		//PxDefaultSimulationFilterShader by default
		MyScene() : Scene() {};

		///A custom scene class
		void SetVisualisation()
		{
			px_scene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
			px_scene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);
		}

		//Custom scene initialisation
		virtual void CustomInit() 
		{
			SetVisualisation();			

			GetMaterial()->setDynamicFriction(.2f);

			///Initialise and set the customised event callback

			dominoMat = CreateMaterial(0.2f, 0.2f, 0.6f);

			my_callback = new MySimulationEventCallback(this);
			px_scene->setSimulationEventCallback(my_callback);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane);

			hammer = new Hammer(PxTransform(PxVec3(5.f, 1.5f, 1.5f)), 1.f, 2.f); //Creating an object of type "Hammer" as defined in BasicActors.h
			hammer->Color(PxVec3(0.f, 0.f, 0.f));
			PxRigidDynamic* px_actor = (PxRigidDynamic*)hammer->Get();
			px_actor->setAngularDamping(10.0f); //Applying angular damping to ensure that the hammer won't freely swing for too long
			px_actor->setSleepThreshold(0.f); //Ensure the hammer will not go to sleep automatically, which stops the joint's motor from moving the hammer
			
			hamJoint = new RevoluteJoint(NULL,
				PxTransform(PxVec3(5.f, 1.423f, 1.5f), PxQuat(PxPi * 2, PxVec3(1.f, 0.f, 0.f))),
				hammer, 
				PxTransform(PxVec3(0.0f, 1.15f, 0.f))); //Connecting the hammer at local position 0, 1.15, 0 to the world position 5, 1.423, 1.5
			hamJoint->DriveVelocity(-1.f); //Setting the maximum power of the drive to be used on the user pressing H

			Add(hammer);


			//set collision filter flags
			// box->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1);
			//use | operator to combine more actors e.g.
			// box->SetupFiltering(FilterGroup::ACTOR0, FilterGroup::ACTOR1 | FilterGroup::ACTOR2);
			//don't forget to set your flags for the matching actor as well, e.g.:
			// box2->SetupFiltering(FilterGroup::ACTOR1, FilterGroup::ACTOR0);


			/*
			//joint two boxes together
			//the joint is fixed to the centre of the first box, oriented by 90 degrees around the Y axis
			//and has the second object attached 5 meters away along the Y axis from the first object.
			RevoluteJoint joint(box, PxTransform(PxVec3(0.f,0.f,0.f),PxQuat(PxPi/2,PxVec3(0.f,1.f,0.f))), box2, PxTransform(PxVec3(0.f,5.f,0.f)));
			*/

			PxTransform startLoc = PxTransform(PxVec3(5.f, 0.f, 2.f), PxQuat(PxPi * 2, PxVec3(0.f, 1.f, 0.f)));

			//PxTransform temp = startLoc;


			startLoc = spawnLine(startLoc, 50, 1.98f);
			startLoc = spawnCorner(startLoc, 25, PxPiDivTwo);
			startLoc = spawnStairs(startLoc, 50);
			startLoc = spawnLine(startLoc, 50, 1.98f);
			startLoc = spawnCorner(startLoc, 25, PxPiDivTwo);
			startLoc = spawnLine(startLoc, 5, 1.90f);
			startLoc.p.y = 0.05f;
			startLoc = spawnCorner(startLoc, 25, PxPiDivTwo);
			startLoc = spawnStairs(startLoc, 50);
			startLoc = spawnCorner(startLoc, 25, -PxPiDivTwo);
			startLoc = spawnStairs(startLoc, 50);
			startLoc = spawnCorner(startLoc, 25, -PxPiDivTwo);
			startLoc = spawnStairs(startLoc, 50);
			startLoc.p.y = 0.05f;
			startLoc = spawnStairs(startLoc, 50);
			startLoc = spawnStairs(startLoc, 25, true);
			startLoc = spawnCorner(startLoc, 25, -PxPiDivTwo);
			startLoc = spawnLine(startLoc, 5, 1.9f);
			startLoc.p.y = 0.05f;
			startLoc = spawnLine(startLoc, 100, 1.98f);
			box->Name("LastDomino"); //Setting the last domino to be spawned to have the name "LastDomino" which will be filtered for

			staticBox = new SBox(startLoc, PxVec3(0.0254f, 0.0508f, 0.009525f)); //Spawn a static trigger box at the end of the domino run
			staticBox->SetTrigger(true, 0); //Setting the static box to be a trigger
			staticBox->Name("TriggerBox");
			Add(staticBox);

			startLoc.p.y += 4.f;
			cloth = new Cloth(startLoc, PxVec2(4.f, 4.f), 20, 20, true);
			Add(cloth);
			
		}

		//Custom udpate function
		virtual void CustomUpdate(bool amIDone)
		{
			if (amIDone == true) {
				isDone = amIDone;
			}
		}

		PxTransform spawnLine(PxTransform startLocation, PxI16 noDominoes, float shrink) // Spawns a straight line in the forward vector of the transform passed to the function
		{
			PxTransform temp = PxTransform(PxVec3(
				startLocation.p.x - (startLocation.q.getBasisVector2().x * 0.0616f),
				startLocation.p.y,
				startLocation.p.z - (startLocation.q.getBasisVector2().z * 0.0616f)
			));
			for (int i = 0; i < noDominoes; i++) { // For loop to place x dominoes
				box = new Box(PxTransform(PxVec3(
					startLocation.p.x + (startLocation.q.getBasisVector2().x * 0.0616f * i),
					startLocation.p.y, 
					startLocation.p.z + (startLocation.q.getBasisVector2().z * 0.0616f * i)
				)), PxVec3(0.0254f, 0.0508f, 0.009525f));

				PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
				px_actor->setGlobalPose(PxTransform(px_actor->getGlobalPose().p ,startLocation.q));
				box->Material(dominoMat);
				box->Color(PxVec3(1.f, 1.f, 1.f));
				box->Name("Domino");
				Add(box);
			}

			PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
			PxVec3 position = px_actor->getGlobalPose().p;

			position.x = position.x + (startLocation.q.getBasisVector2().x * 0.0616f);
			position.z = position.z + (startLocation.q.getBasisVector2().z * 0.0616f);

			if (temp.p.y > 0.1f) { spawnFloor(temp, px_actor->getGlobalPose(), shrink); }

			startLocation.p = position;

			return startLocation;
		}
		// Spawnline -> Start location, number of dominoes (length of line), size of platform below floating dominoes
		PxTransform spawnStairs(PxTransform startLocation, PxI16 noDominoes) {

			for (int i = 0; i < noDominoes; i++) { // For loop to place x dominoes

				PxTransform temp = PxTransform(PxVec3(
					startLocation.p.x + (startLocation.q.getBasisVector2().x * 0.0616f * i),
					startLocation.p.y + (startLocation.q.getBasisVector1().y * 0.0116f * i),
					startLocation.p.z + (startLocation.q.getBasisVector2().z * 0.0616f * i)
				), startLocation.q);

				if ((temp.p.y) > 0.01f) {
					spawnFloor(temp, temp, 2.0f);
				}

				box = new Box(temp, PxVec3(0.0254f, 0.0508f, 0.009525f));

				PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
				px_actor->setGlobalPose(PxTransform(px_actor->getGlobalPose().p, startLocation.q));
				box->Material(dominoMat);
				box->Color(PxVec3(1.f, 1.f, 1.f));
				box->Name("Domino");
				Add(box);
			}

			PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
			PxVec3 position = px_actor->getGlobalPose().p;

			position.x = position.x + (startLocation.q.getBasisVector2().x * 0.0616f);
			position.y = position.y + (startLocation.q.getBasisVector1().y * 0.0116f);
			position.z = position.z + (startLocation.q.getBasisVector2().z * 0.0616f);

			startLocation.p = position;

			return startLocation;
		}
		// Spawnstairs -> Start location, number of dominoes (length of line)
		PxTransform spawnStairs(PxTransform startLocation, PxI16 noDominoes, bool forDown) {

			for (int i = 0; i < noDominoes; i++) { // For loop to place x dominoes

				PxTransform temp = PxTransform(PxVec3(
					startLocation.p.x + (startLocation.q.getBasisVector2().x * 0.0616f * i),
					startLocation.p.y - (startLocation.q.getBasisVector1().y * 0.0116f * i),
					startLocation.p.z + (startLocation.q.getBasisVector2().z * 0.0616f * i)
				), startLocation.q);

				if ((temp.p.y) > 0.01f) {
					spawnFloor(temp, temp, 2.0f);
				}

				box = new Box(temp, PxVec3(0.0254f, 0.0508f, 0.009525f));

				PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
				px_actor->setGlobalPose(PxTransform(px_actor->getGlobalPose().p, startLocation.q));
				box->Material(dominoMat);
				box->Color(PxVec3(1.f, 1.f, 1.f));
				box->Name("Domino");
				Add(box);
			}

			PxRigidDynamic* px_actor = (PxRigidDynamic*)box->Get();
			PxVec3 position = px_actor->getGlobalPose().p;

			position.x = position.x + (startLocation.q.getBasisVector2().x * 0.0616f);
			position.y = position.y - (startLocation.q.getBasisVector1().y * 0.0116f);
			position.z = position.z + (startLocation.q.getBasisVector2().z * 0.0616f);

			startLocation.p = position;

			return startLocation;
		}
		// Spawnstairs (down) -> Start location, number of dominoes (length of line), boolean for overloading downwards stairs
		PxTransform spawnCorner(PxTransform startLocation, PxI16 noDominoes, float angleRad) // Spawns a corner with parameterized number of dominoes, angle and direction
		{
			PxRigidDynamic* px_actor = NULL;
			PxTransform init = startLocation;
			PxQuat tempRot;
			PxTransform temp;
			float maxX = startLocation.p.x;
			float maxZ = startLocation.p.z;
			float minX = startLocation.p.x;
			float minZ = startLocation.p.z;
			temp = startLocation;

			float rotateRad;

			rotateRad = angleRad;

			tempRot = startLocation.q * PxQuat(rotateRad / noDominoes, startLocation.q.getBasisVector1());

			temp.p = PxVec3(
				temp.p.x - (tempRot.getBasisVector2().x * ((0.0616f))),
				temp.p.y,
				temp.p.z - (tempRot.getBasisVector2().z * ((0.0616f))));

			for (int i = 0; i < noDominoes + 1; i++) {
				tempRot = startLocation.q * PxQuat((rotateRad / noDominoes) * i, tempRot.getBasisVector1());

				box = new Box(PxTransform(PxVec3(
					temp.p.x + (tempRot.getBasisVector2().x * (0.0616f)),
					temp.p.y,
					temp.p.z + (tempRot.getBasisVector2().z * (0.0616f))
				)), PxVec3(0.0254f, 0.0508f, 0.009525));

				px_actor = (PxRigidDynamic*)box->Get();
				px_actor->setGlobalPose(PxTransform(px_actor->getGlobalPose().p, tempRot));
				temp = px_actor->getGlobalPose();
				tempRot = px_actor->getGlobalPose().q;

				if (temp.p.x > maxX) {
					maxX = temp.p.x;
				}
				else if (temp.p.x < minX) {
					minX = temp.p.x;
				}

				if (temp.p.z > maxZ) {
					maxZ = temp.p.z;
				}
				else if (temp.p.z < minZ) {
					minZ = temp.p.z;
				}
				box->Material(dominoMat);
				box->Color(PxVec3(1.f, 1.f, 1.f));
				box->Name("Domino");
				Add(box);
			}

			px_actor = (PxRigidDynamic*)box->Get();
			PxVec3 position = px_actor->getGlobalPose().p;
			position.x = position.x + (tempRot.getBasisVector2().x * ((0.0616f)));
			position.z = position.z + (tempRot.getBasisVector2().z * ((0.0616f)));

			tempRot = startLocation.q * PxQuat(rotateRad, startLocation.q.getBasisVector1());

			startLocation.p = position;
			startLocation.q = tempRot;

			if (temp.p.y > 0.1f) {
				cout << minX << endl;
				cout << maxX << endl;
				cout << std::abs(minX - maxX) << endl;
				spawnFloor(init, PxTransform(PxVec3(
					startLocation.p.x - (tempRot.getBasisVector2().x * ((0.0616f))),
					startLocation.p.y,
				startLocation.p.z - (tempRot.getBasisVector2().z * ((0.0616f))))), 1.8f, std::abs(minX - maxX), std::abs(minZ - maxZ)); }


			return PxTransform(startLocation);
		}
		// Spawncorner -> Start location, number of dominoes, angle of corner (radians)

		void spawnFloor(PxTransform start, PxTransform end, float shrinkConst) {

			float diffX = std::abs(start.p.x - end.p.x);
			float diffZ = std::abs(start.p.z - end.p.z);
			float offsetX = 0.0f;
			float offsetZ = 0.0f;

			if (diffX < 0.1f) { offsetX = .03f; }
			if (diffZ < 0.1f) { offsetZ = .03f; }

			float constX;
			if (start.p.x > end.p.x) {
				constX = start.p.x - ((diffX) / 2);
				//cout << "Going inverse X" << endl;
			}
			else {
				constX = start.p.x + ((diffX) / 2);
				//cout << "Going standard X" << endl;
			}

			float constZ;
			if (start.p.z > end.p.z) {
				constZ = start.p.z - ((diffZ) / 2);
				//cout << "Going inverse Z" << endl;
			}
			else {
				constZ = start.p.z + ((diffZ) / 2);
				//cout << "Going standard Z" << endl;
			}


			staticBox = new SBox(PxTransform(PxVec3(constX, start.p.y - 0.15f, constZ)), PxVec3(offsetX + diffX / shrinkConst, 0.1f, offsetZ + diffZ / shrinkConst));

			staticBox->Color(PxVec3(0.f, 0.f, 0.f));

			Add(staticBox);
		}

		void spawnFloor(PxTransform start, PxTransform end, float shrinkConst, float diffX, float diffZ) {
			float offsetX = 0.0f;
			float offsetZ = 0.0f;

			if (diffX < 0.1f) { offsetX = .03f; }
			if (diffZ < 0.1f) { offsetZ = .03f; }

			float constX;
			if (start.p.x > end.p.x) {
				constX = start.p.x - ((diffX) / 2);
				//cout << "Going inverse X" << endl;
			}
			else {
				constX = start.p.x + ((diffX) / 2);
				//cout << "Going standard X" << endl;
			}

			float constZ;
			if (start.p.z > end.p.z) {
				constZ = start.p.z - ((diffZ) / 2);
				//cout << "Going inverse Z" << endl;
			}
			else {
				constZ = start.p.z + ((diffZ) / 2);
				//cout << "Going standard Z" << endl;
			}


			staticBox = new SBox(PxTransform(PxVec3(constX, start.p.y - 0.15f, constZ)), PxVec3(offsetX + diffX / shrinkConst, 0.1f, offsetZ + diffZ / shrinkConst));

			staticBox->Color(PxVec3(0.f, 0.f, 0.f));

			Add(staticBox);
		}

		/// An example use of key release handling
		void HammerRelease()
		{
			PxRevoluteJoint* temp = (PxRevoluteJoint*)hamJoint->Get();
			temp->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, false);
			cerr << "I am realeased!" << endl;
		}

		/// An example use of key presse handling
		void HammerPress()
		{
			PxRevoluteJoint* temp = (PxRevoluteJoint*)hamJoint->Get();
			temp->setRevoluteJointFlag(PxRevoluteJointFlag::eDRIVE_ENABLED, true);
			cerr << "I am pressed!" << endl;
		}

		void Fire(PxTransform camera) { // Function to fire 10 marbles from camera
			if (!bullets.empty() && !bullets2.empty()) { // If statement clearing the list of bullets that is older. This allows for exactly two lists of marbles.
				if (currentList == true) { //Depending on the value of a boolean currentList, clear the list not in current use
					for (int i = 0; i < bullets.size(); i++) {
						bullets[i]->Get()->release(); 
					}
					bullets.clear();
				}
				else
				{
					for (int i = 0; i < bullets2.size(); i++) {
						bullets2[i]->Get()->release();
					}
					bullets2.clear();
				}
			}

			for (int i = 0; i < 10; i++) { //For loop spawning all 10 marbles
				PxTransform marbLoc = camera;
				marbLoc.p.x += (i * 0.1 * marbLoc.q.getBasisVector2().x); //We use the basis vector to orient the marbles position based on the cameras rotation
				marbLoc.p.z += (i * 0.1 * marbLoc.q.getBasisVector2().z);
				marble = new Sphere(marbLoc, 0.013f);
				marble->Color(color_palette[0]);
				PxRigidDynamic* temp = (PxRigidDynamic*)marble->Get();
				Add(marble);
				if (currentList == true) {	bullets.push_back(marble);} //Use the switch boolean to decide which list to add the marbles to
				else {bullets2.push_back(marble);}
				marble->Material(CreateMaterial(0.9f, 0.4f, .658f)); //Applying the material properties of glass to the marbles
				marble->Name("Bullet");
				temp->addForce(camera.q.getBasisVector2() * -0.0002f, PxForceMode::eIMPULSE); //Applying a small, but relative to the size of the marbles, significant impulse
			}
			currentList = !currentList; //Switch the boolean
		}

		bool dominoesDone() {
			return isDone;
		}

	};

	
}
