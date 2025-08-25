c
c     ----------------------------------------------------------------
c
      subroutine plots(x,y,iroute)
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      character*50 proute
c
      if(iplot.le.0) write(19,5) -1,ibcol,ifcol
5     format(i2/2i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine erase
c
c     erase the screen
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) -2
5     format(i2)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine plot(x,y,ipen)
c
c     move to the point (x,y); pen up if ipen=3, pen down if ipen=2
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) 1,x,y,ipen
5     format(i2/2e15.5,i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine number(x,y,ht,xnum,ang,ndeci)
c
c     plot the floating point number 
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if (iplot.le.0) write(19,5) 2,x,y,ht,xnum,ang,ndeci
5     format(i2/5e15.5,i10) 
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine symbol(x,y,ht,label,ang,nchar)
c
c     plot the character string label; special symbols can be plotted
c     with the routine ssymbol
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      character label(nchar),cn*2,form*20
c
      if(iplot.le.0) then
        write (cn,'(i2)') nchar
        form='(i2/i10/4e15.5,'//cn//'a1)'
        write(19,form) 3,nchar,x,y,ht,ang,(label(i),i=1,nchar)
      endif
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine line(x,y,npts)
c
c     connect the (x,y) points with straight line segments
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      real x(npts),y(npts)
c
      if(iplot.le.0) then
        write(19,5) 9,npts
        write(19,15) (x(i),y(i),i=1,npts)
5       format(i2/i10)
15      format(2e15.5)
      end if
c
      return
      end  
c
c     ----------------------------------------------------------------
c
      subroutine empty
c
c     flush the graphics buffer
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) 4
5     format(i2)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine dot(x,y,size,icol)
c
c     plot a dot centred at (x,y) of size isize pixels or mm and colour
c     icol
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      if(iplot.le.0) write(19,5) 10,x,y,size,icol
5     format(i2/3e15.5,i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine pcolor(icol)
c
c     set the colour for polylines
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) 7,icol
5     format(i2/i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine segmnt(iflag)
c
c     open and close Uniras segments
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) 8,iflag
5     format(i2/i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c                 
      subroutine aldone
c
c     wait unitl the user is ready for the next plot 
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      character reply*1
c
      if(iplot.le.0) write(19,5) 5
5     format(i2)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine plotnd
c
c     terminate all Uniras plotting
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
      if(iplot.le.0) write(19,5) 6
5     format(i2)
c
      return
      end
