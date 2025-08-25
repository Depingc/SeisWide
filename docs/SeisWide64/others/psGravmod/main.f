c                 
c     --------------------  g r a v m o d  ---------------------------
c                 
c     forward gravity modelling program
c                 
c     this program calculates the gravity anomaly response of a model
c     composed of a number of constant density contrast polygons and
c     creates model and gravity anomaly plots.
c                 
c                 
c     run sequence:
c                 
c       $r gravmod.o 10=parameters 11=out 12=dump 13=model 15=obdata
c                 
c     the format of some input parameters (unit 10):
c                 
c       &grvpar namelist:
c                 
c     first  = initial gravity station on x-axis (km)
c     last   = last gravity station on x-axis (km)
c     spacng = distance between stations (km)
c     polys  = number of polygons in model
c     blank  = number of polygons (from beginning)
c              that should not be plotted 
c                 
c     the rest of the polygon data (unit 10) for each 
c     polygon is written in the following format:
c                 
c     sigma = density contrast (grams/cc) -- format f7.4
c     n     = # of coordinates in polygon -- format i5
c     r(i)  = x-axis coordinate (km) ------- format f9.3  note: it was f8.3....deping
c     z(i)  = z-axis coordinate (km) ------- format f8.3  note: it was f8.3....deping
c                 
c     note:       
c                 
c     the density contrast for each polygon is the density difference
c     between the region outside the whole model and the density of
c     each polygon
c                 
c                 
c     --------------------------------------------------------------
c      npoly: max number of computed point.
c	 nobs: max number of observed point.
c	 nppoly: max number of points in the gravity model.
      parameter(nppoly=401, npoly=2000, nobs=4000)
      real x(nppoly),z(nppoly),theta(nppoly),phi(nppoly),a(nppoly),
     +     gz(nppoly),r(nppoly),xx(nppoly),zz(nppoly),
     +     first,last,spacng,dist(npoly),anom(npoly,npoly),av(npoly),
     +     px(npoly),pg(npoly),sigma(npoly),xmid(npoly),zmid(npoly),
     +     sigmao(npoly),spos(nobs),sdat(nobs),scpos(nobs),
     +     scdat(nobs),numht
      integer statn,polys,block,gr(npoly),blank,pomit(npoly),
     +        pincl(npoly)
c
      common /cplot/ iplot,isep,iseg,nseg,xwndow,ywndow,ibcol,ifcol,sf
c                 
      namelist /pltpar/ symht,imod,igrav,ilab,idata,idump,icol,
     +                  blank,isymb,dsmax,xwndow,ywndow,iroute,sf,
     +                  xsdata,numht,irmdc,iseg,isigma0,nsmth,nevery
c
      namelist /axepar/ xmin,xmax,xmm,ntickx,ndecix,xtmin,xtmax,
     +                  zmin,zmax,zmm,ntickz,ndeciz,ztmin,ztmax,
     +                  gmin,gmax,gmm,ntickg,ndecig,gtmin,gtmax,
     +                  albht,sep,xorig,yorig,iaxlab
c                 
      namelist /grvpar/ first,last,spacng,polys,den0,pomit,pincl
c                 
      data pomit/npoly*-1/,pincl/npoly*-1/
c                 
c     default parameter values
c                 
      icol=1
      iaxlab=1
      nsmth=0
      nevery=1
      isigma0=0
      irmdc=1
      numht=2.
      xsdata=0.
      sep=10.
      xorig=15.
      yorig=15.
      iplot=1
      iseg=0
      nseg=0
      ibcol=0
      ifcol=1
      sf=1.2
      xwndow=290.
      ywndow=180.
      iroute=1
      dsmax=.05   
      xmin=0.     
      xmax=300.   
      xmm=250.   
      ntickx=-1
      ndecix=-2
      xtmin=-999999.
      xtmax=-999999.
      zmin=0.     
      zmax=50.    
      zmm=75.    
      ntickz=-1
      ndeciz=-2
      ztmin=-999999.
      ztmax=-999999.
      gmin=-100.  
      gmax=100.   
      gmm=75.    
      ntickg=-1
      ndecig=-2
      gtmin=-999999.
      gtmax=-999999.
      albht=2.5   
      symht=2.    
      imod=1      
      igrav=1     
      ilab=0      
      idata=0     
      idump=0     
      blank=0     
      den0=2.92   
      first=0.    
      last=300.   
      spacng=20.  
      polys=1     
      isymb=1     
c
      open(10, file='g.in', status='old')
      open(11, file='g.out')
      open(13, file='g.mod', status='old')
      open(19, file='g.eps', status='unknown')
c                 
      read(10,pltpar)
      read(10,axepar)
      read(10,grvpar)
c
      if(idata.eq.1) open(15, file='g.obs', status='old')
      if(idump.eq.1) open(12, file='g.dump')
c                 
      xscale=(xmax-xmin)/xmm
      zscale=-(zmax-zmin)/zmm
      gscale=(gmax-gmin)/gmm
c                 
      ndata=0     
      if(idata.eq.1) then
111     read(15,*,end=999) sposr,sdatr
        ndata=ndata+1
        spos(ndata)=sposr-xsdata 
        sdat(ndata)=sdatr
        go to 111 
      end if      
c                 
999   if(imod.gt.0.or.igrav.gt.0) then
        call plots(xwndow,ywndow,iroute)
        call segmnt(1)
      end if
c
      if(imod.gt.0) then
        zos=yorig
c        if(igrav.eq.2) zos=zos+gmm+sep
c        call axtick(xmin,xmax,xtmin,xtmax,ntickx,ndecix)
c        call axis(xorig,zos+zmm,xmin,xmax,xmm,xscale,0.,-1,
c     +       xtmin,xtmax,ntickx,ndecix,'DISTANCE (km)',13,albht)
c        call axtick(zmin,zmax,ztmin,ztmax,ntickz,ndeciz)
c        call axis(xorig,zos,zmin,zmax,zmm,zscale,90.,1,
c     +       ztmin,ztmax,ntickz,ndeciz,'DEPTH (km)',10,albht)
      end if      
c                 
      i0=0
c
120   continue
c
      nstat=(last-first)/spacng+1 
      do 10 j=1,nstat 
         dist(j)=first+(j-1)*spacng
         px(j)=(dist(j)-xmin)/xscale+xorig
         do 20 i=1,polys
            anom(i,j)=0.
20       continue 
10    continue    
c                 
      incl=0      
      if(pincl(1).lt.0) incl=1
      do 70 i=1,polys 
         gr(i)=incl 
70    continue    
      do 80 i=1,npoly
         if(pincl(i).lt.0) go to 90
         gr(pincl(i))=1
80    continue    
90    do 110 i=1,npoly
         if(pomit(i).lt.0) go to 1200
         gr(pomit(i))=0
110   continue    
c                 
c                write(23,1815) 4,den,den,xl,z11,xri,z12,xri,z22,xl,z21
c 1815            format(i5,2f7.4/10f8.3)

1200  do 1000 block=1,polys
c                 
         if(idump.eq.1) write(12,5)
5        format('-',13x,'station',8x,'anomaly'/
     +          ' ',12x,'distance',8x,'in mgal')
         read(13,15, end=901) layer,n,sigma(block),sigmao(block)
		if(n>nppoly) stop
15       format(i2,i3,2f7.4)

c	numLines = n*0.1 + 0.99
c	do 116 ii=1, numLines
c		if (ii .eq. numLines) then
c			i2 = n*0.1
c			i2 = n-i2
c		else
c			i2 = 10
c		endif
c         read(13,115) ( r(i+(ii-1)*10),z(i),i=1,i2 )
         read(13,115) ( r(i),z(i),i=1,n )
116   continue
115      format(10f9.3)
         if(i0.eq.0) then
           sigma(block)=sigma(block)-den0
         else
           sigma(block)=sigmao(block)-den0
         end if
         sigmao(block)=sigmao(block)-den0
c                 
         sumx=0.  
         sumz=0.  
         do 30 i=1,n
            sumx=sumx+r(i)
            sumz=sumz+z(i)
30       continue 
         xmid(block)=sumx/float(n)
         zmid(block)=sumz/float(n)
c                 
         m=n+1    
         r(m)=r(1) 
         z(m)=z(1) 
c
         if(i0.eq.1) go to 9000
c
         if(imod.gt.0.and.block.gt.blank) then
           do 50 i=1,m
              xx(i)=(r(i)-xmin)/xscale+xorig
              zz(i)=(z(i)-zmax)/zscale+zos
50         continue
           call line(xx,zz,m)
           if(imod.eq.2) then
             do 51 i=1,m
                xx(i)=xx(i)*sf
                zz(i)=zz(i)*sf
51           continue
             anum=sigma(block)-sigmao(block)
             ircol=3
             if(anum.gt.0.) ircol=2
             if(anum.lt.0.) ircol=4
c            if(icol.eq.1) call rsurf(xx,zz,m,ircol,-1.)
           end if
           if(ilab.gt.0) then
             if(ilab.eq.1) then 
               anum=sigma(block)+den0
               ndeci=2
               ip=1 
             else 
               if(ilab.eq.2) then
                 if(sigma(block).ne.sigmao(block)) then 
                   anum=sigma(block)-sigmao(block) 
                   if(abs(anum).gt.dsmax) write(6,255) block,anum
255                format('***  block: ',i3,' delta sigma: ',f10.3)
                   ndeci=3 
                   ip=1 
                 else
                   ip=0
                 end if
               else 
                 anum=float(block)
                 ndeci=-1
                 ip=1
               end if
             end if
             xp=(xmid(block)-xmin)/xscale+xorig-1.5*symht
             zp=(zmid(block)-zmax)/zscale+zos-symht/2. 
             if(ip.eq.1) call number(xp,zp,numht,anum,0.,ndeci)
           end if 
         end if   
c                 
9000     if(gr(block).eq.0) go to 1000
c                 
         do 2000 statn=1,nstat 
            do 60 j=1,m 
               x(j)=r(j)-dist(statn) 
60          continue 
c                 
            do 3000 i=1,n 
c                 
               if(z(i).eq.z(i+1)) go to 100 
               if(x(i).eq.x(i+1)) go to 200 
               if(x(i).eq.z(i).and.x(i).eq.0.) go to 3000
               if(x(i+1).eq.z(i+1).and.x(i+1).eq.0.) go to 3000
               if(x(i).eq.0.) go to 300 
               if(x(i+1).eq.0.) go to 400 
c                 
               theta(i)=atan2(z(i),x(i)) 
               theta(i+1)=atan2(z(i+1),x(i+1)) 
               if(theta(i+1).eq.theta(i)) go to 3000 
               phi(i)=atan2((z(i+1)-z(i)),(x(i+1)-x(i))) 
               a(i)=x(i+1)+z(i+1)*((x(i+1)-x(i))/(z(i)-z(i+1))) 
               term=tan(phi(i))*alog(cos(theta(i))*(tan(theta(i))-
     +              tan(phi(i)))/(cos(theta(i+1))*(tan(theta(i+1))-
     +              tan(phi(i))))) 
               gz(i)=a(i)*sin(phi(i))*cos(phi(i))*(theta(i)-
     +               theta(i+1)+term) 
               go to 900 
c                 
100            if(z(i).eq.0.) go to 3000
               if(x(i+1).eq.0.) go to 101 
               theta(i+1)=atan2(z(i+1),x(i+1)) 
               go to 102 
101            theta(i+1)=1.570796 
102            if(x(i).eq.0.) go to 103 
               theta(i)=atan2(z(i),x(i)) 
               go to 104 
103            theta(i)=1.570796 
104            gz(i)=z(i)*(theta(i+1)-theta(i)) 
               go to 900 
c                 
200            if(x(i).eq.0.) go to 3000
               theta(i)=atan2(z(i),x(i)) 
               theta(i+1)=atan2(z(i+1),x(i+1)) 
               gz(i)=x(i)*alog(cos(theta(i))/cos(theta(i+1))) 
               go to 900 
c                 
300            theta(i)=1.570796 
               theta(i+1)=atan2(z(i+1),x(i+1)) 
               phi(i)=atan2((z(i+1)-z(i)),(x(i+1)-x(i))) 
               a(i)=x(i+1)+z(i+1)*((x(i+1)-x(i))/(z(i)-z(i+1))) 
               term=tan(phi(i))*alog(cos(theta(i+1))*
     +              (tan(theta(i+1))-tan(phi(i)))) 
               gz(i)=-a(i)*sin(phi(i))*cos(phi(i))*(theta(i+1)-
     +               1.570796+term) 
               go to 900 
c                 
400            theta(i+1)=1.570796 
               theta(i)=atan2(z(i),x(i)) 
               a(i)=x(i+1)+z(i+1)*((x(i+1)-x(i))/(z(i)-z(i+1))) 
               phi(i)=atan2((z(i+1)-z(i)),(x(i+1)-x(i))) 
               term=tan(phi(i))*alog(cos(theta(i))*
     +              (tan(theta(i))-tan(phi(i)))) 
               gz(i)=a(i)*sin(phi(i))*cos(phi(i))*(theta(i)-
     +               1.570796+term) 
900            anom(block,statn)=anom(block,statn)+
     +                           13.34*sigma(block)*gz(i)
3000        continue 
            if(idump.eq.1) write(12,25) dist(statn),anom(block,statn) 
25          format(' ',11x,f8.3,8x,e11.5)
2000     continue 
c                 
         if(idump.eq.1) then
           write(12,35) block,(r(i),z(i),i=1,n)
35         format('0','for block',i3,' the polygon coordinates are:'/
     +            ' ',5x,'distance',8x,'depth'/(' ',7x,f9.3,3x,f8.3))
           write(12,45) sigma(block)
45         format(' ','the density contrast is',f7.4,' gm/cc') 
         end if   
1000  continue    
c
901	polys = block-1
      if(i0.eq.1) go to 8000
c                 
      if(imod.gt.0) then
        call empty
        if(igrav.eq.1) then
          call aldone 
          call erase
        end if
      end if
c                 
      if(igrav.gt.0) then
        call axis(xorig,yorig,xmin,xmax,xmm,xscale,0.,1,
     +       xtmin,xtmax,ntickx,ndecix,'DISTANCE (km)',13,albht)
        call axtick(gmin,gmax,gtmin,gtmax,ntickg,ndecig)
        call axis(xorig,yorig,gmin,gmax,gmm,gscale,90.,1,
     +       gtmin,gtmax,ntickg,ndecig,'ANOMALY (mgal)',
     +       14,albht)
        call box(xorig,yorig,xorig+xmm,yorig+gmm)
      end if      
c                 
8000  continue
      do 130 statn=1,nstat
         av(statn)=0.
         do 140 block=1,polys
            av(statn)=av(statn)+anom(block,statn)
140      continue 
130   continue    
c                 
      if(idata.eq.1) then 
        sum=0.    
        ncstat=0  
        do 180 i=1,ndata
           do 190 j=1,nstat-1
              if(spos(i).ge.dist(j).and.spos(i).le.dist(j+1)) then
                ncstat=ncstat+1
                if(ncstat.eq.1) nstrt=i
                scpos(ncstat)=spos(i)
                scdat(ncstat)=(av(j+1)-av(j))*(spos(i)-dist(j))/
     +                        (dist(j+1)-dist(j))+av(j) 
                sum=sum+scdat(ncstat)-sdat(i) 
                go to 180
              end if 
190        continue
180     continue  
        ave=sum/float(ncstat) 
        if(irmdc.eq.1) then
          do 210 i=1,ncstat
             scdat(i)=scdat(i)-ave
210       continue  
        end if
c       write(6,105) ave
        write(11,105) ave
105     format(/'dc between calculated and observed response: ',
     +         f8.3,' mgal'/) 
      end if      
c                 
      if(idump.eq.1) then
        write(12,55) 
55      format('-','average anomaly')
        write(12,5)
        write(12,25) (dist(statn),av(statn),statn=1,nstat)
        write(11,65)
65      format('      x         z       density density-den0   incl'/)
        do 150 block=1,polys
           write(11,75) xmid(block),zmid(block),sigma(block)+den0,
     +                  sigma(block),gr(block)
75         format(4f10.3,i10)
150     continue  
      end if      
c                 
      if(idata.ne.1) then
        write(11,85)
85      format(/'  distance  calc grav anom'/)
        write(11,95) (dist(i),av(i),i=1,nstat)
95      format(2f10.3)
      else        
        write(11,185) 
185     format(/' distance    obs   calc grav anom  diff'/)
        dmax=0.   
        do 270 i=1,ncstat
           diff=scdat(i)-sdat(nstrt+i-1)
           if(abs(diff).gt.dmax) dmax=abs(diff)
           write(11,195) scpos(i),sdat(nstrt+i-1),scdat(i),diff
195        format(4f10.3) 
270     continue  
        write(6,275) dmax 
        write(11,275) dmax
275     format(/'maximum difference: ',f10.3,' mgal'/)
      end if      
c                 
	a1 = 1./xscale
	a2 = 1./gscale
      if(igrav.gt.0) then
        if(idata.ne.1) then 
          pg(1)=(av(1)-gmin)/gscale+yorig
          call plot(px(1),pg(1),3)
          do 160 i=2,nstat
             pg(i)=(av(i)-gmin)/gscale+yorig
             call plot(px(i),pg(i),2)
160       continue 
        else      
          call pcolor(2)
          do 220 i=1,ncstat
             pg(i)=(scdat(i)-gmin)*a2+yorig
             px(i)=(scpos(i)-xmin)*a1+xorig
220       continue 
          if(nsmth.gt.0) then
            do 2210 i=1,nsmth
               call smooth(pg,ncstat)
2210        continue
          end if
          if(i0.eq.0) then
            call line(px,pg,ncstat)
          else
            dash=xmm/200.
            call dashln(px,pg,ncstat,dash)
          end if
          call pcolor(1)
c                 
          if(i0.eq.1) go to 7000
c
          if(nsmth.gt.0) then
            do 2220 i=1,nsmth
               call smooth(sdat,ndata)
2220        continue
          end if
          do 170 i=1,ndata,nevery
             sppos=(spos(i)-xmin)*a1+xorig
             spdat=(sdat(i)-gmin)*a2+yorig
             call ssymbl(sppos,spdat,symht,isymb)
170       continue
        end if    
c   
7000    call empty
c                 
      end if      
c
      if(isigma0.eq.1.and.i0.eq.0) then
        i0=1
        igrav=1
        idata=1
        rewind(13)
        go to 120
      end if
c                 
      if(imod.gt.0.or.igrav.gt.0) then
        call aldone
        call plotnd(1)
      end if
c
c	pause
      stop        
      end         
c
c     ----------------------------------------------------------------
c
      subroutine smooth(x,n)
c
c     three point triangular smoothing filter
c
      real x(n)  
      m=n-1      
      a=0.77*x(1)+0.23*x(2)
      b=0.77*x(n)+0.23*x(m)
      xx=x(1)
      xr=x(2)
      do 10 i=2,m
         xl=xx
         xx=xr
         xr=x(i+1)
         x(i)=0.54*xx+0.23*(xl+xr)
 10   continue   
      x(1)=a     
      x(n)=b     
      return     
      end
