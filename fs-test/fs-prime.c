#include <stdio.h>
#include <stdlib.h>
	
int main(int argc, char* argv[])
{
	unsigned int writebuffer[3];
	unsigned int i;
	FILE* fp = fopen("post_map.bin", "w");
	printf("File opened.\n");

	for (i = 0; i < 2000000; i++)
	{
		writebuffer[0] = i;
		writebuffer[1] = rand() % 60000;
		writebuffer[2] = rand() % 10;
		
		fwrite(writebuffer, sizeof(i), 3, fp);

		if (i % 10000 == 0) {
			printf("%u keys written\n", i);
		}
	}

	fclose(fp);
	
	return 0;
}	
