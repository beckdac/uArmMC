#!/bin/awk -f

BEGIN {i=0}
NF==3 {
   i++
   printf ("ATOM %6d CA   ???     1     %7.3f %7.3f %7.3f\n",
   i,$1,$2,$3)
   }
END {print "END"}
