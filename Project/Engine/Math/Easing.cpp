#include "Easing.h"
#include "math.h"
#include <algorithm>

Easing::Easing():
	timer_(0),timeRate_(0),
	limitTimer_(0),IsEnd_(false)
{
}

Easing::Easing(const int32_t limitTime):
	timer_(0), timeRate_(0),
	limitTimer_(limitTime), IsEnd_(false)
{
}

void Easing::Update()
{
	timer_++;
	if (timeRate_ >= 1)
	{
		IsEnd_ = true;
	}

	timer_ = std::min(timer_, limitTimer_);
	timeRate_ = std::min((float)timer_ / limitTimer_, (float)1);
}

void Easing::Reset()
{
	timer_ = 0;
	timeRate_ = 0;
	IsEnd_ = false;
}

void Easing::SetEaseLimitTime(const int32_t limitTime)
{
	limitTimer_ = limitTime;
}

void Easing::SetEaseType(const EaseType easeType)
{
	easeType_ = easeType;
}

void Easing::SetIsEnd(const bool isEnd)
{
	IsEnd_ = isEnd;
}

int32_t Easing::GetTimer()
{
	return timer_;
}

int32_t Easing::GetLimitTime()
{
	return limitTimer_;
}

Easing::EaseType Easing::GetEaseType()
{
	return easeType_;
}

bool Easing::GetIsEnd()
{
	return IsEnd_;
}

float Easing::Lerp(const float startPos, const float endPos)
{
	float dis = endPos - startPos;
	return dis * timeRate_ + startPos;
}

Vector3 Easing::Lerp(const Vector3& start, const Vector3& end)
{
	Vector3 dis = end - start;
	return dis * timeRate_ + start;
}

//追加
float Easing::In(float start, float end)
{
	float move = end - start;
	return start + (move * timeRate_ * timeRate_);
}

float Easing::Out(float start, float end)
{
	
	float move = end - start;
	return start + (move * (1 - (1 - timeRate_) * (1 - timeRate_)));
}

Vector3 Easing::OutVec3(Vector3 start, Vector3 end)
{
	Vector3 Vec;
	float x = Out(static_cast <float>(start.x), static_cast <float>(end.x));
	float y = Out(static_cast <float>(start.y), static_cast <float>(end.y));
	float z = Out(static_cast <float>(start.z), static_cast <float>(end.z));

	Vec = { (float)x,(float)y,(float)z };

	return Vec;
}

float Easing::InOut(float start, float end)
{
	float move = end - start;
	if (timeRate_ < 0.5)
	{
		return start + (move * (2 * timeRate_ * timeRate_));
	}
	else
	{
		return start + move * (1 - (((-2 * timeRate_ + 2) * (-2 * timeRate_ + 2)) / 2));
	}
}

Vector3 Easing::InOut(Vector3 start, Vector3 end)
{
	Vector3 Vec;
	float x = InOut(static_cast <float>(start.x), static_cast <float>(end.x));
	float y = InOut(static_cast <float>(start.y), static_cast <float>(end.y));
	float z = InOut(static_cast <float>(start.z), static_cast <float>(end.z));

	Vec = { (float)x,(float)y,(float)z };

	return Vec;
}

float Easing::In_Back(float start, float end)
{
	
	float move = end - start;
	float c1 = 1.70158f;
	float c3 = (c1 + 1);
	return start + (move * (c3 * timeRate_ * timeRate_ * timeRate_ - c1 * timeRate_ * timeRate_));
}

float Easing::Out_Back(float start, float end)
{
	float move = end - start;
	float c1 = 1.70158f;
	float c3 = (c1 + 1);
	return start + (move * (1 + c3 * ((timeRate_ - 1) * (timeRate_ - 1) * (timeRate_ - 1)) + c1 * ((timeRate_ - 1) * (timeRate_ - 1))));
}

float Easing::InOut_Back(float start, float end)
{
	float move = end - start;
	float c1 = 1.70158f;
	float c2 = c1 * 1.525f;

	if (timeRate_ < 0.5f)
	{
		return start + move * ((((2 * timeRate_) * (2 * timeRate_)) * ((c2 + 1) * 2 * timeRate_ - c2)) / 2);
	}
	else
	{
		return start + move * ((((2 * timeRate_ - 2) * (2 * timeRate_ - 2)) * ((c2 + 1) * (timeRate_ * 2 - 2) + c2) + 2) / 2);
	}
}

float Easing::Out_Bounce(float start, float end)
{
	float move = end - start;
	float n1 = 7.5625f;
	float d1 = 2.75f;
	if (timeRate_ < 1 / d1) {
		return start + move * (n1 * timeRate_ * timeRate_);
	}
	else if (timeRate_ < 2 / d1) {
		return start + move * (n1 * (timeRate_ -= 1.5f / d1) * timeRate_ + 0.75f);
	}
	else if (timeRate_ < 2.5f / d1) {
		return start + move * (n1 * (timeRate_ -= 2.25f / d1) * timeRate_ + 0.9375f);
	}
	else {
		return start + move * (n1 * (timeRate_ -= 2.625f / d1) * timeRate_ + 0.984375f);
	}
}

float Easing::In_Bounce(float start, float end)
{
	float move = end - start;
	float n1 = 7.5625f;
	float d1 = 2.75f;
	timeRate_ = 1 - timeRate_;
	if (timeRate_ < 1 / d1) {
		return start + move * (1 - (n1 * timeRate_ * timeRate_));
	}
	else if (timeRate_ < 2 / d1) {
		return start + move * (1 - (n1 * (timeRate_ -= 1.5f / d1) * timeRate_ + 0.75f));
	}
	else if (timeRate_ < 2.5f / d1) {
		return start + move * (1 - (n1 * (timeRate_ -= 2.25f / d1) * timeRate_ + 0.9375f));
	}
	else {
		return start + move * (1 - (n1 * (timeRate_ -= 2.625f / d1) * timeRate_ + 0.984375f));
	}

}

float Easing::InOut_Bounce(float start, float end)
{
	float move = end - start;
	float n1 = 7.5625f;
	float d1 = 2.75f;
	if (timeRate_ < 0.5f)
	{
		timeRate_ = 1 - timeRate_ * 2;
		if (timeRate_ < 1 / d1) {
			return start + move * ((1 - (n1 * timeRate_ * timeRate_)) / 2);
		}
		else if (timeRate_ < 2 / d1) {
			return start + move * ((1 - (n1 * (timeRate_ -= 1.5f / d1) * timeRate_ + 0.75f)) / 2);
		}
		else if (timeRate_ < 2.5f / d1) {
			return start + move * ((1 - (n1 * (timeRate_ -= 2.25f / d1) * timeRate_ + 0.9375f)) / 2);
		}
		else {
			return start + move * ((1 - (n1 * (timeRate_ -= 2.625f / d1) * timeRate_ + 0.984375f)) / 2);
		}
	}
	else
	{
		timeRate_ = timeRate_ * 2 - 1;
		if (timeRate_ < 1 / d1) {
			return start + move * ((n1 * timeRate_ * timeRate_) / 2 + 0.5f);
		}
		else if (timeRate_ < 2 / d1) {
			return start + move * ((n1 * (timeRate_ -= 1.5f / d1) * timeRate_ + 0.75f) / 2 + 0.5f);
		}
		else if (timeRate_ < 2.5f / d1) {
			return start + move * ((n1 * (timeRate_ -= 2.25f / d1) * timeRate_ + 0.9375f) / 2 + 0.5f);
		}
		else {
			return start + move * ((n1 * (timeRate_ -= 2.625f / d1) * timeRate_ + 0.984375f) / 2 + 0.5f);
		}
	}
}


float Easing::easeOutCubic(float start, float end){
	float x = (float)1 - (float)pow(1 - timeRate_, 3);

	return start + (end - start) * x;
}


float Easing::easeInCirc(float start, float end){

	float x = (float)1 - (float)sqrt(1 - pow(timeRate_, 2));

	return start + (end - start) * x;
}

Vector2 Easing::easeInCircVec2(Vector2 start, Vector2 end)
{
	Vector2 Vec;
	float x = easeInCirc(start.x, end.x);
	float y = easeInCirc(start.y, end.y);

	Vec = { static_cast <float>(x),static_cast <float>(y)};

	return Vec;
}

Vector3 Easing::easeInCircVec3(Vector3 start, Vector3 end)
{
	Vector3 Vec;
	float x = easeInCirc(start.x, end.x);
	float y = easeInCirc(start.y, end.y);
	float z = easeInCirc(start.z, end.z);

	Vec = { static_cast <float>(x),static_cast <float>(y),static_cast <float>(z) };

	return Vec;
}

float Easing::LerpBezireQuadratic(const float& start, const float& contRollP, const float& end)
{
	float p1, p2, p3;
	p1 = Lerp(start, contRollP);
	p2 = Lerp(contRollP, end);
	p3 = Lerp(p1, p2);

	return p3;
}

Vector3 Easing::LerpBezireQuadratic(const Vector3& start, const Vector3& contRollP, const Vector3& end)
{

	Vector3 p1, p2, p3;
	p1 = Lerp(start, contRollP);
	p2 = Lerp(contRollP, end);
	p3 = Lerp(p1, p2);

	return p3;
}

Vector3 Easing::easeOutCircVec3(Vector3 start, Vector3 end)
{
	Vector3 InOutVec;
	float x = easeOutCubic(start.x, end.x);
	float y = easeOutCubic(start.y, end.y);
	float z = easeOutCubic(start.z, end.z);

	InOutVec = { x,y,z };

	return InOutVec;
}