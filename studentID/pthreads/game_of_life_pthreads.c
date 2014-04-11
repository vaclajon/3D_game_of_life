#include <stdio.h>
#include <sys/time.h>

int main(void) {
	double time;
	struct timeval lt, ll;

	/* allocate memory and read input data */

	/* set timer */
	gettimeofday(&lt, NULL);

	/* core part */
	printf("HelloWorld\n");

	/* set timer and print measured time*/
	gettimeofday(&ll, NULL);
	time = (double)(ll.tv_sec - lt.tv_sec) + (double)(ll.tv_usec - lt.tv_usec) / 1000000.0;
	fprintf (stderr, "Time : %.6lf\n", time);

	/* write output file */

	return 0;
}
