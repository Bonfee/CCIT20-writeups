#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <immintrin.h>
#include <sys/wait.h>
#include <sys/mman.h>

#include "b64.h"
#include "fastavxbase64.h"

#define KEY1_TOREG key_reg = _mm256_set_epi8(key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0], key[3], key[2], key[1], key[0])
#define KEY2_TOREG key_reg = _mm256_set_epi8(key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4], key[7], key[6], key[5], key[4])

int counter = 0;

void timeout(int sig)
{
	char buf[100];
	sprintf(buf, "%d key/s", counter / 10);
	puts(buf);
	counter = 0;
	alarm(10);
}

void encrypt_reg(char * ret, __m256i clear, __m256i key, __m256i mask)
{
	__m256i ret_reg;
	ret_reg = _mm256_add_epi8(clear, key);
	ret_reg = _mm256_andnot_si256(mask, ret_reg);
	_mm256_storeu_si256(ret, ret_reg);
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
	__m256i tmp_reg, key_reg, enc_low_reg;
	__m256i mask_reg = _mm256_set1_epi8(0x80);
	char * msg = "See you later in the city center";
	__m256i msg_reg = _mm256_loadu_si256(msg);
	size_t msg_len = 32;
	char * enc = "QSldSTQ7HkpIJj9cQBY3VUhbQ01HXD9VRBVYSkE6UWRQS0NHRVE3VUQrTDE="; //real one
//	char * enc = "LyY7KCI3UTI3SCcvMk03KDZYJyw1WCc3NDY6IzMmUTc4SCcmM0waNzNNLh4="; //aaaaaaaa
	int enc_len = strlen(enc);
	size_t enc_bin_len;
	char * buffer1, * buffer2, * enc_bin;
	char * key_init = "aaaaaaaa"; // lenght: 8
	char * key;
	int i = 0, j, z, pid, ignored;
	signal(SIGALRM, timeout);
	//counter = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	buffer1 = malloc(32);
	buffer2 = malloc(32);
	enc_bin = b64_decode_ex(enc, enc_len, &enc_bin_len);
	enc_low_reg = _mm256_loadu_si256(enc_bin);
	/* debugging
	char * enc_again;
	size_t enc_again_len;
	enc_again = base64_encode(enc_bin, enc_bin_len, &enc_again_len);
	print_bin(enc_again, enc_again_len);
	*/
	for (z = 'a'; z <= 'z'; z++){
		pid = fork();
		if (pid == 0) {
			alarm(10);
			key = malloc(9);
			strcpy(key, key_init);
			key[7] = z;
			while (1){
				counter++;
				KEY1_TOREG;
				/* encrypt stp1 */
				tmp_reg = _mm256_add_epi8(msg_reg, key_reg);
				tmp_reg = _mm256_andnot_si256(mask_reg, tmp_reg);
				_mm256_storeu_si256(buffer1, tmp_reg);
				fast_avx2_base64_encode(buffer2, buffer1, msg_len);
				/* encrypt stp2 low */
				KEY2_TOREG;
				tmp_reg = _mm256_loadu_si256(buffer2);
				tmp_reg = _mm256_add_epi8(tmp_reg, key_reg);
				tmp_reg = _mm256_andnot_si256(mask_reg, tmp_reg);
				/* debug
				stp4 = base64_encode(stp3, stp2_len, &stp4_len);
				printf("%s - %s\n", key, stp4);
				*/
				if (_mm256_testc_si256(tmp_reg, enc_low_reg)) {
					/* encrypt stp2 high */
					tmp_reg = _mm256_loadu_si256(buffer2 + 32);
					tmp_reg = _mm256_add_epi8(tmp_reg, key_reg);
					tmp_reg = _mm256_andnot_si256(mask_reg, tmp_reg);
					_mm256_storeu_si256(buffer1, tmp_reg);
					if(!memcmp(buffer1, enc_bin + 32, enc_bin_len - 32)) {
						printf("found: %s\n", key);
						exit(0);
					}
				}
				for (i = 0; i < 8; i++){
					if (key[i] < 'z'){
						for (j = i - 1; j >= 0; j--)
							key[j] = 'a';
						key[i]++;
						break;
					}
				}
				if (i == 8){
					printf("nothing found\n");
					exit(1);
				}
			}
		}
	}
	for (z = 'a'; z < 'z'; z++)
		wait(&ignored);
}
