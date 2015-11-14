#include <stdio.h>
#include "poke.h"

int main(int argc, char** argv)
{
    uint64_t hand;
    uint64_t hole1;
    uint64_t hole2;
    uint64_t temp;
    uint16_t acard;
    char buf[1024];
    uint32_t rank1;
    uint32_t rank2;
    int p1wins = 0;
    int p2wins = 0;
    int ties = 0;
   
    
    hole1 = hole2 = 0;
    
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
    for(int i = 0; i < 2; i++)
    {
	while(1)
	{
	    buf[0] = 0;
	    fprintf(stdout, "Player 2 Hole Card %d: ", i+1);
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
	    if(hole2 & temp || hole1 & temp)
		continue;
	    hole2 = hole2 | temp;
	    break;
	}
	buf[0] = 0;
    }
    printf("\n");
    // all player2 hole cards set
 /*   
    cards_to_text(hole1,buf);
    printf("Player 1 hole cards:(");
    printf(buf);
    printf(")\n");
    
    cards_to_text(hole2,buf);
    printf("Player 2 hole cards:(");
    printf(buf);
    printf(")\n");
    
    return 1;
    
*/
    
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
*/			
						
			rank1 = eval(a|b|c|d|e|hole1,0);
			rank2 = eval(a|b|c|d|e|hole2,0);
/*			
			rank_to_text(rank1, buf);
			printf("p1-->%s",buf);
			for(int y = 35-strlen(buf);y>0; y--)
			    printf(" ");
			
			rank_to_text(rank2, buf);
			printf("p2-->%s",buf);
			for(int y = 35-strlen(buf);y>0; y--)
			    printf(" ");
*/
			if(rank1 > rank2)
			    p1wins++;
			else if(rank2 > rank1)
			    p2wins++;
			else
			    ties++;
			
//			printf("p1=(%d)%.5f p2(%d)=%.5f t=(%d)%.5f\n",p1wins,(double)p1wins/(double)(p1wins+p2wins+ties),p2wins,(double)p2wins/(double)(p1wins+p2wins+ties),ties,(double)ties/(double)(p1wins+p2wins+ties));
			
		    }		    
		}	
	    }    
	}
    }

    printf("p1=(%d)%.5f p2(%d)=%.5f t=(%d)%.5f\n",p1wins,(double)p1wins/(double)(p1wins+p2wins+ties),p2wins,(double)p2wins/(double)(p1wins+p2wins+ties),ties,(double)ties/(double)(p1wins+p2wins+ties));
    
    
    
}