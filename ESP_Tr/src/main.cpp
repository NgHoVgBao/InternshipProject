
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <LiquidCrystal_I2C.h>
#include <string.h>

#define RST_PIN    4    
#define SS_PIN     5    
#define LEDTT        15
#define TXD2 17
#define RXD2 16
#define LEDCHO 2
#define SW1 25
#define SW2 26
#define SW3 27
#define Enter 14
#define del 12


// RFID, Zigbee, LCD, SW, LED

MFRC522 mfrc522(SS_PIN, RST_PIN);
LiquidCrystal_I2C lcd(0x27,16,2);

// 2 led chờ
// 4 RST
// 5 SS
// 12 del
// 14 SW enter
// 15 led trạng thái
// 16 RX 
// 17 TX
// 18 SCK
// 21 SDA
// 22 SCL 
// 19 MISO
// 23 MOSI
// 25 26 27 SWITCH PASSWORD 

int pass=0,xoa=0,tv=0;
int a=0,b=0,c=0;
int saimk=0,reply=0;
int size, cutoff=0;
int a1=2,a2=2,a3=1;

uint8_t buf[9];
uint8_t dlt[9];
uint8_t add[9];
uint8_t unb[9];

char ban[9];
char cmp[9];
char ad[9]="235EFE2C";
char mem[9]="19F40104";
char newmem[9];
char unban[9];
char* list[]={mem,newmem};

bool curE,lstE=HIGH;
bool curD,lstD=HIGH;
bool cur1,lst1=HIGH;
bool cur2,lst2=HIGH;
bool cur3,lst3=HIGH;

void toHexStr(byte *uid, byte len, char *out) {
  for (byte i = 0; i < len; i++) {
    sprintf(&out[i * 2], "%02X", uid[i]);
  }
  out[len * 2] = '\0'; // kết thúc chuỗi
}


void setup() {
  lcd.init();
  lcd.clear();
  Serial.begin(9600);
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);  // UART2 giao tiếp Zigbee
  while (!Serial);            // Đợi Serial sẵn sàng 
  SPI.begin(18, 19, 23, 5);   // SCK, MISO, MOSI, SS 
  mfrc522.PCD_Init();         // Khởi động đầu đọc RFID   
  pinMode(LEDTT, OUTPUT);
  pinMode(LEDCHO, OUTPUT);
  digitalWrite(LEDTT, LOW);
  digitalWrite(LEDCHO, LOW);
  pinMode(SW1, INPUT_PULLUP);
  pinMode(SW2, INPUT_PULLUP);
  pinMode(SW3, INPUT_PULLUP);
  pinMode(Enter, INPUT_PULLUP);
  pinMode(del, INPUT_PULLUP);

}                           // Chờ khoảng vài giây cho hệ thống khởi động


void loop() {
  
  // Nếu có thẻ, kiểm tra xem admin hay member
  // member thì nhập mật khẩu, admin thì thêm, xóa, đổi mật khẩu, truyền tín hiệu
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {             /////////////////
  start:
    int size = mfrc522.uid.size;      // Lưu mã thẻ vào bộ đệm
    for(int i=0;i<size;i++){
      buf[i]=mfrc522.uid.uidByte[i];
    }
  toHexStr(buf, size, cmp);  
  int list_size = sizeof(list) / sizeof(list[0]);
    for(int i=0;i<list_size;i++){
      if(strcmp(cmp,list[i])==0){
        tv++;
        break;
      }
    }

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
  
    while(strncmp(cmp,ban,8)==0){        // nếu mã thẻ trùng với mã thẻ bị cấm thì không làm gì 
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("UID was banned!");
      tv=0;
      delay(1000);
      break;
    }
  
  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    while(strncmp(cmp,ad,8)==0){        // admin thì đổi pass(a), cấm(b), gỡ cấm(c), gửi(enter), trở về(back)
      a=0; b=0; c=0;
      delay(10);
      lcd.setCursor(0,0);
      lcd.print("Hello admin");
      lcd.setCursor(0,1);
      lcd.print("How's everything?");
      delay(10);
      cur1=digitalRead(SW1);
      cur2=digitalRead(SW2);
      cur3=digitalRead(SW3);
      curE=digitalRead(Enter);
      curD=digitalRead(del);
  if(lst1==HIGH && cur1==LOW){   // Đổi pass
      lcd.clear();
    while(1){
      lst1=cur1;
      curE=digitalRead(Enter);
      curD=digitalRead(del);
      lcd.setCursor(0,0);
      lcd.print("Change password?");
      lcd.setCursor(0,1);
      lcd.print(" Confirm   Back");  
    
    if (lstE==HIGH && curE==LOW){
      lcd.clear();
      a1=0; a2=0; a3=0;
      while(pass==0){
        lstE=curE;
      lcd.setCursor(0,0);
      lcd.print(a1);
      lcd.print(a2);
      lcd.print(a3);
      cur1=digitalRead(SW1);
      cur2=digitalRead(SW2);
      cur3=digitalRead(SW3);
      curE=digitalRead(Enter);
      curD=digitalRead(del);
      if(lst1==HIGH && cur1==LOW){
        a1++;
        if(a==10) a=0;
        delay(100);
      }
      lst1=cur1;
      if(lst2==HIGH && cur2==LOW){
        a2++;
        if(b==10) b=0;
        delay(100);
      }
      lst2=cur2;
      if(lst3==HIGH && cur3==LOW){
        a3++;
        if(c==10) c=0;
        delay(100);
      }
      lst3=cur3;
      if(lstE==HIGH && curE==LOW){    // Nhấn enter thì out vòng bắt đầu kiểm tra mật khẩu
       pass++;
       delay(100); 
      }
      lstE=curE;
      if(lstD==HIGH && curD==LOW){    // Xóa hết a,b,c nhập lại từ đầu
       a1=0;
       a2=0;
       a3=0;
       delay(100); 
      }
      lstD=curD;
    }
    pass=0;
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Changed!");
    break; } lstE=curE;
      
    if(lstD==HIGH && curD==LOW){    // Quay về màn hình chính 
      delay(100);
      lcd.clear();
      lstD=curD;
      break;} 
      lstD=curD; }
}
lst1=cur1;

  if(lst2==HIGH && cur2==LOW){   // Cấm thẻ
      lcd.clear();  
      while(1){
      lst2=cur2;
      curE=digitalRead(Enter);
      curD=digitalRead(del);
      lcd.setCursor(0,0);
      lcd.print("Ban card?");
      lcd.setCursor(0,1);
      lcd.print(" Confirm   Back");  
      delay(100);
    if (lstE==HIGH && curE==LOW){
      lcd.clear();
      while(1){
      chothe:
      lstE=curE;
      lcd.setCursor(0,0);
      lcd.print("Insert card");
      if(mfrc522.PICC_IsNewCardPresent()&&mfrc522.PICC_ReadCardSerial()){
        for(int i=0;i<mfrc522.uid.size;i++){
        dlt[i]=mfrc522.uid.uidByte[i]; }
      toHexStr(dlt,mfrc522.uid.size,ban);
      lcd.clear();
      delay(1000);
      if(strncmp(ban,ad,8)==0){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Invalid !");
        memset(ban,0,sizeof(ban));
        goto chothe;
      }
      lcd.setCursor(0,0);
      lcd.print("Completed!");
      delay(1000);
      lcd.clear();
      break;
      }
    }
    memset(cmp,0,sizeof(cmp));
    break; } lstE=curE;
  
    if(lstD==HIGH && curD==LOW){    // Quay về màn hình chính 
      delay(100);
      lcd.clear();
      lstD=curD;
      break; }
      lstD=curD;}  
}
lst2=cur2;
  if(lst3==HIGH && cur3==LOW){   // Gỡ chặn thẻ
      lcd.clear();  
      while(1){
        lst3=cur3;
        curE=digitalRead(Enter);
        curD=digitalRead(del);
        lcd.setCursor(0,0);
        lcd.print("Unban card?");
        lcd.setCursor(0,1);
        lcd.print(" Confirm   Back");  
        delay(100);
    if (lstE==HIGH && curE==LOW){
      lcd.clear();
      while(1){
      lstE=curE;
      lcd.setCursor(0,0);
      lcd.print("Insert card");
      if(mfrc522.PICC_IsNewCardPresent()&&mfrc522.PICC_ReadCardSerial()){
        for(int i=0;i<mfrc522.uid.size;i++){
        unb[i]=mfrc522.uid.uidByte[i]; }
      lcd.clear();
      delay(1000);
      toHexStr(unb,mfrc522.uid.size,unban);
      if(strncmp(unban,ban,8)==0){
      memcpy(ban,0,sizeof(ban));
      memcpy(unban,0,sizeof(unban));
      lcd.setCursor(0,0);
      lcd.print("Completed!");
      delay(1000);
      lcd.clear();
      } else {
      lcd.setCursor(0,0);
      lcd.print("Invalid data!");
      memcpy(unban,0,sizeof(unban));
      delay(1000);
      lcd.clear();
      }
      lcd.clear();
      break;
      }
    }
    memset(cmp,0,sizeof(cmp));
    break; } lstE=curE;
    if(lstD==HIGH && curD==LOW){    // Quay về màn hình chính 
      delay(100);
      lcd.clear();
      lstD=curD;
      break; }
      lstD=curD;}  
}
lst3=cur3;
  if(lstE==HIGH && curE==LOW){   // Truyền mã
      lcd.clear();  
      lstE=curE;
      lcd.setCursor(0,0);
      lcd.print("Transmitting..");
      for(int i=0;i <size;i++){
        Serial2.print(buf[i]<0x10?"0":"");
        Serial2.print(buf[i], HEX); 
      }
      delay(1000);
      lcd.clear();
      lcd.print("Completed!");
      delay(1000);
      lcd.clear();
      while(1){
        lcd.noDisplay();
        lcd.noBacklight();
        if(Serial2.available()){
        reply=Serial2.read();
        if(reply==1) {
        lcd.display();
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Received !");
        delay(500);
        break;}
        }} break;
      }
lstE=curE;
  if(lstD==HIGH && curD==LOW){    // quay về màn hình chính
      lcd.clear();
      lstD=curD;
      delay(100);
      memset(cmp,0,sizeof(cmp));
      break; 
    }
lstD=curD;
        

}

  /////////////////////////////////////////////////////////////////////////////////////////////////////////////////  
    while(tv!=0){        // member thì nhập mật khẩu
    
    lcd.display();
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Loading...");
    delay(1000);
    nhap_matkhau:
    while (pass == 0)
    {
      lcd.setCursor(0,0);             // Nhập mật khẩu abc, hiện LCD
      lcd.print("Password: ");
      lcd.setCursor(0,1);
      lcd.print(a);
      lcd.print(b);
      lcd.print(c);
      digitalWrite(LEDCHO, HIGH);
      cur1=digitalRead(SW1);
      cur2=digitalRead(SW2);
      cur3=digitalRead(SW3);
      curE=digitalRead(Enter);
      curD=digitalRead(del);
      if(lst1==HIGH && cur1==LOW){
        a++;
        if(a==10) a=0;
        delay(100);
      }
      lst1=cur1;
      if(lst2==HIGH && cur2==LOW){
        b++;
        if(b==10) b=0;
        delay(100);
      }
      lst2=cur2;
      if(lst3==HIGH && cur3==LOW){
        c++;
        if(c==10) c=0;
        delay(100);
      }
      lst3=cur3;
      if(lstE==HIGH && curE==LOW){    // Nhấn enter thì out vòng bắt đầu kiểm tra mật khẩu
       pass++;
       delay(100); 
      }
      lstE=curE;
      if(lstD==HIGH && curD==LOW){    // Xóa hết a,b,c nhập lại từ đầu
       a=0;
       b=0;
       c=0;
       delay(100); 
      }
      lstD=curD;
    }
    digitalWrite(LEDCHO, LOW); 
    if((a==a1)&&(b==a2)&&(c==a3)){   // Đúng mật khẩu thì truyền mã RFID đi
      a=0; b=0; c=0;
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Correct");
      digitalWrite(LEDTT, HIGH);
      delay(800);
      digitalWrite(LEDTT, LOW);
      lcd.clear();
      for(int i=0;i <size;i++){
        Serial2.print(buf[i]<0x10?"0":"");
        Serial2.print(buf[i], HEX); 
      }
      lcd.noDisplay();
      lcd.noBacklight();
      while(1){
        if(Serial2.available()){
        reply=Serial2.read();
        if(reply==1) {
        lcd.display();
        lcd.backlight();
        lcd.setCursor(0,0);
        lcd.print("Received !");
        delay(500);
        break;}
        }
      }
      goto tieptuc;                                                   
    } else {
        lcd.clear();
        lcd.setCursor(0,0);       // Sai mật khẩu thì nhảy lên vòng nhập mật khẩu lại từ đầu
        lcd.print("Invalid password");
        delay(1000);
        lcd.clear();
        saimk++;
        a=0; b=0; c=0; pass=0;
        if(saimk==2){
          saimk=0;
          goto tt_bi_hanche;          // Sai 2 lần thì nhảy
        }
        goto nhap_matkhau;
    } 

    tt_bi_hanche:
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Access unable");
    while(1){
      digitalWrite(LEDCHO,HIGH);
      delay(500);
      lcd.clear();      
      lcd.noDisplay();
      lcd.noBacklight();
      Serial2.write(0xFF);
      delay(4500);
      digitalWrite(LEDCHO,LOW);
      tv=0; 
      memset(cmp,0,sizeof(cmp));
      break;
    } }
    tieptuc:
    tv=0;
    pass=0;
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();

  } else {                                                                            /////////////
    // Không có thẻ hoặc không đọc được
    lcd.display();
    lcd.backlight();
    lcd.setCursor(0,0);
    lcd.print("Insert your card");      // Khởi động LCD
  }  
}

