c
c        raybottom.f - extract bottoming points of rays
c                      from a gmt segment file and output
c        input file is output from ray2gmt program
c
c

         character*40 infile,outfile
         character*1 arrow

         real*4 xb(99999),zb(99999)
         real*4  x(99999), z(99999)

         read(5,14) infile
         read(5,14) outfile 
 14      format(a)

         open(10,file=infile,status='old')
         open(21,file=outfile,status='unknown')
         open(22,file='curve',status='unknown')

c
c        assume all read errors are due to
c        the segment deliminators, '>'
c
c        run through the entire file and find
c        bottom points between each set of >'s
c

         read(10,14) arrow

 101     read(10,*,err=101) x(1),z(1)
         read(10,*,err=101) x(2),z(2)

         do i=3,9999999
            read(10,*,err=103,end=900) x(i),z(i)
         end do

 103     icnt=i-1

         zmax=0
         do i=1,icnt
            if(z(i).gt.zmax)then
               zmax=z(i)
               xmax=x(i)
            end if
         end do
         write(21,*) xmax, zmax

         goto 101

c
c        Now rewind the output file and
c        use some logic to generate curves
c        through groups of bottoming points
c        corresponding to a single ray group
c
c        Output to file: curve
c

 900     rewind(21)

         do i=1,999999
            read(21,*,end=102) xb(i),zb(i)
         end do

 102     npts=i-1

         print *, npts

         rewind(21)

         arrow='>'
         write(22,14) arrow
c
         write(22,*) xb(1),zb(1)

         isign=-1
         if(xb(2).ge.xb(1))isign=1


         do i=1,npts-1

            if(xb(i+1).ge.xb(i))then
               jsign=1
            else
               jsign=-1
            end if

            if(zb(i+1).lt.zb(i)-.2)then
               zstep=0
            else
               zstep=1
            end if

            xstep=abs(xb(i+1)-xb(i))

c           if(jsign .ne. isign .or.
            if(zstep .eq.  0.   .or. 
     .         xstep .gt. 50.   ) then
               isign=jsign
               write(22,14) arrow
            end if
            write(22,*) xb(i+1),zb(i+1)

         end do

         stop
         end
            
