/*
 Copyright 2012-2014 Scianta Analytics LLC   All Rights Reserved.  
 Reproduction or unauthorized use is prohibited. Unauthorized
 use is illegal. Violators will be prosecuted. This software 
 contains proprietary trade and business secrets.            
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

void saSpearmanCorrelation(double dX[], double dY[], int nTermCnt, double *fR)
{
    int j;
    double fyt,fxt,ft,fdf;
    double fsyy=0.0,
          fsxy=0.0,
          fsxx=0.0,
          fay =0.0,
          fax =0.0;

    for(j=0;j<nTermCnt;j++)
    {
        fax += dX[j];
        fay += dY[j];
    }
    fax /= nTermCnt;
    fay /= nTermCnt;

    for(j=0;j<nTermCnt;j++)
    {
        fxt=dX[j]-fax;
        fyt=dY[j]-fay;
        fsxx += fxt * fxt;
        fsyy += fyt * fyt;
        fsxy  = fxt*fyt;
    }
    *fR=fsxy/sqrt(fsxx*fsyy);
}
