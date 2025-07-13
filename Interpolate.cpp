#include "Interpolate.h"

float Interpolate(float Current, float Target, float DeltaTime, float Speed)
{
	return Current + Speed * (Target - Current) * DeltaTime;
}
