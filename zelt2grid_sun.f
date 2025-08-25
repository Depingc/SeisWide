c 
c  zelt2grid_sun.f
c
c      Programs reads zelt velocity files line by line, 
c      unformatted except for the zelt sequencing, using a
c      character read for each line outputted to a scratch file.
c      An end-of-line (or end-of-record) is determined as 10 blanks 
c      on a given input record.
c
c      Input is from a parameter file with lines
c
c       input file     :    Zelt v.in 
c      output file 1   :    interface xz file 
c      output file 2   :    binary velocity grid 
c      Nlayers         :    # of layers of v.in to process
c      rMinOff rMaxOff :    min & max offsets of v.in
c      X1out X2out dx  :    X-limits of output grid, dx  [km]
c      Z1out Z2out dz  :    X-limits of output grid, dx  [km]
c
c
c       Output files:  
c
c       (1) model interfaces in an xz, multiple-segment gmt file;  
c       (2) binary file of velocity matrix Grid(nXrow,nZcol)
c
c      rMaxOff: maximum offset off the model (largest X)
c               used to complete layers designated only at X=0. 
c
c      rMinOff: minimum offset of the model
c               used w/ rMaxOff to calculate grid X-dimension and indexing
c               
c      X1out X2out  define output grid dim -> nXrow=1+int((X2out-X1out)/dx)
c      Z1out Z2out  define output grid dim -> nZcol=1+int((Z2out-Z1out)/dz)
c
c      Note: if Z2out .gt. the depth of the Nlayers interface at a given
c            X location, then the output grid will be filled from the bottom
c            of the Nlayers interface to the depth Z2out w/ the velocity at 
c            the interface
c
c
c
c        parameter(Nnodes=1000,nbuf=64e6,
c     .            Nxgmx=10000,Nzgmx=2000)
 
       parameter(Nnodes=2000,nbuf=64e6,
     .            Nxgmx=20000,Nzgmx=2000)

        real*4 Xv_top(Nnodes),  Xv_bot(Nnodes),
     .         Xv_topS(Nnodes), Xv_botS(Nnodes),
     .         Vx_top(Nnodes),  Vx_bot(Nnodes),
     .         Vx_topS(Nnodes), Vx_botS(Nnodes),
     .         Xz_top(Nnodes),  Xz_bot(Nnodes),
     .         Zx_top(Nnodes),  Zx_bot(Nnodes)

        real*4 Xtop(Nxgmx),Ztop(Nxgmx),Vtop(Nxgmx),
     .         Xbot(Nxgmx),Zbot(Nxgmx),Vbot(Nxgmx),
     .         Grid(Nxgmx,Nzgmx)

        integer*4 intX_L(Nnodes), 
     .            intZ_T(Nnodes), intZ_B(Nnodes)
        real*4    Vint_T(Nnodes), Vint_B(Nnodes),
     .            inodes(Nnodes*4)

        real*8    xmin,xmax,dx_km,
     .            ymin,ymax,dy_km,
     .            zmin,zmax


cc      allocatable Xtop, Ztop, Vtop,
cc   .              Xbot, Zbot, Vbot,
cc   .              Grid

        integer*4    layer, cont, dinv(Nnodes)

        integer*4 ioutunit
        parameter(ioutunit=22)

        character*80 infile, velfile, modfile


       read(5,40)  infile 
       read(5,40) modfile 
       read(5,40) velfile 

       read(5,* ) Nlayers
       read(5,* ) rMinOff, rMaxOff

       read(5,* ) X1out, X2out, dx
       read(5,* ) Z1out, Z2out, dz
       read(5,* ) iq_gmtfile
       read(5,* ) iq_check_holes
c
 40    format(a80)
 14    format(a)
c
       nXgrid= 1+int((rMaxOff-rMinOff)/dx)
       nXrow = 1+int((X2out-X1out)/dx)
       nZcol = 1+int((Z2out-Z1out)/dz)
c
       nbytes=   6*4*(nXgrid+4)
     .         +   4*nXrow*nZcol
     .         +20*4*Nnodes

cc     if(nbytes .gt. nbuf)then
cc        print *,' '
cc        print *,'Grid size exceeds buffer. Stopping.'
cc        print *,' '
cc        stop
cc     end if

cc     allocate( Xtop(nXgrid+4),Ztop(nXgrid+4),Vtop(nXgrid+4),
cc   .           Xbot(nXgrid+4),Zbot(nXgrid+4),Vbot(nXgrid+4),
cc   .           Grid(nXrow,nZcol))
cc

       if(nXgrid+4 .lt. Nxgmx .or.
     .    nXrow    .le. Nxgmx .or.
     .    nZcol    .lt. Nzgmx) then
          print *, ' '
          print *, 'Array parameter values are too small.'

          print *, '   Nxgmx=', Nxgmx
          print *, 'nXgrid+4=', nXgrid+4
          print *, '   nXrow=', nXrow
          print *, ' '
          print *, '   Nzgmx=', Nzgmx
          print *, '   nZcol=', nZcol
          print *, ' '
       end if

       do ix=1,nXgrid+4
          Xtop(ix)=0.
          Ztop(ix)=0.
          Vtop(ix)=0.
          Xbot(ix)=0.
          Zbot(ix)=0.
          Vbot(ix)=0.
          do iz=1,nZcol
             Grid(ix,iz)=0.
          end do
       end do
c
c
       io_in = 10
       io_out =11
c
       Nx = 1+int((rMaxOff-rMinOff)/dx)

      open(io_in,file=infile,status='old')
      open(21,file=modfile,status='unknown')

      if(iq_gmtfile .ne. 1)then
      open(ioutunit,file=velfile,status='unknown',
     .              access='direct',recl=nXrow*4)
      end if

c
c     initialize grid and do indexing
c
      jxgrd1=1+int((X1out-rMinOff)/dx)
      jxgrd2=1+int((X2out-rMinOff)/dx)
      jzgrd1=1+int(Z1out/dz)
      jzgrd2=1+int(Z2out/dz)
c
      print *, '              '
      print *, '     Output file : ',velfile
      print *, '           nXrow = ',nXrow
      print *, '           nZcol = ',nZcol
      print *, '                   '
      print *, '          jxgrd1 = ',jxgrd1
      print *, '          jxgrd2 = ',jxgrd2
      print *, '          jzgrd1 = ',jzgrd1
      print *, '          jzgrd2 = ',jzgrd2
      print *, '              '
c

cc     Grid=0.

c
c      skip layer zero
c
       read(io_in,*) layer
       if (layer .eq. 0) then
          read(io_in,*) layer
       else
          rewind(io_in)
       end if
c
       write(21,14) '>'
c
c
       do ilay=1,Nlayers
c
c        *** Interface block
c
         do icl=1,Nnodes
            Xz_bot(icl)=0.
            Zx_bot(icl)=0.
         end do
c
         call line_cr(io_in,io_out)
c
         do i=1,Nnodes
            Xz_bot(i)=-99999.
         end do
         read(io_out,*,end=10,err=10) layer, (Xz_bot(i),i=1,Nnodes) 
 10      ipts1=0
         do i=1,Nnodes
            if(Xz_bot(i).ne.-99999.)then
               ipts1=ipts1+1
            end if
         end do
         close(io_out)
c
         read(io_in,*)  cont, (Zx_bot(i),i=1,ipts1)
         if (ilay .ne. Nlayers) then
         read(io_in,*)        (  dinv(i),i=1,ipts1) 
         end if
         print *,'Layer ',layer,',  Points ',ipts1,',  continue ',cont
c  
 101     if( cont .eq. 1) then  
            ipts1a=ipts1+1
            call line_cr(io_in,io_out)
            read(io_out,*,err=11,end=11) layer, 
     .                              (Xz_bot(i),i=ipts1a,Nnodes)  
 11         ipts1=0
            do i=1,Nnodes
               if(Xz_bot(i).ne.-99999.)then
                  ipts1=ipts1+1
               end if
            end do
            close(io_out)
            read(io_in,*)  cont, (Zx_bot(i),i=ipts1a,ipts1) 
            read(io_in,*)        (  dinv(i),i=ipts1a,ipts1)
         print *,'Layer ',layer,',  Points ',ipts1,',  continue ',cont
            goto 101
         end if
c
c
         if (ilay .ne. Nlayers) then
c
c           *** Top velocity block
c
            call line_cr(io_in,io_out)

            do i=1,Nnodes
               Xv_top(i)=-99999.
            end do
            read(io_out,*,err=20,end=20) layer, (Xv_top(i),i=1,Nnodes)
 20         ipts2=0
            do i=1,Nnodes
               if(Xv_top(i).ne.-99999.)then
                  ipts2=ipts2+1
               end if
            end do
            close(io_out)
            read(io_in,*)  cont, (Vx_top(i),i=1,ipts2)
            read(io_in,*)        (  dinv(i),i=1,ipts2)
 201        if( cont .eq. 1) then   
               ipts2a=ipts2+1 
               call line_cr(io_in,io_out)
               read(io_out,*,err=21,end=21) layer,
     .                          (Xv_top(i),i=ipts2a,Nnodes)
 21            ipts2 = 0 
               do i=1,Nnodes
                  if(Xv_top(i).ne.-99999.)then
                     ipts2=ipts2+1
                  end if
               end do
               close(io_out)
               read(io_in,*)  cont, (Vx_top(i),i=ipts2a,ipts2)  
               read(io_in,*)        (  dinv(i),i=ipts2a,ipts2)
               goto 201 
            end if 
c
c           Condition for fixed value across interface
c
            if(Vx_top(ipts2) .eq. 0.)then
               ipts2=ipts3S
               do i=1,ipts3S
                  Vx_top(i)=Vx_botS(i)
                  Xv_top(i)=Xv_botS(i)
               end do
            end if
c
c           *** Bot velocity block
c
            call line_cr(io_in,io_out)
            do i=1,Nnodes
               Xv_bot(i)=-99999.
            end do
            read(io_out,*,err=30,end=30) layer, (Xv_bot(i),i=1,Nnodes)
 30         ipts3 = 0
            do i=1,Nnodes
               if(Xv_bot(i).ne.-99999.)then
                  ipts3=ipts3+1
               end if
            end do
            close(io_out)
            read(io_in,*)  cont, (Vx_bot(i),i=1,ipts3)
            read(io_in,*)        (  dinv(i),i=1,ipts3)
 301        if( cont .eq. 1) then   
               ipts3a=ipts3+1  
               call line_cr(io_in,io_out)
               read(io_out,*,err=31,end=31) layer,
     .                              (Xv_bot(i),i=ipts3a,Nnodes) 
 31            ipts3 = 0
               do i=1,Nnodes
                  if(Xv_bot(i).ne.-99999.)then
                     ipts3=ipts3+1
                  end if
               end do
               close(io_out)
               read(io_in,*)  cont, (Vx_bot(i),i=ipts3a,ipts3)   
               read(io_in,*)        (  dinv(i),i=ipts3a,ipts3) 
               goto 301  
            end if
c
c           Condition for zero velocity gradient
c
            print *,'         ipts3=',ipts3
            print *,' Vx_bot(ipts3)=',Vx_bot(ipts3)
            if(Vx_bot(ipts3) .eq. 0.)then
               ipts3=ipts2
               do i=1,ipts3
                  Vx_bot(i)=Vx_top(i)
                  Xv_bot(i)=Xv_top(i)
               end do
            end if
c           !
         end if
c        !
c        ! end if on ilayer<Nlayers
c        !
c        !
c        Old output interface data
c
c        do j=1,ipts1
c           write(21,*) Xz_bot(j), Zx_bot(j), layer
c        end do
         if (ipts1 .eq. 1) then
c            write(21,*) rMaxOff, Zx_bot(1), layer
             Xz_bot(2)=rMaxOff
             Zx_bot(2)=Zx_bot(1)
             ipts1=2
         end if
c        if( ilay .ne. Nlayers) then
c            write(21,14) '>'
c        end if
         if (ipts2 .eq. 1) then
             Xv_top(2)=rMaxOff
             Vx_top(2)=Vx_top(1)
             ipts2=2
         end if
         if (ipts3 .eq. 1) then
             Xv_bot(2)=rMaxOff
             Vx_bot(2)=Vx_bot(1)
             ipts3=2
         end if
c
c
c        interpolate x,z,v for interfaces
c
c        first do top of layer
c
         if(ilay .gt. 1)then

            Xtop(1)=Xz_top(1)
            Ztop(1)=Zx_top(1)
            jx=1
            do i=2,ipts1S 
               Next=nint((Xz_top(i)-Xtop(jx))/dx)
               do j=jx+1,jx+Next
                  Xtop(j)=rMinOff+dx*float(j-1)
                  call getZ(Xz_top(i-1),Xz_top(i),
     .                      Zx_top(i-1),Zx_top(i), 
     .                      Xtop(j),Ztop(j))
               end do
               jx=jx+Next
            end do
c
            if(Xv_topS(ipts2S) .ne. rMaxOff)then
               Xv_topS(ipts2S+1)=Xv_topS(ipts2S)
               Vx_topS(ipts2S+1)=Vx_topS(ipts2S)
               ipts2S=ipts2S+1
            end if

            Vtop(1)=Vx_topS(1)
            jx=1
            do i=2,ipts2S
               Next=nint((Xv_topS(i)-Xtop(jx))/dx)
               do j=jx+1,jx+Next
                  call getZ(Xv_topS(i-1),Xv_topS(i),
     .                      Vx_topS(i-1),Vx_topS(i),
     .                      Xtop(j),Vtop(j))
               end do
               jx=jx+Next
            end do
c
c        Now do bottom layer
c
            Xbot(1)=Xz_bot(1)
            Zbot(1)=Zx_bot(1)
            jx=1
            do i=2,ipts1 
               Next=nint((Xz_bot(i)-Xbot(jx))/dx)
               do j=jx+1,jx+Next
                  Xbot(j)=rMinOff+dx*(j-1)
                  call getZ(Xz_bot(i-1),Xz_bot(i),
     .                      Zx_bot(i-1),Zx_bot(i),
     .                      Xbot(j),Zbot(j))
               end do
               jx=jx+Next
            end do
c
            if(Xv_botS(ipts3S) .ne. rMaxOff)then
               Xv_botS(ipts3S+1)=Xv_botS(ipts3S)
               Vx_botS(ipts3S+1)=Vx_botS(ipts3S)
               ipts3S=ipts3S+1
            end if

            Vbot(1)=Vx_botS(1)
            jx=1
            do i=2,ipts3S
               Next=nint((Xv_botS(i)-Xbot(jx))/dx)
               do j=jx+1,jx+Next
                  call getZ(Xv_botS(i-1),Xv_botS(i),
     .                      Vx_botS(i-1),Vx_botS(i),
     .                      Xbot(j),Vbot(j))
               end do
               jx=jx+Next
            end do
c
c
c           New output interface data
c
            do ix=jxgrd1,jxgrd2
               write(21,*) Xtop(ix), Ztop(ix), layer
            end do
            if( ilay .ne. Nlayers) then
                write(21,14) '>'
            end if
c
c
c           Load interface-node data
c
c
c           check for nodes at left of model

            ind=0
            if(Xv_topS(1) .ne. rMinOff .and.
     .         Xv_botS(1) .ne. rMinOff)then
               ind=1
               intX_L(ind)=1
               intZ_T(ind)=1+ int(Ztop(1)/dz)
               intZ_B(ind)=1+nint(Zbot(1)/dz)
               Vint_T(ind)=Vtop(1)
               Vint_B(ind)=Vbot(1)
            end if
            
            j=0
            do i=1,ipts1S
               j=j+1
               inodes(j)=1+int((Xz_top(i)-rMinOff)/dx)
            end do
            do i=1,ipts1
               j=j+1
               inodes(j)=1+int((Xz_bot(i)-rMinOff)/dx)
            end do
            do i=1,ipts2S
               j=j+1
               inodes(j)=1+int((Xv_topS(i)-rMinOff)/dx)
            end do
            do i=1,ipts3S
               j=j+1
               inodes(j)=1+int((Xv_botS(i)-rMinOff)/dx)
            end do
            nodes2=ipts1S+ipts1+ipts2S+ipts3S
            call sort(nodes2,inodes)

            ind=ind+1
            jnd=inodes(1)
            intX_L(ind)=jnd
            intZ_T(ind)=1+ int(Ztop(jnd)/dz)
            intZ_B(ind)=1+nint(Zbot(jnd)/dz)
            Vint_T(ind)=Vtop(jnd)
            Vint_B(ind)=Vbot(jnd)

            do i=2,nodes2
               if(inodes(i).ne.inodes(i-1))then
                  ind=ind+1
                  jnd=inodes(i)
                  intX_L(ind)=jnd
                  intZ_T(ind)=1+ int(Ztop(jnd)/dz)
                  intZ_B(ind)=1+nint(Zbot(jnd)/dz)
                  Vint_T(ind)=Vtop(jnd)
                  Vint_B(ind)=Vbot(jnd)
               end if
            end do
c
            Nodes_L=ind
            Ntrpz=Nodes_L-1
c
            do it=1,Ntrpz
c
               ix1=intX_L(it)
               ix2=intX_L(it+1)

               x1=0.
               x2=float(ix2-ix1)
               s1=float(intZ_T(it+1)-intZ_T(it))/x2
               s2=float(intZ_B(it+1)-intZ_B(it))/x2

               if(intZ_T(it)   .eq. intZ_B(it)   .and.
     .            intZ_T(it+1) .eq. intZ_B(it+1))goto 901

               v1=Vint_T(it)
               v2=Vint_T(it+1)
               v3=Vint_B(it)
               v4=Vint_B(it+1)

               iz0=intZ_T(it)
               b1=0.
               b2=float(intZ_B(it)-iz0)

               c1 = x2*(s2*v1-s1*v3)+b2*(v2-v1)
               c2 = s2*(v2-v1)-s1*(v4-v3)
               c3 = x2*(v3-v1)
               c4 = v1-v2+v4-v3
               c5 = b2*x2*v1
               c6 = (s2-s1)*x2
               c7 = b2*x2

               jx1=max(ix1,jxgrd1)
               jx2=min(ix2,jxgrd2)
   
               do jx=jx1,jx2
   
                  X=float(jx-ix1)
                  iz1=iz0+int(s1*X)
                  iz2=iz0+int(s2*X+b2)

                  if((c6*X+c7) .ne. 0.)then
                     iz1=min(iz1,jzgrd2)
                     iz2=min(iz2,jzgrd2)
                     deno=1./(c6*X+c7)
                     topf  =(c1*X+c2*X*X+c5)
                     topm  =(c3+c4*X)
   
                     do iz=iz1,iz2
                        Z=float(iz-iz0)
                        kx=jx-jxgrd1+1
                        kz=iz-jzgrd1+1
                        grid(kx,kz)=(topf+Z*topm)*deno
                     end do
                  end if
               end do
 901        end do
c           !
         end if
c
c        !   if on ilayer>1
c
c        shuffling for saved top interface 
c
         ipts1S=ipts1
         ipts2S=ipts2
         ipts3S=ipts3

         do ip=1,ipts1
            Xz_top(ip)=Xz_bot(ip)
            Zx_top(ip)=Zx_bot(ip)
         end do

         do ip=1,ipts2
            Xv_topS(ip) = Xv_top(ip)
            Vx_topS(ip) = Vx_top(ip)
         end do

         do ip=1,ipts3
            Xv_botS(ip) = Xv_bot(ip)
            Vx_botS(ip) = Vx_bot(ip)
         end do

         do ip=ipts2+1,Nnodes
            Xv_topS(ip)=0.
            Vx_topS(ip)=0.
         end do

         do ip=ipts3+1,Nnodes
            Xv_botS(ip)=0.
            Vx_botS(ip)=0.
         end do
 
         do ip=ipts1+1,Nnodes
            Xz_top(ip)=0.
            Zx_top(ip)=0.
         end do
c        ! 
      end do
c     !
c     end loop over layers
c
c
      if(iq_check_holes .eq. 1)then
      type *, ' Checking for holes, outputting data ... '
 
      nHoles=0
      nzholes=0

      do iz=1,nZcol
         do ix=1,nXrow
            if(Grid(ix,iz) .eq. 0.)then
               nHoles=nHoles+1
               i_touch=1
               if(iz .gt. 1 .and. iz .lt. nZcol .and. 
     .            Grid(ix,iz-1) .ne. 0 .and.
     .            Grid(ix,iz+1) .ne. 0) then
                  Grid(ix,iz)=(Grid(ix,iz-1)+Grid(ix,iz+1))/2.
               else if(iz .gt. 1 .and. 
     .            Grid(ix,iz-1) .ne. 0 ) then
                  Grid(ix,iz)=Grid(ix,iz-1)
               else if(iz .lt. nZcol .and. 
     .            Grid(ix,iz+1) .ne. 0 ) then
                  Grid(ix,iz)=Grid(ix,iz+1)
               end if
               if(Grid(ix,iz).eq.0)nzholes=nzholes+1
            end if
         end do
      end do
      end if
c
c
      if(iq_gmtfile .eq. 1)then
c
         print *,' '
         print *,'Outputting grid, GRDfile type=1 ...'
         print *,' '
c        !
         zmax=-99999999.
         zmin= 99999999.
c
         do iz=1,nZcol
            do ix=1,nXrow
               zmax=max(Grid(ix,iz),zmax)
               zmin=min(Grid(ix,iz),zmin)
            end do
         end do
         xmin=X1out
         xmax=X2out
         ymin=Z1out
         ymax=Z2out
         dx_km=dx
         dy_km=dz
            Nx=nXrow
            Ny=nZcol

         call open_gmtfile(ioutunit,velfile,
     .                     dx_km,Nx,xmin,xmax,
     .                     dy_km,Ny,ymin,ymax,zmin,zmax)

         do iz=1,nZcol
            do ix=1,nXrow
               Vtop(ix)=Grid(ix,iz)
            end do
            iword=224+(iz-1)*nXrow
            CALL PODISC( ioutunit, 1, iword)
            CALL WRDISC( ioutunit,  Vtop,  nXrow, ISTAT )
         end do
      else
         print *,' '
         print *,'Outputting grid ...'
         print *,' '
         do iz=1,nZcol
            write(ioutunit,rec=iz) (Grid(ix,iz),ix=1,nXrow)
         end do
      end if
            
      NxNz=nXrow*nZcol


      type *, '              '
      type *, '     Output file : ',velfile
      type *, '              Nx = ',nXrow
      type *, '              Nz = ',nZcol
      type *, '              '
      type *, '           Nx*Nz = ',NxNz
      type *, ' # of Grid Holes = ',nHoles
      type *, ' remaining holes = ',nzholes
      type *, '              '
c
c
cc     deallocate( Xtop,Ztop,Vtop,
cc   .             Xbot,Zbot,Vbot,Grid)

      close(21)
      close(23)
      close(ioutunit)

 999  stop
      end
c
c-----------------------------------------------------------------
c
      subroutine getZ(X1,X2,Z1,Z2,XX,ZZ)

      if(X2.eq.X1)then
         ZZ = Z1
      else
         slope = (Z2-Z1)/(X2-X1) 
         ZZ = Z1+(XX-X1)*slope 
      end if
 
      return
      end

c
c-----------------------------------------------------------------
c
      subroutine line_cr(io_in,io_out)

      byte         icard(200)
      character*200 card

      equivalence(card(1:1),icard(1))

      read(io_in,200) card
 200  format(a200)    
      do i=1,200 
         if(icard(i) .eq. 32)then 
            do j=1,10
               if(icard(i+j) .ne. 32) goto 79
            end do
            iend=i-1 
            goto 81 
 79      end if 
      end do 
 81   open( io_out,status='scratch')
      write(io_out,14) card(1:iend)
      rewind(io_out)
c     write(40,14) card(1:iend)
c      write(6,14) card(1:iend)
 14   format(a)

      return
      end
c
c--------------------------------------------------------------
c
      subroutine open_gmtfile(ioutunit,file,
     .                        dx_km,Nx,xmin,xmax,
     .                        dy_km,Ny,ymin,ymax,zmin,zmax)

      character*80 file 
      character*800 card

      character*80  title
      character*320 command
      character*160 comments
      character*80  x_units,y_units,z_units

        integer*4   Nx,Ny

           real*8   xmin, xmax, dx_km,
     .              ymin, ymax, dy_km,
     .              zmin, zmax, 
     .              z_scale_factor,
     .              z_add_offset 
           real*8   doubles(10)

        integer*4   ioutunit
        integer*4   node_offset
        integer*2   Nx2,Ny2
c
        equivalence( card(  1: 80), x_units),
     .             ( card( 81:160), y_units),
     .             ( card(161:240), z_units),
     .             ( card(241:320), title  ),
     .             ( card(321:640), command),
     .             ( card(641:800), comments)

      z_scale_factor=1.
      z_add_offset=0.

      doubles(1)=xmin
      doubles(2)=xmax
      doubles(3)=ymin
      doubles(4)=ymax
      doubles(5)=zmin
      doubles(6)=zmax
      doubles(7)=dx_km  
      doubles(8)=dy_km  
      doubles(9)=z_scale_factor
      doubles(10)=z_add_offset

c
      x_units(1:2)='km'
      y_units(1:2)='km'
c     z_units(1:9)='undefined'

      title(1:26)='GMT binary GRD file type=1'
      command(1:22)='Generated by zelt2grid'
      comments(1:18)='Lizarralde is cool'

      node_offset=0
      Nx2=int(Nx)
      Ny2=int(Ny)
c
      do i=1,80
         if(file(i:i) .eq. ' ') then
            file(i:i) = char(0)
            goto 32    
         end if
      end do

 32   call getfil(-3, ioutunit, file, istat)
c
         CALL PODISC( ioutunit, 1, 0)


         CALL WRDISCB( ioutunit,  Nx            ,  4)
         CALL WRDISCB( ioutunit,  Ny            ,  4)
         CALL WRDISCB( ioutunit,  node_offset   ,  4)
         CALL WRDISCB( ioutunit,  node_offset   ,  4)
         CALL WRDISCB( ioutunit,  xmin          ,  8)
         CALL WRDISCB( ioutunit,  xmax          ,  8)
         CALL WRDISCB( ioutunit,  ymin          ,  8)
         CALL WRDISCB( ioutunit,  ymax          ,  8)
         CALL WRDISCB( ioutunit,  zmin          ,  8)
         CALL WRDISCB( ioutunit,  zmax          ,  8)
         CALL WRDISCB( ioutunit,  dx_km         ,  8)
         CALL WRDISCB( ioutunit,  dy_km         ,  8)
         CALL WRDISCB( ioutunit,  z_scale_factor,  8)
         CALL WRDISCB( ioutunit,  z_add_offset  ,  8)
         CALL WRDISCB( ioutunit,  card          ,800)
c        CALL WRDISCB( ioutunit,  x_units       , 80)
c        CALL WRDISCB( ioutunit,  y_units       , 80)
c        CALL WRDISCB( ioutunit,  z_units       , 80)
c        CALL WRDISCB( ioutunit,  title         , 80)
c        CALL WRDISCB( ioutunit,  command       ,320)
c        CALL WRDISCB( ioutunit,  comments      ,160)

      return
      end

