import java.applet.*;
import java.awt.*;

class Images
 {
  Image arrowImg, denImg, ruinImg, swampImg, scrubImg,
        peakImg, vulcanoImg, templeImg, cursedImg,
        poolImg, raftImg, bridgeImg,
        redDenImg, greenDenImg, grayDenImg, yellowDenImg,
        blackDenImg, lizardImg, noLizardImg, backImg;

  MediaTracker tracker;
  int tracked;
  int count;

  public Images()
   {
   }

  public boolean imagesLoaded()
  {
    boolean done;
    int i;
  
    done = true;
    for (count=i=0; i< tracked; i++)
     {
       if (tracker.checkID(i,true))
         count++;
       else
        {
          done=false;
          break;
        }
      }
    return done;
  }

  }
