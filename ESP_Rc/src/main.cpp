#define BLYNK_TEMPLATE_ID "TMPL6moogXIcp"
#define BLYNK_TEMPLATE_NAME "cutoff"
#define BLYNK_AUTH_TOKEN "-hTypl8xcMhuSBrTW_pRPFwczLG_n4qK"

#include <Arduino.h>
#include <LiquidCrystal_I2C.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <DHT.h>

// Admin được mở cửa và cấp quyền bật tắt đèn, gửi thông báo tới gmail bằng Blynk
// Sau quãng thời gian này thì hết hạn cấp quyền 
// Member được mở cửa, đèn sáng và không có quyền gì khác  

#define warn 4        // Buzzer 
#define LED 5         // LED 
#define wait 12       // LED chờ
#define pin 13        // SG90
#define fb 14         // Nút gửi feedback
#define DHTPIN 15     // Đọc nhiệt độ
#define RXD2 16
#define TXD2 17       // Zigbee
#define door 25       // Nút mở đóng khóa
#define tivi 26       // LCD
#define led_ctrl 27   // Nút bật tắt LED
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);
LiquidCrystal_I2C lcd(0x27,16,2);
uint8_t buf[10];

int idx=0,solanbam=0;
int out_loop=0;
int completed=0, sw=0;

bool curL,lstL=HIGH;  // Light LED
bool curD,lstD=HIGH;  // Door
bool curS,lstS=HIGH;  // Screen TV
bool curF,lstF=HIGH;  // Feedback
bool state=LOW, khoa=LOW;

char ad[9]="235EFE2C";
char mem[9]="19F40104";
char compare[9];
char ssid[]="capheden";
char pass[]="conheosuy";

float t,h;
unsigned long int startTime;

void setup()
{
  lcd.init();
  lcd.noDisplay();
  lcd.noBacklight();
  pinMode(led_ctrl, INPUT_PULLUP);
  pinMode(fb, INPUT_PULLUP);
  pinMode(door, INPUT_PULLUP);
  pinMode(tivi, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  pinMode(wait,OUTPUT);
  pinMode(warn,OUTPUT);
  digitalWrite(warn,LOW);
  digitalWrite(LED,LOW);
  digitalWrite(wait,LOW);
  dht.begin();
  Blynk.begin(BLYNK_AUTH_TOKEN,ssid,pass);
  Serial.begin(9600);  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2); // Zigbee
}

//235EFE2C 19F40104
void loop() {
  // Nhận tín hiệu
Blynk.run();
while (Serial2.available() && (idx < 10)) { 
  Blynk.run();
  buf[idx] = Serial2.read(); 
  digitalWrite(wait,LOW);
  if (buf[0] == 0xFF) {
    digitalWrite(warn,HIGH);
    Blynk.logEvent("warning", "Someone is trying to break into the system");
    delay(4000);
    digitalWrite(warn,LOW);
    idx=0;
    completed=0;
    memset(buf,0,sizeof(buf));
    break;
  }
  idx++;
  completed++;
} 

if(completed>=8) {
  digitalWrite(wait,LOW);
  for(int i=0;i<idx;i++) compare[i]=char(buf[i]);
  if(strncmp(compare,ad,8)==0) {
      Blynk.logEvent("log_in","We've noticed an Admin login from the system. If this is you, skip this email");
      digitalWrite(LED,HIGH);
      delay(1000);
      digitalWrite(LED,LOW);
      while (out_loop==0) { 
        h = dht.readHumidity();
        t = dht.readTemperature();
        Blynk.run();
        curL = digitalRead(led_ctrl);
        curD = digitalRead(door);
        curF = digitalRead(fb);
        curS = digitalRead(tivi);
        if (lstL == HIGH && curL == LOW) {  // LED  
          state = !state;
          digitalWrite(LED,state);
          delay(200);  
        }
        lstL=curL;
        if (lstD == HIGH && curD == LOW) {  // Door  
          khoa = !khoa;
          delay(200);
        }
          lstD=curD;
        if(khoa==HIGH){
          if (isnan(h) && isnan(t)) {
            lcd.setCursor(0,0);
            lcd.print("DHT error");
          } else {
          lcd.setCursor(0,0);
          lcd.print("Temp: "); lcd.print(t); lcd.print(" *C");
          lcd.setCursor(0,1);
          lcd.print("Hum: "); lcd.print(h); lcd.print(" %");      }
        } else {
          lcd.clear();
        }

        if(lstS == HIGH && curS == LOW){
          sw = !sw;
        }
          lstS=curS;
        if(sw==HIGH) {
          lcd.clear();
          lcd.display();                   // Tivi  
          lcd.backlight();
        } else {
          lcd.clear();
          lcd.noBacklight();
          lcd.noDisplay();
        }

        if(lstF == HIGH && curF == LOW){  // Feedback
          lstF=curF;  
          out_loop++;
        }
        lstF=curF;
      }
      memset(compare, 0, sizeof(compare));
      Serial2.write(1);
      completed=0; idx=0;
      }


if(strncmp(compare,mem,8)==0) {
    digitalWrite(wait,LOW);
    digitalWrite(LED,HIGH);
    lcd.backlight();
    lcd.display();
    lcd.setCursor(0,0);
    lcd.print("Welcome !");
    delay(5000);
    lcd.clear();
    lcd.noBacklight();
    lcd.noDisplay();
    memset(compare, 0, sizeof(compare));
    digitalWrite(LED,LOW);
    Serial2.write(1);
    completed=0; idx=0;  
    }
  }
  out_loop=0; sw=0;
  khoa=LOW; solanbam=0;
  state=LOW;
  digitalWrite(wait,HIGH);    
}