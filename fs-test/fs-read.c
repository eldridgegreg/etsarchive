#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>

int main(int argc, char* argv[])
{
	srand(time(NULL));
	FILE* fp;
	struct timeval t1, t2;
	unsigned int i = rand() % 2000000;
	unsigned int readbuffer[3];
	gettimeofday(&t1, NULL);
	fp = fopen("post_map.bin", "rb");
	fseek(fp, i*3*sizeof(i), SEEK_CUR);
	fread(readbuffer, sizeof(i), 3, fp);
	fclose(fp);
	gettimeofday(&t2, NULL);
	
	long msec = (t2.tv_sec-t1.tv_sec)*1000 + (0.5+((t2.tv_usec-t1.tv_usec)/1000.0));
	
	printf("Read complete @ %u - %u %u %u | %ld ms\n", i, readbuffer[0], readbuffer[1], readbuffer[2], msec);

	return 0;
}	
