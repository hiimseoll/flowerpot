#include <DHT11.h>
#include <SoftwareSerial.h>
#include <EEPROM.h>

#define RXD 2
#define TXD 3
SoftwareSerial wifi(RXD, TXD);

//URL, WIFI
String url = "xxx.xxx.xxx.xxx";  //데이터 수신 및 송신에 쓰이는 URL
String port = "xxxxx";  //데이터 수신 및 송신에 쓰이는 PORT
String ssid = "";    //wifi ssid
String apPass = "";  //wifi password 비번 없으면 ""

//digital pins
int dht = 4;
int led = 5;
int pump = 6;
int fan = 7;

//analog pins
int cds = A0;
int soil = A2;

//변수
unsigned long int fanStart= 0, fanEnd = 0;// 시작, 끝 / 주기 시작, 주기  끝
unsigned long int periodConv = 0, periSt = 0, periEn = 0;
String str, getStr, dataInput, period;    //str, dataInput, getStr <- 데이터 송수신용 문자열  / period <- 물 공급 주기
boolean errCode[6] = {false, false, false, false, false, false}; 
// wifi에러, 서버연결에러, 송신에러, 수신에러, 펌프에러, 임시;
//  wifiErr  servErr   tranErr  receErr  pumpErr tempErr
boolean doItNow = false;  // 바로 물 주기 (기본 false);
boolean pumpState = true;//펌프 상태(true: 정상, false: 비정상(물 부족))
boolean fanFlag = false;
String p[3];  //tempPeriod

float temp, humi;
int soilHumi, cdsValue;
//  온도   습도   토양습도    조도센서 

unsigned long time;

DHT11 dht11(dht); 

void setup(){//셋업
  
  if(EEPROM.read(0) == 255) period = "000";
  else{
    for(int i=0; i<3; i++) period += EEPROM.read(i);
    periodConv = EEPROM.read(3);
  }
  periSt = millis();
  
  Serial.begin(9600);
  wifi.begin(9600);
  //pinMode(relay, OUTPUT);
  pinMode(led, OUTPUT);
  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);

  wifiConnect();
  dataIn();
}

void connServer(){
  delay(300);
  printAndDelay("AT+CWMODE=3", 300, false);
  printAndDelay("AT+CIPSTART=\"TCP\",\"" + url + "\"," + port, 1000, true);
  str = wifi.readStringUntil('U');

  Serial.println(str);

  if(str.substring(str.indexOf('O'), str.indexOf('O')+6) == "ONNECT"){
    Serial.println("Server Connection OK\n");
  }
  else{
    Serial.println("Server Connection ERROR\n");
    errCodeSave("servErr");
  }
}

void errCodeSave(String err){
  if(err.equals("wifiErr")) errCode[0] = true;
  else if(err.equals("servErr")) errCode[1] = true;
  else if(err.equals("tranErr")) errCode[2] = true;
  else if(err.equals("receErr")) errCode[3] = true;
  else if(err.equals("pumpErr")) errCode[4] = true;
  else if(err.equals("tempErr")) errCode[5] = true;
  else Serial.println("Wrong ERROR code");

  Serial.print(err);
  Serial.println(" saved");
}

void clearSerial(){         //와이파이 연결 보조 함수 (시리얼 버퍼 지우기)
  while(wifi.available() > 0){
    wifi.read();
  }
}

void printAndDelay(String strr, int t, boolean b){   //와이파이 연결 보조 함수 (시리얼 출력 보조)
  wifi.print(strr);
  delay(100);
  if(b)clearSerial();
  wifi.println();
  delay(t);
}

void wifiConnect(){       //와이파이 연결
  clearSerial();
  delay(500);
  printAndDelay("AT", 200, false);
  str = wifi.readStringUntil('U');

  if(str.substring(str.indexOf('O'), str.indexOf('O') + 2) == "OK"){
    Serial.println("WIFI OK");
  }
  else{
    Serial.println("WIFI ERROR");
    errCodeSave("wifiErr");
  }
  
  printAndDelay("AT+CWMODE=1", 200, false);
  printAndDelay("AT+CWJAP=\"" + ssid + "\",\"" + apPass + "\"", 8000, false);
  printAndDelay("AT+CIFSR", 200, true);
  str = wifi.readStringUntil('U');
  
  if(str.substring(str.indexOf('0'), str.indexOf('0') + 7) != "0.0.0.0"){
    Serial.println("WIFI CONNECTING (No IP -> FAILURE)");
  }
  else{
    Serial.println("WIFI CONNECTION ERROR");
  }
  
  Serial.print("IP ADDRESS: ");
  Serial.println(str);
}


void dataOut(){    //센서 데이터 수집(컨트롤) 및 전송
  lightRun();
  humidTempRun();
  soilHumidRun();
  Serial.print("errCode :");
  
  getStr = "GET /input.php?cdsVal=" + String(map(cdsValue, 0, 1023, 0, 100));
  getStr += "&humi=" + String(humi) + "&temp=" + String(temp);
  getStr += "&soilHumi=" + String(map(soilHumi, 0, 1023, 100, 0)) + "&errCode=";
  for(int i=0; i<6; i++){
    getStr += String(errCode[i]);
    Serial.print(String(errCode[i]));
  }
  Serial.println();
  getStr += "\r\n\r\n";

  connServer();
  delay(300);
  wifi.print("AT+CIPSEND=");
  wifi.println(getStr.length()+1);
  delay(100);
  printAndDelay(getStr, 300, true);
  str = wifi.readStringUntil('U');

  if(str.substring(str.indexOf('S'), str.indexOf('K')+1) == "SEND OK"){
    Serial.println("Data Transmitting OK");
  }
  else{
    Serial.println("Data Transmitting ERROR");
    errCodeSave("tranErr");
  }
  delay(500);
  printAndDelay("AT+CIPCLOSE", 300, false);
}

void dataIn(){    // 데이터 수신
  connServer();
  delay(300);
  getStr = "GET /aaa1phpa=1\r\n\r\n";
  wifi.print("AT+CIPSEND=");
  wifi.println(getStr.length()+1);
  delay(100);
  clearSerial();
  str = "";

  printAndDelay(getStr, 100, false);
  str = wifi.readStringUntil('U');
  delay(100);

  str.trim();
  dataInput = str.substring(str.indexOf('d')+1, str.indexOf('d')+6);
  Serial.print("\ndata from server: ");
  Serial.println(dataInput);
  
  period = dataInput.substring(0, 3);
  if(!pumpState){
    pumpState = (dataInput.charAt(3) == '1') ? true : false;
    errCode[4] = !pumpState;
  }
  doItNow = (dataInput.charAt(4) == '1') ? true : false;
  Serial.println();
  Serial.print("period: ");
  Serial.println(period);
  Serial.print("pumpState: ");
  Serial.println(pumpState);
  Serial.print("doItNow: ");
  Serial.println(doItNow);

  for(int i=0; i<3; i++) EEPROM.write(i, period.charAt(i));
  
  periodConv = 0;
  if(period != "000"){

    for(int i=0; i<3; i++)p[i] = "";
    for(int i=0; i<3; i++)p[i] += period.charAt(i);

    for(int i=0; i<((p[0].toInt()*30) + (p[1].toInt()*10) + p[2].toInt()); i++){
      periodConv += 86400;
    }
    EEPROM.write(3, periodConv);
  }
  
  printAndDelay("AT+CIPCLOSE", 300, false);
}

int humidTempRun(){ //온습도
  int err;
  if((err = dht11.read(humi, temp)) == 0){
    Serial.print("temperature:");
    Serial.print(temp);
    Serial.print(" humidity:");
    Serial.print(humi);
    Serial.println();
  }
  else{
    Serial.println();
    Serial.print("Error No :");
    Serial.print(err);
    Serial.println();    
  }  
}

void lightRun(){ //조도
  cdsValue = analogRead(cds);
  Serial.print("cds: ");
  Serial.println(cdsValue);

  if(cdsValue < 30){
    digitalWrite(led, HIGH);
    Serial.println("LIGHT ON");
    /*
    if(!pumpState){                 //임시 펌프 오류 해제
      pumpState = true;
      Serial.println("\nCHECKED!!");
    }*/
  }
  else {
    digitalWrite(led, LOW);
  }
}

void soilHumidRun(){            // 토양 습도 센서
  soilHumi = analogRead(soil);
  Serial.print("Soil Humidity: ");
  Serial.println(soilHumi);

  if(pumpState){

    if(soilHumi >= 1000){
      Serial.println("\nLOW SOIL HUMIDITY\n");
      if(period == "000"){
        waterPumpRun(false);
      }
      else{
        Serial.println(millis());
        Serial.println(periSt);
        Serial.println(periodConv);
        
        if(millis() / 1000 - periSt > periodConv) waterPumpRun(false);
        else Serial.println("Waiting for period");
      }
    }
    if(doItNow){
      doItNow = false;
      waterPumpRun(true);
    }
  }
  else Serial.println("\nWater pump has been disabled.\n");
}

void waterPumpRun(boolean flag){       //워터펌프 작동 (릴레이 모듈)
  int cnt = 0;
  int startTime, endTime;
  
  digitalWrite(pump, HIGH);
  startTime = millis();
  
  Serial.println("Water pump ON");
  
  
  if(!flag){
    while(true){
      if(analogRead(soil) <= 800){    //소요시간 측정
        digitalWrite(pump, LOW);
        Serial.println("Compeleted!");
        Serial.print((endTime - startTime) / 1000);
        Serial.println(" secs");
        pumpState = true;
        break;
      }
      else{
        Serial.print(".");
        cnt++;
      }
      if(cnt >= 25){
        Serial.println();
        cnt = 0;
      }
      endTime = millis();
  
      if((endTime - startTime) > 5000){     // 펌프가 5초 이상 작동되면 강제종료
        digitalWrite(pump, LOW);
        Serial.println("Pump malfunction or lack of water.");
        errCodeSave("pumpErr");
        pumpState = false;
        break;
      }
      delay(100);
    }
    periSt = millis();
  }
  else{
    while(true){
      endTime = millis();
      if(endTime - startTime > 1500){
        digitalWrite(pump, LOW);
        pumpState = true;
        break;
      }
      delay(100);
    }
  }
}

void fanRun(){  //블로워팬 작동
  fanEnd = millis();
  if((fanEnd - fanStart) / 1000 >= 3600){
    if(fanFlag){
      Serial.println("FAN OFF");
      digitalWrite(fan, LOW);
      fanStart = millis();
    }
    else{
      Serial.println("FAN ON");
      digitalWrite(fan, HIGH);
    }
    fanFlag = !fanFlag;
  }
}

void loop(){    //루프
  Serial.println();
  dataOut();
  dataIn();
  fanRun();
  delay(1000);
  clearSerial();
}
