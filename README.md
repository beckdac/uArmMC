uArmMC
======

LiteArm / uArm master controller software for host system


======
Some notes for solving kinematics


( (L1 * cos(t1) + (L2 * cos(t1+t2))^2 + ( (L1 * sin(t1) + (L2 * sin(t1+t2))^2

solve L1^2+2 L1 L2 cos(t2)+L2^2 for t2

x = (sin(p) * (a1 * cos(t1)) + (a2 * cos(t1) - (t2)))^2;
y = (cos(p) * (a1 * cos(t1)) + (a2 * cos(t1) - (t2)))^2;
z = (a2 * sin(t1)) + (a2 * sin(t1) - (t2))^2;

wolframafied
x = (sin(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))
y = (cos(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))
z = (a2 sin(t1) + (a2 sin(t1 - t2)))^2

simplify (sin(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))^2 + (cos(p) (a1 cos(t1)) + (a2 cos(t1 - t2)))^2 + (a2 sin(t1) + (a2 sin(t1 - t2)))^2

result:
a1^2 cos^2(p) cos^2(t1)+a1^2 sin^2(p) cos^2(t1)+2 a1 a2 cos(p) cos(t1) cos(t1-t2)+2 a1 a2 sin(p) cos(t1) cos(t1-t2)+a2^2 sin^2(t1-t2)+2 a2^2 sin(t1) sin(t1-t2)+2 a2^2 cos^2(t1-t2)+a2^2 sin^2(t1)

solve a1^2 cos^2(p) cos^2(t1)+a1^2 sin^2(p) cos^2(t1)+2 a1 a2 cos(p) cos(t1) cos(t1-t2)+2 a1 a2 sin(p) cos(t1) cos(t1-t2)+a2^2 sin^2(t1-t2)+2 a2^2 sin(t1) sin(t1-t2)+2 a2^2 cos^2(t1-t2)+a2^2 sin^2(t1) for p

moved to sage math

