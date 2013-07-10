#include <stdio.h>
#include <stdlib.h>
#include <libmemcached/memcached.h>

bool set(memcached_st *memc, unsigned int key, unsigned int topic, unsigned int page)
{
	memcached_return rc;
	char keybuffer[4];
	char valbuffer[8];
	
	*((unsigned int*)keybuffer) = key;
	*((unsigned int*)valbuffer) = topic;
	*((unsigned int*)(valbuffer+4)) = page;

	rc = memcached_set(memc, keybuffer, 4, valbuffer, 8, (time_t)0, (uint32_t)0);
	
	if (rc != MEMCACHED_SUCCESS) {
		printf("Couldn't store key: %s\n", memcached_strerror(memc, rc));
	}
}
	
int main(int argc, char* argv[])
{
	memcached_st *memc;
	const char *config = "--SERVER=localhost --BINARY-PROTOCOL"; 
	unsigned int i;
	
	memc = memcached(config, strlen(config));
	
	//*((unsigned int*)buffer) = i;
	//unsigned int j = *((unsigned int*)buffer);

	
	printf("Connection opened.\n");

	for (i = 0; i < 2000000; i++)
	{
		unsigned int topic = rand() % 60000;
		unsigned int page = rand() % 10;
		set(memc, i, topic, page);	

		if (i % 10000 == 0) {
			printf("%u keys set\n", i);
		}
	}
	
	memcached_free(memc);
	return 0;
}	
