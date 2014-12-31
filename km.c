#include <math.h>

#include "error.h"
#include "print.h"

const float PI=3.14159265359;

// units in mm
const float a1=145.5;			// shoulder to elbow length
const float a2=158.7;			// elbow to wrise length
const float b0=93;				// bed to shoulder height
#warning estimate, need to remeasure
const float a3=0;			// sum of base to shoulder in plane of surface and wrist to effector

// for translating from real space to servo space
typedef enum { servoL = 0, servoR = 1, servoRot = 2, servoHRot = 3, servoH = 4 } servoID_t;

typedef struct km_servo {
	float	s_min, s_max;
	float	t_min, t_max;
} km_servo_t;

#define SERVOS 5
km_servo_t km_servo[SERVOS] = {
	{ 115, 50, -87, -27 },	// elbow
	{ 40, 90, 15, 65,  },	// shoulder
	{ 10, 170, -80, 80 },	// base
	{ 10, 170, -80, 80 },	// hand rotation
	{ 10, 170, -80, 80 }	// hand
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

void rotate_point(float px, float py, float ox, float oy, float theta, float *rpx, float *rpy) {
	*rpx = cos(theta) * (px-ox) - sin(theta) * (py-oy) + ox;
	*rpy = sin(theta) * (px-ox) + cos(theta) * (py-oy) + oy;
}

void polar2cart(float r, float theta, float *a, float *b) {
	*a = r * cosf(theta);
	*b = r * sinf(theta);
}

// Get polar coords from cartesian ones
void cart2polar(float a, float b, float *r, float *theta) {
	// Determine magnitude of cartesian coords
	*r = sqrtf(a*a + b*b);

	// Don't try to calculate zero-magnitude vectors' angles
	if(r == 0) return;

	float c = a / *r;
	float s = b / *r;

	// Safety!
	if(s > 1) s = 1;
	if(c > 1) c = 1;
	if(s < -1) s = -1;
	if(c < -1) c = -1;

	// Calculate angle in 0..PI
	*theta = acos(c);

	// Convert to full range
	if(s < 0) *theta *= -1;
}

// Get angle from a triangle using cosine rule
int cosangle(float opp, float adj1, float adj2, float *theta) {
	// Cosine rule:
	// C^2 = A^2 + B^2 - 2*A*B*cos(angle_AB)
	// cos(angle_AB) = (A^2 + B^2 - C^2)/(2*A*B)
	// C is opposite
	// A, B are adjacent
	float den = 2*adj1*adj2;

	if(den==0) return 0;
	float c = (adj1*adj1 + adj2*adj2 - opp*opp)/den;

	if(c>1 || c<-1) return 0;

	*theta = acos(c);

	return 1;
}

// Solve angles!
int solve(float x, float y, float z, float *phi, float *thetaS, float *thetaE) {
	// Solve top-down view
	float r, th0;
	cart2polar(y, x, &r, &th0);

	// Account for the wrist length!
	r -= a3;

	// In arm plane, convert to polar
	float ang_P, R;
	cart2polar(r, z - b0, &R, &ang_P);

	// Solve arm inner angles as required
	float B, C;
	if(!cosangle(a2, a1, R, &B)) return 0;
	if(!cosangle(R, a1, a2, &C)) return 0;

	// Solve for servo angles from horizontal
	*phi = th0;
	*thetaS = ang_P + B;
	*thetaE = C + *thetaS - PI;

	return 1;
}

void unsolve(float phi, float thetaS, float thetaE, float *x, float *y, float *z) {
	// Calculate u,v coords for arm
	float u01, v01, u12, v12;
	polar2cart(a1, thetaS, &u01, &v01);
	polar2cart(a2, thetaE, &u12, &v12);

	// Add vectors
	float u, v;
	u = u01 + u12 + a3;
	v = v01 + v12;

	// Calculate in 3D space - note x/y reversal!
	polar2cart(u, phi, y, x);
	*z = v + b0;
}

float distance(float x1, float y1, float z1, float x2, float y2, float z2) {
	float dx = x2-x1;
	float dy = y2-y1;
	float dz = z2-z1;
	return sqrtf(dx*dx + dy*dy + dz*dz);
}
///

void km_test(void) {
	float x, y, z, phi, thetaS, thetaE, X, Y, Z, d, sphi, sthetaS, sthetaE;
	char line[1024];

	while (fgets(line, 1024, stdin) != NULL) {
		phi = thetaS = thetaE = X = Y = Z = 0;
		sscanf(line, "%f %f %f", &x, &y, &z);
		if (!solve(x, y, z, &phi, &thetaS, &thetaE)) {
			printf("> no solution %.2f %.2f %.2f\n", x, y, z);
			continue;
		}
		printf(">\t%.2f\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\n", x, y, z, radians2degrees(phi), radians2degrees(thetaS), radians2degrees(thetaE));
		unsolve(phi, thetaS, thetaE, &X, &Y, &Z);
		d = distance(x, y, z, X, Y, Z);
		printf("<\t%.2f\t%.2f\t%.2f\t\t%.2f\t%.2f\t%.2f\t%.2f\n", X, Y, Z, radians2degrees(phi), radians2degrees(thetaS), radians2degrees(thetaE), d);
		sphi = map(radians2degrees(phi), km_servo[servoRot].t_min, km_servo[servoRot].t_max, km_servo[servoRot].s_min, km_servo[servoRot].s_max);
		sthetaS = map(radians2degrees(thetaS), km_servo[servoR].t_min, km_servo[servoR].t_max, km_servo[servoR].s_min, km_servo[servoR].s_max);
		sthetaE = map(radians2degrees(thetaE), km_servo[servoL].t_min, km_servo[servoL].t_max, km_servo[servoL].s_min, km_servo[servoL].s_max);
		printf("G1 W1 F10 O%d R%d L%d\n", (int)roundf(sphi), (int)roundf(sthetaS), (int)roundf(sthetaE));
		if (d > 0.01)
			fatal_error("distance between forward and reverse solutions is > 0.01\n");
		{
			float ex, ey, wx, wy;
			rotate_point(a1, b0, 0, b0, thetaS, &ex, &ey);
			//printf("%f\n", distance(ex, ey, 0, 0, 0, 0));
			rotate_point(ex+a2, ey, ex, ey, thetaE, &wx, &wy);
			//printf("%f\n", distance(wx, wy, 0, ex, ey, 0));
			printf("0 %.2f\n%.2f %.2f\n%.2f %.2f\n", b0, ex, ey, wx, wy);
		}
	}
}

#if 0
	float origin[3] = { 0, 0, 0 };
	float shoulder[3] = { 0, 27, 125 };
	float x, y, z, X, Y, Z, d;
	float stO, stL, stR, phi, tL, tR, mtO, mtL, mtR;

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
				ik(x, y, z, &mtO, &mtL, &mtR);
				printf("km_test.2\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\n", radians2degrees(mtL), radians2degrees(mtR), radians2degrees(mtO), x, y, z);
			}
		}
	}
#endif
