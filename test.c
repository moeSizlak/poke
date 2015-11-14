#include <stdio.h>
#include <stdint.h>
#include "poke.h"

int main(int argc, char** argv)
{
    char buf[256];
    uint16_t acard;
    uint64_t board;
    uint64_t temp;
    uint64_t hole;
    uint32_t rank;

    
    board = 0;
    for(int i = 0; i < 5; i++)
    {
	while(1)
	{
	    buf[0] = 0;
	    fprintf(stdout, "Board Card %d: ", i+1);
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
	    if(board & temp)
		continue;
	    board = board | temp;
	    break;
	}
	buf[0] = 0;
    }
    
    while(1) // Loop through hole cards forever
    {
	
	hole = 0;
	for(int i = 0; i < 2; i++)
	{
	    while(1)
	    {
		buf[0] = 0;
		fprintf(stdout, "Hole Card %d: ", i+1);
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
		if(board & temp || hole & temp)
		    continue;
		hole = hole | temp;
		break;
	    }
	    buf[0] = 0;
	}
	// all board and hole cards set
	rank = eval(board | hole, 0);
	rank_to_text(rank, buf);
	printf("Holdem rank: %d - %s\n",rank,buf);
	
	rank = eval_seqwild(board | hole);
	rank_to_text(rank, buf);
	printf("FBA    rank: %d - %s\n\n",rank,buf);
	
	
    }
    
    return 0;
}