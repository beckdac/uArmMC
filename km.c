#include <math.h>

#include "error.h"
#include "print.h"

// units in mm
const float Z_offset = 137.;	// base to shoulder
const float a1=145.5;			// shoulder to elbow length
const float a2=158.7;			// elbow to wrise length

typedef enum { servoL = 0, servoR = 1, servoRot = 2, servoHRot = 3, servoH = 4 } servoID_t;

typedef struct km_servo {
	float	s_min, s_max;
	float	t_min, t_max;
} km_servo_t;

#define SERVOS 5
km_servo_t km_servo[SERVOS] = {
	{ 115, 50, 40, 100 },
	{ 40, 90, 30, 80,  },
	{ 10, 170, -90, 90 },
	{ 10, 170, -90, 90 },
	{ 10, 170, -90, 90 }
};

float map(float x, float in_min, float in_max, float out_min, float out_max) {
	return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

float degrees2radians(float degrees) {
	return (M_PI / 180.) * degrees;
}

float radians2degrees(float radians) {
	return radians * (180. / M_PI);
}

////
void polar2cart(float r, float theta, float *a, float *b)
{
    *a = r * cosf(theta);
    *b = r * sinf(theta);
}

void unsolve(float phi, float t1, float t2, float *x, float *y, float *z)
{
    // Calculate u,v coords for arm
    float u01, v01, u12, v12;
    polar2cart(a1, degrees2radians(t1), &u01, &v01);
    polar2cart(a2, degrees2radians(t1) - degrees2radians(t2), &u12, &v12);

    // Add vectors
    float u, v;
    u = u01 + u12; // + L3;
    v = v01 + v12;

    // Calculate in 3D space - note x/y reversal!
    polar2cart(u, degrees2radians(phi), y, x);
    *z = v;
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2)
{
    float dx = x2-x1;
    float dy = y2-y1;
    float dz = z2-z1;

    return dx*dx + dy*dy + dz*dz;
}
///

void fk(float phi, float theta1, float theta2, float *x, float *y, float *z) {
	// http://profmason.com/?p=569
	// phi - rotation
	// theta1 is inclination of shoulder from base plane
	// theta2 is divergence from vector of shoulder
	// theta3 is inclination of elbow from base plane
	// theta4 is angle between shoulder vector and arm vector
	float theta3, theta4; 
	theta3 = (180 - theta2) - (180 - theta1);
	theta4 = (180 - theta2);
	
	*x = sinf(degrees2radians(phi)) * ((a1 * cosf(degrees2radians(theta1))) + (a2 * cosf(degrees2radians(theta1) - degrees2radians(theta2))));
	*y = cosf(degrees2radians(phi)) * ((a1 * cosf(degrees2radians(theta1))) + (a2 * cosf(degrees2radians(theta1) - degrees2radians(theta2))));
	*z = (a2 * sinf(degrees2radians(theta1))) + (a2 * sinf(degrees2radians(theta1) - degrees2radians(theta2)));
}

void km_test(void) {
	float origin[3] = { 0, 0, 0 };
	float shoulder[3] = { 0, 27, 125 };
	float x, y, z, X, Y, Z, d;
	float stO, stL, stR, phi, tL, tR;

	for (stO = km_servo[servoRot].s_min; stO <= km_servo[servoRot].s_max; stO += 4) {
		phi = map(stO, km_servo[servoRot].s_min, km_servo[servoRot].s_max, km_servo[servoRot].t_min, km_servo[servoRot].t_max);
		for (stL = km_servo[servoL].s_max; stL <= km_servo[servoL].s_min; stL += 4) {
			tL = map(stL, km_servo[servoL].s_min, km_servo[servoL].s_max, km_servo[servoL].t_min, km_servo[servoL].t_max);
			for (stR = km_servo[servoR].s_min; stR <= km_servo[servoR].s_max; stR += 4) {
				tR = map(stR, km_servo[servoR].s_min, km_servo[servoR].s_max, km_servo[servoR].t_min, km_servo[servoR].t_max);
				fk(phi, tR, tL, &x, &y, &z);
				//printf("km_test.0\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", tL, tR, phi, x, y, z);
				printf("km_test.0\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", stL, stR, stO, x, y, z);
				unsolve(phi, tR, tL, &X, &Y, &Z);
				d = distance(x, y, z, X, Y, Z);
				printf("km_test.1\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", stL, stR, stO, X, Y, Z, d);
			}
		}
	}
}
