import java.util.*;
import java.awt.*;
import java.io.*;
// import images;
// import LizCanvas;

/*
class CtrlFrame extends Frame
{
	public CtrlFrame(String s)
	{
		super(s);
	}

public synchronized boolean mouseDown(java.awt.Event evt, int x, int y)
{
   int selBtn;

   selBtn = terrainCv.getSelected();
   if (selBtn == 9)
   {
     buyLizPnl.show();
     buyWhatPnl.hide();
   }
   else
    {
      setBuyInfoText();
      buyLizPnl.hide();
      buyWhatPnl.show();
    }
// if ((evt.target instanceof Label)&&(headerClick==false))
// only panels seem to react
//   if ((x>=ctrlX)&&(x<x+controlPnlWidth)&&
//       (y>=ctrlY)&&(y<y+headerHeight)&&

  if  ((x>=ctrlX)&&(x<x+controlPnlWidth)&&
       (y>=ctrlY)&&(y<y+headerHeight))
   {
     if (( evt.target == controlPnl) &&
          (evt.target instanceof Panel)&&
          (headerClick == false))
      {
         headerClick=true;
         ignoreDrag = true;
//      controlPnl.hide();
        controlPnl.disable();
         dragXofs = x-ctrlX;
         dragYofs = y-ctrlY;
//      priceLbl.setText("CLICK"+ctrlX);
      }
   }
  else
  {
    cv.mouseClick(x,y,selBtn,
                  nbrChoice.getSelectedIndex(),
                  lizColChoice.getSelectedIndex());
  }

   return true;
}

}
*/


class ControlPanel extends Panel
{
  static final int headerHeight = 20;
  static final Font headerFont = new Font ("Helvetica",Font.BOLD,12);
  static final Font normFont = new Font ("Helvetica",Font.PLAIN,11);

  void draw3DRectLo(int x, int y, int w, int h, Graphics g)  // java did'nt work
  {
    g.setColor(Color.darkGray);
    g.drawRect(x,y,w,h);
    g.setColor(Color.white);
    g.drawLine(x+w,y+h, x+w, y+1);
    g.drawLine(x+1, y+h, x+w, y+h);
  }
  void draw3DRectHi(int x, int y, int w, int h, Graphics g)  // java did'nt work
  {
    g.setColor(Color.lightGray);
    g.drawRect(x,y,w,h);
    g.setColor(Color.black);
    g.drawLine(x+w,y+h, x+w, y+1);
    g.drawLine(x+1, y+h, x+w, y+h);
  }

  public void init()
  {
    setBackground(Color.lightGray);
  }

  public void paint(Graphics g)
  {
    Dimension size = size();
    draw3DRectHi(0,0,size.width-1,size.height-1,g);
    g.setColor(new Color(0,0,160));
    g.fillRect(5,5,size.width-10,headerHeight);
    g.setColor(Color.white);
    g.setFont(headerFont);
    g.drawString("Lizards! Setup",10,18);

    draw3DRectLo(4,4,size.width-9,size.height-22-7, g);
    draw3DRectLo(4,size.height-23, size.width-9, 18, g);
    g.setFont(normFont);
    g.setColor(Color.black);
    g.drawString("1200 points remaining.",10,size.height-10);
  }
//  public void update(Graphics g)
//  {
//    paint(g);
//  }
}


public class CFrame extends Frame implements Runnable {

  static final int worldPnlHeight = 440;
  static final int worldChoicePnlHeight = 30;
  static final int controlPnlWidth = 340;
  static final int clientWidth = controlPnlWidth-10;
  static final int clientXOfs = 5;
  static final int controlPnlHeight = 280;
  static final int buttonPnlHeight = 70;
  static final int headerHeight = 20;
  final Font font = new Font ("Helvetica",Font.PLAIN,12);
  int mouseX, mouseY;
  boolean ignoreDrag;
  Thread motor;

  Images images;
  int appletWidth, appletHeight;

  LizCanvas cv;
  TerrainCanvas terrainCv;

  Scrollbar hScroll, vScroll;
  Panel buttonPnl, worldPnl, terrainPnl, worldChoicePnl, buyLizPnl;
  Panel buyInfoPnl, clickPnl, buyWhatPnl, pricePnl, enterPnl;
  ControlPanel controlPnl;
  Button enterBtn;
  Choice worldCh;
  Choice lizColChoice;
  Choice nbrChoice;
  Label worldCostLbl, pointsLbl, clickLbl, priceLbl,
        buyWhatLbl;
  Button closeBtn;
  boolean headerClick = false;
  boolean dragging = false;
  int ctrlX=100, ctrlY=250;
  int dragXofs=0, dragYofs=0;
//  Frame buyFrm;


  short DefaultWorld1Hexes[]=
                     {  0x00,0,0,0,0,0,0,
                        0x00,0,0,0,0,0,0,
                        0x84,0,0,0,0,0,0,
                        0x81,0,0,0,0,50,0,
                        0x85,0,0,20,0,0,0,
                        0x81,0,0,0,0,50,0,
                        0x85,0,0,0,0,0,0,
                        0x88,0,0,0,0,0,110,
                        0x85,0,0,0,0,0,0,
                        0x84,0,0,0,0,0,0,
                        0x80,30,0,0,0,0,0, 
                        0x00,0,0,0,0,0,0 };

  short CustomTempl1Hexes[]=
                     {  0x8A,0,0,0,0,0,0,                       
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x00,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x00,0,0,0,0,0,0 };

  short CustomTempl2Hexes[]=
                     {  0x00,0,0,0,0,0,0,
                        0x00,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x8A,0,0,0,0,0,0,
                        0x00,0,0,0,0,0,0};
/*
void xgetImage(Image img)
{
   Dimension d = img.size();

   img = createImage(24, 24);
   img.getGraphics().drawImage(offScreenImage, 0, 0, null);
}
*/


public void  init() {
  int x,y,w,h, ypos, i;
  boolean done;

  setLayout(null); // rest follows in single column
  setFont(font);

  ypos = 1;
//  setBackground(Color.green);

  images = new Images();

  images.arrowImg = getImage(getDocumentBase(), getParameter("arrowImg"));
  images.denImg = getImage(getDocumentBase(), getParameter("denImg"));
  images.ruinImg = getImage(getDocumentBase(), getParameter("ruinImg"));
  images.swampImg = getImage(getDocumentBase(), getParameter("swampImg"));
  images.scrubImg = getImage(getDocumentBase(), getParameter("scrubImg"));
  images.peakImg = getImage(getDocumentBase(), getParameter("peakImg"));
  images.vulcanoImg = getImage(getDocumentBase(), getParameter("vulcanoImg"));
  images.templeImg = getImage(getDocumentBase(), getParameter("templeImg"));
  images.cursedImg = getImage(getDocumentBase(), getParameter("cursedImg"));
  images.poolImg = getImage(getDocumentBase(), getParameter("poolImg"));
  images.raftImg = getImage(getDocumentBase(), getParameter("raftImg"));
  images.bridgeImg = getImage(getDocumentBase(), getParameter("bridgeImg"));

  images.redDenImg = getImage(getDocumentBase(), getParameter("redDenImg"));
  images.greenDenImg = getImage(getDocumentBase(), getParameter("greenDenImg"));
  images.grayDenImg = getImage(getDocumentBase(), getParameter("grayDenImg"));
  images.yellowDenImg = getImage(getDocumentBase(), getParameter("yellowDenImg"));
  images.blackDenImg = getImage(getDocumentBase(), getParameter("blackDenImg"));
  images.lizardImg = getImage(getDocumentBase(), getParameter("lizardImg"));
  images.noLizardImg = getImage(getDocumentBase(), getParameter("noLizardImg"));
  images.backImg = getImage(getDocumentBase(), getParameter("backImg"));

        images.tracked = 0;
        images.tracker = new MediaTracker(this);
        images.tracker.addImage(images.arrowImg, images.tracked++);
        images.tracker.addImage(images.denImg, images.tracked++);
        images.tracker.addImage(images.ruinImg, images.tracked++);
        images.tracker.addImage(images.swampImg, images.tracked++);
        images.tracker.addImage(images.scrubImg, images.tracked++);
        images.tracker.addImage(images.peakImg, images.tracked++);
        images.tracker.addImage(images.vulcanoImg, images.tracked++);
        images.tracker.addImage(images.templeImg, images.tracked++);
        images.tracker.addImage(images.cursedImg, images.tracked++);
        images.tracker.addImage(images.poolImg, images.tracked++);
        images.tracker.addImage(images.raftImg, images.tracked++);
        images.tracker.addImage(images.bridgeImg, images.tracked++);
        images.tracker.addImage(images.redDenImg, images.tracked++);
        images.tracker.addImage(images.greenDenImg, images.tracked++);
        images.tracker.addImage(images.grayDenImg, images.tracked++);
        images.tracker.addImage(images.yellowDenImg, images.tracked++);
        images.tracker.addImage(images.blackDenImg, images.tracked++);
        images.tracker.addImage(images.lizardImg, images.tracked++);
        images.tracker.addImage(images.noLizardImg, images.tracked++);
        images.tracker.addImage(images.backImg, images.tracked++);


  worldCh = new Choice();
  worldCh.addItem("My World");
  worldCh.addItem("Startup World 1");
  worldCh.addItem("Startup World 2");
  worldCh.addItem("Startup World 3");
  worldCh.addItem("Startup World 4");
  worldCh.addItem("Template World 1");
  worldCh.addItem("Template World 2");
  worldCh.addItem("Template World 3");
  worldCh.addItem("Template World 4");
  worldCh.addItem("Template World 5");
  worldCh.addItem("Template World 6");
  worldCh.addItem("Template World 7");
  worldCh.addItem("Template World 8");

  hScroll  = new Scrollbar(Scrollbar.HORIZONTAL,50,100,0,100);
  vScroll  = new Scrollbar(Scrollbar.VERTICAL,50,100,0,100);

  appletWidth  = Integer.parseInt(getParameter("width"));
  appletHeight = Integer.parseInt(getParameter("height"));

  cv = new LizCanvas(appletWidth, appletHeight, images);
  cv.reshape(0,0,appletWidth,appletHeight);
  cv.setLayout(null);

  terrainCv = new TerrainCanvas(controlPnlWidth,64, images);

//  worldPnl = new Panel();
//  worldPnl.setLayout(null);
//  worldPnl.resize(appletWidth,appletHeight);
//  worldPnl.add(cv);
//  worldPnl.add("East", vScroll);
//  worldPnl.add("South", hScroll);

//  add (worldPnl);
 

  
  worldChoicePnl = new Panel();
  worldChoicePnl.setLayout(new FlowLayout(FlowLayout.CENTER));
  worldChoicePnl.reshape(clientXOfs,25,clientWidth,worldChoicePnlHeight);
  worldChoicePnl.add(worldCh);
  ypos = worldChoicePnlHeight+25;

//  worldChoicePnl.add(worldCostLbl);
//  worldChoicePnl.add(pointsLbl);
//  worldChoicePnl.add(new Button("Enter Lizards!"));

//  buyLizBtn = new Button("Buy Lizards!");

  nbrChoice = new Choice();
  nbrChoice.addItem("10");
  nbrChoice.addItem("20");
  nbrChoice.addItem("30");
  nbrChoice.addItem("40");
  nbrChoice.addItem("50");
  nbrChoice.addItem("60");
  nbrChoice.addItem("70");
  nbrChoice.addItem("80");
  nbrChoice.addItem("90");
  nbrChoice.addItem("100");

  lizColChoice = new Choice();
  lizColChoice.addItem("Den Lizards");
  lizColChoice.addItem("Red Warrior Lizards");
  lizColChoice.addItem("Green Warrior Lizards");
  lizColChoice.addItem("Gray Warrior Lizards");
  lizColChoice.addItem("Yellow Warrior Lizards");
  lizColChoice.addItem("Black Warrior Lizards");

  buttonPnl = new Panel();
  buttonPnl.setLayout(new FlowLayout());
  buttonPnl.reshape(clientXOfs,ypos, clientWidth,buttonPnlHeight);
  terrainCv.reshape(1,1,10*30+2,64);
  buttonPnl.add (terrainCv);
  ypos += buttonPnlHeight;

  clickPnl = new Panel();
  clickPnl.reshape(clientXOfs,ypos,clientWidth,30);

  clickLbl = new Label("Click on a hexagon to place lizards for 1100 points:     ");
  clickLbl.resize(clientWidth,30);
  clickPnl.add(clickLbl);
  
  ypos += 30;

  buyWhatPnl = new Panel();
  buyWhatPnl.reshape(clientXOfs,ypos, clientWidth,30);
  buyWhatLbl = new Label("buy what");
  buyWhatPnl.add(buyWhatLbl);

  buyLizPnl = new Panel();
  buyLizPnl.reshape(clientXOfs,ypos, clientWidth,30);
  buyLizPnl.add (nbrChoice);
  buyLizPnl.add (lizColChoice);

  ypos += 30;

  pricePnl = new Panel();
  pricePnl.reshape(clientXOfs,ypos, clientWidth,30);
  priceLbl = new Label("for 100 points.");
  pricePnl.add (priceLbl);
  ypos+=30;


  enterPnl = new Panel();
  enterPnl.reshape(clientXOfs,ypos,clientWidth,30);
  enterBtn = new Button("Enter LIZARDS!");
  enterPnl.add(enterBtn);
  ypos += 30;

  controlPnl = new ControlPanel();
  controlPnl.setLayout(null);
  controlPnl.reshape(ctrlX,ctrlY,controlPnlWidth,controlPnlHeight);

  controlPnl.add (worldChoicePnl);
  controlPnl.add (buttonPnl);
  controlPnl.add (clickPnl);
  controlPnl.add (buyWhatPnl);
  controlPnl.add (buyLizPnl);
  controlPnl.add (pricePnl);
  controlPnl.add (enterPnl);
  controlPnl.setBackground(Color.lightGray);
  //controlPnl.hide();

  CtrlFrame ctrlFrame = new CtrlFrame("Control panel");
  ctrlFrame.add(controlPnl);
  ctrlFrame.show();

//  cv.add(controlPnl);

  setBuyInfoText();

  cv.SetWorld(3,4,DefaultWorld1Hexes);

  add (cv);

//  buyFrm = new Frame("Lizards! Shopping...");
//  buyFrm.setLayout(null);
//  buyFrm.reshape(30,30,appletWidth,100); resize/shape doesn't seem to work
//  buyFrm.setResizable(false);
//  buyFrm.add("Center",terrainCv);
//  buyFrm.show();
}

 // Paint is the main part of the program

//public void paint(Graphics g) {
//  terrainCv.paint(g);
//}

void setBuyInfoText()
{
  int lizard, selected;
  String clickTo = "Click on a hexagon to buy ";

  selected = terrainCv.getSelected();
  if (selected != 9)
     priceLbl.setText("");
  switch (selected)
   {
     case 1:
        clickLbl.setText("Click on a den hexagon to select your home den.        ");
        break;
     case 2:
        clickLbl.setText(clickTo+"a red den for 110 points.  ");
        break;
     case 3:
        clickLbl.setText(clickTo+"a green den for 185 points. ");
        break;
     case 4:
        clickLbl.setText(clickTo+"a gray den for 185 points.  ");
        break;
     case 5:
        clickLbl.setText(clickTo+"a yellow den for 175 points. ");
        break;
     case 6:
        clickLbl.setText(clickTo+"a black den for 300 points. ");
        break;
     case 7:
        clickLbl.setText(clickTo+"a ruin for 40 points.     ");
        break;
     case 10:
        clickLbl.setText(clickTo+"plains for 15 points.     ");
        break;
     case 11:
        clickLbl.setText(clickTo+"swamp for 10 points.     ");
        break;
     case 12:
        clickLbl.setText(clickTo+"scrub for 10 points.     ");
        break;
     case 13:
        clickLbl.setText(clickTo+"a fertile area for 25 points. ");
        break;
     case 14:
        clickLbl.setText(clickTo+"a peak for 10 points.    "); 
        break;
     case 15:
        clickLbl.setText(clickTo+"a vulcano for 10 points.   ");
        break;
     case 8:
        clickLbl.setText(clickTo+"a temple for 40 points.");
        break;
     case 16:
        clickLbl.setText(clickTo+"a cursed area for 15 points.");
        break;
     case 17:
        clickLbl.setText(clickTo+"a whirlpool for 5 points.");
        break;
     case 18:
        clickLbl.setText(clickTo+"water for free.");
        break;
     }
}

void enterLizards() throws IOException
{
  OutputStream os = new FileOutputStream("file.txt");

  os.write((int)'Y');
  os.write((int)'E');
  os.write((int)'S');
  os.write((int)'!');
  os.close();
}

public boolean action(Event evt, Object arg) 
{

  if (evt.target instanceof Button)
   {
      if ("Enter LIZARDS!".equals(arg))
        {
          controlPnl.hide();
          controlPnl.show();
          try
          {
            enterLizards();
          } catch (IOException e) {}

        }
   }
  else
  if (evt.target instanceof Choice)
   {
      cv.invalidate();
      if ("My world".equals(arg))
        {
          buttonPnl.hide();
//          buyFrm.hide();
        }
      else if ("Startup World 1".equals(arg))
       {
          buttonPnl.hide();
          cv.SetWorld(3,4,DefaultWorld1Hexes);
//          buyFrm.hide();
       }
      else if ("Startup World 2".equals(arg))
       {
          buttonPnl.hide();
//          buyFrm.hide();
       }
          
      else if ("Startup World 2".equals(arg))
       {
          buttonPnl.hide();
//          buyFrm.hide();
       }         
      else if ("Startup World 4".equals(arg))
       {
          buttonPnl.hide();
//          buyFrm.hide();
       }        
      else if ("Template World 1".equals(arg))
       {
         cv.SetWorld(3,3,CustomTempl1Hexes);
         buttonPnl.show();
//         buyFrm.show();
        }
      else if ("Template World 2".equals(arg))
       {
         cv.SetWorld(3,4,CustomTempl2Hexes);
         buttonPnl.show();
//         buyFrm.show();
       }
      else
        return false;
          
       return true;
   }

   return false;
}

public synchronized boolean mouseUp(java.awt.Event evt, int x, int y)
{
int newX,newY;

if (ignoreDrag==true)
{
  ignoreDrag=false;
  return false;
}
if ((x<0)||(y<0)) // sometimes called with that kind of shit!!
  return false;

  if (headerClick)
   {
     newX=x-dragXofs;
     newY=y-dragYofs;
     if (newX < 0)
       newX = 0;// dragXofs = 0;
     if (newX+controlPnlWidth > appletWidth)
      {
       newX = appletWidth-controlPnlWidth;
//       dragXofs = controlPnlWidth;
      }
     if (newY<0)
       newY = 0;//dragYofs = 0;
     if (newY+controlPnlHeight > appletHeight)
     {
       newY=appletHeight-controlPnlHeight;
//       dragYofs = controlPnlHeight;
     }
priceLbl.setText("UP "+x+","+y);
   ctrlX=newX;ctrlY=newY;
     controlPnl.move(ctrlX, ctrlY);
     controlPnl.show();
     controlPnl.enable();
     headerClick=false;
     return true;
   }
  return false;
}

public synchronized boolean mouseDrag(java.awt.Event evt, int x, int y)
{
  int newX, newY;

//x=mouseX; y=mouseY;

if ((x<0)||(y<0)||(ignoreDrag==true))
  return false;

  if (headerClick)
  {
     dragging = true;
        priceLbl.setText("DRAG "+x);
     newX=x-dragXofs;
     newY=y-dragYofs;
     if (newX < 0)
       newX = 0;// dragXofs = 0;
     if (newX+controlPnlWidth > appletWidth)
      {
       newX = appletWidth-controlPnlWidth;
//       dragXofs = controlPnlWidth;
      }
     if (newY<0)
       newY = 0;//dragYofs = 0;
     if (newY+controlPnlHeight > appletHeight)
     {
       newY=appletHeight-controlPnlHeight;
//       dragYofs = controlPnlHeight;
     }
     if  ((ctrlX != newX) && (ctrlY != newY))
      {
//         controlPnl.hide();
/*         cv.getGraphics().setXORMode(Color.black); doesn't work*/
//         cv.repaint(ctrlX, ctrlY,
//                    controlPnlWidth, headerHeight);
//         cv.getGraphics().drawRect(newX,newY,
//                                   controlPnlWidth,controlPnlHeight);
//         cv.getGraphics().setPaintMode();
        ctrlX=newX; ctrlY=newY;
        controlPnl.move(ctrlX,ctrlY);
        dragging = false;
        return true;
      }
  }
  return false; 
}

public synchronized boolean mouseDown(java.awt.Event evt, int x, int y)
{
   int selBtn;

   selBtn = terrainCv.getSelected();
   if (selBtn == 9)
   {
     buyLizPnl.show();
     buyWhatPnl.hide();
   }
   else
    {
      setBuyInfoText();
      buyLizPnl.hide();
      buyWhatPnl.show();
    }
// if ((evt.target instanceof Label)&&(headerClick==false))
// only panels seem to react
//   if ((x>=ctrlX)&&(x<x+controlPnlWidth)&&
//       (y>=ctrlY)&&(y<y+headerHeight)&&

  if  ((x>=ctrlX)&&(x<x+controlPnlWidth)&&
       (y>=ctrlY)&&(y<y+headerHeight))
   {
     if (( evt.target == controlPnl) &&
          (evt.target instanceof Panel)&&
          (headerClick == false))
      {
         headerClick=true;
         ignoreDrag = true;
//      controlPnl.hide();
        controlPnl.disable();
         dragXofs = x-ctrlX;
         dragYofs = y-ctrlY;
//      priceLbl.setText("CLICK"+ctrlX);
      }
   }
  else
  {
    cv.mouseClick(x,y,selBtn,
                  nbrChoice.getSelectedIndex(),
                  lizColChoice.getSelectedIndex());
  }

   return true;
}

public void paint(Graphics g)
{
/*
int x,y,wImg,hImg;
Dimension size = size();

x=y=0;
     wImg=hImg=96;
     while (y<size.height)
      {
         g.drawImage(backImg,x,y,this);
         x += wImg;
         if (x>size.width)
         {
            x  = 0;
            y += hImg;
         }
      }
*/  
    setBackground(Color.white);//getBackground());
}

public void update(Graphics g)
{
  paint(g);
}
/*
public boolean handleEvent(Event e)
 {
   switch (e.id)
    {
     case Event.MOUSE_MOVE:
       mouseX = e.x;
       mouseY = e.y;
//       priceLbl.setText(""+e.x+","+e.y);
       break;
     case Event.MOUSE_DRAG:
       myMouseDrag(e,e.x,e.y);
       break;
     case Event.MOUSE_DOWN:
       myMouseDown(e,e.x,e.y);
       break;
     case Event.MOUSE_UP:
//       priceLbl.setText("UP"+ctrlX);
       myMouseUp(e,e.x,e.y);
       break;
     default:
       return false;
    }
    return true;
 }
*/

public void start() {
  motor = new Thread(this);
  motor.start();
}

public void stop() {
  motor.stop();
}

public void run() {
  int i=0;
  motor.setPriority(Thread.MIN_PRIORITY);
  while (!(terrainCv.imagesLoaded = cv.imagesLoaded = images.imagesLoaded()))
    {
//      clickLbl.setText(""+images.count);
      cv.repaint();
      try { motor.sleep(200); } catch (InterruptedException e){}
    }
//  clickLbl.setText("done");
//  clickLbl.setText("FROM: "+System.getProperty("QUERY_STRING"));
  controlPnl.show();
  cv.update(getGraphics());
  cv.repaint();
  while (true)
    {
//      clickLbl.setText(""+images.count);
      i= ++i & 7;
      cv.animate();
      try { motor.sleep(80+8*(i^5)); } catch (InterruptedException e){}
    }
}
}

