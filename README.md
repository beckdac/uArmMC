uArmMC
======

LiteArm / uArm master controller software for host system


====== some measurements
S	E	angle between bed and elbow to wrist vector
R90	L50	-22
R40	L50	-32
R90	L115 -90
R40 L115 -85

At y = 140 mm when z = 0, actual z = 12.6
at y = 220 mm when z = 0, actual z = 17.3

======
Some notes for solving kinematics


( (L1 * cos(t1) + (L2 * cos(t1+t2))^2 + ( (L1 * sin(t1) + (L2 * sin(t1+t2))^2

solve L1^2+2 L1 L2 cos(t2)+L2^2 for t2

x = (sin(p) * (a1 * cos(t1)) + (a2 * cos(t1) - (t2)))
y = (cos(p) * (a1 * cos(t1)) + (a2 * cos(t1) - (t2)))
z = (a2 * sin(t1)) + (a2 * sin(t1) - (t2))

wolframafied
x = (sin(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))
y = (cos(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))
z = (a2 sin(t1) + (a2 sin(t1 - t2)))^2

simplify (sin(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))^2 + (cos(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))^2 + (a2 sin(t1) + (a2 sin(t1 - t2)))^2

result:
a1^2 cos^2(p) cos^2(t1)+a1^2 sin^2(p) cos^2(t1)+2 a1 a2 cos(p) cos(t1) cos(t1-t2)+2 a1 a2 sin(p) cos(t1) cos(t1-t2)+a2^2 sin^2(t1-t2)+2 a2^2 sin(t1) sin(t1-t2)+2 a2^2 cos^2(t1-t2)+a2^2 sin^2(t1)

solve a1^2 cos^2(p) cos^2(t1)+a1^2 sin^2(p) cos^2(t1)+2 a1 a2 cos(p) cos(t1) cos(t1-t2)+2 a1 a2 sin(p) cos(t1) cos(t1-t2)+a2^2 sin^2(t1-t2)+2 a2^2 sin(t1) sin(t1-t2)+2 a2^2 cos^2(t1-t2)+a2^2 sin^2(t1) for p

moved to sage math

phi, thetaS, thetaE, a1, a2, x, y, z = var('phi thetaS thetaE a1 a2 x y z')
eqx = sin(phi) * (a1 * cos(thetaS)) + (a2 * cos(thetaS - thetaE)) == x
eqy = cos(phi) * (a1 * cos(thetaS)) + (a2 * cos(thetaS - thetaE)) == y
eqz = (a2 * sin(thetaS)) + (a2 * sin(thetaS - thetaE)) == z
solve([eqx,eqy,eqz],phi, thetaS, thetaE)

plyaing with 2d while 3d runs:
thetaS, thetaE, a1, a2, y, z = var('thetaS thetaE a1 a2 y z')
eqy = (a1 * cos(thetaS)) + (a2 * cos(thetaS+thetaE)) == y
eqz = (a1 * sin(thetaS)) + (a2 * sin(thetaS+thetaE)) == z
solve([eqy, eqz], thetaS, thetaE)
