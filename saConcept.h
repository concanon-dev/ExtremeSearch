/*
 (c) 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
//-----------------------------------------------------------------
//--(c)2012-2014 Scianta Analytics LLC
//-----------------------------------------------------------------
//
#ifndef __saConcept__
#define __saConcept__

// List the types of shapes for semantic terms
#define SA_CONCEPT_SHAPE_CURVE "curve"
#define SA_CONCEPT_SHAPE_CURVEDECREASE "curvedecrease"
#define SA_CONCEPT_SHAPE_CURVEINCREASE "curveincrease"
#define SA_CONCEPT_SHAPE_LINEAR "linear"
#define SA_CONCEPT_SHAPE_LINEARDECREASE "lineardecrease"
#define SA_CONCEPT_SHAPE_LINEARINCREASE "linearincrease"
#define SA_CONCEPT_SHAPE_PI "pi"
#define SA_CONCEPT_SHAPE_TRAPEZOID "trapezoid"
#define SA_CONCEPT_SHAPE_TRIANGLE "triangle"

#define SA_CONCEPT_DEFAULT_ALFACUT 0.1
#define SA_CONCEPT_DEFAULT_OVERLAP 1.5
#define SA_CONCEPT_DEFAULT_SDEV_SIZE 2.0
#define SA_CONCEPT_MAXPOINTS 16
#define SA_CONCEPT_TRAPEZOID_SIZE 0.25
#define SA_CONCEPT_VECTORSIZE 256

typedef struct saConceptStruct
{
    char *format;
    char *name;
    char *shape; 
    double domainMin, domainMax;
    double points[SA_CONCEPT_MAXPOINTS];
    float alfacut;
    float indexVector[SA_CONCEPT_VECTORSIZE]; // indexVector[i] = ((float)i/SIZE)*(max-min)
    float vector[SA_CONCEPT_VECTORSIZE]; // The truth vector
    int numPoints; 
} saConceptType;
typedef saConceptType *saConceptTypePtr;

#endif
