#include <stdio.h>
#include <stdlib.h>
#include <libmemcached/memcached.h>

void read(memcached_st *memc, unsigned int key)
{ 
	char keybuffer[4];
	uint32_t flags;
	size_t len;
	memcached_return_t err;

	(*(unsigned int*)keybuffer) = key;

	const char* valbuffer = memcached_get(memc, keybuffer, 4, &len, &flags, &err);
	
	unsigned int topic = 0;
	unsigned int page = 0;
	
	if (len == 8) {
		topic = *(unsigned int*)valbuffer;
		page = *(unsigned int*) (valbuffer+4);
	}
	printf("%lu bytes returned for %u. [%u %u]\n", len, key, topic, page);
}
int main(int argc, char* argv[])
{
	unsigned int i;
	memcached_st *memc;
	const char *config = "--SERVER=localhost --BINARY-PROTOCOL"; 
	memc = memcached(config, strlen(config));
	printf("Connection opened.\n");

	for (i = 0; i < 1000; i++)
	{
		read(memc, rand() % 2000000);	
	}
	
	memcached_free(memc);
	return 0;
}	
