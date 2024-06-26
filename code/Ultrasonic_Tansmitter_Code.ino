void setup() 
{
    Serial.begin(9600);
    pinMode(3,OUTPUT);
}

void loop() 
{
   send("Hello World\n\n");   
}

void send(String msg)
{
   byte ch;
   unsigned int pos = 0;
   unsigned int sz = msg.length();
   while(pos<sz)
   {
      ch = msg.charAt(pos);
      Serial.print((char)ch);
      tone(3,40000);
      delay(10);
      noTone(3);
      for(int i=0;i<8;i++)
      {
         boolean b;
         b = bitRead(ch,7-i);
         if(b) 
         {
            tone(3,40000);
            delay(2);
         }
         else
         {
            tone(3,40000);
            delay(4);
         }
         noTone(3);
         delay(11);
      }
      pos++;
   }
}
