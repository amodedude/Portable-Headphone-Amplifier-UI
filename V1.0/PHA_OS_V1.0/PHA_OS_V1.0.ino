// Portable Headphone Amplifier Embedded Software
// John August 

// Include the UTFT and UTouch Libraries
// for programing the 3.2" LCD screen
#include <UTFT.h>
#include <UTouch.h>

// Define the orientation of the touch screen. 
#define TOUCH_ORIENTATION  PORTRAIT

// Initialize display
// ------------------
// Set the ITDB02-3.2S LCD panel communication pins
// -----------------------------------------------------------
// Standard Arduino Mega/Due shield            : <display model>,38,39,40,41
//
UTFT    myGLCD(ITDB32S,38,39,40,41);

// Initialize touchscreen
// ----------------------
// Set touch screen communication pins
// -----------------------------------------------------------
// Standard Arduino Mega/Due shield            :  6, 5, 4, 3, 2
//
UTouch  myTouch( 6, 5, 4, 3, 2);


// Declare Font Sizes
extern uint8_t SmallFont[];
extern uint8_t BigFont[];

// Define wlcome bitmap image
extern unsigned short HeadPhones_LCD[76800];
extern unsigned short MUTE_ON[1704];
extern unsigned short MUTE_OFF[1704];
extern unsigned short VOL_UP[1704];
extern unsigned short VOL_DOWN[1704];
extern unsigned short LCD_CAL[1704];

uint32_t cx, cy;
uint32_t rx[8], ry[8];
uint32_t clx, crx, cty, cby;
float px, py;
int dispx, dispy, text_y_center;
uint32_t calx, caly, cals;
char buf[13];



// Set up the LCD screen 
void setup()
{
  // Initialize the LCD
  myGLCD.InitLCD();
  
  // Clear the screen
  myGLCD.clrScr();
  
  // Set the initial font size
  myGLCD.setFont(SmallFont);

  // Set touch orientaiton
  myTouch.InitTouch(TOUCH_ORIENTATION);
  myTouch.setPrecision(PREC_MEDIUM);
  dispx=myGLCD.getDisplayXSize();
  dispy=myGLCD.getDisplayYSize();
  text_y_center=(dispy/2)-6;
  
  // Display the Startup Logo
  startup();
}

void drawCrossHair(int x, int y)
{
  myGLCD.drawRect(x-10, y-10, x+10, y+10);
  myGLCD.drawLine(x-5, y, x+5, y);
  myGLCD.drawLine(x, y-5, x, y+5);
}

// Read touch coordinate for calibration
void readCoordinates()
{
  int iter = 5000;
  int failcount = 0;
  int cnt = 0;
  uint32_t tx=0;
  uint32_t ty=0;
  boolean OK = false;
  
  while (OK == false)
  {
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("*  PRESS  *", CENTER, text_y_center);
    while (myTouch.dataAvailable() == false) {}
    myGLCD.print("*  HOLD!  *", CENTER, text_y_center);
    while ((myTouch.dataAvailable() == true) && (cnt<iter) && (failcount<10000))
    {
      myTouch.calibrateRead();
      if (!((myTouch.TP_X==65535) || (myTouch.TP_Y==65535)))
      {
        tx += myTouch.TP_X;
        ty += myTouch.TP_Y;
        cnt++;
      }
      else
        failcount++;
    }
    if (cnt>=iter)
    {
      OK = true;
    }
    else
    {
      tx = 0;
      ty = 0;
      cnt = 0;
    }
    if (failcount>=10000)
      fail();
  }

  cx = tx / iter;
  cy = ty / iter;
}

// Calibrate the screen
void calibrate(int x, int y, int i)
{
  myGLCD.setColor(255, 255, 255);
  drawCrossHair(x,y);
  myGLCD.setBackColor(255, 0, 0);
  readCoordinates();
  myGLCD.setColor(255, 255, 255);
  myGLCD.print("* RELEASE *", CENTER, text_y_center);
  myGLCD.setColor(80, 80, 80);
  drawCrossHair(x,y);
  rx[i]=cx;
  ry[i]=cy;
  while (myTouch.dataAvailable() == true) {}
}

// Wait for a touch event 
void waitForTouch()
{
  while (myTouch.dataAvailable() == true) {}
  while (myTouch.dataAvailable() == false) {}
  while (myTouch.dataAvailable() == true) {}
}

void toHex(uint32_t num)
{
  buf[0] = '0';
  buf[1] = 'x';
  buf[10] = 'U';
  buf[11] = 'L';
  buf[12] = 0;
  for (int zz=9; zz>1; zz--)
  {
    if ((num & 0xF) > 9)
      buf[zz] = (num & 0xF) + 55;
    else
      buf[zz] = (num & 0xF) + 48;
    num=num>>4;
  }
}

// Initiate welcome screen 
void startup()
{
  myGLCD.setColor(255, 0, 0);
  myGLCD.setBackColor(0, 0, 0);
  
  myGLCD.fillScr(255, 255, 255);
  myGLCD.setColor(255, 255, 255);
   
  
  
  myGLCD.drawBitmap (0,0,320,240, HeadPhones_LCD, 1);
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.print(" Portable Headphone Amplifier V1.0 ", CENTER, 185);
  myGLCD.print(" **Tap to Continue** ", CENTER, 200);
  
  // Wait for user action 
  waitForTouch();
  myGLCD.clrScr();
}

void done()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.drawLine(0, 14, dispx-1, 14);
  myGLCD.print("UTouch Calibration", CENTER, 1);
  myGLCD.setBackColor(0, 0, 0);
  
  if (dispx==220)
  {  
    myGLCD.print("To use the new calibration", LEFT, 30);
    myGLCD.print("settings you must edit the", LEFT, 42);
    myGLCD.setColor(160, 160, 255);
    myGLCD.print("UTouchCD.h", LEFT, 54);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("file and change", 88, 54);
    myGLCD.print("the following values. The", LEFT, 66);
    myGLCD.print("values are located right", LEFT, 78);
    myGLCD.print("below the opening comment.", LEFT, 90);
    myGLCD.print("CAL_X", LEFT, 110);
    myGLCD.print("CAL_Y", LEFT, 122);
    myGLCD.print("CAL_S", LEFT, 134);
    toHex(calx);
    myGLCD.print(buf, 75, 110);
    toHex(caly);
    myGLCD.print(buf, 75, 122);
    toHex(cals);
    myGLCD.print(buf, 75, 134);
  }
  else
  {  
    myGLCD.print("CALIBRATION COMPLETE", CENTER, 30);
    myGLCD.print("To use the new calibration", LEFT, 50);
    myGLCD.print("settings you must edit the", LEFT, 62);
    myGLCD.setColor(160, 160, 255);
    myGLCD.print("UTouchCD.h", LEFT, 74);
    myGLCD.setColor(255, 255, 255);
    myGLCD.print("file and change", 88, 74);
    myGLCD.print("the following values.", LEFT, 86);
    myGLCD.print("The values are located right", LEFT, 98);
    myGLCD.print("below the opening comment in", LEFT, 110);
    myGLCD.print("the file.", LEFT, 122);
    myGLCD.print("CAL_X", LEFT, 150);
    myGLCD.print("CAL_Y", LEFT, 162);
    myGLCD.print("CAL_S", LEFT, 174);

    toHex(calx);
    myGLCD.print(buf, 75, 150);
    toHex(caly);
    myGLCD.print(buf, 75, 162);
    toHex(cals);
    myGLCD.print(buf, 75, 174);
  }
  
}

void fail()
{
  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, dispx-1, 13);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.drawLine(0, 14, dispx-1, 14);
  myGLCD.print("UTouch Calibration FAILED", CENTER, 1);
  myGLCD.setBackColor(0, 0, 0);
  
  myGLCD.print("Unable to read the position", LEFT, 30);
  myGLCD.print("of the press. This is a", LEFT, 42);
  myGLCD.print("hardware issue and can", 88, 54);
  myGLCD.print("not be corrected in", LEFT, 66);
  myGLCD.print("software.", LEFT, 78);
  
  while(true) {};
}

// Run Calibration Routine
void calibrateScreen()
{
  myGLCD.setColor(80, 80, 80);
  drawCrossHair(dispx-11, 10);
  drawCrossHair(dispx/2, 10);
  drawCrossHair(10, 10);
  drawCrossHair(dispx-11, dispy/2);
  drawCrossHair(10, dispy/2);
  drawCrossHair(dispx-11, dispy-11);
  drawCrossHair(dispx/2, dispy-11);
  drawCrossHair(10, dispy-11);
  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("***********", CENTER, text_y_center-12);
  myGLCD.print("***********", CENTER, text_y_center+12);

  calibrate(10, 10, 0);
  calibrate(10, dispy/2, 1);
  calibrate(10, dispy-11, 2);
  calibrate(dispx/2, 10, 3);
  calibrate(dispx/2, dispy-11, 4);
  calibrate(dispx-11, 10, 5);
  calibrate(dispx-11, dispy/2, 6);
  calibrate(dispx-11, dispy-11, 7);
  
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals=(long(dispx-1)<<12)+(dispy-1);
  else
    cals=(long(dispy-1)<<12)+(dispx-1);

  if (TOUCH_ORIENTATION == PORTRAIT)
    px = abs(((float(rx[2]+rx[4]+rx[7])/3)-(float(rx[0]+rx[3]+rx[5])/3))/(dispy-20));  // PORTRAIT
  else
    px = abs(((float(rx[5]+rx[6]+rx[7])/3)-(float(rx[0]+rx[1]+rx[2])/3))/(dispy-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    clx = (((rx[0]+rx[3]+rx[5])/3));  // PORTRAIT
    crx = (((rx[2]+rx[4]+rx[7])/3));  // PORTRAIT
  }
  else
  {
    clx = (((rx[0]+rx[1]+rx[2])/3));  // LANDSCAPE
    crx = (((rx[5]+rx[6]+rx[7])/3));  // LANDSCAPE
  }
  if (clx<crx)
  {
    clx = clx - (px*10);
    crx = crx + (px*10);
  }
  else
  {
    clx = clx + (px*10);
    crx = crx - (px*10);
  }
  
  if (TOUCH_ORIENTATION == PORTRAIT)
    py = abs(((float(ry[5]+ry[6]+ry[7])/3)-(float(ry[0]+ry[1]+ry[2])/3))/(dispx-20));  // PORTRAIT
  else
    py = abs(((float(ry[0]+ry[3]+ry[5])/3)-(float(ry[2]+ry[4]+ry[7])/3))/(dispx-20));  // LANDSCAPE

  if (TOUCH_ORIENTATION == PORTRAIT)
  {
    cty = (((ry[5]+ry[6]+ry[7])/3));  // PORTRAIT
    cby = (((ry[0]+ry[1]+ry[2])/3));  // PORTRAIT
  }
  else
  {
    cty = (((ry[0]+ry[3]+ry[5])/3));  // LANDSCAPE
    cby = (((ry[2]+ry[4]+ry[7])/3));  // LANDSCAPE
  }
  if (cty<cby)
  {
    cty = cty - (py*10);
    cby = cby + (py*10);
  }
  else
  {
    cty = cty + (py*10);
    cby = cby - (py*10);
  }
  
  calx = (long(clx)<<14) + long(crx);
  caly = (long(cty)<<14) + long(cby);
  if (TOUCH_ORIENTATION == LANDSCAPE)
    cals = cals + (1L<<31);

  done();
}

bool MUTE = false;

// Display the options menu
void loop()
{  
  
  delay(200);
  // Set text and Background color
  myGLCD.setColor(0, 0, 0);
  myGLCD.setBackColor(255, 255, 255);
  myGLCD.fillScr(255, 255, 255);

  // Display the main menu buttons
  myGLCD.drawBitmap (250,20,71,24, LCD_CAL, 1);
  myGLCD.drawBitmap (240,65,71,24, VOL_UP, 1);
  myGLCD.drawBitmap (240,110,71,24, VOL_DOWN, 1);
  if(MUTE == true)
    myGLCD.drawBitmap (240,155,71,24, MUTE_ON, 1);
  else{
    myGLCD.drawBitmap (240,155,71,24, MUTE_OFF, 1);
  }
  //Get last touch position
  long X_val;
  long Y_val;
  
  while(myTouch.dataAvailable()!= true)  // Wait for touch event
  while(myTouch.dataAvailable()== true){ // Determine Coordinates
  myTouch.read();//reads touch
  X_val = myTouch.getX();
  Y_val = myTouch.getY();
  delay(500);
  myGLCD.print("X:", 10, 185);
  myGLCD.print("Y:", 10, 195);
  myGLCD.printNumI(X_val,30,185);
  myGLCD.printNumI(Y_val,30,195);  
  myGLCD.print("Last Command: ", 10, 205);
  
  // Determine which button was pressed
  if(X_val > 60 && X_val < 95 && Y_val > 5 && Y_val < 80){
    myGLCD.print("Volume Up", RIGHT, 205);
    MUTE = false; // toggle mute
  }
  else if(X_val > 105 && X_val < 140 && Y_val > 5 && Y_val < 80){
    myGLCD.print("Volume Down", RIGHT, 205);
    MUTE = false; // toggle mute
}
  else if(X_val > 150 && X_val < 185 && Y_val > 5 && Y_val < 80){
    myGLCD.print("Mute", RIGHT, 205);
    MUTE = !MUTE; // toggle mute
  }
}

  
  
  //while(true) {}
}
