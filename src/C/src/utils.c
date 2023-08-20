#include <stdio.h>
#include <stdlib.h>

#ifdef WIN32
#include <io.h>
#define F_OK 0
#define access _access
#else
#include <unistd.h>
#endif

#include <openssl/rand.h>

#include "utils.h"

static const int MAX_TRIES = 5;

void message_digest_to_hash(unsigned char* md, char* hash, int digest_length)
{
    for(int x=0; x<digest_length; x++)
        sprintf(&hash[x*2], "%02hhx", md[x]);
}

char* get_random_uint64_hexstr(void)
{
    unsigned char buffer[8];
    int tries = 0;
    int rc = 1;
    do
    {
        rc = RAND_bytes(buffer, sizeof(buffer));
        if(rc != 1)
            tries++;

    } while( rc != 1 && tries < MAX_TRIES);

    if(rc != 1)
    {
        printf("Error: exceeded the max tries to get random bytes\n");
        exit(rc);
    }

    char *hexstr = malloc((sizeof(buffer) * 2)+ 1);
    if(hexstr == NULL)
        return NULL;

    for(int x=0; x<sizeof(buffer); x++)
        sprintf(&hexstr[x*2], "%02hhx", buffer[x]);

    return hexstr;
}

int file_exists(const char* filename)
{
    return (access(filename, F_OK) == 0);
}

int save_to_file(const char* filename, unsigned char* data, size_t bytes_to_write)
{
    FILE* fout = fopen(filename, "wb");
    if(fout == NULL)
    {
        printf("Error opening file \'%s\'\n", filename);
        return 0;
    }

    size_t bytes_written = fwrite(data, sizeof(data[0]), bytes_to_write, fout);
    printf("Wrote %zu bytes out of %zu requested\n", bytes_written,  bytes_to_write);
    fclose(fout);
    return 1;
}

size_t read_in_file(const char* filename, unsigned char** buffer)
{
    FILE* fin = fopen(filename, "rb");
    if(fin == NULL)
    {
        printf("Error opening file \'%s\'\n", filename);
        return -1;
    }

    fseek(fin, 0, SEEK_END);
    size_t file_size = ftell(fin);
    fseek(fin, 0, SEEK_SET);

    *buffer = malloc(file_size + 1);
    if(*buffer == NULL)
        return -1;

    size_t read_bytes = fread(*buffer, sizeof(unsigned char), file_size, fin);
    fclose(fin);
    buffer[read_bytes] = 0;
    return read_bytes;
}