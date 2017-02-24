/*
 * PhaseSpaceTracker.hpp
 *
 *  Created on: Jun 9, 2016
 *      Author: demo
 */

#ifndef PHASESPACETRACKER_HPP_
#define PHASESPACETRACKER_HPP_

#include "ProjectOrion.h"
#include <vector>
#include <iostream>
#include <cstring>

//#include <unistd.h>
#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
//#include <netdb.h>

#include "PhaseSpace/owl.hpp"
#include "PhaseSpace/owl_math.h"

#include "Types.hpp"

#include "json/json.h"
#include <string>
#include <istream>
#include <streambuf>

#define X 0
#define Y 1
#define Z 2

struct PhaseSpaceMarker
{
	int id;
	std::string name;
	std::string options;
};

class PhaseSpaceRigidBody
{
public:
	PhaseSpaceRigidBody(std::string filename)
	{
		FILE* f = fopen(filename.c_str(), "rb");
		fseek(f, 0, SEEK_END);
		long fsize = ftell(f);
		fseek(f, 0, SEEK_SET);

		json = (char*)malloc(fsize + 1);
		jsonlength = fsize + 1;
		fread((void*)json, fsize, 1, f);
		json[fsize] = 0;

		fclose(f);

		Json::Value root;
		Json::Reader reader;

		reader.parse(json, json + jsonlength, root, false);

		Json::Value trackers = root["trackers"];
		parseTracker(trackers); //use the first tracker
	}

	~PhaseSpaceRigidBody()
	{
		free(json);
	}

private:
	char* json;
	int jsonlength;

	int id;
	std::vector<PhaseSpaceMarker> markers;

	PhaseSpaceMarker parseMarker(Json::Value token)
	{
		PhaseSpaceMarker marker;

		marker.id = token["id"].asInt();
		marker.name = token["name"].asString();
		marker.options = token["options"].asString();

		return marker;
	}

	void parseTracker(Json::Value tracker)
	{
		id = tracker["id"].asInt();

		for(unsigned int i = 0; i < tracker["markers"].size(); i++)
		{
			markers.push_back(parseMarker(tracker["markers"][i]));
		}
	}

public:
	void Create(OWL::Context& owl)
	{
		owl.createTracker(id, "rigid", "head");
		for(unsigned int i = 0; i < markers.size(); i++)
		{
			owl.assignMarker(id, markers[i].id, markers[i].name.c_str(), markers[i].options.c_str());
		}
	}
};

class PhaseSpaceTracker
{

public:
	PhaseSpaceTracker(std::string Address, PhaseSpaceRigidBody* headrigidbody)
	{
		headRigidBody = headrigidbody;

		headPosition.assign(3, 0);
		headLookat.assign(3,0);
		leftFoot.assign(3, 0);
		rightFoot.assign(3, 0);
		headUp.assign(3,0);
		address = Address;

		flags = 0;

		head_id = 0;
		leftFoot_id = 25;
		rightFoot_id = 38;

		connected = false;
		valid = false;

	}
	~PhaseSpaceTracker()
	{
		if(owl.isOpen())
		{
			owl.done();
			owl.close();
		}
	}

	void Connect()
	{
		int result = owl.open(address);

		if (result == 0)
		{
			printf("ERROR: Timeout connecting to PhaseSpace\n");
			return;
		}
		if (result < 0)
		{
			printf("ERROR: Error connecting to PhaseSpace");
			return;
		}

		result = owl.initialize();

		if (result < 0)
		{
			printf("ERROR: Error initialising PhaseSpace");
			return;
		}

		connected = true;

		/* create the trackers */

		headRigidBody->Create(owl);

		/* Create some tracker using the markers so the server will look for them */

		owl.createTracker(1, "point", "lfoot", "");
		owl.assignMarker(1, leftFoot_id, "25", "");

		owl.createTracker(2, "point", "rfoot", "");
		owl.assignMarker(1, rightFoot_id, "38", "");

		owl.streaming(1);
	}

	bool isConnected()
	{
		return connected;
	}

	void Update()
	{
		valid = false;

		orientationcond = 0;

		if(owl.isOpen() && owl.property<int>("initialized"))
		{
			const OWL::Event *event = owl.nextEvent(0);	//no timeout - poll
			if (!event)
			{
				return;
			}

			if(event->type_id() == OWL::Type::ERROR)
			{
				//cerr << event->name() << ": " << event->str() << endl; //commented out in case nonfatal errors cause the main loop to stall
				return;
			}

			if(event->type_id() == OWL::Type::FRAME)
			{
				if(event->find("markers", markers) > 0)
				{
					for (OWL::Markers::iterator m = markers.begin(); m != markers.end(); m++)
					{
						if (m->cond > 0)
						{
							if (m->id == leftFoot_id)
							{
								leftFoot[0] = m->x * 0.1f;
								leftFoot[1] = m->y * 0.1f;
								leftFoot[2] = m->z * 0.1f;
							}
							if (m->id == rightFoot_id)
							{
								rightFoot[0] = m->x * 0.1f;
								rightFoot[1] = m->y * 0.1f;
								rightFoot[2] = m->z * 0.1f;
							}
						}
					}
				}

				if (event->find("rigids", rigids) > 0)
				{
					for (OWL::Rigids::iterator m = rigids.begin(); m != rigids.end(); m++)
					{
						if (m->cond > 0)
						{
							if (m->id == head_id)
							{
								headPosition[0] = m->pose[0] * 0.1f;
								headPosition[1] = m->pose[1] * 0.1f;
								headPosition[2] = m->pose[2] * 0.1f;

								memcpy(&orientation, &(m->pose[3]), sizeof(float) * 3);

								orientationcond = m->cond;

								float forward[3] = { 0.0f,0.0f,-1.0f }; //-1 because rigid body was created 'backwards'
								float up[3] = { 0.0f,1.0f,0.0f };
								owl_mult_qvq(orientation.data(), forward, headLookat.data());
								owl_mult_qvq(orientation.data(), up, headUp.data());

								valid = true;
							}
						}
					}
				}
			}
		}
	}

private:

	OWL::Context owl;
	OWL::Markers markers;
	OWL::Rigids rigids;

	bool connected;
	bool valid;

	std::string address;
	size_t flags;

	unsigned int head_id;
	unsigned int leftFoot_id;
	unsigned int rightFoot_id;

	std::vector<float> headPosition;
	std::vector<float> headLookat;
	std::vector<float> headUp;
	std::vector<float> leftFoot;
	std::vector<float> rightFoot;

	float orientationcond;

	quaternion orientation;

	PhaseSpaceRigidBody* headRigidBody;

public:

	std::vector<float> GetHeadPosition()
	{
		return headPosition;
	}

	std::vector<float> GetHeadUp()
	{
		return headUp;
	}

	std::vector<float> GetHeadLookat()
	{
		return headLookat;
	}

	std::vector<float> GetLeftFoot()
	{
		return leftFoot;
	}

	std::vector<float> GetRightFoot()
	{
		return rightFoot;
	}

	quaternion GetOrientation()
	{
		return orientation;
	}

	float GetOrientationCond()
	{
		return orientationcond;
	}

	bool GetValid()
	{
		return valid;
	}

private:


};

#endif /* PHASESPACETRACKER_HPP_ */
