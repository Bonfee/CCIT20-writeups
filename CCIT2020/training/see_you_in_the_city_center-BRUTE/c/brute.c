#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "base64.h"
#include "b64.h"

int counter = 0;

void timeout(int sig)
{
	char buf[100];
	sprintf(buf, "%d key/s", counter / 10);
	puts(buf);
	counter = 0;
	alarm(10);
}

char * encrypt(char* clear, int clear_len, char* key, int key_len)
{
	char * enc, * ret;
	int i;
	enc = (char *) malloc(clear_len);
	for (i = 0; i < clear_len; i++){
		enc[i] = (clear[i] + key[i % key_len]) % 128;
	}
	return enc;
}

void print_bin(char * s, int l)
{
	int i;
	for (i = 0; i < l; i++)
		putchar(s[i]);
	putchar('\n');
}

int main(int argc, char ** argv)
{
	char * msg = "See you later in the city center"; //lenght: 32
	int msg_len = strlen(msg);
	char * enc = "QSldSTQ7HkpIJj9cQBY3VUhbQ01HXD9VRBVYSkE6UWRQS0NHRVE3VUQrTDE=";
	int enc_len = strlen(enc);
	char * enc_bin;
	size_t enc_bin_len;
	char *stp1, *stp2, *stp3, *stp4;
	char * key_init = "aaaaaaaa"; // lenght: 8
	char * key;
	int i = 0, j;
	size_t stp1_len, stp2_len, stp3_len, stp4_len;
	key = malloc(9);
	strcpy(key, key_init);
	signal(SIGALRM, timeout);
	if (argc == 2){
		key[7] = argv[1][0];
	}
	enc_bin = b64_decode_ex(enc, enc_len, &enc_bin_len);
	/* debugging
	char * enc_again;
	size_t enc_again_len;
	enc_again = base64_encode(enc_bin, enc_bin_len, &enc_again_len);
	print_bin(enc_again, enc_again_len);
	*/
	alarm(10);
	while(1){
		counter++;
		stp1 = encrypt(msg, msg_len, key, 4);
		stp2 = base64_encode(stp1, msg_len, &stp2_len);
		free(stp1);
		stp3 = encrypt(stp2, stp2_len, key + 4, 4);
		free(stp2);
		/* debug 
		stp4 = base64_encode(stp3, stp2_len, &stp4_len);
		printf("%s - %s\n", key, stp4);
		*/
		if (!memcmp(stp3, enc_bin,  enc_bin_len)){
			stp4 = base64_encode(stp3, stp2_len, &stp4_len);
			printf("found\n%s - %s\n", key, stp4);
			exit(0);
		}
		free(stp3);
		for (i = 0; i < 7; i++){
			if (key[i] < 'z'){
				for (j = i - 1; j >= 0; j--){
					key[j] = 'a';
				}
				key[i]++;
				break;
			}
		}
		if (i == 7){
			exit(1);
		}
	}
}
