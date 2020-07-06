import java.util.*;
import java.awt.*;
import java.applet.*;
import java.io.*;
// import Images;
// import LizCanvas;

class Border3D
{
	void draw3DRectHi(int x, int y, int w, int h, Graphics g)  // java did'nt work
	{
	    g.setColor(Color.lightGray);
	    g.drawRect(x,y,w,h);
	    g.setColor(Color.black);
	    g.drawLine(x+w,y+h, x+w, y+1);
	    g.drawLine(x+1, y+h, x+w, y+h);
	}
	void draw3DRectLo(int x, int y, int w, int h, Graphics g)  // java did'nt work
	{
		g.setColor(Color.darkGray);
		g.drawRect(x,y,w,h);
		g.setColor(Color.white);
		g.drawLine(x+w,y+h, x+w, y+1);
		g.drawLine(x+1, y+h, x+w, y+h);
	}


	public void paintBorder(Graphics g, int x0, int y0, Dimension size)
	{
		draw3DRectHi(x0,y0,size.width-1,size.height-1,g);
		draw3DRectLo(x0+4,y0+4,size.width-9,size.height-9,g);
		g.setColor(Color.white);
		g.drawLine(x0+1,y0+1, x0+size.width-2, y0+1);
		g.drawLine(x0+1,y0+1, x0+1, y0+size.height-2);
		g.setColor(Color.darkGray);
		g.drawLine(x0+1,y0+size.height-2, x0+size.width-2, y0+size.height-2);
		g.drawLine(x0+size.width-2, y0+1, x0+size.width-2, y0+size.height-2);
		g.setColor(Color.lightGray);
		//g.setColor(Color.red);
		g.drawRect(x0+2,y0+2, size.width-5, size.height-5);
		g.drawRect(x0+3,y0+3, size.width-7, size.height-7);
		g.setColor(Color.lightGray);
		g.drawLine(x0+5,y0+size.height-6, x0+size.width-6, y0+size.height-6);
		g.drawLine(x0+size.width-6,y0+5, x0+size.width-6, y0+size.height-6);
	}

	public void paint(Graphics g, Dimension size)
	{
		paintBorder(g,0,0,size);
	}

}

class BorderPanel extends Panel
{
	Border3D border;

	public void init()
	{
		setBackground(Color.lightGray);
		border = new Border3D();
	}

	public void paint(Graphics g)
	{
		border.paint(g, size());
	}
}

class BorderBottomJoinPanel extends BorderPanel
{
	public void paint(Graphics g)
	{
		Dimension size = size();
		size.height += 3;
		border.paintBorder(g,0,0,size);
	}
}

class BorderTopJoinPanel extends BorderPanel
{
	public void paint(Graphics g)
	{
		Dimension size = size();
		size.height += 3;
		border.paintBorder(g,0,-3,size);
	}
}


class HelpPanel extends BorderTopJoinPanel
{
	Dimension size;
	String line;
	FontMetrics fm;
	
	public void init()
	{
	//	setBackground(Color.lightGray);
	//	border = new Border3D();
		line = "Init";
		super.init();
        //setFont(new Font ("Helvetica",Font.PLAIN,11));
	}

	public void setText(String l)
	{
		line = "Click on a hexagon to "+ l;
		repaint(6,6,size.width-6,size.height-6);
	}

	public void paint(Graphics g)
	{
		size = size();
		fm = g.getFontMetrics();
		int h = fm.getHeight();

		g.setColor(Color.black);
		g.drawString(line,8, size.height-8);
		super.paint(g);
	}
}

class AccountPanel extends BorderTopJoinPanel
{
	Dimension size;
	int points;
	FontMetrics fm;

	public void setPoints(int somePoints)
	{
		points = somePoints;
		repaint();
	}

	public void paint(Graphics g)
	{
		size = size();
		fm = g.getFontMetrics();
		int h = fm.getHeight();

		if (points >= 0)
			g.setColor(Color.black);
		else
			g.setColor(Color.red);
		g.drawString("Account: "+points+" Points",8, size.height-8);
		super.paint(g);
//		border.paintTopJoin(g, size);
	}
}


/*
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
	g.setColor(Color.lightGray);
	g.fillRect(0,0,size.width,size.height);

//    draw3DRectHi(0,0,size.width-1,size.height-1,g);
    //g.setColor(new Color(0,0,160));
	//g.fillRect(5,5,size.width-10,headerHeight);
    //g.setColor(Color.white);
    //g.setFont(headerFont);
    //g.drawString("Lizards! Setup",10,18);

    //draw3DRectLo(4,4,size.width-9,size.height-22-7, g);
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
*/


class CtrlPanel extends BorderBottomJoinPanel
{
	static final int worldChoicePnlHeight = 30;
	static final int buttonPnlHeight = 5*32+4;

	Border3D border;
	HelpPanel helpPnl;
	Choice worldCh;
    ButtonCanvas buttonCv;
	Panel buttonPnl, worldPnl, terrainPnl, worldChoicePnl, buyLizPnl;
	Panel buyInfoPnl, clickPnl, buyWhatPnl, pricePnl, enterPnl;
	ControlPanel controlPnl;
	Button enterBtn;
	Choice lizColChoice, lizTypeChoice;
	Choice nbrChoice;
	Label worldCostLbl, pointsLbl, clickLbl, priceLbl, buyWhatLbl;


	public void init(int controlPnlWidth, int controlPnlHeight, Images images, HelpPanel aHelpPnl)
	{
		int ypos = 5;
		int clientLeft = 6;
		int clientRight = 12;

		//setResizable(false);
		super.init();

		helpPnl = aHelpPnl;
		Panel worldTextPnl = new Panel();
		worldTextPnl.reshape(clientLeft,ypos,controlPnlWidth-clientRight,25);
		worldTextPnl.add(new Label("World Selection:"));
		ypos +=25;


		border = new Border3D();

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

  	    buttonCv = new ButtonCanvas(controlPnlWidth-clientRight,64, images);

		worldChoicePnl = new Panel();
		worldChoicePnl.setLayout(new FlowLayout(FlowLayout.CENTER));
		worldChoicePnl.reshape(clientLeft,ypos,controlPnlWidth-clientRight,worldChoicePnlHeight);
		worldChoicePnl.add(worldCh);
		ypos += worldChoicePnlHeight+10;

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
		nbrChoice.disable();
		
		lizColChoice = new Choice();
		lizColChoice.addItem("Red");
		lizColChoice.addItem("Green");
		lizColChoice.addItem("Gray");
		lizColChoice.addItem("Yellow");
		lizColChoice.addItem("Black");
		lizColChoice.disable();

		lizTypeChoice = new Choice();
		lizTypeChoice.addItem("Den Lizards");
		lizTypeChoice.addItem("Warrior Lizards");
		lizTypeChoice.disable();

		
		buttonPnl = new Panel();
		buttonPnl.setLayout(new FlowLayout());
		buttonPnl.reshape(clientLeft,ypos, controlPnlWidth-clientRight,buttonPnlHeight);
		buttonCv.reshape(clientLeft,1,4*30+2,5*32);
		buttonPnl.add (buttonCv);
		ypos += buttonPnlHeight;

		Panel selTextPnl = new Panel();
		selTextPnl.reshape(clientLeft,ypos,controlPnlWidth-clientRight,25);
		selTextPnl.add(new Label ("Lizard Selection:"));
		ypos += 25;

		Panel buyNumPnl = new Panel();
		buyNumPnl.reshape(clientLeft,ypos, controlPnlWidth-clientRight,30);
		buyNumPnl.add (nbrChoice);
		buyNumPnl.add (lizColChoice);
		ypos += 30;

		Panel buyLizPnl = new Panel();
		buyLizPnl.reshape(clientLeft,ypos, controlPnlWidth-clientRight,30);
		buyLizPnl.add (lizTypeChoice);
		ypos += 40;

		clickPnl = new Panel();
		clickPnl.reshape(clientLeft,ypos,controlPnlWidth-clientRight,25);
		clickLbl = new Label("Click on a hexagon to");
		clickPnl.add(clickLbl);
		//ypos += 25;
		
		buyWhatPnl = new Panel();
		buyWhatPnl.reshape(clientLeft,ypos, controlPnlWidth-clientRight,25);
		buyWhatLbl = new Label("buy what " + insets().top);
		buyWhatPnl.add(buyWhatLbl);
		//ypos+=25;
		
		pricePnl = new Panel();
		pricePnl.reshape(clientLeft,ypos, controlPnlWidth-clientRight,25);
		priceLbl = new Label("for 100 points.");
		pricePnl.add (priceLbl);
		//ypos+=25;

		enterPnl = new Panel();
		enterPnl.reshape(clientLeft,ypos,controlPnlWidth-clientRight,30);
		enterBtn = new Button("Enter LIZARDS!");
		enterPnl.add(enterBtn);
		ypos += 30;
	
		resize(controlPnlWidth,controlPnlHeight);
		setLayout(null);
		add (worldTextPnl);
		add (worldChoicePnl);
		add (buttonPnl);
		add (selTextPnl);
		add (buyNumPnl);
		add (buyLizPnl);
		//add (clickPnl);
		//add (buyWhatPnl);
		//add (pricePnl);
		add (enterPnl);
		setBackground(Color.lightGray);
	}

	void setBuyInfoText()
	{
		int lizard, selected;

		selected = buttonCv.getSelected();
		if (selected != 9)
			priceLbl.setText("");
		switch (selected)
		{
		case 1:
			helpPnl.setText("select your home den.");
			break;
		case 2:	        
			helpPnl.setText("buy a red den for 110 points.");
			break;
		case 3:
	        helpPnl.setText("buy a green den for 185 points.");
			break;
		case 4:
	        helpPnl.setText("buy a gray den for 185 points.");
			break;
		case 5:
	        helpPnl.setText("buy a yellow den for 175 points.");
			break;
		case 6:
	        helpPnl.setText("buy a black den for 300 points.");
			break;
		case 7:
	        helpPnl.setText("buy a ruin for 40 points.");
			break;
		case 9:
			int n =  (nbrChoice.getSelectedIndex()+1);
			int pt;;
			if (lizTypeChoice.getSelectedIndex() == 0)
			{
				switch (lizColChoice.getSelectedIndex())
				{
				case 0:
					pt = 15;
					break;
				case 1:
				case 3:
					pt = 25;
					break;
				case 2:
					pt = 30;
					break;
				default:
					pt = 50;
				}
			}
			else
			{
				switch (lizColChoice.getSelectedIndex())
				{
				case 0:
					pt = 10;
					break;
				case 1:
				case 2:
					pt = 20;
					break;
				case 3:
					pt = 15;
					break;
				default:
					pt = 30;
				}
			}
			String s = nbrChoice.getSelectedItem()+" "+lizColChoice.getSelectedItem()+" "
				+lizTypeChoice.getSelectedItem();
			helpPnl.setText("buy "+s+" for "+(pt*n)+" points.");
			break;
		case 10:
	        helpPnl.setText("buy plains for 15 points.");
			break;
		case 11:
	        helpPnl.setText("buy swamp for 10 points.");
			break;
		case 12:
	        helpPnl.setText("buy scrub for 10 points.");
			break;
		case 13:
	        helpPnl.setText("buy a fertile area for 25 points.");
			break;
		case 14:
	        helpPnl.setText("buy a peak for 10 points."); 
			break;
		case 15:
	        helpPnl.setText("buy a volcano for 10 points.");
			break;
		case 8:
	        helpPnl.setText("buy a temple for 40 points.");
			break;
		case 16:
	        helpPnl.setText("buy a cursed area for 15 points.");
			break;
		case 17:
	        helpPnl.setText("buy a whirlpool for 5 points.");
			break;
		case 18:
	        helpPnl.setText("place water for free.");
			break;
		}
	}

	public void paint(Graphics g)
	{
		super.paint(g);
	}
/*  
public boolean action(Event evt, Object arg) 
{
/*
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

        }   }
  else
  /
  if (evt.target instanceof Choice)
   {
      lizWorldCv.invalidate();
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

/*
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
*/
}


public class jtest extends Applet implements Runnable {

  final Font font = new Font ("Helvetica",Font.PLAIN,12);
  int mouseX, mouseY;
  boolean ignoreDrag;
  Thread motor;

  Images images;
  int appletWidth, appletHeight;

  LizCanvas lizWorldCv;
  CtrlPanel ctrlPnl;
  Border3D border;

  Scrollbar hScroll, vScroll;
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
  setBackground(Color.white);

  images = new Images();

  images.arrowImg   = getImage(getDocumentBase(), getParameter("arrowImg"));
  images.denImg     = getImage(getDocumentBase(), getParameter("denImg"));
  images.ruinImg    = getImage(getDocumentBase(), getParameter("ruinImg"));
  images.swampImg   = getImage(getDocumentBase(), getParameter("swampImg"));
  images.scrubImg   = getImage(getDocumentBase(), getParameter("scrubImg"));
  images.peakImg    = getImage(getDocumentBase(), getParameter("peakImg"));
  images.vulcanoImg = getImage(getDocumentBase(), getParameter("vulcanoImg"));
  images.templeImg  = getImage(getDocumentBase(), getParameter("templeImg"));
  images.cursedImg  = getImage(getDocumentBase(), getParameter("cursedImg"));
  images.poolImg    = getImage(getDocumentBase(), getParameter("poolImg"));
  images.raftImg    = getImage(getDocumentBase(), getParameter("raftImg"));
  images.bridgeImg  = getImage(getDocumentBase(), getParameter("bridgeImg"));

  images.redDenImg    = getImage(getDocumentBase(), getParameter("redDenImg"));
  images.greenDenImg  = getImage(getDocumentBase(), getParameter("greenDenImg"));
  images.grayDenImg   = getImage(getDocumentBase(), getParameter("grayDenImg"));
  images.yellowDenImg = getImage(getDocumentBase(), getParameter("yellowDenImg"));
  images.blackDenImg  = getImage(getDocumentBase(), getParameter("blackDenImg"));
  images.lizardImg    = getImage(getDocumentBase(), getParameter("lizardImg"));
  images.noLizardImg  = getImage(getDocumentBase(), getParameter("noLizardImg"));
  images.backImg      = getImage(getDocumentBase(), getParameter("backImg"));

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


  hScroll  = new Scrollbar(Scrollbar.HORIZONTAL,50,100,0,100);
  vScroll  = new Scrollbar(Scrollbar.VERTICAL,50,100,0,100);
  //add(vScroll);
	//add(hScroll);

  appletWidth  = Integer.parseInt(getParameter("width"));
  appletHeight = Integer.parseInt(getParameter("height"));

  int ctrlFrameWidth = 200;
  int lizWorldWidth = appletWidth - ctrlFrameWidth - 12;
  int helpPnlHeight = 26;
  int lizWorldHeight = appletHeight - helpPnlHeight - 12 + 3;
  lizWorldCv = new LizCanvas(lizWorldWidth, lizWorldHeight, images);
  lizWorldCv.reshape(6,6,lizWorldWidth,lizWorldHeight);
  lizWorldCv.setLayout(null); 
  lizWorldCv.SetWorld(3,4,DefaultWorld1Hexes);
  //add (lizWorldCv);
  
  HelpPanel helpPnl = new HelpPanel();
  helpPnl.reshape(0,appletHeight-helpPnlHeight,lizWorldWidth+12,helpPnlHeight);
  helpPnl.init();
  add(helpPnl);

  AccountPanel accountPnl = new AccountPanel();
  accountPnl.reshape(lizWorldWidth+12,appletHeight-helpPnlHeight,ctrlFrameWidth,helpPnlHeight);
  accountPnl.init();
  accountPnl.setPoints(-235);
  add(accountPnl);

  ctrlPnl = new CtrlPanel();
  ctrlPnl.init(ctrlFrameWidth, appletHeight-helpPnlHeight, images, helpPnl);
  ctrlPnl.reshape(lizWorldWidth+12,0,ctrlFrameWidth,appletHeight-helpPnlHeight);
  ctrlPnl.hide();
  add (ctrlPnl);

  BorderPanel lizBackPnl = new BorderBottomJoinPanel();
  lizBackPnl.init();
  lizBackPnl.reshape(0,0,lizWorldWidth+12,lizWorldHeight+12-3);
  lizBackPnl.setLayout(null);
  lizBackPnl.add(lizWorldCv);
  lizBackPnl.setBackground(Color.white);
  add (lizBackPnl);

  setBackground(Color.white);//getBackground());
}

 // Paint is the main part of the program

//public void paint(Graphics g) {
//  terrainCv.paint(g);
//}


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
/*
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
*/
  if (evt.target instanceof Choice)
   {
      lizWorldCv.invalidate();
      if ("My world".equals(arg))
        {
//          buttonPnl.hide();
//          buyFrm.hide();
        }
      else if ("Startup World 1".equals(arg))
       {
//          buttonPnl.hide();
          lizWorldCv.SetWorld(3,4,DefaultWorld1Hexes);
//          buyFrm.hide();
       }
      else if ("Startup World 2".equals(arg))
       {
//          buttonPnl.hide();
//          buyFrm.hide();
       }
          
      else if ("Startup World 2".equals(arg))
       {
//          buttonPnl.hide();
//          buyFrm.hide();
       }         
      else if ("Startup World 4".equals(arg))
       {
          //buttonPnl.hide();
//          buyFrm.hide();
       }        
      else if ("Template World 1".equals(arg))
       {
         lizWorldCv.SetWorld(3,3,CustomTempl1Hexes);
//         buttonPnl.show();
//         buyFrm.show();
        }
      else if ("Template World 2".equals(arg))
       {
         lizWorldCv.SetWorld(3,4,CustomTempl2Hexes);
//         buttonPnl.show();
//         buyFrm.show();
       }
      else
	  {
		  ctrlPnl.setBuyInfoText();
        return false;
	  }
          
       return true;
   }

   return false;
}

/*

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

/*
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
/         cv.getGraphics().setXORMode(Color.black); doesn't work
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

*/
public synchronized boolean mouseDown(java.awt.Event evt, int x, int y)
{
   int selBtn;

   selBtn = ctrlPnl.buttonCv.getSelected();
   if (selBtn == 9)
   {
	   /*
     buyLizPnl.show();
     buyWhatPnl.hide();
	 */
      ctrlPnl.setBuyInfoText();
	  ctrlPnl.lizColChoice.enable();
	  ctrlPnl.nbrChoice.enable();
	  ctrlPnl.lizTypeChoice.enable();
   }
   else
    {
      ctrlPnl.setBuyInfoText();
	  ctrlPnl.lizColChoice.disable();
	  ctrlPnl.nbrChoice.disable();
  	  ctrlPnl.lizTypeChoice.disable();
/*
      buyLizPnl.hide();
      buyWhatPnl.show();
	  */
    }
// if ((evt.target instanceof Label)&&(headerClick==false))
// only panels seem to react
//   if ((x>=ctrlX)&&(x<x+controlPnlWidth)&&
//       (y>=ctrlY)&&(y<y+headerHeight)&&

/*---
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
   */
 // else
  {
    lizWorldCv.mouseClick(x,y,selBtn,
                  (ctrlPnl.nbrChoice.getSelectedIndex()+1)*10,
                  ctrlPnl.lizColChoice.getSelectedIndex(),
				  ctrlPnl.lizTypeChoice.getSelectedIndex());
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
//	border.paint(g, size());
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
  boolean imgLoaded = false;

  motor.setPriority(Thread.MIN_PRIORITY);
  
  while (!imgLoaded)
    {
//      clickLbl.setText(""+images.count);
	  imgLoaded = images.imagesLoaded();
	  ctrlPnl.buttonCv.imagesLoaded = lizWorldCv.imagesLoaded = imgLoaded;
      lizWorldCv.repaint();
	  try { motor.sleep(200); } catch (InterruptedException e){}
    }
//  clickLbl.setText("done");
//  clickLbl.setText("FROM: "+System.getProperty("QUERY_STRING"));
  ctrlPnl.show();
  lizWorldCv.update(getGraphics());
  lizWorldCv.repaint();
  while (true)
    {
//      clickLbl.setText(""+images.count);
      i= ++i & 7;
      lizWorldCv.animate();
      try { motor.sleep(80+8*(i^5)); } catch (InterruptedException e){}
    }
}
}

