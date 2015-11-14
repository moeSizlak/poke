#include <stdio.h>
#include <stdint.h>
#include <pthread.h>
#include "poke.h"

#define NUMTHREADS 4
uint64_t hole1;
unsigned long fp1wins = 0;
unsigned long fp2wins = 0;
unsigned long fties = 0;
pthread_mutex_t lock;
pthread_mutex_t klock;
int k = 0;

void* eval_wrapper(void* a)
{
    int n;
    n = (int)a;
    
    uint32_t rank1;
    uint32_t rank2;
    unsigned long p1wins = 0;
    unsigned long p2wins = 0;
    unsigned long ties = 0;
    unsigned long tp1wins = 0;
    unsigned long tp2wins = 0;
    unsigned long tties = 0;
    uint64_t hole2;
    char buf[1024];
    int k1;

    
    for(uint64_t d1 = (uint64_t)1 << 51; d1; d1 = d1 >> 1)
    {
	if((d1 & hole1))
	    continue;	
	
	for(uint64_t d2 = d1 >> n+1; d2 ; d2 = d2 >> NUMTHREADS)
	{
	    if((d2 & hole1))
		continue;
	    	    
	    hole2 = d1 | d2;
	    
	    for(uint64_t a = (uint64_t)1 << 51; a; a = a >> 1)
	    {
		if((a & hole1) || (a & hole2))
		    continue;	
		
		for(uint64_t b = a >> 1; b ; b = b >> 1)
		{
		    if((b & hole1) || (b & hole2))
			continue;
		    
		    for(uint64_t c = b >> 1; c ; c = c >> 1)
		    {
			if((c & hole1) || (c & hole2))
			    continue;
			
			for(uint64_t d = c >> 1; d ; d = d >> 1)
			{
			    if((d & hole1) || (d & hole2))
				continue;
			    
			    for(uint64_t e = d >> 1; e ; e = e >> 1)
			    {
				if((e & hole1) || (e & hole2))
				    continue;
				
				
				rank1 = eval(a|b|c|d|e|hole1,0);
				rank2 = eval(a|b|c|d|e|hole2,0);						    

				if(rank1 > rank2)
				{
				    p1wins++;
				    tp1wins++;
				}
				else if(rank2 > rank1)
				{
				    p2wins++;
				    tp2wins++;
				}
				else
				{
				    ties++;
				    tties++;
				}
				
/*				
				cards_to_text(hole1,buf);
				printf("p1-->(");
				printf(buf);
				printf(")  ");
				
				cards_to_text(hole2,buf);
				printf("p2-->(");
				printf(buf);
				printf(")  ");
				
				printf("(");
				cards_to_text(a,buf);
				printf(buf);
				printf(" ");
				cards_to_text(b,buf);
				printf(buf);
				printf(" ");
				cards_to_text(c,buf);
				printf(buf);
				printf(" ");
				cards_to_text(d,buf);
				printf(buf);
				printf(" ");
				cards_to_text(e,buf);
				printf(buf);			
				printf(")      ");
				
				rank_to_text(rank1, buf);
				printf("p1-->%s",buf);
				for(int y = 35-strlen(buf);y>0; y--)
				    printf(" ");
				
				rank_to_text(rank2, buf);
				printf("p2-->%s",buf);
				for(int y = 35-strlen(buf);y>0; y--)
				    printf(" ");
					
				printf("p1=(%d)%.5f p2(%d)=%.5f t=(%d)%.5f\n",p1wins,(double)p1wins/(double)(p1wins+p2wins+ties),p2wins,(double)p2wins/(double)(p1wins+p2wins+ties),ties,(double)ties/(double)(p1wins+p2wins+ties));
*/					
			    }		    
			}	
		    }    
		}
	    }
	    
	    pthread_mutex_lock(&klock);
	    k++;
	    k1 = k;
	    pthread_mutex_unlock(&klock);
	    
	    cards_to_text(hole1,buf);
	    printf("{n=%2d k1=%5d} p1-->(",n,k1);
	    printf(buf);
	    printf(")  ");
	    
	    printf("p2-->(");
	    cards_to_text(d1,buf);
	    printf(buf);
	    printf(" ");
	    cards_to_text(d2,buf);
	    printf(buf);
	    printf(")  ");
	    
	    printf("|     p1[ %8.5f ]   p2[ %8.5f ]   tie[ %8.5f ]       (%ld, %ld, %ld)\n",100.0*((double)(p1wins)/(double)(p1wins+p2wins+ties)),100.0*((double)(p2wins)/(double)(p1wins+p2wins+ties)),100.0*((double)(ties)/(double)(p1wins+p2wins+ties)),p1wins,p2wins,ties);
	    
	    p1wins = p2wins = ties = 0;
	}
    }  
    
    pthread_mutex_lock(&lock);
    fp1wins += tp1wins;
    fp2wins += tp2wins;
    fties   += tties;  
    pthread_mutex_unlock(&lock);
    return NULL;
}


int main(int argc, char** argv)
{
    pthread_t thread[NUMTHREADS];
    uint64_t temp;
    uint16_t acard;
    char buf[1024];
    int q;

    hole1 = 0;

    
    for(int i = 0; i < 2; i++)
    {
	while(1)
	{
	    buf[0] = 0;
	    fprintf(stdout, "Player 1 Hole Card %d: ", i+1);
	    gets(buf);
	    if(strlen(buf) != 2)
	    {
		//printf("Len != 2)\n");
		continue;
	    }
	    if((acard = text_to_bit(buf[0])) == 0)
	    {
		//printf("text_to_bit(%c) == 0\n",buf[0]);
		continue;
	    }
	    switch(buf[1])
	    {
		case 'c':
		case 'C': temp = ((uint64_t)acard); break;
		case 'd':
		case 'D': temp = ((uint64_t)acard) << 13; break;
		case 'h':
		case 'H': temp = ((uint64_t)acard) << 26; break;
		case 's':
		case 'S': temp = ((uint64_t)acard) << 39; break;
		default:  continue;		
	    }
	    if(hole1 & temp)
		continue;
	    hole1 = hole1 | temp;
	    break;
	}
	buf[0] = 0;
    }
    // all player1 hole cards set
    printf("\n");
    
    if(pthread_mutex_init(&lock,NULL) != 0)
    {
	printf("ERROR; return code from pthread_mutex_init() is %d\n", q);
	return 0;
	
    }
    if(pthread_mutex_init(&klock,NULL) != 0)
    {
	printf("ERROR; return code from pthread_mutex_init() is %d\n", q);
	return 0;
	
    }
    
    for(int g = 0; g < NUMTHREADS; g++)
    {
	q = pthread_create(&thread[g], NULL, eval_wrapper, (void *)g); 
	if(q)
	{
	    printf("ERROR; return code from pthread_create() is %d\n", q);
	    return 0;
	}
    }
    
    for(int g = 0; g < NUMTHREADS; g++)
    {
	q = pthread_join(thread[g], NULL);
	if (q)
	{
	    printf("ERROR; return code from pthread_join() is %d\n", q);
	    return 0;
	}
    }
    if(pthread_mutex_destroy(&lock) != 0)
    {
	printf("ERROR; return code from pthread_mutex_destroy() is %d\n", q);
	return 0;
	
    }

    if(pthread_mutex_destroy(&klock) != 0)
    {
	printf("ERROR; return code from pthread_mutex_destroy() is %d\n", q);
	return 0;
	
    }
    
    printf("\n\n * ");
    cards_to_text(hole1,buf);
    printf("(");
    printf(buf);
    printf(")    ");
    printf("WIN[ %8.5f ]   LOSE[ %8.5f ]   TIE[ %8.5f ]\n",100.0*((double)(fp1wins)/(double)(fp1wins+fp2wins+fties)),100.0*((double)(fp2wins)/(double)(fp1wins+fp2wins+fties)),100.0*((double)(fties)/(double)(fp1wins+fp2wins+fties)));
}