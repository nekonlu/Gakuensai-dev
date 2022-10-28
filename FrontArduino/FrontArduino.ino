#include <LiquidCrystal.h>
#include <Wire.h>
#define ENOUGH_NUM 8
LiquidCrystal lcd(12,11,10,5,4,3,2);

void setup(){
    lcd.begin( 16, 2 );
    lcd.clear();
    lcd.setCursor(0, 0);
    //Serial.begin�̒l�͐e�@�Ɉˑ����ĕύX����B
    Serial.begin(9600);
    while(!Serial);
}
void loop(){
    int i=0;
    int t_ind;
    int m_ind;
    char data[ENOUGH_NUM];
    if (Serial.available()>0){
        // data=Serial.readStringUntil(0);
        for(i=0;data[i]!=',';i++){
            t_ind=i+2;
        }
        for(i=t_ind;data[i]!=',';i++){
            m_ind=i+2;
        }
        lcd.setCursor(0, 0);
        if (data[m_ind]=='0'){  //start
            lcd.print("Please start.");
        }
        else if(data[m_ind]=='1'){  //whilegame
            for(i=0;data[i]!=' ';i++){
                lcd.print(data[i]);
            }
            lcd.setCursor(1, 0);
            for(i=m_ind;data[i]!=' ';i++){
                lcd.print(data[i]);
            }
        }
        else if(data[m_ind]=='2'){  //finish
            lcd.print("end the game.");
        }
        else{
            lcd.println("error.");
            lcd.print(data);
        }
    }
    delay(1000);
}