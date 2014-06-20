/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "saHash.h"

/* Create a new hashtable. */
saHashtableTypePtr saHashCreate(int size) 
{
    saHashtableTypePtr hashtable = NULL;
    int i;

    if (size < 1)
        return(NULL);

    /* Allocate the table itself. */
    if ((hashtable = malloc(sizeof(saHashtableType))) == NULL)
        return(NULL);

    /* Allocate pointers to the head nodes. */
    if ((hashtable->table = malloc(sizeof(saHashentryTypePtr) * size)) == NULL)
    	return(NULL);

    for(i = 0; i < size; i++)
    	hashtable->table[i] = NULL;

    hashtable->size = size;

    return(hashtable);	
}

saHashtableTypePtr saHashCreateDefault()
{
    return(saHashCreate(SA_HASHTABLE_DEFAULTSIZE));
}

/* Hash a string for a particular hash table. */
int saHashHash(saHashtableTypePtr hashtable, char *key) 
{
    unsigned long int hashval = 0;
    int i = 0;

    /* Convert our string to an integer */
    while(hashval < ULONG_MAX && i < strlen(key))
    {
    	hashval = hashval << 8;
    	hashval += key[ i ];
    	i++;
    }
    return(hashval % hashtable->size);
}

/* Create a key-value pair. */
saHashentryTypePtr saHashNewpair(char *key, void *value) 
{
    saHashentryTypePtr newpair;

    if ((newpair = malloc(sizeof(saHashentryType))) == NULL)
    	return(NULL);

    strcpy(newpair->key, key);
    newpair->value = value;
    newpair->next = NULL;

    return(newpair);
}

/* Insert a key-value pair into a hash table. */
void saHashSet(saHashtableTypePtr hashtable, char *key, char *value ) 
{
    int bin = 0;
    saHashentryTypePtr newpair = NULL;
    saHashentryTypePtr next = NULL;
    saHashentryTypePtr last = NULL;

    bin = saHashHash(hashtable, key);

    next = hashtable->table[bin];

    while(next != NULL && next->key != NULL && strcmp(key, next->key) > 0) 
    {
    	last = next;
    	next = next->next;
    }

    /* There's already a pair.  Let's replace that string. */
    if (next != NULL && next->key != NULL && strcmp(key, next->key) == 0) 
    {
    	//free(next->value);
    	next->value = value;
    }
    else
    {
        /* Nope, could't find it.  Time to grow a pair. */
        newpair = saHashNewpair(key, value);

    	/* We're at the start of the linked list in this bin. */
    	if (next == hashtable->table[bin])
        {
    	    newpair->next = next;
    	    hashtable->table[bin] = newpair;
    	}
        else if (next == NULL) 
        {
    	    /* We're at the end of the linked list in this bin. */
    	    last->next = newpair;
    	} 
        else  
        {
    	    /* We're in the middle of the list. */
    	    newpair->next = next;
            last->next = newpair;
    	}
    }
}

/* Retrieve a key-value pair from a hash table. */
void *saHashGet(saHashtableTypePtr hashtable, char *key) 
{
    int bin = 0;
    saHashentryTypePtr pair;

    bin = saHashHash(hashtable, key);

    /* Step through the bin, looking for our value. */
    pair = hashtable->table[bin];
    while(pair != NULL && pair->key != NULL && strcmp(key, pair->key) > 0) 
    	pair = pair->next;

    /* Did we actually find anything? */
    if (pair == NULL || pair->key == NULL || strcmp(key, pair->key) != 0)
    	return NULL;
    return pair->value;
}

/*
   Leave for testing purposes....

int main( int argc, char **argv ) {

    saHashtableTypePtr hashtable = saHashCreate( 65536 );

    saHashSet( hashtable, "key1", "inky" );
    saHashSet( hashtable, "key2", "pinky" );
    saHashSet( hashtable, "key3", "blinky" );
    saHashSet( hashtable, "key4", "floyd" );

    printf( "%s\n", saHashGet( hashtable, "key1" ) );
    printf( "%s\n", saHashGet( hashtable, "key2" ) );
    printf( "%s\n", saHashGet( hashtable, "key3" ) );
    printf( "%s\n", saHashGet( hashtable, "key4" ) );

    return 0;
}
*/
