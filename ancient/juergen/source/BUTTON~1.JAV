import java.awt.*;
import java.applet.*;
//import Clock2;
import images;

class ImageButton extends Canvas
{
  Image img;
  String text;
  Color color;
  boolean selected;
  int xp, yp;
  final Font font = new Font ("Helvetica",Font.PLAIN,8);
  int fontHeight;

  public ImageButton(Image aImg, Color aColor, String aText)
   {
     img = aImg;

     color = aColor;
     xp = 0;
     yp = 0;
     selected = false;
     text = aText;
     reshape(0,0,30,30);
   }

  public ImageButton(Image aImg, String aText)
   {
     img = aImg;
     color = Color.lightGray;
     xp = 0;
     yp = 0;
     selected = false;
     text = aText;
     reshape(0,0,30,30);
   }

  void drawButtonShadow(Graphics g)
   {
     g.setColor(Color.black);
     g.drawLine(xp+29,yp,xp+29,yp+29);  // |
     g.drawLine(xp,yp+29,xp+29,yp+29);    // _
     g.setColor(Color.darkGray);
     g.drawLine(xp+28,yp+1,xp+28,yp+28);  // |
     g.drawLine(xp+1,yp+28,xp+28,yp+28);  // _

     g.setColor(Color.white);
     g.drawLine(xp,yp,xp+28,yp);         
     g.drawLine(xp,yp,xp,yp+28);
     if (text != "")
      {
         g.setColor(Color.black);
         g.drawString(text,xp+4,yp+fontHeight);
      }
   }

  void drawPressedButtonShadow(Graphics g)
   {
     g.setColor(Color.white);
     g.drawLine(xp+29,yp+1,xp+29,yp+29);  // |
     g.drawLine(xp,yp+29,xp+29,yp+29);    // _

     g.setColor(Color.black);
     g.drawLine(xp,yp,xp+29,yp);         
     g.drawLine(xp,yp,xp,yp+29);
     g.setColor(Color.darkGray);
     g.drawLine(xp+1,yp+1,xp+28,yp+1);         
     g.drawLine(xp+1,yp+1,xp+1,yp+28);

     if (text != "")
      {
         g.setColor(Color.black);
         g.drawString(text,xp+5,yp+fontHeight+1);
      }
   }

  public void paint(Graphics g)
  {
     g.setFont(font);
     FontMetrics fm = g.getFontMetrics();
     fontHeight = fm.getHeight();

     if (selected)
      {
        g.setColor(color);
        g.fillRect(xp,yp,30,30);//size.width,size.height);
        if (img != null)
           g.drawImage(img,xp+3,yp+4,this);
        drawPressedButtonShadow(g);
      }
     else
      {
        g.setColor(color);
        g.fillRect(xp,yp,30,30);//size.width,size.height);
        if (img != null)
           g.drawImage(img,xp+2,yp+3,this);
        drawButtonShadow(g);
      }
    }

    public void move(int x, int y)
    {
      xp=x;
      yp=y;
      reshape(x,y,30,30);
    }
}

class ButtonCanvas extends Canvas
{
  static final int NBR_OF_BUTTONS = 20;
  static final int BUTTONS_PER_ROW = 4;
  static final int NBR_OF_ROWS = 5;

  static final short HEXID = 0;
  static final short REDID = 1;
  static final short GREENID = 2;
  static final short GRAYID = 3;
  static final short YELLOWID = 4;
  static final short BLACKID = 5;
  static final short DENID = GRAYID;
  static final short SIZEHEX = 6;

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


  Images images;
  boolean imagesLoaded;
  ImageButton imgButton[];// = new ImageButton[NBR_OF_BUTTONS];

  Dimension size;
  int worldWidth, worldHeight, hHexes, vHexes;
  int painting = 0;
  short worldHexes[];
  int currentSelected;

  public ButtonCanvas(
                   int aWidth,
                   int aHeight,
                   Images someImages)
   {
        int i;

        images = someImages;
        imagesLoaded = false;

        imgButton = new ImageButton[NBR_OF_BUTTONS];

        imgButton[0] = new ImageButton(images.arrowImg,"");
        imgButton[0].selected = true;
        currentSelected = 0;
        imgButton[1] = new ImageButton(images.denImg,"home");
        imgButton[2] = new ImageButton(images.redDenImg, "110");
        imgButton[3] = new ImageButton(images.greenDenImg, "185");
        imgButton[4] = new ImageButton(images.grayDenImg, "185");
        imgButton[5] = new ImageButton(images.yellowDenImg, "175");
        imgButton[6] = new ImageButton(images.blackDenImg, "300");
        imgButton[7] = new ImageButton(images.ruinImg, "40");
        imgButton[8] = new ImageButton(images.templeImg, "40");
        imgButton[9] = new ImageButton(images.lizardImg, "");

        imgButton[10] = new ImageButton(null, new Color(128,128,0), "15");        
        imgButton[11] = new ImageButton(images.swampImg, "10");
        imgButton[12] = new ImageButton(images.scrubImg, "10");
        imgButton[13] = new ImageButton(null, new Color(0,128,0), "25");
        imgButton[14] = new ImageButton(images.peakImg, "10");
        imgButton[15] = new ImageButton(images.vulcanoImg, "10");
        imgButton[16] = new ImageButton(images.cursedImg, "15");
        imgButton[17] = new ImageButton(images.poolImg, "5");
        imgButton[18] = new ImageButton(null,new Color(128,128,255),"free");
        imgButton[19] = new ImageButton(images.noLizardImg,"");

        for (i=0; i< NBR_OF_BUTTONS; i++)
          imgButton[i].move((i%BUTTONS_PER_ROW)*30+1,(i/BUTTONS_PER_ROW)*30+1);

        size = new Dimension(aWidth,aHeight);
   }
/*
  public int inside(int x, int y)
  {
    if ((x>0)&&(x<size.width)&&(y>1)&&(y<size.height))
      return 1;
    return 0;
  }
*/


  public void paint(Graphics g)
   {
     int i;

     painting++;
     if (!imagesLoaded)
       return;

     g.setColor(Color.black);
     g.drawRect(0,0,30*BUTTONS_PER_ROW+1,30*NBR_OF_ROWS+1);

     for (i=0; i < NBR_OF_BUTTONS; i++)
       imgButton[i].paint(g);
     
     painting = 0;
   }

   public void update(Graphics g)
    {
      if (painting == 0)
        paint(g);
    }

    public int getSelected()
    {
       return currentSelected;
    }

public boolean mouseDown(java.awt.Event evt, int x, int y)
{
  int b;

  if ((x < BUTTONS_PER_ROW*30)&&(y < NBR_OF_ROWS*30))
  {
    b = (y/30)*BUTTONS_PER_ROW + x/30;
    if ((b == currentSelected)||(b >= NBR_OF_BUTTONS))
      return false;
    imgButton[currentSelected].selected = false;
    imgButton[currentSelected].paint(getGraphics());
    imgButton[b].selected = true;
    imgButton[b].paint(getGraphics());
    currentSelected = b;
  }
  return false; // returns always false to allow owner to react
}
}



