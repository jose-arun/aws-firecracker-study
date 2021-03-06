#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

/*
** Source reference: https://github.com/EthanGYoung/gvisor_analysis/blob/master/experiments/execute/network_throughput/net.c
** Sample Execution Command:
** make net
** ./net 1 5MB
*/

int NUM_TRIALS;
char* DOWNLOAD_FILE_SIZE;

/* TODO: Take out this method in a helper file.*/
struct timespec diff(struct timespec start, struct timespec end)
{
        struct timespec temp;
        if ((end.tv_nsec - start.tv_nsec) < 0)
        {
                temp.tv_sec = end.tv_sec - start.tv_sec - 1;
                temp.tv_nsec = 1000000000 + end.tv_nsec - start.tv_nsec;
        }
        else
        {
                temp.tv_sec = end.tv_sec - start.tv_sec;
                temp.tv_nsec = end.tv_nsec - start.tv_nsec;
        }
        return temp;
}

int main(int argc, char *argv[]) {
	/* Parse command line args */
	if (argc != 3) {
          printf("ERROR: Usage: ./net <number of trials> <download file size>\n");
          return 0;
  }

  NUM_TRIALS = atoi(argv[1]);
  DOWNLOAD_FILE_SIZE = argv[2];

  printf("Starting Network Throughput Test for %d calls on %s file\n", NUM_TRIALS, DOWNLOAD_FILE_SIZE);

  struct timespec ts0;
  clock_gettime(CLOCK_REALTIME, &ts0);

  for (int i = 0; i < NUM_TRIALS; i++) {
          char str[1000];
          strcpy(str, "curl -so /dev/null \"http://ipv4.download.thinkbroadband.com/");
          strcat(str, DOWNLOAD_FILE_SIZE);
          strcat(str, ".zip\"");
          system(str);
  }

  struct timespec ts1;
  clock_gettime(CLOCK_REALTIME, &ts1);
  struct timespec t = diff(ts0, ts1);

  float elapsed_time = t.tv_sec + t.tv_nsec/(float)1000000000;
  int string_len = strlen(DOWNLOAD_FILE_SIZE);
  char download_file_size_str [100];
  strncpy (download_file_size_str,  DOWNLOAD_FILE_SIZE, string_len-2);
  download_file_size_str[string_len-1] = '\0';
  int download_file_size_in_Mb = (atoi(download_file_size_str))*8;

  printf("LOG_OUTPUT: Average for %d calls:\n", NUM_TRIALS);
  printf("LOG_OUTPUT: Network download time average = %.12f seconds\n", elapsed_time/NUM_TRIALS);
  if (DOWNLOAD_FILE_SIZE[1] == 'G') {
    download_file_size_in_Mb = download_file_size_in_Mb*1000;
  }
  printf("LOG_OUTPUT: Network throughput = %.12f Mbps\n", (download_file_size_in_Mb/elapsed_time) * NUM_TRIALS);

  return 0;
}
