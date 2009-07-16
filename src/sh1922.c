/*****************************************************************************/
/*                                                                           */
/*                                                                           */
/* (c) Copyright 1989,1990,1991,1992 by                                      */
/*     Senter for Industriforskning, Oslo, Norway                            */
/*     All rights reserved. See the sisl-copyright.h for more details.            */
/*                                                                           */
/*****************************************************************************/

#include "sisl-copyright.h"

/*
 *
 * $Id: sh1922.c,v 1.2 2001-03-19 15:59:06 afr Exp $
 *
 */

#define SH1922

#include "sislP.h"

#if defined(SISLNEEDPROTOTYPES)
void
      sh1922(double et[],int im,int ik,double etau[],int in,double ea[],
	     int nfirst[],int nlast[],int *jstat)
#else
void sh1922(et,im,ik,etau,in,ea,nfirst,nlast,jstat)
   double et[];
   int im;
   int ik;
   double etau[];
   int in;
   double ea[];
   int nfirst[];
   int nlast[];
   int *jstat;
#endif     
/*
*********************************************************************
* 
* PURPOSE    : To compute the B-spline refinement transformation matrix
*              from the spline space generated by the knot vector etau
*              to the refined spline space generated by the refined knot
*              vector et.
* 
* INPUT      : et     - Real array of length (im+ik) containing the refined
*                       knot vector.
*              im     - The dimension of the spline space corresponding to et.
*	       ik     - The order of the spline space.
*              etau   - Real array of length (in+ik) containing the original 
*                       knot vector.
*              in     - The dimension of the spline space corresponding
*                       to etau.
*
* 
* OUTPUT     : ea     - Real array of dimension (im*ik) containing 
*                       the B-spline refinement matrix from the knot vector
*                       etau to the knot vector et. This matrix has
*                       dimension im*in but since at most
*                       ik entries are nonzero in each row, it can
*                       be stored in a im*ik array together
*                       with two integer arrays indicating the position
*                       of the first and last nonzero elements in each
*                       row.
*              nfirst - Integer array of dimension (im) containing 
*                       pointers to the first nonzero element of each row 
*                       of the B-spline refinement matrix from etau to et.
*              nlast  - Integer array of dimension (im) containing 
*                       pointers to the last nonzero element of each row 
*                       of the B-spline refinement matrix from etau to et.
*              jstat      - status messages  
*                                         > 0      : warning
*                                         = 0      : ok
*                                         < 0      : error
*             
* 
* METHOD     : 
*
*
* REFERENCES : 
*              
*
* USE        :
*
*-
* CALLS      :   
*
* WRITTEN BY : Vibeke Skytt, SI, 05.92, on the basis of a routine
*              written by Tom Lyche and Knut Moerken, 12.85.
*
*********************************************************************
*/
{ 
   int kstat = 0;
   int kj1,kj2;
   int kih1,kih2;
   int ki,kj,kjh,kkj;
   int kmu;
   int kmuprm;
   int knu;
   double *sah = SISL_NULL;
   
   if ((sah = new0array(ik,DOUBLE)) == SISL_NULL) goto err101;
   
   /* The first and last few rows of the refinement matrix ea may contain
      only zeroes. The first task is to find the first and the last
      nonzero rows (pointed at by kj1 and kj2).  First kj2 are determined. */

   for (kmu=in+ik-1; DEQUAL(etau[kmu-1],etau[kmu]); kmu--);
   
   for (kjh=im-1; et[kjh+ik]>etau[kmu]; kjh--);
   
   ki = kjh;
   if (DEQUAL(et[kjh+ik],etau[kmu]))
      {
	 for (; DEQUAL(et[ki-1+ik],et[kjh+ik]); ki--);
      }
      
   kj2 = MIN(im-1,ki+in+ik-kmu-1);
   
   /* Determine kj1.  */
       
   for (kmu=0; DEQUAL(etau[kmu+1],etau[kmu]); kmu++);
   
   for (kjh=0; et[kjh]<etau[kmu]; kjh++);
   
   ki = kjh;
   if (DEQUAL(et[kjh],etau[kmu]))
      {
	 for (; DEQUAL(et[ki+1],et[kjh]); ki++);
      }
   kj1 = MAX(0,ki-kmu);      
      
   /*  Set all elements of ea to zero. */
      
   memzero(ea,im*ik,DOUBLE);
   
   /* Determine the refinement transformation.  */
   /* Indicate that rows 1,2,..,kj1 of ea are zero by setting nfirst>nlast
      for these rows.  */
   
   for (kj=0; kj<kj1; kj++)
     {
	nfirst[kj] = ik;
	nlast[kj] = 0;
     }
   
   /* Similarily for rows kj2+1, ... im-1 of ea.  */
   
   for (kj=kj2+1; kj<im; kj++)
     {
	nfirst[kj] = ik;
	nlast[kj] = 0;
     }
   
   /* Compute rows kj1 to kj2 of ea. */
   
   for (kj=kj1; kj<=kj2; kj++)
     {
	for (; etau[kmu+1] <= et[kj]; kmu++);
	kkj = kj;
	sh1929(etau,in,ik,kmu,et,im,kkj,sah,&kmuprm,&knu,&kstat);
	if (kstat < 0) goto error;
		       
       /* Get the pointers right.  */
		       
       nfirst[kj] = MAX(kmuprm-knu,0);
       nlast[kj] = MIN(kmuprm,in-1);
       kih1 = nfirst[kj] + ik - kmuprm - 1;
       kih2 = nlast[kj] + ik - kmuprm - 1;
       for (ki=kih1; ki<=kih2; ki++)
	 ea[kj*ik+ki] = sah[ki];
     }

   /* Refinement transformation matrix computed.  */

   *jstat = 0;
   goto out;
   
   /* Error in space allocation.  */
   
   err101: *jstat = -101;
   goto out;
   
   /* Error in lower level routine.  */
   
   error: *jstat = kstat;
   goto out;
   
   out:
      /* Free scratch used for local array.  */
      
      if (sah != SISL_NULL) freearray(sah);
	  
      return;
}
   
