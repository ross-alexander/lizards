 import java.applet.*;
 import java.awt.*;
 
 public class A extends JApplet
 {
        private int w, h;
        public void init( )
        {
        	w = 45;
        	h = 50;
        }
    
        public void paint(Graphics g)
        {
        	g.drawRect(w, h, 20, 80);
        }
 }

