#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <unistd.h>

#include <termios.h>

#include <signal.h>

#define BUFSIZESTEP 1024
#define LISTSIZESTEP 1024


typedef struct listitem_s {
	unsigned int delt;
	unsigned int loc;
	unsigned int count;
} listitem_t;

char * buffer = 0;
size_t buffersize = 0;
size_t bufferplace = 0;
//initialize list with a no item
listitem_t * list = 0;
size_t listplace = 0;
size_t listsize = 0;


void printem(int dummy){
	int i;
	for(i = 0; i < listplace; i++){
//		printf("list item %i: delt %i, loc %i, count %i\n", i, list[i].delt, list[i].loc, list[i].count);
		unsigned int k, place = list[i].loc, max = list[i].count;
		for(k = 0; k < max; k++)putc(buffer[place+k], stdout);
		if(i < listplace -1)usleep(list[i+1].delt * 1000);
	}
}



int main(const int argc, const char ** argv){


	static struct termios oldt, newt;
	tcgetattr( STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON);
	tcsetattr( STDIN_FILENO, TCSANOW, &newt);
	listplace = 1;
	listsize = LISTSIZESTEP;
	list = malloc(listsize * sizeof(listitem_t));
	list[0].delt = 0;
	list[0].loc = 0;
	list[0].count = 0;

	unsigned int deltprec = 0;
	//grab time of start
//	signal(SIGINT, printem);
//	signal(SIGHUP, printem);

	struct timespec thetime;
	clock_gettime(CLOCK_REALTIME, &thetime);


	long long unsigned int startticks = thetime.tv_sec * 1000 + thetime.tv_nsec/1000000;




	char in;
	for(in = getc(stdin); in != EOF; in = getc(stdin)){
		//grab time of input
		clock_gettime(CLOCK_REALTIME, &thetime);
		long long unsigned int curticks = thetime.tv_sec * 1000 + thetime.tv_nsec/1000000;
		unsigned int deltaticks = curticks - startticks;
		//check if its been a long enough time
		if(deltaticks > deltprec){
			//add to new one
			if(listplace+1 >= listsize){
				listsize = (listplace + LISTSIZESTEP) & ~(LISTSIZESTEP-1);
				list = realloc(list, listsize * sizeof(listitem_t));
			}
			list[listplace].delt = deltaticks;
			list[listplace].loc = bufferplace;
			list[listplace].count = 1;
			listplace++;

			startticks = curticks;
		} else { //otherwise add to current one
			list[listplace-1].count++;
		}


		if(bufferplace+1 >= buffersize){
			buffersize = (bufferplace + BUFSIZESTEP) & ~(BUFSIZESTEP-1);
			buffer = realloc(buffer, buffersize);
		}
		buffer[bufferplace]=in;
		putc(in, stdout);
		bufferplace++;
	}
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt);
	while(1)printem(0);





//	while(1){
//		puts(buffer);
//		sleep(1);
//	}




	return 0;
}
