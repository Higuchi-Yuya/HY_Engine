#include "Util.h"
#include <corecrt_math.h>

uint32_t Util::GetDight(const uint32_t value)
{
	if (value == 0)
	{
		return 1;
	}

	uint32_t tempValue = value;
	uint32_t digit = 0;
	while (tempValue != 0)
	{
		tempValue /= 10;
		digit++;
	}
	return digit;
}

uint32_t Util::GetDightNumber(const int32_t value, const uint32_t dight)
{
	if (dight > GetDight(value) || dight == 0)
	{
		return -1;
	}

	uint32_t num1 = (uint32_t)powf(10.f, (float)dight);
	uint32_t num2 = (uint32_t)powf(10.f, (float)dight - 1);
	return (value % num1) / num2;
}
