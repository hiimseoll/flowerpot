[![Hits](https://hits.seeyoufarm.com/api/count/incr/badge.svg?url=https%3A%2F%2Fgithub.com%2Fhiimseoll%2Fhiimseoll&count_bg=%237D3D9A&title_bg=%235C2323&icon=&icon_color=%23D34444&title=hits&edge_flat=true)](https://hits.seeyoufarm.com)

씨애랑 소프트웨어 전시회(2020) WALL-E [설동호 홍태성] [우승]
==


## 프로젝트명: WALL-E
영화 월-E에 나오는 장화와 식물을 모티브로 제작.

작중 월-E가 식물이 다치지 않도록 소중히 다루는 모습에서 영감을 얻음.

![walle_ex](/imgs/walle-1.JPG)


## 팀 화분 제작소
<img src="/imgs/potFactoryLogo1.png" width="100" height="100">

## 팀원:

### 20205189 설동호 (팀장)
  + 아두이노 프로그래밍.
  + 센서, 모듈 및 부품을 활용해 하드웨어 구상 및 제작.

### 20205278 홍태성
  + 웹 프로그래밍.
  + 3D 모델링 작업을 통해 작품의 외관 설계 및 출력

## 시연 영상
   https://www.youtube.com/watch?v=_7uzHuX7X4Q&feature=youtu.be

## 제작 동기:
  오랫동안 집을 비운 뒤 들어왔을 때 시들어있는 식물을 보고, 자동으로 식물을 관리해 주는 화분이 있으면 좋겠다는 생각에 프로젝트를 계획.
  
  집에 푸르름을 더하고 싶지만 제대로 관리할 자신이 없는 사람들을 위한 화분.
 
 

## 프로젝트 설명:

###   사용 기술(프로그램, 언어 등):

####    하드웨어

+ 아두이노
  + ESP8266 와이파이 모듈
  + 토양습도 센서
  + 조도센서(CDS)
  + DHT11 온습도 센서
  + 워터펌프(릴레이 모듈)
  + 블로워 팬
+ 전원
  + 리튬 폴리머 배터리
  + 충방전 모듈(완충 보호)
  + 태양전지판
+ 외관
  + Fusion 360
  + CUBICON 3D 프린터

#### 서버

+ 리눅스 우분투
  + 아파치 HTTP 서버

#### 웹

+ html/css
+ php
+ JavaScript

### 연결도:
  
![connection1](/imgs/connection1.JPG)


## 작동 과정:


<img src="/imgs/ppt1.gif" width="600">


### 기능 설명:

#### 아두이노 (하드웨어):
조도, 온습도, 토양습도 센서를 통해 측정한 값을 와이파이 모듈을 통해 서버로 전달한다.

서버의 설정값에 따라 물을 자동 또는 사용자가 설정한 주기에 맞춰 펌프를 작동시켜 공급한다.

블로워 팬은 한시간에 한 번 10초가량 작동된다.

물이 다 떨어지면 오류 코드를 서버로 전송해 사용자가 물을 재공급 할 수 있도록 한다.

태양전지를 통해 리튬 배터리를 충전하고 아두이노 전원으로 사용한다. 만일의 경우를 대비하여 C 타입 충전 내장하고 있다.

로커스위치로 전원을 제어한다.

#### 웹, 서버:
서버는 리눅스 우분투 OS를 사용하며 아파치 HTTP 서버로 구동된다.

아두이노로부터 받은 데이터를 차트로 표현한다.

데이터를 저장하여 평균값 그래프를 제공한다.

설정 탭을 통해 화분의 물 공급 주기를 설정한다.

#### 외관:
식물이 있는 화분은 본체에서 따로 분리할 수 있다.

화분 밑부분을 열어 물통을 꺼내고 물을 다시 채울 수 있다.


## 사진:


<img src="/imgs/arduino1.jpg" width="250"><img src="/imgs/arduino4.jpg" width="250"><img src="/imgs/arduino3.jpg" width="250"><br><br>

<img src="/imgs/web1.png" width="250"> <img src="/imgs/web2.png" width="250"> <img src="/imgs/web3.png" width="250">

<br>웹 인터페이스<br>

<img src="/imgs/model1.png" width="300"> <img src="/imgs/model2.png" width="300">
<br>3D 모델<br>


Q&A-> [theseol16@gmail.com]
