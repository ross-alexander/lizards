import java.awt.*;
import java.applet.*;
import images;

/*
struct THex
 {
   byte  hex; // lower 4 bits = hex type (vulcano..etc)
             // bit 7 = visible
   short redLizards;
   short greenLizards;
   short yellowLizards;
   short grayLizards;
   short blackLizards;
 };

class TWorld
 {
   byte width, height;
   short world[];
 };
*/


class LizCanvas extends Panel
{
  static final short HEXID = 0;
  static final short DENID = 1;
  static final short REDID = 2;
  static final short GREENID = 3;
  static final short GRAYID = 4;
  static final short YELLOWID = 5;
  static final short BLACKID = 6;
  static final short SIZEHEX = 7;

  static final short HOMEDENHEX = 0;
  static final short RUINHEX = 1;
  static final short PLAINSHEX = 2;
  static final short SWAMPHEX = 3;
  static final short SCRUBHEX = 4;
  static final short FERTILEHEX = 5;
  static final short PEAKHEX = 6;
  static final short VULCANOHEX = 7;
  static final short TEMPLEHEX = 8;
  static final short CURSEDHEX = 9;
  static final short WATERHEX = 10;
  static final short POOLHEX = 11;
  static final short RAFTHEX = 12;
  static final short BRIDGEHEX = 13;
  static final short EMPTYHEX = 15;

  static final short REDDENHEX = 16;
  static final short GREENDENHEX = 17;
  static final short GRAYDENHEX = 18;
  static final short YELLOWDENHEX = 19;
  static final short BLACKDENHEX = 20;

  static final int w  = 15;
  static final int ww = 30;
  static final int h  = 13;
  static final int hh = 26;

  static final Font normFont = new Font ("Helvetica",Font.PLAIN,11);
  static final Font smallFont = new Font ("Helvetica",Font.PLAIN,9);
 
  Image denImg, ruinImg, swampImg, scrubImg,
        peakImg, vulcanoImg, templeImg, cursedImg,
        poolImg, raftImg, bridgeImg, backImg;

  Dimension size;
  int worldWidth, worldHeight, hHexes, vHexes;
  short worldHexes[];
  short flagPositions[];
  int painting = 0;
  int hexXpoints[] = {-30, -15,  15, 30, 15, -15, -30};
  int hexYpoints[] = {  0, -26, -26,  0, 26,  26,  0};

  Images images;
  boolean imagesLoaded;
  int mapXofs, mapYofs;
  int selectedHexX, selectedHexY;
  Color paintcol;
    Border3D border;

  public LizCanvas(//short aWorldWidth,
                   //short aWorldHeight,
                   //short aWorldHexes[],
                   int aWidth,
                   int aHeight,
                   Images someImages
                   /*
                   Image aBackImg,
                   Image aDenImg, Image aRuinImg,
                   Image aSwampImg, Image aScrubImg,
                   Image aPeakImg, Image aVulcanoImg,
                   Image aTempleImg, Image aCursedImg,
                   Image aPoolImg, Image aRaftImg,
                   Image aBridgeImg
                   */
                   )
   {
  Scrollbar hScroll, vScroll;

        setLayout(null);
		border = new Border3D();
        images = someImages;
        backImg = images.backImg;
        denImg = images.denImg;
        ruinImg = images.ruinImg;
        swampImg =images.swampImg;
        scrubImg   = images.scrubImg;
        peakImg    = images.peakImg;      
        vulcanoImg = images.vulcanoImg;
        templeImg = images.templeImg;
        cursedImg = images.cursedImg;
        poolImg = images.poolImg;
        raftImg = images.raftImg;
        bridgeImg  = images.bridgeImg;
        size = new Dimension(aWidth,aHeight);

        worldWidth = 0;
        hHexes = aWidth / 52;
        vHexes = aHeight / 52;
        imagesLoaded=false;
        selectedHexX=selectedHexY = 0;
        paintcol = new Color(3,3,15);

		flagPositions = new short [32*32];

  hScroll  = new Scrollbar(Scrollbar.HORIZONTAL,0,10000,0,0);
  vScroll  = new Scrollbar(Scrollbar.VERTICAL,0,10000,0,0);
add (hScroll);
add (vScroll);
hScroll.reshape(0,aHeight-16,aWidth-16,16);
vScroll.reshape(aWidth-16,0,16,aHeight-16);
//hScroll.setBackground(new Color(192,192,192));
//vScroll.setBackground(new Color(164,164,164));
//hScroll.show();
//vScroll.show();
   }


  public synchronized void animate()
  {
    int hexX, hexY,xp,yp,hexBase;
    Graphics g;

    g = getGraphics();
    for (hexX=0; hexX < worldWidth; hexX++) // linear faster
     for (hexY=0; hexY < worldHeight; hexY++)
      {
        xp = hexX*3*w+ww + mapXofs;
        yp = hexY*4*h+hh*2 + mapYofs;
        if ((hexX&1)!=0)
           yp -= hh;

        hexBase = (hexX + hexY*worldWidth)*SIZEHEX;
        if ((worldHexes[hexBase] & 0x80) == 0) // not visible
          continue;
		int hexCode = worldHexes[hexBase] & 31;
		if ((hexCode == HOMEDENHEX)||((hexCode >= REDDENHEX)&&(hexCode<=BLACKDENHEX)))
        {
			int flagPos = flagPositions[hexBase];

			Color dencol;
			switch (hexCode)
			{
			case REDDENHEX:
				dencol = Color.red;
				break;
			case GREENDENHEX:
				dencol = Color.green;
				break;
			case GRAYDENHEX:
				dencol = new Color(96,96,96);
				break;
			case YELLOWDENHEX:
				dencol = Color.yellow;
				break;
			case BLACKDENHEX:
				dencol = Color.black;
				break;	
			default:
				dencol = Color.black;
			}
               g.setColor(Color.lightGray);
               g.drawRect(xp+9,yp+5,10,6);
               g.setColor(dencol);
               g.fillRect(xp+5,yp+5,4,6);
               if ((flagPos == 0) || (flagPos == 4))// fix
               {
                 g.fillRect(xp+9,yp+5,10,6);
               }
               /*
              else if (flagPos == 1) // up fix
              {
                 g.fillRect(xp+9,yp+4,5,6);
                 g.fillRect(xp+13,yp+5,5,6);
              }
              else if (flagPos == 2) // up up
              {
                 g.fillRect(xp+9,yp+4,10,6);
              }
              else if (flagPos == 3) //fix up
              {
                 g.fillRect(xp+9,yp+5,5,6);
                 g.fillRect(xp+13,yp+4,5,6);
              }
              */
              else if (flagPos == 1) // down fix
              {
                 g.fillRect(xp+9,yp+6,5,6);
                 g.fillRect(xp+13,yp+5,5,6);
              }
              else if (flagPos == 2) // 6 down down
              {
                 g.fillRect(xp+9,yp+6,10,6);
              }
              else // fix down
              {
                g.fillRect(xp+9,yp+5,5,6);
                g.fillRect(xp+13,yp+6,5,6);
              }                          
              flagPos = (++flagPos) & 3;
			  flagPositions[hexBase] = (short)flagPos;
        }
      }
  }

  public void addHexLizards(int hexX, int hexY, int nbr, int lizColId, int lizTypeId)
  {
    int hexBase, selHexBase;

    if ((hexX >= worldWidth) || (hexY >= worldHeight))
      return;

    hexBase = (hexX + hexY*worldWidth)*SIZEHEX;
         
    if ((worldHexes[hexBase] & 0x80) == 0) // not visible
      return;

	if (lizTypeId ==0)
		worldHexes[hexBase+1] += (short)(nbr);
	else
		worldHexes[hexBase+1+1+lizColId] += (short)(nbr);
  }

  public void removeHexLizards(int hexX, int hexY)
  {
    int hexBase, selHexBase, i;

    if ((hexX >= worldWidth) || (hexY >= worldHeight))
      return;

    hexBase = (hexX + hexY*worldWidth)*SIZEHEX;
         
    if ((worldHexes[hexBase] & 0x80) == 0) // not visible
      return;

    for (i=1;i<7;i++)
      worldHexes[hexBase+i] = 0;
  }

  public void selectHex(int hexX, int hexY, int newIcon)
  {
    int hexBase, selHexBase;

    if ((hexX >= worldWidth) || (hexY >= worldHeight))
      return;

    hexBase = (hexX + hexY*worldWidth)*SIZEHEX;
         
    if ((worldHexes[hexBase] & 0x80) == 0) // not visible
      return;

    selHexBase = (selectedHexX + selectedHexY*worldWidth)*SIZEHEX;
    worldHexes[selHexBase] &= (short)~0x40;
    worldHexes[hexBase] |= 0x40;
    selectedHexX = hexX;
    selectedHexY = hexY;
   
    if ((newIcon >= 0)&&(newIcon<32))
    {
       worldHexes[hexBase] &= (short)~31;
       worldHexes[hexBase] |= (short)newIcon;
    }
   
  }

  public void SetWorld(int aWorldWidth,
                       int aWorldHeight,
                       short aWorldHexes[])
  {
     worldWidth  = aWorldWidth;
     worldHeight = aWorldHeight;
     worldHexes  = aWorldHexes;
     repaint(1);
  }

  public void hexagon(int x0, int y0, int duumy, Graphics g, Color col)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+hexXpoints[i];
         ypoints[i] = y0+hexYpoints[i];
       }
      g.setColor(col);
      g.drawPolygon(xpoints,ypoints,7);
   }

  public void drawHexagonScaled(int x0, int y0, int a, int b, Graphics g, Color col)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+(hexXpoints[i] * a)/b;
         ypoints[i] = y0+(hexYpoints[i] * a)/b;
       }
      g.setColor(col);
      g.drawPolygon(xpoints,ypoints,7);
   }
/*
  public void hexagon3Dlight(int x0, int y0, int r, Graphics g)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+hexXpoints[i];
         ypoints[i] = y0+hexYpoints[i];
       }
      g.setColor(Color.lightGray.brighter());
      g.fillPolygon(xpoints,ypoints,7);
      g.setColor(Color.black);
      g.drawPolygon(xpoints,ypoints,7);
      g.setColor(Color.white);
      g.drawLine(xpoints[1]+1, ypoints[1]+1, xpoints[2]-1, ypoints[2]+1);
   }
*/
  public void hexagon3D(int x0, int y0, int r, Graphics g)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+hexXpoints[i];
         ypoints[i] = y0+hexYpoints[i];
       }
      g.setColor(Color.lightGray);
      g.fillPolygon(xpoints,ypoints,7);
      g.setColor(Color.black);
      g.drawPolygon(xpoints,ypoints,7);
      g.setColor(Color.white);
      g.drawLine(xpoints[1]+1, ypoints[1]+1, xpoints[2]-1, ypoints[2]+1);
   }

   public void homehexagon3D(int x0, int y0, int r, Graphics g)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+hexXpoints[i];
         ypoints[i] = y0+hexYpoints[i];
       }
      g.setColor(new Color(128,128,128));
      g.fillPolygon(xpoints,ypoints,7);
      g.setColor(Color.black);
      g.drawPolygon(xpoints,ypoints,7);
      g.setColor(Color.white);
      g.drawLine(xpoints[1]+1, ypoints[1]+1, xpoints[2]-1, ypoints[2]+1);
   }

  public void fillHexagon(int x0, int y0, int r, Graphics g, Color col)
   {
      int xpoints[] = new int[7];
      int ypoints[] = new int[7];
      int i;
  
      for (i=0;i<7;i++)
       {
         xpoints[i] = x0+hexXpoints[i];
         ypoints[i] = y0+hexYpoints[i];
       }
      g.setColor(col);
      g.fillPolygon(xpoints,ypoints,7);
   }
/*
  int hexXtoScreen(int hx, int ww, int xofs)
  {
    return (hx*3*w+ww-xofs);
  }

  int hexYtoScreen(int hx, int hy, int h, int xofs)
  {
    int yp;

    yp = hy*4*h+hh*2 + yofs;
    if ((hx&1)!=0)
       yp -= hh;
  }
*/

  void paintHex(int hexX, int hexY, Graphics g)
  {
         int xp,yp,hexBase,sum,i,wtotal,pos,wleft,max,x0,y0,w0,h0,w1;
         int maxId, slice, nbrOfMax, lastSliceId, adjust;
         int slices[] = new int[7];
         String s;
         Color colors[] = new Color[7], textcolor;

         colors [0] = Color.black;
         colors [1] = Color.red;
         colors [2] = Color.green;
         colors [3] = Color.lightGray;
         colors [4] = Color.yellow;
         colors [5] = Color.black;

         if ((hexX >= worldWidth) || (hexY >= worldHeight))
           return;

         xp = hexX*3*w+ww + mapXofs;
         yp = hexY*4*h+hh*2 + mapYofs;
         if ((hexX&1)!=0)
            yp -= hh;

         hexBase = (hexX + hexY*worldWidth)*SIZEHEX;

         if ((worldHexes[hexBase] & 0x80) == 0) // not visible
           return;

         textcolor = Color.black;
         switch (worldHexes[hexBase] & 31) // lower 4 bit = hex type
          {
            case HOMEDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(denImg,xp-12,yp-2,this);
		       textcolor = Color.black;
               break;

            case RUINHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(ruinImg,xp-12,yp-2,this);
               break;

            case PLAINSHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,0));
               hexagon(xp,yp,ww,g, Color.black);
	           textcolor = Color.white;
               break;

            case SWAMPHEX:
               fillHexagon(xp,yp,ww,g, new Color(0,128,0));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(swampImg,xp-12,yp-2,this);
               textcolor = Color.white;
               break;

            case SCRUBHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,0));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(scrubImg,xp-12,yp-2, this);
	           textcolor = Color.white;
               break;

            case FERTILEHEX:
               fillHexagon(xp,yp,ww,g, new Color(0,128,0));
               hexagon(xp,yp,ww,g, Color.black);
	           textcolor = Color.white;
               break;

            case PEAKHEX:
//               hexagon3D(xp,yp,ww,g);
               fillHexagon(xp,yp,ww,g, new Color(128,128,0));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(peakImg,xp-12,yp-2, this);
		       textcolor = Color.white;
               break;

            case VULCANOHEX:
//              hexagon3D(xp,yp,ww,g);
               fillHexagon(xp,yp,ww,g, new Color(128,128,0));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(vulcanoImg,xp-12,yp-2, this);
	           textcolor = Color.white;
               break;

            case TEMPLEHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(templeImg,xp-12,yp-2,this);
               break;

            case CURSEDHEX:
//               hexagon3D(xp,yp,ww,g);
               fillHexagon(xp,yp,ww,g, new Color(128,128,128));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(cursedImg,xp-12,yp-2,this);
		       textcolor = Color.white;
               break;

            case WATERHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,255));
               hexagon(xp,yp,ww,g, Color.black);
               break;

            case POOLHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,255));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(poolImg,xp-12,yp-2,this);
               break;

            case RAFTHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,255));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(images.raftImg,xp-12,yp-2,this);
               break;

            case BRIDGEHEX:
               fillHexagon(xp,yp,ww,g, new Color(128,128,255));
               hexagon(xp,yp,ww,g, Color.black);
               g.drawImage(bridgeImg, xp-12,yp-2, this);
               break;

            case REDDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(images.redDenImg,xp-12,yp-2,this);
               break;
            case GREENDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(images.greenDenImg,xp-12,yp-2,this);
               break;
            case GRAYDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(images.grayDenImg,xp-12,yp-2,this);
               break;
            case YELLOWDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(images.yellowDenImg,xp-12,yp-2,this);
               break;
            case BLACKDENHEX:
               hexagon3D(xp,yp,ww,g);
               g.drawImage(images.blackDenImg,xp-12,yp-2,this);
               break;             
            default:
               hexagon(xp,yp,ww,g,Color.black);
               break;
          }

         if ((worldHexes[hexBase] & 0x40) != 0)
            drawHexagonScaled(xp,yp,7,8,g,Color.black);

         for (max=maxId=sum=0,i=2;i<7;i++)
           sum += worldHexes[hexBase+i];

if (sum>0)
{
         g.setColor(Color.black);
         x0=xp-ww+7;
         y0=yp-5;
         w0=2*ww-14;
         h0=4;

         w1 = (sum * 3)/10+2;
         if (w1 < w0)
          {
            x0 += (w0 - w1)>>1;
            w0  = w1;
          }

         wleft = wtotal = w0-1;
         nbrOfMax = 1;
         max = adjust = 0;
         for (lastSliceId=i=2;i<7;i++)
         {
            slice = worldHexes[hexBase+i];
            if (slice == 0)
              continue;   
            lastSliceId = i;
            slice = (slice*wtotal+(sum>>1))/sum;
            if (slice < 2)
                {
                  adjust += 2-slice;
                  slice = 2;
                }
            else
             if (slice > max)
                {
                  max = slice;
                  nbrOfMax = 1;
                }
             else
               if (slice == max)
                   nbrOfMax++;
   
            slices[i-1] = slice;             
          }

         adjust = (adjust+nbrOfMax>>1)/nbrOfMax;
         for (pos=x0+1,i=2;i<7;i++)
          {
             slice = slices[i-1];
             if (slice == 0)
               continue;
             if (slice == max)
               slice -= adjust;
             g.setColor(colors[i-1]);
             if ((slice > wleft)||(i==lastSliceId)) //last
               slice = wleft;
             g.fillRect(pos,y0+1,slice,3);
             pos   += slice;
             wleft -= slice;
          }

         g.setColor(Color.black);
         g.drawRect(x0,y0,w0,h0);
         g.setColor(Color.white);
         g.drawLine(x0+1,y0+h0,x0+w0,y0+h0);
         g.drawLine(x0+w0,y0+h0,x0+w0,y0+1);

//         s = hexX+","+hexY;
         g.setColor(textcolor);
         g.setFont(smallFont);
         g.drawString(""+sum,xp-w+6,y0-3);
}
         if (worldHexes[hexBase+1] > 0)
         {
           g.setColor(textcolor);
           g.setFont(smallFont);
           g.drawString(""+worldHexes[hexBase+1],xp-18,yp+10);
          }
  }

  public synchronized void paintClipped(int xc, int yc, int wc, int hc,Graphics g)
   {
     int x,y,wWorld, hWorld, hexBase;
     int wImg, hImg, rgb;

     size = size();

     if (imagesLoaded == false)
     {
     /*
        g.setColor(Color.lightGray);
        g.drawRect(10,10,20*20,20);
        g.setColor(Color.green);
        g.fillRect(10,10,images.count*20,20);
   */

        paintcol = paintcol.brighter();
        g.setColor(paintcol);
        g.fillRect(0,0,size.width,size.height);

g.setColor(Color.black);
g.drawString(""+images.count,20,40);
        return;
     }

     wImg=hImg=96;
     x=y=0;
     
     while (y<size.height)
      {
         if (((x+wImg >= xc) && (x < xc+wc))
           ||((y+hImg > yc) && (y < yc+hc)))
           g.drawImage(backImg,x,y,this);
         x += wImg;
         if (x>size.width)
         {
            x  = 0;
            y += hImg;
         }
      }
     

     if (worldWidth == 0)
     {
//	    border.paintBottomJoin(g, size);
        g.drawString("No world specified.",100,100);
        return;
     }

     wWorld = (worldWidth > hHexes ? hHexes:worldWidth);
     hWorld = (worldHeight > vHexes ? vHexes:worldHeight);
     mapXofs = (worldWidth >= hHexes ? 0: (size.width-(worldWidth+1)*52)>>1);
//     mapYofs = (worldHeight >= hHexes ? 0: (size.height-(worldHeight+1)*52)>>1);
mapYofs=0;
     for (x=0; x < wWorld; x++)
      for (y=0; y < hWorld; y++)
        paintHex(x,y,g);
//      g.setColor(Color.lightGray);
//      g.drawRect(0,0,size.width-1,size.height-1);
//	 border.paintBottomJoin(g, size);
   }

   public synchronized void paint(Graphics g)
   {
     Dimension size = size();

     paintClipped(0,0,size.width,size.height,g);
   }


   public final synchronized void update (Graphics g)
   {
      paint(g);
   }
/*
   public void repaint(int x, int y, int w, int h)
   {
       paintClipped(x,y,w,h,getGraphics());
   }
*/
  Point screentoHex(int xp,int yp)
  {
    Point point = new Point(0,0);
    int w3 = 3*w;
    int h4 = 4*h;
    int xn, yn, xmod, ymod, hexBase;
    boolean oddX;
    Graphics g;


     g = getGraphics();
     g.setColor(Color.white);
     g.fillRect(0,20,60,20);
     g.fillRect(0,40,60,20);
     g.fillRect(0,0,60,20);
     g.setColor(Color.black);


    xn = xp - mapXofs;
    yn = yp - mapYofs;
    point.x = xn/w3;
    oddX = ((point.x&1) != 0);
    if (oddX)
     {
       point.y = yn/h4;
       ymod = yn % h4;
     }
    else
     {
       point.y = (yn-hh)/h4;
       ymod = (yn-hh) % h4;
     }

    xmod = xn % w3;
    
    if (xmod < w)    // left of hex
      if (ymod < hh) // top half of hex
      {
         if  (xmod < ((hh-ymod)*w)/hh)
         {
            point.x--;
            if (oddX)
              point.y--;
            g.drawString("x--, y--",0,20);
         }
       }
      else // bottom half of hex
       {
         if(xmod < ((ymod - hh)*w)/hh)
           point.x--;
         if (!oddX)
           point.y++;           
         g.drawString("x--",0,20);
       }

     g.drawString(""+point.x+","+point.y+" / "
     +(xn%(3*w))+","+(yn%(4*h)),0,40);
     g.drawString(""+xmod+","+ymod+" / "+((ymod*w)/h),0,60);

    if ((point.x >= worldWidth)||(point.y >= worldHeight)||
       (point.x < 0)||(point.y<0))
     {
       point.x=-1;
       return point;
     }
    hexBase = (point.x + point.y*worldWidth)*SIZEHEX;
    if ((worldHexes[hexBase] & 0x80) == 0)
      point.x = -1;

    return point;
  }


  int translateButton(int btn)
  {
    switch(btn)
    {
      case 1: return HOMEDENHEX;
      case 2: return REDDENHEX;
      case 3: return GREENDENHEX;
      case 4: return GRAYDENHEX;
      case 5: return YELLOWDENHEX;
      case 6: return BLACKDENHEX;
      case 7: return RUINHEX;
      case 8: return TEMPLEHEX;

      case 10: return PLAINSHEX;
      case 11: return SWAMPHEX;
      case 12: return SCRUBHEX;
      case 13: return FERTILEHEX;
      case 14: return PEAKHEX;
      case 15: return VULCANOHEX;
      case 16: return CURSEDHEX;
      case 17: return POOLHEX;
      case 18: return WATERHEX;
      default:
        return -1;
    }
  }

   public void mouseClick(int x, int y, int btn, int nbr, int lizColId, int lizTypeId)
   {
     Point point;
     int oldSelX,oldselY;
     Graphics g;
     int newIcon;

     point = screentoHex(x, y);
     if(point.x < 0)
       return;

     newIcon = translateButton(btn);

     oldSelX = selectedHexX;
     oldselY = selectedHexY;
     selectHex(point.x,point.y,newIcon);
     if (btn == 9)
      {
         addHexLizards(point.x,point.y,nbr,lizColId, lizTypeId);
      }
     else
      if (btn==19)
        removeHexLizards(point.x,point.y);

     paintHex(oldSelX,oldselY,g=getGraphics());
     paintHex(selectedHexX,selectedHexY,g);
   }
}


