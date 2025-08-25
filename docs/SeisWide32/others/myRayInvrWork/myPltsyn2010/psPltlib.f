c
c     version 1.0  June 1992
c
c     ----------------------------------------------------------------
c     |                                                              |
c     |           ***********  X P L T L I B  ***********            |  
c     |                                                              |
c     |         Plot library to convert Calcomp-like calls           |   
c     |                     to xbuplot calls                         |
c     |                                                              |
c     |                   Written by C. A. Zelt                      |
c     |           Modified from pltlib by T. J. Cote (GSC)           |
c     |     Postscript added by D. Demanet (Liege Univ, Belgium)     |
c     |                                                              |
c     |                Geological Survey of Canada                   |   
c     |                  Ottawa, Canada K1A 0Y3                      |
c     |                                                              |
c     ----------------------------------------------------------------
c
c     The routine pcolor is required only if the local graphics allows
c     colour plotting. 
c
c     ----------------------------------------------------------------
c
      subroutine plots (x, y, iroute)
c
c     initialize the plot; this must be the first plot call
c
c     iroute = 1 plots to the screen
c              2 creates a postscript file
c              3 creates a uniras file for the VERSATEC plotter
c              4 creates a postscript file for the colour plotter
c              5 creates a legal size postscript file
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
      common /cps/ ipscol,xcurr,ycurr
c
      integer fill
      real x0, y0
c
	  ipscol=1
c       else
c         ipscol=0
c       endif
	write(19,*) '%! xpltlib'

ccz	write(19,*) 'initgraphics'
	write(19,*) '/initxplt'
	write(19,*) '{90 rotate'
	write(19,*) '50 -600 translate'
	write(19,*) '2.834 2.834 scale'
	write(19,*) '0.0 setlinewidth'
	write(19,*) '/Helvetica findfont 12 scalefont setfont'
	write(19,*) '} def'
	write(19,*) 'initxplt'
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
      integer fill
      real x0, y0
c 
	write(19,*) 'erasepage'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine plot (x, y, ipen)
c
c     move to the point (x,y); pen up if ipen=3, pen down if ipen=2
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
      common /cps/ ipscol,xcurr,ycurr
c
      integer pen
c
      pen = 3 - ipen 
	if (ipen .eq. 3) then
          write (19,*)  x, y, ' moveto'
	else
          write (19,*)  xcurr, ycurr, ' moveto'
          write (19,*)  x, y, ' lineto'
          write (19,*)  'stroke'
	endif
      xcurr=x
      ycurr=y
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine fill (x, y, npts)
c
c     connect the (x,y) points with filled area. designed by deping C, Mar 2008
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      real x (npts),y (npts)
c
        write (19,*) 'newpath'
        write (19,*) x(1), y(1), ' moveto'
	do 5 i=2,npts
  5       write (19,*) x(i), y(i), ' lineto'
        write (19,*) ' closepath fill'
c
      return
      end  
c
c
c     ----------------------------------------------------------------
c
      subroutine airvar (sp, tp, jstart, jend, tmean)
c
c     fill the sp and tp arrays with filled area. designed by deping C, Mar 2008
c     *
c     ****
c     ******
c     ****
c     **
c    -
c  ---
c  -
c
      real xfill(9999),yfill(9999)
      real sp(1),tp(1)
c
c.... This code is for fill area, added by Deping C.  Mar 2008###################
            ind = 0
            do 622 j=jstart,jend
            aj0 = sp(j-1) - tmean
            aj1 = sp(j) - tmean
            if (aj1.gt.0  .and.  aj0.gt.0) then 
                ind = ind + 1
                xfill(ind) = sp(j)
                yfill(ind) = tp(j)
            else if (aj1.gt.0  .and.  aj0.le.0) then 
                ind = ind + 1
                xfill(ind) = tmean
                a = sp(j)-sp(j-1)
                if (abs(a) .lt. 0.0000001) then 
                    yfill(ind) = tp(j-1)
                else
                    yfill(ind) = tp(j-1) + (tp(j) - tp(j-1))*
     +                           (tmean - sp(j-1))/a
                end if
                if (yfill(ind) .lt. tp(j-1)) yfill(ind) = tp(j-1)
                ind = ind + 1
                xfill(ind) = sp(j)
                yfill(ind) = tp(j)
            else if (aj1.eq.0  .and.  aj0.gt.0) then 
                ind = ind + 1
                xfill(ind) = sp(j)
                yfill(ind) = tp(j)
                ind = ind + 1
                xfill(ind) = tmean
                yfill(ind) = tp(j)
                if (ind .gt. 9999) then
                    print *, "Too many points exceeding 9999"
                    return
                endif
                if (ind .gt. 2) then
                   call fill(xfill, yfill, ind)
                end if
                ind = 0
            else if (aj1.lt.0  .and.  aj0.ge.0) then 
                ind = ind + 1
                xfill(ind) = tmean
                a = sp(j)-sp(j-1)
                if (abs(a) .lt. 0.0000001) then 
                    yfill(ind) = tp(j-1)
                else
                    yfill(ind) = tp(j-1) + (tp(j) - tp(j-1))*
     +                           (tmean - sp(j-1))/a
                end if
                if (yfill(ind) .gt. tp(j)) yfill(ind) = tp(j)
                if (ind .gt. 9999) then
                    print *, "Too many points exceeding 9999"
                    return
                endif
                if (ind .gt. 2) then
                   call fill(xfill, yfill, ind)
                end if
                ind = 0
            else
                ind = 0
            end if
622         continue 
c..... end of fill area coding#############################
cc
      return
      end  
c
c
c     ----------------------------------------------------------------
c
      subroutine number (x, y, ht, xnum, ang, ndeci)
c
c     plot the floating point number 
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
      character*30 buffor
      integer lon
c 
		write (19,*) x, y, ' moveto'
		write (19,*) 'gsave'
		write (19,*) ang, ' rotate'
		write (19,*) '/Helvetica findfont ',ht,' scalefont setfont'
		if (xnum .eq. 0.) then
			lon=1
		else
			if (xnum .gt. 0.) then
				lon=int(log10(abs(xnum)))+1
			else
				lon=int(log10(abs(xnum)))+2
			endif
		endif
		if (ndeci .lt. 0) then
			write (buffor,6) '(a1,i',lon,',a6)'
6			format(a5,i2,a4)
			write(19,buffor) '(',int(xnum),') show'
		else
			lon=lon+ndeci+2
			write (buffor,5) '(a1,f',lon,'.',ndeci,',a6)'
5			format(a5,i2,a1,i2,a4)
			write(19,buffor) '(',xnum,') show'
		endif
		write (19,*) 'grestore'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine symbol (x, y, ht, label, ang, nchar)
c
c     plot the character string label
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      character label(nchar)
      character*30 buffor
c

        write (19,*) x, y, ' moveto'
        write (19,*) 'gsave'
        write (19,*) ang, ' rotate'
        write (19,*) '/Helvetica findfont ',ht,' scalefont setfont'
	write (buffor,5) '(a1,',nchar,'a1,a6)'
  5     format(a4,i3,a6)
	write(19,buffor) '(',(label(i),i=1,nchar),') show'
        write (19,*) 'grestore'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine line (x, y, npts)
c
c     connect the (x,y) points with straight line segments
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      real x (npts),y (npts)
c
        write (19,*) 'newpath'
        write (19,*) x(1), y(1), ' moveto'
	do 5 i=2,npts
  5       write (19,*) x(i), y(i), ' lineto'
        write (19,*) 'stroke'
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
        write (19,*) '% flushbuffer'
c
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine dot (x, y, size, icol)
c
c     plot a dot centred at (x,y) of size isize pixels or mm and colour
c     icol
c
      integer fill_flag
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
	write(19,*) 'gsave'
	call pcolor(icol)
	write(19,*) 'newpath'
	write(19,*) x, y, size/2, ' 0 360 arc fill'
	write(19,*) 'grestore'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine pcolor (icol)
c
c     set the colour for polylines
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
      common /cps/ ipscol,xcurr,ycurr
c 
	if (ipscol .eq. 1) then
	  if (icol.eq.0) then
	    write(19,*) '1 setgray'
          elseif (icol .eq. 1) then
	    write(19,*) '0 setgray'
          elseif (icol .eq. 2) then
	    write(19,*) '1 0 0 setrgbcolor'
          elseif (icol .eq. 3) then
	    write(19,*) '0 1 0 setrgbcolor'
          elseif (icol .eq. 4) then
	    write(19,*) '0 0 1 setrgbcolor'
          elseif (icol .eq. 5) then
	    write(19,*) '1 1 0 setrgbcolor'
          elseif (icol .eq. 6) then
	    write(19,*) '1 0 1 setrgbcolor'
          elseif (icol .eq. 7) then
	    write(19,*) '0 1 1 setrgbcolor'
          elseif (icol .eq. 8) then
	    write(19,*) '1 0.5 0 setrgbcolor'
          elseif (icol .eq. 9) then
	    write(19,*) '1 0 0.5 setrgbcolor'
          else
	    write(19,*) '0 setgray'
	  endif
	else
	  if (icol.eq.0) then
	    write(19,*) '1 setgray'
          elseif (icol .eq. 1) then
	    write(19,*) '0 setgray'
          else
	    write(19,*) real(icol)/10., ' setgray'
	  endif
	endif
5     format (i2/i10)
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine segmnt (nsegd)
c
c     open and close Uniras segments
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c 
	write(19,*) '% segment'
c
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
	write(19,*) 'showpage'
	write(19,*) 'initxplt'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine plotnd(iflag)
c
c     terminate all Uniras plotting
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      character reply*1
c
	write(19,*) 'showpage'
c
      return
      end
c
c     ----------------------------------------------------------------
c
      subroutine get_event (x, y, button, key)
c
c     get a key or a button press event
c     returns x, y of cursor. button and key pressed
c
ccc
      integer button
      character*1 key
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c
      return
      end
