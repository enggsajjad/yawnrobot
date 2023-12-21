
#define CLK 28
#define DT 27

int counter = 0;
int currentStateCLK;
int lastStateCLK;
String currentDir ="0";
int x=0;

void setup() 
{
  pinMode(CLK,INPUT_PULLUP);
  pinMode(DT,INPUT_PULLUP);
  Serial.begin(115200);
   
  lastStateCLK = digitalRead(CLK);
  Serial.print("Direction: ");
  Serial.print(currentDir);
  Serial.print(" | Counter: ");
  Serial.println(counter);
  
}

void loop() {
  
 
  lastStateCLK = currentStateCLK; 
  currentStateCLK = digitalRead(CLK);
    
 if (currentStateCLK != lastStateCLK  && currentStateCLK == 1)
 {

     if (digitalRead(DT) != currentStateCLK) 
     {
       currentDir ="CW";
      x++;
     } 
    else
    {
       currentDir ="CCW";
      x--;
    }

    Serial.print("Direction: ");
    Serial.print(currentDir);
    Serial.print(" | Counter: ");
    Serial.println(x);
 

 return;
}


  
  lastStateCLK = currentStateCLK;


  delay(1);
}
