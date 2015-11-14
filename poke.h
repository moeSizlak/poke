#ifndef _POKE_H
#define _POKE_H
#include <stdint.h>
#include <string.h>

// Individual card masks:
#define CARD_A (1)
#define CARD_K (2)
#define CARD_Q (4)
#define CARD_J (8)
#define CARD_T (16)
#define CARD_9 (32)
#define CARD_8 (64)
#define CARD_7 (128)
#define CARD_6 (256)
#define CARD_5 (512)
#define CARD_4 (1024)
#define CARD_3 (2048)
#define CARD_2 (4096)

// Powers of 14:
#define FT0 (1)       // 14^0
#define FT1 (14)      // 14^1
#define FT2 (196)     // 14^2
#define FT3 (2744)    // 14^3
#define FT4 (38416)   // 14^4
#define FT5 (537824)  // 14^5

// 5 card hand rankings:
#define HIGH_CARD(a,b,c,d,e)   (0 + (((a)*FT4)+((b)*FT3)+((c)*FT2)+((d)*FT1)+(e)))
#define ONE_PAIR(a,b,c,d)      (537824 + (((a)*FT3)+((b)*FT2)+((c)*FT1)+(d)))
#define TWO_PAIR(a,b,c)        (576240 + (((a)*FT2)+((b)*FT1)+(c)))
#define THREE_OF_A_KIND(a,b,c) (578984 + (((a)*FT2)+((b)*FT1)+(c)))
#define STRAIGHT(a)            (581728 + (a))
#define FLUSH(a,b,c,d,e)       (581742 + (((a)*FT4)+((b)*FT3)+((c)*FT2)+((d)*FT1)+(e)))
#define FULL_HOUSE(a,b)        (1119566 + (((a)*FT1)+(b)))
#define FOUR_OF_A_KIND(a,b)    (1119762 + (((a)*FT1)+(b)))
#define STRAIGHT_FLUSH(a)      (1119958 + (a))
#define FIVE_OF_A_KIND(a)      (1119972 + (a))
#define MAX_RANK               (1119985)

// Straight bit masks (5-high through Ace-high)
const static uint16_t straights[10] = {7681, 7936, 3968, 1984, 992, 496, 248, 124, 62, 31};

//Card Bit masks (Ace through 2)
const static uint16_t card[13] = {1,2,4,8,16,32,64,128,256,512,1024,2048,4096};
const static char* card_text[14] = {"0","2","3","4","5","6","7","8","9","T","J","Q","K","A"};
const static char* suit_text[4] = {"c","d","h","s"};

// Lowest 13 bit bitmask
#define LOW13 8191

uint32_t eval(const uint64_t cards, const unsigned int wilds)
{
    uint16_t suits[4];
    int suit_count[4];
    uint16_t suits_and;
    uint16_t suits_or;
    int card_count;
    int quads[16];
    int trips[16];
    int pairs[16];
    int singles[16];
    int quadsi;
    int tripsi;
    int pairsi;
    int singlesi;
    uint16_t quadsm;
    uint16_t tripsm;
    uint16_t pairsm;  
    uint16_t singlesm;
    uint16_t temp;
    uint16_t temp2;
    int flush_cards[5];
    uint32_t flush_rank;
    uint32_t straight_rank;
    int k;
    int j;
    int i;
    int p;
    int q;
    
    if(wilds >= 5)
    {
	// Five aces:
        return FIVE_OF_A_KIND(13);
    }
////////////////////////DO STUFF//////////////////////////////////////////    
    suits[0] = cards & LOW13;
    suits[1] = (cards >> 13) & LOW13;
    suits[2] = (cards >> 26) & LOW13;
    suits[3] = (cards >> 39) & LOW13; 
    
    suits_and = suits[0] & suits[1] & suits[2] & suits[3];
    suits_or  = suits[0] | suits[1] | suits[2] | suits[3];
    
    card_count = 0;
    for (k=0; k<4; k++)
    {
	temp = suits[k];
	suit_count[k] = 0;
	while (temp)
	{
	    card_count++ ;
	    suit_count[k]++;
	    temp = temp & (temp - 1) ;
	}
    }    
    
    quadsi = 0;
    tripsi = 0;
    pairsi = 0;
    singlesi = 0;
    quadsm = suits_and;
    tripsm = ((suits[1] & suits[2] & suits[3])
	| (suits[0] & suits[2] & suits[3])
	| (suits[0] & suits[1] & suits[3])
	| (suits[0] & suits[1] & suits[2]))
	& (~quadsm);
    pairsm = ((suits[0] & suits[1]) | (suits[0] & suits[2]) | (suits[0] & suits[3])
        | (suits[1] & suits[2]) | (suits[1] & suits[3])
        | (suits[2] & suits[3]))
        & (~quadsm) & (~tripsm);
    singlesm = suits_or & (~quadsm) & (~tripsm) & (~pairsm);
    
    temp = 1;
    for(k = 13; k > 0; k--)
    {
	if(quadsm & temp)
	{
	    quads[quadsi++] = k;
	}
	else if(tripsm & temp)
	{
	    trips[tripsi++] = k;
	}
	else if(pairsm & temp)
	{
	    pairs[pairsi++] = k;
	}
	else if(singlesm & temp)
	{
	    singles[singlesi++] = k;
	}
	temp = temp << 1;
    }
///////////////////CHECK FOR FIVE OF A KIND///////////////////////////////    
    k = 0;
    if(wilds == 4)
    {
	if(singlesi != 0)
	    k = singles[0];
	if(tripsi != 0)
	{
	    if(trips[0] > k)
		k = trips[0];
	}
	if(pairsi != 0)
	{
	    if(pairs[0] > k)
		k = pairs[0];
	}
	if(quadsi != 0)
	{
	    if(quads[0] > k)
		k = quads[0];
	}   
	if(k == 0)
	    return FOUR_OF_A_KIND(13, 0);
    }
    else if(wilds == 3)
    {
	if(pairsi != 0)
	    k = pairs[0];
	if(tripsi != 0)
	{
	    if(trips[0] > k)
		k = trips[0];
	}
	if(quadsi != 0)
	{
	    if(quads[0] > k)
		k = quads[0];
	}   
    }
    else if(wilds == 2)
    {
	if(tripsi != 0)
	    k = trips[0];
	if(quadsi != 0)
	{
	    if(quads[0] > k)
		k = quads[0];
	}
    }
    else if(wilds == 1)
    {
	if(quadsi != 0)
	    k = quads[0];
    }
    if(k > 0)
	return FIVE_OF_A_KIND(k);
///////////////CHECK FOR STRAIGHT FLUSH//////////////////////////////////
    for(i = 13; i >= 4; i--)
    {
	for(j=0; j < 4; j++)
	{
	    temp = suits[j] & straights[i-4];
	    k = 0;
	    while (temp)
	    {
		k++ ;
		temp = temp & (temp - 1) ;
	    }
	    if((k + wilds) >= 5)
	    {
		return STRAIGHT_FLUSH(i);
	    }
	}
    }
    
//////////////////CHECK FOR FOUR OF A KIND////////////////////////////////
    k = 0;
    i = 0;
    if(wilds == 3)
    {
	if(singlesi != 0)
	{
	    k = singles[0];
	    if(singlesi > 1)
		i = singles[1];
	}
	else
	{
	    return THREE_OF_A_KIND(13,0,0);
	}
    }
    else if(wilds == 2)
    {
	if(pairsi != 0)
	{
	    k = pairs[0];
	    if(pairsi > 1)
		i = pairs[1];
	    if(singlesi != 0)
	    {
		if(singles[0] > i)
		    i = singles[0];
	    }
	}
    }
    else if(wilds == 1)
    {
	if(tripsi != 0)
	{
	    k = trips[0];
	    if(tripsi > 1)
		i = trips[1];
	    if(pairsi != 0)
	    {
		if(pairs[0] > i)
		    i = pairs[0];
	    }
	    if(singlesi != 0)
	    {
		if(singles[0] > i)
		    i = singles[0];
	    }
	}
    }
    else if(wilds == 0)
    {
	if(quadsi != 0)
	{
	    k = quads[0];
	    if(quadsi > 1)
		i = quads[1];
	    if(tripsi != 0)
	    {
		if(trips[0] > i)
		    i = trips[0];
	    }
	    if(pairsi != 0)
	    {
		if(pairs[0] > i)
		    i = pairs[0];
	    }
	    if(singlesi != 0)
	    {
		if(singles[0] > i)
		    i = singles[0];
	    }
	}
    }
    if(k > 0)
	return FOUR_OF_A_KIND(k,i);
//////////////////PROCESS 1 or 2 WILDS/////////////////////////////////////
// Check for flush with 1 or 2 wild cards
    flush_rank = 0; //flush rank
    for(i = 0; i < 4; i++) // suits
    {
	if((suit_count[i] + wilds) >= 5)
	{
	    temp = 1; // ace card bitmask
	    k = 0;    // flush_cards index
	    p = wilds;    // number of wilds left to use
	    q = 13;   // ace rank
	    while(k < 5)
	    {
		if(temp & suits[i])
		    flush_cards[k++] = q;
		else if(p > 0)
		{
		    flush_cards[k++] = q;
		    p--;
		}			
		temp = temp << 1;
		q--;
	    }
	    k = FLUSH(flush_cards[0],flush_cards[1],flush_cards[2],flush_cards[3],flush_cards[4]);
	    if(k > flush_rank)
		flush_rank = k;
	}
    }
    
// Check for straight with 1 or 2 wild cards:
    if(flush_rank == 0)
    {
	straight_rank = 0;
	for(i = 13; i >= 4; i--)
	{
	    temp = suits_or & straights[i-4];
	    k = 0;
	    while (temp)
	    {
		k++ ;
		temp = temp & (temp - 1) ;
	    }
	    if(k + wilds >= 5)
	    {
		straight_rank = STRAIGHT(i);
		break;
	    }
	}
    }

if(wilds == 2)
{
    if(flush_rank != 0)
	return flush_rank;
    if(straight_rank != 0)
	return straight_rank;
    if(singlesi == 0)
	return ONE_PAIR(13,0,0,0);
    if(singlesi == 1)
	return THREE_OF_A_KIND(singles[0],0,0);
    if(singlesi == 2)
	return THREE_OF_A_KIND(singles[0],singles[1],0);		
    return THREE_OF_A_KIND(singles[0],singles[1],singles[2]);
}    
//////////////////PROCESS 1 WILD/////////////////////////////////
if(wilds == 1)
{
    if(pairsi >= 2)
	return FULL_HOUSE(pairs[0],pairs[1]);
    if(flush_rank != 0)
	return flush_rank;
    if(straight_rank != 0)
	return straight_rank;
    if(pairsi == 1)
    {
	if(singlesi >= 2)
	    return THREE_OF_A_KIND(pairs[0],singles[0],singles[1]);
	if(singlesi == 1)
	    return THREE_OF_A_KIND(pairs[0],singles[0],0);
	return THREE_OF_A_KIND(pairs[0],0,0);	
    }
    if(singlesi == 0)
	return HIGH_CARD(13,0,0,0,0);
    if(singlesi == 1)
	return ONE_PAIR(singles[0],0,0,0);
    if(singlesi == 2)
	return ONE_PAIR(singles[0],singles[1],0,0);
    if(singlesi == 3)
	return ONE_PAIR(singles[0],singles[1],singles[2],0);		
    return ONE_PAIR(singles[0],singles[1],singles[2],singles[3]);
}
/////////////////DONE CHECKING WILDCARDS/////////////////////////////////
////////////////CHECK FOR FULL BOAT, FLUSH, STRAIGHT, TRIPS//////////////

    if(tripsi != 0)
    {
	k = trips[0];
	i = 0;
	if(pairsi != 0)
	{
	    i = pairs[0];
	}
	if(tripsi > 1)
	{
	    if(trips[1] > i)
		i = trips[1];
	}
	if(i != 0)
	{
	    return FULL_HOUSE(k,i);
	}

	if(flush_rank != 0)
	    return flush_rank;
	if(straight_rank != 0)
	    return straight_rank;
	if(singlesi == 0)
	    return THREE_OF_A_KIND(k,0,0);
	if(singlesi == 1)
	    return THREE_OF_A_KIND(k,singles[0],0);
	return THREE_OF_A_KIND(k,singles[0],singles[1]);
    }
    if(flush_rank != 0)
	return flush_rank;
    if(straight_rank != 0)
	return straight_rank;

/////////////////CHECK FOR TWO PAIR//////////////////////////////////////
    if(pairsi >= 2)
    {
	k = 0;
	if(pairsi >= 3)
	    k = pairs[2];
	if(singlesi != 0)
	{
	    if(singles[0] > k)
		k = singles[0];
	}   
	return TWO_PAIR(pairs[0],pairs[1],k);
    }
/////////////////CHECK FOR ONE PAIR//////////////////////////////////////
    if(pairsi != 0)
    {
	if(singlesi >= 3)
	    return ONE_PAIR(pairs[0],singles[0],singles[1],singles[2]);
	if(singlesi == 2)
	    return ONE_PAIR(pairs[0],singles[0],singles[1],0);
	if(singlesi == 1)
	    return ONE_PAIR(pairs[0],singles[0],0,0);
	return ONE_PAIR(pairs[0],0,0,0);
    }
//////////////////////HIGH CARD/////////////////////////////////////////
    if(singlesi >= 5)
	return HIGH_CARD(singles[0],singles[1],singles[2],singles[3],singles[4]);
    if(singlesi == 4)
	return HIGH_CARD(singles[0],singles[1],singles[2],singles[3],0);
    if(singlesi == 3)
	return HIGH_CARD(singles[0],singles[1],singles[2],0,0);
    if(singlesi == 2)
	return HIGH_CARD(singles[0],singles[1],0,0,0);
    if(singlesi == 1)
	return HIGH_CARD(singles[0],0,0,0,0);
    return HIGH_CARD(0,0,0,0,0);
}


uint32_t eval_seqwild(const uint64_t cards)
{
    uint64_t mydeck;
    uint16_t suits[4];
    uint16_t mysuit;
    int k;
    int j;
    int i;
    int p;
    int q;    
    uint32_t rank;
    uint32_t myrank;
    
    rank = eval(cards, 0);
   
    suits[0] = cards & LOW13;
    suits[1] = (cards >> 13) & LOW13;
    suits[2] = (cards >> 26) & LOW13;
    suits[3] = (cards >> 39) & LOW13;    
    
    for(i = 0; i <=3; i++)
    {
	mydeck = 0;
	for(q = 0; q <=3; q++)
	{
	    if(q != i)
	    {
		mydeck = (mydeck << 13) | suits[q];		
	    }
	}
	mydeck = mydeck << 13;
	
	j = 12;
	while(j >= 1 )
	{
	    mysuit = suits[i];
	    
	    if(suits[i] & card[j])
	    {
		k = 1;
		j--;
		while( j >= 0 && (suits[i] & card[j]) )
		{
		    mysuit = mysuit & (~(card[j] | card[j+1]));
		    k++;
		    j--;		
		}
		if(k >= 2)
		{
		    myrank = eval(mydeck | mysuit, k);
		    if(myrank > rank)
			rank = myrank;		    
		}
	    }	 
	    j--;   
	}
    }
    return rank;
}

void rank_to_text(const uint32_t rank, char* text)
{
    int a;
    int b;
    int c;
    int d;
    int e;
    int i = 0;
    uint32_t temp;
    
    temp = rank;
    
    if(!text)
    {
	return;
    }
    text[0] = 0;
    
    if(rank == 0)
    {
	strcpy(text, "No Cards");
    }    
    else if(rank >= MAX_RANK)
    {
	strcpy(text, "Error: Hand ranking greater than MAX_RANK !");
    }    
    else if(rank < ONE_PAIR(0,0,0,0))
    {
	a = temp / FT4;
	temp = temp - (a*FT4);
	b = temp / FT3;
	temp = temp - (b*FT3);
	c = temp / FT2;
	temp = temp - (c*FT2);
	d = temp / FT1;
	temp = temp - (d*FT1);
	e = temp;
	
	strcpy(text,   "HIGH CARD (");
	strcat(text, card_text[a]);
	strcat(text, ",");
	strcat(text, card_text[b]);
	strcat(text, ",");
	strcat(text, card_text[c]);
	strcat(text, ",");
	strcat(text, card_text[d]);
	strcat(text, ",");
	strcat(text, card_text[e]);
	strcat(text, ")");
	
    }
    else if(rank < TWO_PAIR(0,0,0))
    {
	temp = temp - ONE_PAIR(0,0,0,0);
	a = temp / FT3;
	temp = temp - (a*FT3);
	b = temp / FT2;
	temp = temp - (b*FT2);
	c = temp / FT1;
	temp = temp - (c*FT1);
	d = temp;
	
	strcpy(text, "ONE PAIR (");
	text += strlen("ONE PAIR (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	strcat(text, " ");
	text += 3;
	strcpy(text, "KICKERS (");
	text += strlen("KICKERS (");
	strcat(text, card_text[b]);
	strcat(text, ",");
	strcat(text, card_text[c]);
	strcat(text, ",");
	strcat(text, card_text[d]);
	strcat(text, ")");
		
    }
    else if(rank < THREE_OF_A_KIND(0,0,0))
    {
	temp = temp - TWO_PAIR(0,0,0);
	a = temp / FT2;
	temp = temp - (a*FT2);
	b = temp / FT1;
	temp = temp - (b*FT1);
	c = temp;
	
	strcpy(text, "TWO PAIR (");
	text += strlen("TWO PAIR (");
	strcat(text, card_text[a]);
	strcat(text, ",");
	strcat(text, card_text[b]);
	strcat(text, ")");
	strcat(text, " ");
	text += 5;
	strcpy(text, "KICKER (");
	text += strlen("KICKER (");
	strcat(text, card_text[c]);
	strcat(text, ")");
		
	
    }
    else if(rank < STRAIGHT(0))
    {
	temp = temp - THREE_OF_A_KIND(0,0,0);
	a = temp / FT2;
	temp = temp - (a*FT2);
	b = temp / FT1;
	temp = temp - (b*FT1);
	c = temp;
	
	strcpy(text, "THREE OF A KIND (");
	text += strlen("THREE OF A KIND (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	strcat(text, " ");
	text += 3;
	strcpy(text, "KICKERS (");
	text += strlen("KICKERS (");
	strcat(text, card_text[b]);
	strcat(text, ",");
	strcat(text, card_text[c]);
	strcat(text, ")");
		
	    
	
    }
    else if(rank < FLUSH(0,0,0,0,0))
    {
	temp = temp - STRAIGHT(0);
	a = temp;
	
	strcpy(text, "STRAIGHT (");
	text += strlen("STRAIGHT (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	
	
    }
    else if(rank < FULL_HOUSE(0,0))
    {
	temp = temp - FLUSH(0,0,0,0,0);
	a = temp / FT4;
	temp = temp - (a*FT4);
	b = temp / FT3;
	temp = temp - (b*FT3);
	c = temp / FT2;
	temp = temp - (c*FT2);
	d = temp / FT1;
	temp = temp - (d*FT1);
	e = temp;
	
	strcpy(text, "FLUSH (");
	text += strlen("FLUSH (");
	strcat(text, card_text[a]);
	strcat(text, ",");
	strcat(text, card_text[b]);
	strcat(text, ",");
	strcat(text, card_text[c]);
	strcat(text, ",");
	strcat(text, card_text[d]);
	strcat(text, ",");
	strcat(text, card_text[e]);
	strcat(text, ")");
	
	
    }
    else if(rank < FOUR_OF_A_KIND(0,0))
    {
	temp = temp - FULL_HOUSE(0,0);
	a = temp / FT1;
	temp = temp - (a*FT1);
	b = temp;
	
	strcpy(text, "FULL HOUSE (");
	text += strlen("FULL HOUSE (");
	strcat(text, card_text[a]);
	strcat(text, ",");
	strcat(text, card_text[b]);
	strcat(text, ")");
	
    }
    else if(rank < STRAIGHT_FLUSH(0))
    {
	temp = temp - FOUR_OF_A_KIND(0,0);
	a = temp / FT1;
	temp = temp - (a*FT1);
	b = temp;
	
	strcpy(text, "FOUR OF A KIND (");
	text += strlen("FOUR OF A KIND (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	strcat(text, " ");
	text += 3;
	strcpy(text, "KICKER (");
	text += strlen("KICKER (");
	strcat(text, card_text[b]);
	strcat(text, ")");
	
	
    }
    else if(rank < FIVE_OF_A_KIND(0))
    {
	temp = temp - STRAIGHT_FLUSH(0);
	a = temp;
	
	strcpy(text, "STRAIGHT FLUSH (");
	text += strlen("STRAIGHT FLUSH (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	
	
    }
    else // if(rank < MAX_RANK)
    {
	temp = temp - FIVE_OF_A_KIND(0);
	a = temp;
	
	strcpy(text, "FIVE OF A KIND (");
	text += strlen("FIVE OF A KIND (");
	strcat(text, card_text[a]);
	strcat(text, ")");
	
	
	
    }
    
    return;
}

uint16_t text_to_bit(char c)
{
    switch(c)
    {
	case '2': return card[12];
	case '3': return card[11];
	case '4': return card[10];
	case '5': return card[9];
	case '6': return card[8];
	case '7': return card[7];
	case '8': return card[6];
	case '9': return card[5];
	case 't':
	case 'T': return card[4];
	case 'j':
	case 'J': return card[3];
	case 'q':
	case 'Q': return card[2];
	case 'k':
	case 'K': return card[1];
	case 'a':
	case 'A': return card[0];
	default:  return 0;
    }    
}

void cards_to_text(uint64_t cards, char* text)
{
    uint16_t temp;
    uint16_t temp2;
    uint16_t temp3;
    uint16_t suits[4];
    int first = 1;
    
    if(!text)
	return;
    
    text[0] = 0;
    
    suits[0] = cards & LOW13;
    suits[1] = (cards >> 13) & LOW13;
    suits[2] = (cards >> 26) & LOW13;
    suits[3] = (cards >> 39) & LOW13; 
    
    for (int k=0; k<4; k++)
    {
	temp = card[12]; //card[12] is '2'
	  
	for(int i = 1; i <= 13; i++)
	{
	    if(temp & suits[k])
	    {
		if(first)
		{
		    first = 0;
		}
		else
		{
		    strcat(text," ");
		}
		
		strcat(text,card_text[i]);
		strcat(text,suit_text[k]);
	    }
	    temp = temp >> 1;
	}
    }    
    
}

#endif