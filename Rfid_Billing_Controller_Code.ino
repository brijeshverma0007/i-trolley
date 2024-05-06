
#include <SPI.h>
#include <MFRC522.h>
#include<LiquidCrystal.h>
#include <SoftwareSerial.h>

#define SS_PIN 10
#define RST_PIN 9
#define BUZZER 2
#define Rbutton 5      // remove product
#define Bbutton 6       // bill 
String ssid="Note9";                // Wifi network SSID
String password ="prashant97";           // Wifi network password
boolean DEBUG = true;               //show additional data
int responseTime = 1000;           //communication timeout

LiquidCrystal lcd(14,15,16,17,18,19);        // declare pin on 20x4 LCD
SoftwareSerial wifiSerial(7,8);              // RX, TX of arduino
MFRC522 mfrc522(SS_PIN, RST_PIN);              // Create MFRC522 instance.

int getProductId(String );
int checkList(int );
String scanProduct();
void addProduct(String);
void removeProduct(String);
void displayBill();
void displayDatabase();

void StartServer();
void generateBill();
void sendToWeb(String d);
String sendToWifi(String command, const int timeout, boolean debug);
String addInHtmlTable(String NAME, int COST, int QTY);

struct products
{
  String tag;
  String pname;
  int pid;
  int cost;
  int quantity ;
  };
  struct products list[]={ {" 2C E0 15 AA","Pen",1,25,0} ,
                           {" 94 81 54 13", "Book",2,100,0},
                           {" E9 90 8D 48","soap",3,69,0} , 
                           {" 5B 55 6C AA","extra",4,20,0} 
                          };

struct products current[10];
int item = 0;
int total=0;
String code4 = "<body><h1 align=center> SHOPPING BILL <h1><br><table id=\"t01\"><tr><th>NAME</th><th>COST</th><th>QTY</th></tr>";
String code5 = "";

 
void setup() 
{ 
  Serial.begin(9600);   // Initiate a serial communication
  while (!Serial) {
     Serial.println(".");  }           // wait for serial port to connect.  
  Serial.println("Serial started");

  wifiSerial.begin(9600);
  while (!wifiSerial) {
      Serial.println(".");    }         // wait for serial port to connect.  
  Serial.println("esp connected");
  
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  
  lcd.begin(20,4);
  lcd.clear();
  lcd.print("      I-TROLLEY   ");
  delay(1000);
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("NAME COST QTY TOTAL");
  
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);
  pinMode(Rbutton,INPUT);
  pinMode(Bbutton,INPUT);
  digitalWrite(Rbutton,HIGH);
  digitalWrite(Bbutton,HIGH);   

  displayDatabase();

}

void loop() 
{ int i=0,j=-1,k,pid;

 if ( ! mfrc522.PICC_IsNewCardPresent())                // Look for new cards
  {
    return;
  }
 
  if ( ! mfrc522.PICC_ReadCardSerial())                    // Select one of the cards
  {
    return;
  }
  
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  tone(BUZZER, 500);
  delay(300);
  noTone(BUZZER);

/*  if(digitalRead(Bbutton)==0)
    { Serial.println("Bbutton pressed");
      StartServer();
      generateBill();
      
      }   */
      
  if( (digitalRead(Rbutton)==0) && (content != "") )
  { 
        removeProduct(content);
  }
  if( content != "")
    { 
          addProduct(content);     }

    if(digitalRead(Bbutton)==0)
    { Serial.println("Bbutton pressed");
      StartServer();
      generateBill();      
      }  
      
   displayBill(); 

} 

int getProductId(String tag)
{ int i;
  String temp;
  
  for(i=0;i<4;i++)
  {    
    temp = list[i].tag;
         if(tag == temp)
         {  
               return list[i].pid;}
          }
  }
  
 int checkList(int pid)
 {  int i;
    for(i=0; i<item ;i++)
    {  
      if(  pid == current[i].pid )
         {
             return i; 
            }   
      }
    return -1;  
  } 

String scanProduct()
{   
  
  if ( ! mfrc522.PICC_IsNewCardPresent())                // Look for new cards
  {
    return;
  }
 
  if ( ! mfrc522.PICC_ReadCardSerial())                    // Select one of the cards
  {
    return;
  }
  
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  content.toUpperCase();
  tone(BUZZER, 500);
   delay(300);
   noTone(BUZZER);
  return content;
  }

void addProduct(String content)
{ int pid,j;

     pid=getProductId(content);  
     j=checkList(pid);  
  
     if(j<0)
    {
      item++; 
      current[item-1].pname = list[pid-1].pname;
      current[item-1].pid = list[pid-1].pid;
      current[item-1].cost = list[pid-1].cost;
      current[item-1].quantity = 1; 
      total += list[pid-1].cost;       
     }
     else
      {
        current[j].cost = current[j].cost + list[pid-1].cost ;
        current[j].quantity++ ;
        total += list[pid-1].cost; 
        } 
  
}

void removeProduct(String content)
{   int j,pid;
     pid=getProductId(content);  
     j=checkList(pid);  
     if(current[j].quantity > 0 )
     {
           current[j].cost = current[j].cost - list[pid-1].cost ;
           current[j].quantity-- ;
           total -= list[pid-1].cost; 
     }   
  }

void displayDatabase()
{
  Serial.println();
  Serial.println("**********PRODUCT DATABASE****************");
  int i;
  Serial.println("NAME      ID   COST");
  for(i=0; i<4; i++)
    {
       Serial.print(list[i].pname);
       Serial.print("   ");
        Serial.print(list[i].pid);
       Serial.print("   ");
       Serial.print(list[i].cost);
       Serial.println("   ");
      }
      Serial.println("*****************************************");
 Serial.println("Put your card to the reader...");
  }  

void displayBill()
{ int i;
   Serial.println("=======================================");
  Serial.println("NAME        COST   QUANTITY");
  for(i=0; i<item; i++)
    {  
     if(current[i].quantity > 0 ){ 
       Serial.print(current[i].pname);
       Serial.print("   ");
       lcd.setCursor(0,i+1);
       lcd.print(current[i].pname);
       Serial.print(current[i].cost);
       Serial.print("   ");
       lcd.setCursor(5,i+1);
       lcd.print(current[i].cost);
       Serial.println(current[i].quantity);
       lcd.setCursor(10,i+1);
       lcd.print(current[i].quantity);
      }}
      Serial.print("\ntotal = ");
      Serial.println(total);
      lcd.setCursor(14,1);
       lcd.print(total);
  }

void StartServer()
{   Serial.println("Start server");
  sendToWifi("AT",responseTime,DEBUG);
  sendToWifi("AT+CWMODE=1",responseTime,DEBUG);                             // configure as Station mode
  sendToWifi("AT+CWJAP=\""+ssid+"\",\""+password+"\"",responseTime,DEBUG);  // connect to wifi
  delay(5000);
  sendToWifi("AT+CIFSR",responseTime,DEBUG);                                // get ip address
  sendToWifi("AT+CIPMUX=1",responseTime,DEBUG);                             // configure for single connections
  sendToWifi("AT+CIPSERVER=1,80",1000,DEBUG);                               // turn on server on port 80
  delay(500);
  Serial.println("Wifi connection is running Server Started!");    
}

void generateBill()
{   Serial.println("...Bill Generation ...");  
 
  String code4 = "<body><h1 align=center> SHOPPING BILL <h1><br><table id=\"t01\"><tr><th>NAME</th><th>COST</th><th>QTY</th></tr>";
  String code5 = ""; 

  while(1){
  while(wifiSerial.available())
   {
      if(wifiSerial.find("0,CONNECT"))
      {   Serial.println("if ke andar");  
         sendToWeb(code4);
         delay(400);
         for(int i=0 ; i<item ; i++)
         {  code5 = addInHtmlTable(current[i].pname, current[i].cost, current[i].quantity); 
           Serial.println(code5);
           code5 = "";  
          }
           
         code5 += "<td colspan=\"3\"> <b>TOTAL :  Rs<b>";
         code5 += total;
         code5 +="</td></table></body></html>";      
         sendToWeb(code5);
         delay(400);
         tone(BUZZER, 500);
         delay(600);
         noTone(BUZZER);
         delay(200);
         tone(BUZZER, 500);
         delay(300);
         noTone(BUZZER);
          delay(1000);
         while(1)
         {  ///   WAIT......................
         }    
    }}}    
    
 }

 void sendToWeb(String d)
{
   String cipSend = " AT+CIPSEND=0,";
   int len = d.length() + 4;
   cipSend +=len;
   //cipSend +="\r\n";
   sendToWifi(cipSend,1000,DEBUG);
   sendToWifi(d,1000,DEBUG); 
}

String sendToWifi(String command, const int timeout, boolean debug)
{
   String response = "";
   command+="\r\n\r\n";
   wifiSerial.print(command);
   long int time = millis();
   while( (time+timeout) > millis())
    {
       while(wifiSerial.available())
        {
           char c = wifiSerial.read(); // read the next character.
           response+=c;
         }  
     }
                
    if(debug)
     {
        Serial.print(response); //displays the esp response messages in arduino Serial monitor
     }
    return response;
}

String addInHtmlTable(String NAME, int COST, int QTY)
{  String str = "<tr><td>";
   str += NAME;
   str += "</td><td>";
   str += COST;
   str += "</td><td>";
   str += QTY;
   str += "</td></tr>";
   return str; 
  }
