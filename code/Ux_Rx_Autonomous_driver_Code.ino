#define enA 9
#define in1 4
#define in2 5
#define enB 10
#define in3 6
#define in4 7
int motorSpeedA = 0;
int motorSpeedB = 0;

int rx1;
int rx2;
int rx3;
int diff1;
int diff2; 

void moveLeft();
void moveRight();
void moveStraight();
 
void setup() {
  Serial.begin(9600);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT);
  pinMode(in4, OUTPUT);

  digitalWrite(in1,LOW);
  digitalWrite(in2,HIGH);
  digitalWrite(in3,LOW);
  digitalWrite(in4,HIGH);
  
}

void loop() {
 
  rx1 = analogRead(A0);
  rx2 = analogRead(A1);
  rx3 = analogRead(A2);                                            
  diff1 = rx1-rx2;
  diff2 = rx3-rx2;
  Serial.println(rx1);
  Serial.println(rx2);
  Serial.println( rx3);
  

delay(2000);
           
if( abs(diff1 - diff2) < 50 )          //  Move Straight
    {
        moveStraight();       
     }
else if((diff1 - diff2) > 50 )                  // Turn Left
    { 
      moveRight();
    }  
else                               // Turn Right
   {
      moveLeft();
    }
    


  
  
}

void moveStraight()
{
  Serial.println("Straight");
   motorSpeedA = 255;
   motorSpeedB = 255; 
   analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
   analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
   
}

void moveLeft()
{
  Serial.println("Left");
  if( (abs(diff1) < 50) && (rx2 > rx1) )
      {  motorSpeedA = 200;
         motorSpeedB = 255;    }
   
   if( (abs(diff1) < 50) && (rx2 < rx1) )
      {  motorSpeedA = 150;
         motorSpeedB = 255;    }

   if( (abs(diff1) > 50) && (rx3 < 20 ) && (rx2 < 100) )
      {  motorSpeedA = 75;
         motorSpeedB = 255;    }         
   analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
   analogWrite(enB, motorSpeedB); // Send PWM signal to motor B 
           
}

void moveRight()
{
  Serial.println("Right");
  if( (abs(diff2) < 50) && (rx2 > rx3) )
      {  motorSpeedA = 200;
         motorSpeedB = 255;    }
   
  if( (abs(diff2) < 50) && (rx2 < rx3) )
      {  motorSpeedA = 150;
         motorSpeedB = 255;    }

  if( (abs(diff2) > 50) && (rx1 < 20 ) && (rx2 < 100) )
      {  motorSpeedA = 75;
         motorSpeedB = 255;    }
  analogWrite(enA, motorSpeedA); // Send PWM signal to motor A
  analogWrite(enB, motorSpeedB); // Send PWM signal to motor B
  
}  
