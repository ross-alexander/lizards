import java.awt.*
import java.applet.*

class GrayCanvas extends Canvas
 {
   Color gray;

   public GrayCanvas(float g)
    {
       gray = new Color(g,g,g);
    }

   public void paint(Graphics g)
    {
      Dimension size = size();
      g.setColor(gray);
      g.

