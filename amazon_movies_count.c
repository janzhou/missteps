#include "pcm.h"
#include "amazon_movies.h"
#include "arielapi.h"

int main(int argc, char *argv[])
{
	char *buf;
	struct pcm_thread pcm_threads[PCM_NUM_BANKS];

	pcm_param(argc, argv);

	memset(pcm_threads, 0, sizeof(pcm_threads));

	buf = calloc(1, PCM_SIZE);
	if (!buf) {
		perror("Unable to allocate memory");
		return errno;
	}

	int rows[PCM_NUM_ROWS], r;
	for(r = 0; r < PCM_NUM_ROWS; r++) {
		rows[r] = r;
	}

	pcm_r2t_contention_free(pcm_threads, PCM_NUM_BANKS, rows, PCM_NUM_ROWS, buf);

	pcm_threads_map_count_fn(pcm_threads, PCM_NUM_BANKS, amazon_movies_cnt_local);	

	if (amazon_movies_init_mem(buf, "../movies.txt"))
		return errno;

	ariel_enable();

	pcm_threads_run(pcm_threads, PCM_NUM_BANKS);
	pcm_threads_reduce_count_fn(pcm_threads, PCM_NUM_BANKS, amazon_movies_cnt_global);

	printf("Amazon reviews count: %lu\n", amazon_movies_get_global_cnt());

	free(buf);

	return 0;
}
