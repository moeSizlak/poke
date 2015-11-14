#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define NUMDECKS 8
#define MAXSPLITS 3
#define MAXACESPLITS 3
#define MAXTENSPLITS 3
#define ALLOWSPLITANYTENS 1
#define ONECARDAFTERACESPLIT 1
#define ALLOWDOUBLEAFTERSPLIT 1
#define ALLOWDOUBLEAFTERACESPLIT 0
#define BJAFTERACESPLIT 0
#define BJAFTERTENSPLIT 0
#define NATURALAFTERACESPLIT 0
#define NATURALAFTERTENSPLIT 0
#define ALLOWDOUBLE 1
#define ALLOWSURRENDER 0
#define H17 0
#define DEBUG 0

#define STAND 0
#define HIT 1
#define SPLIT 2
#define DOUBLE 3
#define SURRENDER 4

#define PERM_HIT 1
#define PERM_SPLIT 2
#define PERM_DOUBLE 4
#define PERM_SURRENDER 8

void shuffle(int *array, size_t n)
{
    int i;
    for(i = 0; i < n; i++)
    {
        array[i] = i % 52;      
    }
    
    if (n > 1)
    {
        size_t i;
	for (i = 0; i < n - 1; i++)
	{
	    size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
	    int t = array[j];
	    array[j] = array[i];
	    array[i] = t;
	}
    }
}

int bj(unsigned int* cards)
{
    int c1, c2;
    
    c1 = cards[0] % 13;
    c2 = cards[1] % 13;
    
    if((c1 == 12 && c2 > 7 && c2 < 12) || (c2 == 12 && c1 > 7 && c1 < 12))
        return 1;
    else
        return 0;  
}

unsigned int softcount(unsigned int* cards, unsigned int n)
{
    unsigned int score = 0;
    int i,j;
    
    for(i=0; i<n; i++)
    {
        j = cards[i] % 13;
        if(j <= 7)
	    score += (j + 2);
	else if(j <= 11)
	    score += 10;
	else 
	    score += 1;      
    }
  
    return score;  
}

unsigned int hardcount(unsigned int* cards, unsigned int n)
{
    unsigned int score = 0;
    int i,j;
    int ace = 0;
    
    for(i=0; i<n; i++)
    {
        j = cards[i] % 13;
        if(j <= 7)
	{
	    score += (j + 2);
	}
	else if(j <= 11)
	{
	    score += 10;
	}
	else
	{
	    if(!ace)
	    {
	        score += 11;
	        ace = 1;
	    }
	    else
	    {
	        score += 1;
	    }
	}
    }
    
    if(score > 21 && ace)
    {
        score -= 10; 
    }
  
    return score;  
}

int dealerdecide(unsigned int* cards, unsigned int n)
{
    int hard = hardcount(cards, n);
    int soft = softcount(cards, n);
    
    if(hard <= 16)
        return HIT;
    
    if(H17 && hard == 17 && soft == 7)
        return HIT;
    
    return STAND;  
}

int playerdecide(unsigned int dcard, unsigned int* cards, unsigned int n, int perm)
{
    const static unsigned int pair_table[13][13] = 
    {
/*2*/ {2,2,2,2,2,2,1,1,1,1,1,1,1},
/*3*/ {2,2,2,2,2,2,1,1,1,1,1,1,1},
/*4*/ {1,1,1,2,2,1,1,1,1,1,1,1,1},
/*5*/ {5,5,5,5,5,5,5,5,1,1,1,1,1},
/*6*/ {2,2,2,2,2,1,1,1,1,1,1,1,1},
/*7*/ {2,2,2,2,2,2,1,1,1,1,1,1,1},
/*8*/ {2,2,2,2,2,2,2,2,2,2,2,2,2},
/*9*/ {2,2,2,2,2,0,2,2,0,0,0,0,0},
/*T*/ {0,0,0,0,0,0,0,0,0,0,0,0,0}, 
/*J*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*Q*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*K*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*A*/ {2,2,2,2,2,2,2,2,2,2,2,2,2}
    };
    
    const static unsigned int soft_table[8][13] = 
    {
/*2*/ {1,1,1,5,5,1,1,1,1,1,1,1,1},
/*3*/ {1,1,1,5,5,1,1,1,1,1,1,1,1},
/*4*/ {1,1,5,5,5,1,1,1,1,1,1,1,1},
/*5*/ {1,1,5,5,5,1,1,1,1,1,1,1,1},
/*6*/ {1,5,5,5,5,1,1,1,1,1,1,1,1},
/*7*/ {0,6,6,6,6,0,0,1,1,1,1,1,1},
/*8*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*9*/ {0,0,0,0,0,0,0,0,0,0,0,0,0}
    };
    
    const static unsigned int hard_table[17][13] =
    {
/*4*/  {1,1,1,1,1,1,1,1,1,1,1,1,1},
/*5*/  {1,1,1,1,1,1,1,1,1,1,1,1,1},
/*6*/  {1,1,1,1,1,1,1,1,1,1,1,1,1},
/*7*/  {1,1,1,1,1,1,1,1,1,1,1,1,1},
/*8*/  {1,1,1,1,1,1,1,1,1,1,1,1,1},
/*9*/  {1,5,5,5,5,1,1,1,1,1,1,1,1},
/*10*/ {5,5,5,5,5,5,5,5,1,1,1,1,1},
/*11*/ {5,5,5,5,5,5,5,5,5,5,5,5,1},
/*12*/ {1,1,0,0,0,1,1,1,1,1,1,1,1},
/*13*/ {0,0,0,0,0,1,1,1,1,1,1,1,1},
/*14*/ {0,0,0,0,0,1,1,1,1,1,1,1,1},
/*15*/ {0,0,0,0,0,1,1,1,7,7,7,7,1},
/*16*/ {0,0,0,0,0,1,1,7,8,8,8,8,7},
/*17*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*18*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*19*/ {0,0,0,0,0,0,0,0,0,0,0,0,0},
/*20*/ {0,0,0,0,0,0,0,0,0,0,0,0,0}
    };
// 0 == STAND
// 1 == HIT
// 2 == SPLIT
// 3 == DOUBLE
// 4 == SURRENEDR
//
// 5 == DOUBLE/HIT
// 6 == DOUBLE/STAND
// 7 == SURRENDER/HIT
// 8 == SURRENDER/HIT except STAND if more than 2 cards    
    
    unsigned int hard = hardcount(cards, n);
    unsigned int soft = softcount(cards, n);
    unsigned int issoft = ((hard == soft) ? 0 : 1);
    int i,j,k;
    int ret;
    
    if(hard >= 21)
    {
        ret=STAND;
    }
    else if(perm & PERM_SPLIT)
    {
        ret = pair_table[cards[0]%13][dcard%13];
    }  
    else if(issoft)
    {
        i = hard - 11 - 2;
	if(i < 0)
	{
	    ret = HIT;
	}
	else
	{
	    ret = soft_table[i][dcard%13];
	}
    }
    else
    {
	i = hard - 4;
	ret = hard_table[i][dcard%13];
    }
 
    assert( ret>=0 && ret<=8 && ret!=3 && ret!=4 );
    assert( !(ret==2 && !(perm & PERM_SPLIT)) );


    if(ret == 0)
    {
    }
    else if(ret == 1 && !(perm & PERM_HIT))
    {
        ret = STAND;
    }
    else if(ret == 5)
    {
        if(perm & PERM_DOUBLE)
        {
            ret = DOUBLE;
        }
        else
        {
            if(perm & PERM_HIT)
            {
                ret = HIT;
            }
            else
            {
                ret = STAND;
            }
        }
    }
    else if(ret == 6)
    {
        if(perm & PERM_DOUBLE)
        {
            ret = DOUBLE;
        }
        else
        {
            ret = STAND;
        }
    }
    else if(ret == 7 || ret == 8)
    {
        if(!(perm & PERM_SURRENDER))
        {
            if(perm & PERM_HIT)
            {
                if(ret == 7 || n == 2)
                {
                    ret = HIT;
                }
                else
                {
                    ret = STAND;
                }                
            }
            else
            {
                ret = STAND;
            }
        }
        else
        {
            ret = SURRENDER;
        }
    }


    return ret;
}

int main(int argc, char** argv)
{
    int i,j,k;
    int numcards = 52*NUMDECKS;
    unsigned int stack[52*NUMDECKS];
    uint64_t p1wins = 0;
    uint64_t p1losses = 0;
    uint64_t p1pushes = 0;
    unsigned int cc = 0;    
    unsigned int p1cards[21];
    unsigned int dcards[21];
    unsigned int p1cc = 0;
    unsigned int dcc = 0;
    unsigned int p1score = 0;
    unsigned int dscore = 0;
    int first = 1;
    uint64_t shoes = 0;
    int decision = -1;
    int splitlevel = 0;
    int numsplits = 0;
    int doubled = 0;
    int acesplit = 0;
    int tensplit = 0;    
    int perms = 0;

    srand((unsigned int) time(NULL));
    shuffle(stack, numcards);

    while(1)
    {
        if(DEBUG) // START DEBUG ///////////////////////////////////////////////
	{
	    if(first == 1)
	    {
		first = 0;
	    }
	    else
	    {
		printf("Dealer: ");
		for(i = 0; i < dcc; i++)
		{
		    j = dcards[i] % 13;
		    if(j <= 7)
		        printf("%d,", j + 2);
		    else if (j < 12)
			printf("10,");
		    else
			printf("A,");		  
		}
		printf(" (%d)\t\t", hardcount(dcards, dcc));		  
		printf("Player 1: ");
		for(i = 0; i < p1cc; i++)
		{
		    j = p1cards[i] % 13;
		    if(j <= 7)
			printf("%d,", j + 2);
		    else if (j < 12)
			printf("10,");
		    else
			printf("A,");
		}
		printf(" (%d)", hardcount(p1cards, p1cc));
		if(numsplits)
		    printf("(S)");
		if(doubled)
		    printf("(D)");
		printf("\t");
		printf("(%llu,%llu,%llu)\n",p1wins,p1losses,p1pushes);
	
	    }
	}// END DEBUG/////////////////////////////////////////////////////////////////

        doubled = 0;
        if(splitlevel)
        {
            splitlevel--;
            p1cc = 1;
            p1cards[p1cc++] = stack[cc++];
            goto splitpoint;

        }      
      
        p1cc = 0;
	dcc = 0;
	numsplits = 0;
        acesplit = 0;
        tensplit = 0;	

        if(cc >= numcards - 20)
	{
	    shoes++;
	    if(shoes == 50000000)
	        break;
	    
	    shuffle(stack, numcards);
	    cc = 0;
	}
	
        p1cards[p1cc++] = stack[cc++];
	dcards[dcc++] = stack[cc++];
	p1cards[p1cc++] = stack[cc++];
	dcards[dcc++] = stack[cc++];
	
	if(bj(dcards))
	{
	    if(bj(p1cards))
	        p1pushes++;
	    else
	        p1losses+=2;	    
	    continue;
	}

splitpoint:

        if(bj(p1cards))
	{
	    if(numsplits == 0)
	    {
	        p1wins+=3;
	    }
	    else if((acesplit && BJAFTERACESPLIT) || (tensplit && BJAFTERTENSPLIT))
	    {
		p1wins+=3;
	    }
	    else if((acesplit && NATURALAFTERACESPLIT) || (tensplit && NATURALAFTERTENSPLIT))
	    {
	        p1wins+=2;
	    }
	    else
	    {
	        goto p1done;
	    }
	    continue;	  
	}
	
//FIRST DECISION

        perms = 0;

        if(ALLOWDOUBLE)
        {
            if(numsplits == 0)
            {
                perms = perms | PERM_DOUBLE;
            }
            else if(ALLOWDOUBLEAFTERSPLIT)
            {
                if(!acesplit || ALLOWDOUBLEAFTERACESPLIT)
                {
                    perms = perms | PERM_DOUBLE;
                }
            }
        }              

        i = p1cards[0]%13;
        j = p1cards[1]%13;
        if(numsplits < MAXSPLITS)
        {
            if(i==j || (ALLOWSPLITANYTENS && (i>7 && i<12 && j>7 && j<12)))
            {
                if(i==12)
		{
		    if(numsplits < MAXACESPLITS)
		    {
			perms = perms | PERM_SPLIT;
		    }
		}
                else if(i > 7)
		{
		    if(numsplits < MAXTENSPLITS)
		    {
			perms = perms | PERM_SPLIT;
		    }
		}
		else
		{
		    perms = perms | PERM_SPLIT;  
		}
            }
        }
        
        perms = perms | (ALLOWSURRENDER ? PERM_SURRENDER : 0); 

        perms = perms | PERM_HIT;
        if(acesplit && ONECARDAFTERACESPLIT)
	{
            perms = perms & ~(PERM_HIT | PERM_DOUBLE | PERM_SURRENDER);
	}        
 
        assert( !( !acesplit && !(perms & PERM_HIT)) );
	assert(!(p1cards[0]%13 == 12 && p1cards[1]%13 == 12 && numsplits<3 && !(perms & PERM_SPLIT)));
	decision = playerdecide(dcards[0],p1cards,p1cc,perms); 
        assert(   decision >= 0 && decision <= 4 );
        assert( !(decision == HIT && !(perms & PERM_HIT)) );
        assert( !(decision == SPLIT && !(perms & PERM_SPLIT)) );
        assert( !(decision == DOUBLE && !(perms & PERM_DOUBLE)) );
        assert( !(decision == SURRENDER && !(perms & PERM_SURRENDER)) );
        assert( !(decision == HIT && hardcount(p1cards, p1cc) >= 21) );	


        if(decision == SPLIT)
	{
	    numsplits++;
	    splitlevel++;
            if(i > 7)
                if(i < 12)
                    tensplit = 1;
                else
                    acesplit = 1;
	    
	    p1cc = 1;
	    p1cards[p1cc++] = stack[cc++];	    
	    goto splitpoint;	  
	}
	else if(decision == DOUBLE)
	{
            doubled=1;
	    p1cards[p1cc++] = stack[cc++];
	    goto p1done;
	}
	
	while(decision == HIT)
	{
	    p1cards[p1cc++] = stack[cc++];
            if(hardcount(p1cards,p1cc) >= 21)
                goto p1done;
            decision = playerdecide(dcards[0], p1cards, p1cc,PERM_HIT);
            assert(   decision == 0 || decision == 1 );
            assert( !(decision == HIT && hardcount(p1cards, p1cc) >= 21) );

	}
	
p1done:
	p1score = hardcount(p1cards,p1cc);
	if(p1score > 21)
	{
	    p1losses+=2;
            if(doubled)
                p1losses+=2;
	    continue;	  
	}
	
	while(dealerdecide(dcards,dcc))
	{
	    dcards[dcc++] = stack[cc++];
	}
	
	dscore = hardcount(dcards,dcc);
	if(dscore > 21)
	{
	    p1wins+=2;
            if(doubled)
                p1wins+=2;
	    continue;	  
	}
	
	if(p1score > dscore)
	{
	    p1wins+=2;
            if(doubled)
                p1wins+=2;
	    continue;	  
	}
	else if(dscore > p1score)
	{
	    p1losses+=2;
            if(doubled)
                p1losses+=2;
	    continue;
	}
	else
	{
	    p1pushes++;
	    continue;
	}   
      
    }
    
    printf("Player 1 wins:   %llu (%4.8f\%)\n", p1wins, (double)(100*p1wins)/(p1wins+p1losses)); 
    printf("Player 1 losses: %llu (%4.8f\%)\n", p1losses, (double)(100*p1losses)/(p1wins+p1losses)); 
//  printf("Player 1 pushes: %llu (%4.2f\%)\n", p1pushes, (double)(100*p1pushes)/(p1wins+p1losses+p1pushes)); 
    
    return 0;
}
