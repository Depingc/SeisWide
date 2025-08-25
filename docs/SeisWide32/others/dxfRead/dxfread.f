      program dxfread
c************************************************************
c
c     written by : Gordon Oakey
c                  
c     This program reads the 'AutoCAD' transfer file (.dxf)
c     and extracts the x-y coordinates.
c
c     Output file contains : X,Y,layer-name
c
c************************************************************
c
      character*40 filinp,filout,filnam
      character*20 string,lyr,layer(1000),olyr,oldl
c
      scale=1000000.
      xltmn=1000000000.
      xltmx=-1000000000.
      xlnmn=1000000000.
      xlnmx=-1000000000.
c
10    format(2f16.4,3x,a20)
11    format(a20)
c
      print*,'give name of input file'
      read(*,'(a)')filinp
      print*,'give name of output file'
      read(*,'(a)')filout
c      print*,'give name of stats file for layer names'
c      read(*,'(a)')filnam
	filnam = 'lNames.txt'
c
      open(1,file=filinp,status='old')
      open(2,file=filout,status='new')
      open(3,file=filnam,status='new')
c
      print*,'processing file : ',filinp
c
c ** skip header section
c
80    read(1,'(a)',end=100)string   
      if(string.ne.'ENTITIES')goto 80
c
c ** extract lines
c
100   read(1,*,end=200,err=199)inum
c
      if(inum.lt.10)then
        read(1,'(a)')string   
        if(inum.eq.8)lyr=string
        if(string.eq.'VERTEX')ist=1
        if(string.eq.'SEQEND'.and.ipo.gt.0)then
          ist=0
          ipo=0
          nlin=nlin+1
          if(jmod(nlin,100).eq.0)then
            print*,'writing layer :',lyr,' line :',nlin
          endif
          if(icls.eq.1)then
            write(2,10)oldx,oldy,oldl
            icls=0
          endif
          write(2,10)9999.,9999.,lyr
          if(nlin.eq.1)then
            ilyr=1
            layer(1)=lyr
            goto 120
          endif
          if(lyr.ne.olyr)then
            do 110 i=1,ilyr
110           if(lyr.eq.layer(i))goto 120
            ilyr=ilyr+1
            layer(ilyr)=lyr
          endif
120       continue
          olyr=lyr
        endif
      elseif(inum.lt.60)then
        read(1,*)real
         if(inum.ge.10.and.inum.le.18)xval=real
         if(inum.ge.20.and.inum.le.28)then
          if(lyr.ne.'0')itot=itot+1
          yval=real
          if(ist.eq.0)goto 100
          ipo=ipo+1
          if(ipo.eq.1.and.icls.eq.1)then
            oldx=xval
            oldy=yval
            oldl=lyr
          endif
          write(2,10)xval,yval,lyr
         endif
      elseif(inum.eq.70)then
        icls=1
        read(1,*)idum
      else
        read(1,*)idum
      endif
c
      goto 100
c
199   print*,'found End-Of-File before SEQEND'
      print*,'processing section read'
c
200   continue
c
      write(3,'(a40)')filinp
      do i=1,ilyr
         write(3,11)layer(i)
      enddo
c
      print*,'totl number of lines  :',nlin
      print*,'totl number of points :',itot
      print*,'number of layer names :',ilyr
c
      close(1)
      close(2)
      close(3)
c
      end
