#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define CLK 2
#define DT 3
#define SW A0

int counter = 0;
int currentStateCLK;
int lastStateCLK, btnState;
String currentDir ="0";
unsigned long lastButtonPress = 0;
int X_CENTER = SCREEN_WIDTH / 2;
int Y_CENTER = SCREEN_HEIGHT -1;
int x1, Y1, x2, y2;
double angle;
float radius=Y_CENTER;
String number[8]={"0","0","100","75","50","25","0","0"};
int x=0;
int k=0;

void setup() 
{
  pinMode(CLK,INPUT_PULLUP);
  pinMode(DT,INPUT_PULLUP);
  pinMode(SW, INPUT_PULLUP);
  Serial.begin(9600);
   if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
  lastStateCLK = digitalRead(CLK);
  Serial.print("Direction: ");
  Serial.print(currentDir);
  Serial.print(" | Counter: ");
  Serial.println(counter);
  display.display();
  delay(100); 
display.clearDisplay();
display.display();
display.setTextColor(SSD1306_WHITE);

display.setTextSize(1); 
  display.clearDisplay();
  display.setTextSize(2); 
  display.setCursor(20,5);
  display.print("Teach Me");
  display.setCursor(10,25);
  display.print("Something");
  display.setCursor(23,45);
  display.print("Project");
  display.display();
  delay(1000);
  display.clearDisplay();
  draw();
  display.display();
}

void loop() {
  
 
  lastStateCLK = currentStateCLK; 
  currentStateCLK = digitalRead(CLK);
  btnState = digitalRead(SW);
   
 if (currentStateCLK != lastStateCLK  && currentStateCLK == 1)
 {

     if (digitalRead(DT) != currentStateCLK) 
     {
     k=1;
      currentDir ="CW";
      x++;
     } 
    else
    {
    k=2;
      currentDir ="CCW";
      x--;
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(x);
      
 //   if(counter<10)
     // display.print("0");
  //  display.print(counter);
  //  display.display();      // Show initial text

 return;
}

  else if(k==1 || k==2)
  {
    if (x<=0)
    x=0;
    if (x>=100)
    x=100;
    k=0;
    Serial.println(x);
    display.setTextSize(1);  
    display.clearDisplay();
    draw();
    display.setTextSize(2);
    display.setCursor(55,35);
    if(x<10)
    display.print("0");
    display.print(x);
   int  j  = map(x, 0, 100, 270, 89);
   
    angle = j;
    angle = angle * 0.0174533;

    x1 = X_CENTER-1 ;
    Y1 = Y_CENTER ;
    if(x>=100)
    Y1 = Y_CENTER -1;
    x2 = X_CENTER + (sin(angle) * (radius-25));
    y2 = Y_CENTER + (cos(angle) * (radius-25));
    display.drawLine(x1,Y1,x2,y2,SSD1306_WHITE);
   display.fillCircle(X_CENTER, Y_CENTER, 3 ,SSD1306_WHITE);
   display.display();
  }
  
  lastStateCLK = currentStateCLK;

  if (btnState == LOW) 
  {
    if (millis() - lastButtonPress > 50) 
    Serial.println("Button pressed!");
    lastButtonPress = millis();
    x=0;
    k=1;
  }
  
  delay(1);
}



void draw()
{
  display.drawCircle(X_CENTER, Y_CENTER, radius ,SSD1306_WHITE);
  
    for(int j=2; j<=6; j++)
   {
    angle = j*45;
    angle = angle * 0.0174533;

    x1 = X_CENTER + (sin(angle) * radius);
    Y1 = Y_CENTER + (cos(angle) * radius);
    x2 = X_CENTER + (sin(angle) * (radius-10));
    y2 = Y_CENTER + (cos(angle) * (radius-10));
    display.drawLine(x1,Y1,x2,y2,SSD1306_WHITE);
    x2 = X_CENTER + (sin(angle) * (radius - 15));
    y2 = Y_CENTER + (cos(angle) * (radius - 15));
    if(j==6)
    display.setCursor(x2-2,y2-6);
    else if(j==2)
    display.setCursor(x2-13,y2-6);
    else
    display.setCursor(x2-2,y2-2);
    display.print(number[j]);
   
     
  }
   display.fillCircle(X_CENTER, Y_CENTER, 3 ,SSD1306_WHITE);
      
      for(int j=10; j<=30; j++)
   {
    angle = j*9;
    angle = angle * 0.0174533;

    x1 = X_CENTER + (sin(angle) * radius);
    Y1 = Y_CENTER + (cos(angle) * radius);
    x2 = X_CENTER + (sin(angle) * (radius-5));
    y2 = Y_CENTER + (cos(angle) * (radius-5));
    display.drawLine(x1,Y1,x2,y2,SSD1306_WHITE);
    
    

   
  
  }
}
