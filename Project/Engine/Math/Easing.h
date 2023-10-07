#pragma once

#include "Vector3.h"

//追加
class Easing
{
public:
	static double In(double start, double end, double time, double max_time);

	static double Out(double start, double end, double time, double max_time);
	static Vector3 OutVec3(Vector3 start, Vector3 end, double time, double max_time);

	static double InOut(double start, double end, double time, double max_time);

	static double In_Back(double start, double end, double time, double max_time);

	static double Out_Back(double start, double end, double time, double max_time);

	static double InOut_Back(double start, double end, double time, double max_time);

	static double Out_Bounce(double start, double end, double time, double max_time);

	static double In_Bounce(double start, double end, double time, double max_time);

	static double InOut_Bounce(double start, double end, double time, double max_time);

	static double easeOutCubic(double start, double end, double time, double max_time);
	static Vector3 InOutVec3(Vector3 start, Vector3 end, double time, double max_time);

	static double easeInCirc(double start, double end, double time, double max_time);
	static Vector3 easeInCircVec3(Vector3 start, Vector3 end, double time, double max_time);



};
