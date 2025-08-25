      program dxfread_pts
c************************************************************
c
c     written by : Gordon Oakey
c                  
c     This program reads the 'AutoCAD' transfer file (.dxf)
c     and extracts the x-y-Z coordinates for POINTS
c
c     Output file contains : X,Y,Z
c
c************************************************************
c
      character*40 filinp,filout
      character*20 string
      real*8 xval,yval
c
      print*,'give name of input file'
      read(*,'(a)')filinp
      print*,'give name of output file'
      read(*,'(a)')filout
c
      open(1,file=filinp,status='old')
      open(2,file=filout,status='new')
c
      print*,'processing file : ',filinp
c
c ** skip until find points
c
80    read(1,'(a)',end=100)string   
c
      if(string.eq.'POINT')then
        read(1,'(a)',end=100)string
        read(1,'(a)',end=100)string
        read(1,'(a)',end=100)string
        read(1,'(a)',end=100)string
        read(1,'(a)',end=100)string
        read(1,*)xval
        read(1,'(a)',end=100)string
        read(1,*)yval
        read(1,'(a)',end=100)string
        read(1,*)zval
        write(2,*)xval,yval,zval
      endif
c      
      goto 80
c
100   close(1)
      close(2)
c
      end
