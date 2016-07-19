#include "stdafx.h"

#include "assert.h"
#include <chrono>
#include <thread>
#include <tuple> //%deprecated

#include "GLBase.h"
#include "Scene.h"
#include "PhysicalObject.h"
#include "DrawScene.h"

using namespace std;
using namespace chrono;

void GameUpdateLoop(
	Scene & i_oScene)
{
	//Setup timers
	auto nUpdateInterval = chrono::milliseconds(16);
	chrono::steady_clock::time_point nCurrentTime = chrono::steady_clock::now();
	chrono::steady_clock::time_point nNextUpdateTime = nCurrentTime	+ nUpdateInterval;
	
	while(true)
	{
		while(nCurrentTime < nNextUpdateTime)
		{
			nCurrentTime = chrono::steady_clock::now();
		}
		
		duration<double> oTimeSinceLastUpdate = duration_cast<duration<double>>(
			nCurrentTime - (nNextUpdateTime - nUpdateInterval));
		double dTimeSinceLastUpdate = /*0.001 * */oTimeSinceLastUpdate.count();
		assert(dTimeSinceLastUpdate > 0.0);

		nNextUpdateTime = nCurrentTime + nUpdateInterval;

		//%Are we sure we don't want += nUpdateInterval, with the option to 
		//%Skip updates if we fall behind. That is, consume events except most recent.
		//%Right now we are effectively specifying "at least 16 milliseconds update interval"
		//%could store time since last update inside physics objects.
		//%that way we can skip events without accumulating a deltaT
		
		//Input devices update
		//Consuming this information: Update camera position/frame
		
		//Physics tick
		//i_oScene.AdvanceStates(dTimeSinceLastUpdate); //%uncomment
		
		//Graphics update
		i_oScene.DrawAll();
	}
}