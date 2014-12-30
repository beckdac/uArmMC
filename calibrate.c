#include "error.h"
#include "print.h"

#include "cmd.h"

typedef struct calib {
	float x, y, z;
	int r, l, rot;
} calib_t;

calib_t calib[9];

void calibrate(void) {
	float x, y, z;
	int i = 0;
	char line[1024];
	char last_line[1024];

	for (x = 0; x < 3; ++x)
		for (y = 0; y < 3; ++y)
			for (z = 0; z < 1; ++z) {
				print("calibrate %.2f\t%.2f\t%.2f\n", x, y, z);
				while (fgets(line, 1024, stdin) != NULL) {
					if (line[0] == 'O' && line[1] == 'K') {
						calib[i].x = x;
						calib[i].y = y;
						calib[i].z = z;
						printf("%s", last_line);
						++i;
						break;
					}
					cmd_execute(line, strlen(line));
					strcpy(last_line, line);
				}
				if (line[0] == '\0') {
					x = 3;
					y = 3;
					break;
				}
			}
}
