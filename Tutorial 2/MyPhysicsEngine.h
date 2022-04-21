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

	///Custom scene class
	class MyScene : public Scene
	{
		Plane* plane;
		Box* box;
		Sphere* sphere;
		CompoundObject* comp;
		RectEnclosure* comp3;
		ConvexMesh* pyramid;

		PxRigidDynamic* mBox;

	public:
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
			
			GetMaterial(0)->setDynamicFriction(.1f);
			GetMaterial(0)->setRestitution(.9f);

			GetMaterial(1)->setDynamicFriction(.5f);
			GetMaterial(1)->setRestitution(.3f);

			plane = new Plane();
			plane->Color(PxVec3(210.f/255.f,210.f/255.f,210.f/255.f));
			Add(plane); 


			//sphere = new Sphere(PxTransform(PxVec3(3.0f, 10.f, 3.0f)));
			//sphere->Color(PxVec3(1.f, .1f, .1f));
			//Add(sphere);

			/*comp = new CompoundObject(PxTransform(PxVec3(8.0f, 10.f, 6.0f)), PxTransform(PxVec3(-.5f, .0f, .0f)), PxTransform(PxVec3(.5f, .0f, .0f)));
			comp->Color(PxVec3(255.f / 255.f, 204.f / 255.f, 179.f / 255.f));
			Add(comp);*/

			comp3 = new RectEnclosure(PxTransform(PxVec3(8.0f, 10.f, 6.0f)), .01f, PxVec3(1.f, 1.f, 1.f));
			comp3->Color(PxVec3(255.f / 255.f, 204.f / 255.f, 179.f / 255.f));
			Add(comp3);

			static vector<PxVec3> wedge_verts = { PxVec3(1,1,-1), PxVec3(1,0,-1), PxVec3(-1,1,-1), PxVec3(-1,0,-1), PxVec3(1,0,1), PxVec3(-1,0,1) };

			pyramid = new ConvexMesh(wedge_verts);
			pyramid->Color(PxVec3(1.f, 0.f, 0.f));
			Add(pyramid);

			for (int i = 0; i < 100; i++) {
				box = new Box(PxTransform(PxVec3(.0f, 0.25f, (0.0258f * 2) * i)), PxVec3(0.0254f, 0.0508f, 0.009525));
				box->Color(PxVec3(.01f * i, 1 - .01f * i, .1f));
				Add(box);
				
			}

		}

		//Custom udpate function
		virtual void CustomUpdate() 
		{
			/*PxTransform pose = ((PxRigidBody*)box->Get())->getGlobalPose(); rotating cube
			PxQuat newQuat = PxQuat(0.1f, PxVec3(1.f, 0.f, 0.f));
			pose.q *= newQuat;
			((PxRigidBody*)box->Get())->setGlobalPose(pose);*/



		}
	};
}
