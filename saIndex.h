/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#ifndef _SA_INDEX_H
#define _SA_INDEX_H

#define SA_INDEX_MAX_TUPLES 512

static char *tupleString[SA_INDEX_MAX_TUPLES];
static int numTuples = 0;

#endif

#ifndef _SA_INDEX_C

extern inline int getIndex(char *, char *, char *, char *);

#endif
