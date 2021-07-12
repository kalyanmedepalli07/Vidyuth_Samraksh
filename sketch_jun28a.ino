#include <SoftwareSerial.h>
#include <LiquidCrystal.h>
//-----------------------------------------------------------------------------------------------------------------------------------
SoftwareSerial esp(7, 8);
LiquidCrystal lcd( 12,11, 2,3, 4, 5);
unsigned char j=0;
unsigned int p1,q1;
unsigned int x,y,z,p,q;

int s1p=1;
int temp=0;
float xpow,xkwh,ckwh,xcost,ccost,x1,y1;
float time=0;
const int bzr=13;
const int rly=6;
const int s2=10;
//---------------------------------------------------------------------------
String pow_level_string = "";
String kwh_level_string = "";
String cost_level_string = "";
String ApiKey = "X614V1KYN1XCLJ92";
String field1 = "&field1=";
String field2 = "&field2=";
String field3 = "&field3=";
//-----------------------------------------------------------------------------------------------------------------------------------
void setup()
{
pinMode(bzr,OUTPUT);
pinMode(rly,OUTPUT);
digitalWrite(bzr,0);
digitalWrite(rly,1);
lcd.begin(16,2);
  
  Serial.begin(9600);
  esp.begin(115200);
  
  lcd.begin(16,2);
  esp.println("AT+RST");
  
lcd.clear();
lcd.setCursor(0,0);  
lcd.print("VIDYUTH     ");   
lcd.setCursor(0,1);  
lcd.print("  SAMRAKSH  ");
delay(5000);
}
//-----------------------------------------------------------------------------------------------------------------------------------
void loop()
{
  sense_loop();
  post(); 
  delay(700);
if (ckwh>1)
 {
    if(ckwh<1.6)
    {
       delay(1000);  
       lcd.clear();
       lcd.setCursor(0,0);  
       lcd.print("SENDING SMS..");   
       lcd.setCursor(0,1);  
       lcd.print("UNITS OVER DRAWN");
       digitalWrite(bzr,1);
       sendSMS();
       digitalWrite(bzr,0);
       temp=0;
    }
}
 delay(1000);
}
//-----------------------------------------------------------------------------------------------------------------------------------
void post()
{
  String cmd = "AT+CIPSTART=\"TCP\",\"";
  cmd += "184.106.153.149"; // api.thingspeak.com
  cmd += "\",80";
  esp.println(cmd);

  if(esp.find("ERROR"))
  {
  //Serial.println("AT+CIPSTART error");
  
  return;
  }
  delay(100);
  //------------------------add fields--------------------
  String getStr = "GET /update?api_key=";     
  getStr += ApiKey;
  getStr += field1;
  getStr += pow_level_string;
  
  getStr += field2;
  getStr += kwh_level_string;
  
  getStr += field3;
  getStr += cost_level_string;
  getStr += "\r\n\r\n";
 // Serial.print(getStr);
 //------------------------------------------------------
  cmd = "AT+CIPSEND=";
  cmd += String(getStr.length());
  
  //cmd += String(str.length());
  esp.println(cmd);
  
  if(esp.find(">"))
  {
  esp.print(getStr);
  //esp.print(str);
    
  delay(2000);
  }
}
//-----------------------------------------------------------------------------------------------------------------------------------
void sense_loop()
{
  temp=0;
for(z=0;z<5;z++)
{  

 x=analogRead(0);
 y=analogRead(1);
 
  x1=map(x,0,1023,0,255);
  y1=map(y,0,1023,0,255);
  xpow=((x1/400)*y1);
  xkwh=(((xpow)/1000)); 
  xcost=(xkwh*5);
  
  ccost=ccost+xcost;
  ckwh=ckwh+xkwh;
  lcd.clear();
  lcd.setCursor(0,0);  
  lcd.print("P:       K:     ");   
  lcd.setCursor(0,1);  
  lcd.print("COST:           ");
  
  lcd.setCursor(2,0);
  lcd.print("      ");
  lcd.setCursor(12,0);
  lcd.print("    ");
  lcd.setCursor(6,1);
  lcd.print("         ");
    
  lcd.setCursor(2,0);
  lcd.print(x1);
  
  lcd.setCursor(12,0);
  lcd.print(ckwh);

  lcd.setCursor(6,1);
  lcd.print(ccost);
  delay(2000);
}
if (x1>160)
   { 
    digitalWrite(bzr,1); 
    digitalWrite(rly,0); 
    delay(500);
   }
   else
   { 
    digitalWrite(bzr,0);
    digitalWrite(rly,1);  
   }
   pow_level_string = String (x1) ;
   kwh_level_string = String (ckwh) ;
   cost_level_string = String (ccost) ; 
   }
// **  SUB FUNCTIONS **
void sendSMS()
{
  for(j=0;j<2;j++)
  {
  Serial.print("AT+CMGF=1\r");                                                        // AT command to send SMS message
  delay(2000);
  if(j==0)
  {
  Serial.println("AT+CMGS=\"+919391200601\"");  // recipient's mobile number, in international format
  }
  if(j==1)
  {
  Serial.println("AT+CMGS=\"+919848324067\"");
  }
  delay(2000);
  if(ckwh>=1.5)
  {
    Serial.print("Consumed more energy than limit");
  }
  else
  {
  Serial.print("You are about to reach the limit..  Power:");        // message to send
  Serial.println (x1);
  Serial.print("UNITS(kWh) :");
  Serial.println(ckwh);
  Serial.print("Total cost :");
  Serial.println(ccost);
  Serial.print("Use remaining ");
  Serial.print(1.5-ckwh);
  Serial.print(" kWh");
  Serial.print(" efficiently.");
  }
  delay(2000);
  Serial.println((char)26);                       // End AT command with a ^Z, ASCII code 26
  delay(2000); 
  Serial.println();
  delay(2000);
}
}
//******
