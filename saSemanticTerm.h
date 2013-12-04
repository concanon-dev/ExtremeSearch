/*
 (c) 2012-2013 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
//-----------------------------------------------------------------
//--(c)2012-2013 Scianta Analytics LLC
//-----------------------------------------------------------------
//
#ifndef __saSemanticTerm__
#define __saSemanticTerm__

// List the types of shapes for semantic terms
#define SA_SEMANTICTERM_SHAPE_CURVE "curve"
#define SA_SEMANTICTERM_SHAPE_CURVEDECREASE "curvedecrease"
#define SA_SEMANTICTERM_SHAPE_CURVEINCREASE "curveincrease"
#define SA_SEMANTICTERM_SHAPE_LINEAR "linear"
#define SA_SEMANTICTERM_SHAPE_LINEARDECREASE "lineardecrease"
#define SA_SEMANTICTERM_SHAPE_LINEARINCREASE "linearincrease"
#define SA_SEMANTICTERM_SHAPE_PI "pi"
#define SA_SEMANTICTERM_SHAPE_TRAPEZOID "trapezoid"
#define SA_SEMANTICTERM_SHAPE_TRIANGLE "triangle"

#define SA_SEMANTICTERM_DEFAULT_ALFACUT 0.1
#define SA_SEMANTICTERM_DEFAULT_OVERLAP 1.5
#define SA_SEMANTICTERM_DEFAULT_SDEV_SIZE 2.0
#define SA_SEMANTICTERM_MAXPOINTS 16
#define SA_SEMANTICTERM_TRAPEZOID_SIZE 0.25
#define SA_SEMANTICTERM_VECTORSIZE 256

typedef struct saSemanticTermStruct
{
    char *format;
    char *name;
    char *shape; 
    double domainMin, domainMax;
    double points[SA_SEMANTICTERM_MAXPOINTS];
    float alfacut;
    float indexVector[SA_SEMANTICTERM_VECTORSIZE]; // indexVector[i] = ((float)i/SIZE)*(max-min)
    float vector[SA_SEMANTICTERM_VECTORSIZE]; // The truth vector
    int numPoints; 
} saSemanticTermType;
typedef saSemanticTermType *saSemanticTermTypePtr;

#endif
