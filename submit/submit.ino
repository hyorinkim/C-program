#include <SoftwareSerial.h>

#define BT_RXD 8
#define BT_TXD 7

SoftwareSerial gpsSerial(11,12);
SoftwareSerial bluetooth(BT_RXD,BT_TXD);

/* DC 모터 돌리기 */
int touch=0;
int valKnock=0;
int valKnock_count=0;
const int motorPin = 9;     //상수 선언 : 모터와 연결된 핀 번호 저장
const int knockIn=5;

/* GPS,BlueTooth */
char c = ""; // Wn 인지 구분 및 str에 저장.
String str = ""; // \n 전까지 c 값을 저장.
long LatM=0;
long LongM=0;
String targetStr = "GPGGA"; // str의 값이 NMEA의 GPGGA 값인지 타겟

void setup()
{
  pinMode(motorPin, OUTPUT);      //9번핀을 출력용으로 설정

  pinMode(5,INPUT);
  Serial.begin(9600);
  Serial.println("Start GPS... ");
  bluetooth.begin(9600);  
  gpsSerial.begin(9600);
}


void loop()
{   
  touch=analogRead(A0);
  
  spiningMotor();              //모터 작동시키는 함수 호출
   
  valKnock=digitalRead(5);
  GPSwithbluetooth();
}

/*
 모터 작동시키는 함수
 : 입력받은 값을 시리얼 포트를 이용해 보내고 그에 맞는 속도로 모터가 돕니다.
*/
void spiningMotor()
{ 
  if(touch<700){                        //터치 했을때 값이 줄어들어서 500이하면 모터가 돌아간다.

    knockSensor();
  }else{
    analogWrite(motorPin,0);            //건들지 마시오.
    valKnock_count=0;
  }
   
}

void knockSensor()
{
  if(valKnock==LOW && valKnock_count==0){                 //노크가 들어옴(누를떄가 low)
    analogWrite(motorPin, 100); 
    Serial.println("1단계11111111111111111111");
    valKnock_count=1;
    delay(1000);                                          //valKnock를 HIGH로 바꾸기위한 시간 딜레이
    delay(1000);
    
  }else if(valKnock==LOW && valKnock_count==1){
    analogWrite(motorPin, 175);                             //speed만큼으로 모터 돌리기(속도 2단계)
    Serial.println("2단계22222222222222222222");
    valKnock_count=2;
    delay(1000);
    delay(1000);
  }else if(valKnock==LOW && valKnock_count==2){
    analogWrite(motorPin, 250);                             //speed만큼으로 모터 돌리기(속도 2단계)
    Serial.println("3단계33333333333333333333");
    valKnock_count=0;
    delay(1000);
    delay(1000);
  }
}

void GPSwithbluetooth(){
    if(gpsSerial.available()) // gps 센서 통신 가능?
    {
      c=gpsSerial.read(); // 센서의 값 읽기
      if(c == '\n'){ // \n 값인지 구분.
        // \n 일시. 지금까지 저장된 str 값이 targetStr과 맞는지 구분

        if(targetStr.equals(str.substring(1, 6))){

          // NMEA 의 GPGGA 값일시
          Serial.println(str);

          // , 를 토큰으로서 파싱.
          int first = str.indexOf(",");
          int two = str.indexOf(",", first+1);
          int three = str.indexOf(",", two+1);
          int four = str.indexOf(",", three+1);
          int five = str.indexOf(",", four+1);

          // Lat과 Long 위치에 있는 값들을 index로 추출

          String Lat = str.substring(two+1, three);
          String Long = str.substring(four+1, five);

          // Lat의 앞값과 뒷값을 구분

          String Lat1 = Lat.substring(0, 2);
          String Lat2 = Lat.substring(2);

          // Long의 앞값과 뒷값을 구분

          String Long1 = Long.substring(0, 3);
          String Long2 = Long.substring(3);

          // 좌표 계산.

          double LatF = ((Lat1.toDouble() + Lat2.toDouble()/60)*100000);
          float LongF = ((Long1.toFloat() + Long2.toFloat()/60)*100000);
          Serial.println(LatF);
          Serial.println(LongF);
          LatM=floor(LatF*10);
          LongM=floor(LongF*10);
               
         String location = "";
         location.concat(LatM);
         location.concat(LongM);
         
         Serial.println(location);
         //Serial.println(LongM);
          bluetooth.print(location);     
          //delay(1000);   
          
        }

        // str 값 초기화 
        str = "";
      }else{ // \n 아닐시, str에 문자를 계속 더하기
        str += c;
      }

    }
}
 
