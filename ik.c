#include <math.h>

#include "error.h"
#include "print.h"

#include "ik.h"
#include "fk.h"

float L1=142.5; //Shoulder to elbow length
float L2=155.7; //Elbow to wrise length
float L3=30.3; //Length from wrist to hand PLUS base centre to shoulder

// Get polar coords from cartesian ones
void cart2polar(float a, float b, float *r, float *theta)
{
	// Determine magnitude of cartesian coords
	*r = sqrt(a*a + b*b);

	// Don't try to calculate zero-magnitude vectors' angles
	if(*r == 0) return;

	float c = a / *r;
	float s = b / *r;

	// Safety!
	if(s > 1) s = 1;
	if(c > 1) c = 1;
	if(s < -1) s = -1;
	if(c < -1) c = -1;

	// Calculate angle in 0..M_PI
	*theta = acos(c);

	// Convert to full range
	if(s < 0) *theta *= -1;
}

// Get angle from a triangle using cosine rule
int cosangle(float opp, float adj1, float adj2, float *theta)
{
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
int solve(float x, float y, float z, float *a0, float *a1, float *a2)
{
	// Solve top-down view
	float r, th0;
	cart2polar(y, x, &r, &th0);

	// Account for the wrist length!
	r -= L3;

	// In arm plane, convert to polar
	float ang_P, R;
	cart2polar(r, z, &R, &ang_P);

	// Solve arm inner angles as required
	float B, C;
	if(!cosangle(L2, L1, R, &B)) return 0;
	if(!cosangle(R, L1, L2, &C)) return 0;

	// Solve for servo angles from horizontal
	*a0 = th0;
	*a1 = ang_P + B;
	*a2 = C + *a1 - M_PI;

	return 1;
}

void ik_test() {
	// Counters
	int valid=0, invalid=0;
	float pvalid;

	// Test!
	float x, y, z;
	float t0, t1, t2;
	for(x=-100; x<=100; ++x)  for(y=0; y<=100; ++y) for(z=-50; z<=100; ++z)
	{
		if(solve(x, y, z, &t0, &t1, &t2))
		{
			// Check!
			float X, Y, Z, d, Xi, Yi, Zi, di, dt0, dt1, dt2;
			unsolve(t0, t1, t2, &X, &Y, &Z);

			if((d = distance(x, y, z, X, Y, Z)) > 0.01) {
				warning("solution for (%.2f, %.2f, %.2f) resulted in (%.2f, %.2f, %.2f) w/ distance = %/.4f\n", x, y, z, X, Y, Z, d);
			}

			// do integer math (convert to degrees, round then back to radians)
			dt0 = t0 * (180./M_PI);
			dt1 = t1 * (180./M_PI);
			dt2 = t2 * (180./M_PI);
			//print("iktest.0\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", t0, t1, t2, dt0, dt1, dt2);
			t0 = (M_PI/180.) * (float)((int)dt0);
			t1 = (M_PI/180.) * (float)((int)dt1);
			t2 = (M_PI/180.) * (float)((int)dt2);
			//print("iktest.0\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f", dt0, dt1, dt2, t0, t1, t2);
			unsolve(t0, t1, t2, &Xi, &Yi, &Zi);
			di = distance(x, y, z, Xi, Yi, Zi);
			print("iktest\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t%.4f\t%.2f\t%.2f\t%.2f\t%.4f", x, y, z, X, Y, Z, d, Xi, Yi, Zi, di);

			++valid;
		}
		else
		{
			++invalid;
		}
	}

	//Test results
	print("valid: %d\n", valid);
	print("invalid: %d\n", invalid);
	print("total: %d\n", valid+invalid);

	pvalid = 100.0 * (float)valid / (float)(valid + invalid);
	print("reachable: %.2f%\n", pvalid);
}
