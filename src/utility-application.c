#include <stdio.h>
#include <string.h>
#include <sys/errno.h>
#include <stdlib.h>

void application_fail()
{
	fprintf(stderr, "%s\n", strerror(errno));
	exit(EXIT_FAILURE);
}
