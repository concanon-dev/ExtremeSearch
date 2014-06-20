/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef __sahedges
#define __sahedges

#define SA_HEDGE_ERROR -1
#define SA_HEDGE__NOISE 0
#define SA_HEDGE_ABOUT 1
#define SA_HEDGE_ABOVE 2
#define SA_HEDGE_AFTER 3
#define SA_HEDGE_BEFORE 4
#define SA_HEDGE_BELOW 5
#define SA_HEDGE_CLOSE 6
#define SA_HEDGE_EXTREMELY 7
#define SA_HEDGE_GENERALLY 8
#define SA_HEDGE_NOT 9
#define SA_HEDGE_POSITIVE 10
#define SA_HEDGE_SLIGHTLY 11
#define SA_HEDGE_SOMEWHAT 12
#define SA_HEDGE_VERY 13
#define SA_HEDGE_VICINITY 14

#define SA_HEDGE_MAXWORDS 10000
#define SA_HEDGE_MAXWORDSIZE 62

typedef struct saSynonymTableStruct
{
    char charbuf[SA_HEDGE_MAXWORDS*(SA_HEDGE_MAXWORDSIZE+2)];
    char *hedge[SA_HEDGE_MAXWORDS];
    char *word[SA_HEDGE_MAXWORDS];
    int numWords;
} saSynonymTableType;
typedef saSynonymTableType *saSynonymTableTypePtr;

#endif
