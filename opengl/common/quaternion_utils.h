#ifndef QUATERNION_UTILS_H
#define QUATERNION_UTILS_H

quat RotationBetweenVectors(vec3 start, vec3 dest);

quat LookAt(vec3 direction, vec3 desiredUp);

quat RotateTowards(quat q1, quat q2, float maxAngle);

class quaternion_utils
{
public:
	quaternion_utils();
	~quaternion_utils();
};


#endif // QUATERNION_UTILS_H