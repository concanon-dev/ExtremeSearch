// Copyright 2014, Scianta Analytics LLC
// All rights reserved.
//
// Author: joseph.mcglynn@sciantaanalytics.com (Joseph McGlynn)

#include <iostream>
#include "../../saHash.h"
#include "gtest/gtest.h"

extern "C" {
saHashtableTypePtr saHashCreate(int size);
void saHashSet(saHashtableTypePtr hashtable, char *key, char *value );
void *saHashGet(saHashtableTypePtr hashtable, char *key);
}

TEST(saContext, saHash) {

    saHashtableTypePtr hashtable = saHashCreate( 65536 );
    saHashSet( hashtable, "key1", "inky" );
    saHashSet( hashtable, "key2", "pinky" );
    saHashSet( hashtable, "key3", "blinky" );
    saHashSet( hashtable, "key4", "floyd" );
    char *result1 = (char *)saHashGet( hashtable, "key1" );
    char *result2 = (char *)saHashGet( hashtable, "key2" );
    char *result3 = (char *)saHashGet( hashtable, "key3" );
    char *result4 = (char *)saHashGet( hashtable, "key4" );

    EXPECT_STREQ("inky", result1);
    EXPECT_STREQ("pinky", result2);
    EXPECT_STREQ("blinky", result3);
    EXPECT_STREQ("floyd", result4);
}

