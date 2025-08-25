c
c  ray2gmt.f: Take an r2.out file from Zelt dump (plot parameter
c             idump=1) and extract the rays into a multiple-segment 
c             gmtfile.
c
c             Output is X,Z of the rays in model coordinates.
c
c             Some rays that exit the side of the model may not
c             have endpoints at the exit location and end up looking
c             like stranded rays.  You could add logic to this code
c             to search for those if you like.
c			-FIXED wsh 11 Dec 96
c
c             June 17,97 -dlz
c             Replaced all ray-selection logic with ray-code 1 selection.  
c             Also, stuck in very important logic for bypassing read
c             errors caused by text messages (due to ray-tracing
c             problems) within the ray file.
c
c             June 5,98 -dlz
c             re-instated zmin
c
c             
c

      character*6  test
      character*40 infile,outfile
      character*70 card

      integer*4 gr,ray,npt

      real x(100000), z(100000)

      read(5,14) infile
      read(5,14) outfile
c
      read(5,*)  zmin
c     read(5,*)  zcrazy
c     read(5,*)  iq_2pt

 14   format(a)
   
      open(10,file=infile,status='old')
      open(20,file=outfile,status='unknown')

      icnt=0
      do while(test .ne. 'gr ray')
         read(10,14) test
         icnt=icnt+1
      end do

      j=1
      npt_last=0
      last_gr=-9 

      do i=1,9999999
 20      icnt=icnt+1
         read(10,01,err=900,end=999) gr,ray,npt,x(j),z(j)
 01      format(i2,x,i2,x,i3,f8.3,f8.3,f8.3)
         if(ray .eq. 0)goto 20
c        if(iq_2pt .eq. 1 .and. ray .eq. 0)goto 20

         igr_ok=gr

         if(npt .gt. npt_last)then
            j=j+1
         else if(npt .lt. npt_last) then
            if(z(j-1) .le. zmin) then
              write(20,14) '>'
              do k=1,j-1
                  write(20,*) x(k),z(k)
c                 if(z(k) .lt. zcrazy) write(20,*) x(k),z(k)
              end do
              if(igr_ok .ne. last_gr)then
                 print *, 'Ray group ',gr
                 last_gr=igr_ok
              end if
            end if
            x(1)=x(j)
            z(1)=z(j)
            j=2
         end if
         
         npt_last = npt

         goto 20

c900     backspace(10)
c        read(10,14)card
c        print *, 'Error Line ', icnt,':',card
 900     print *, 'Error Line ', icnt
         goto 20

       end do

 999   stop
       end
