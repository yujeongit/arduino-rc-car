#include <Servo.h>

/*
  L9110S 모터드라이버 연결

  오른쪽 모터
  L9110S A_1A - D6
  L9110S A_1B - D11

  왼쪽 모터
  L9110S B_1A - D3
  L9110S B_1B - D5
*/

int A_1A = 6;
int A_1B = 11;
int B_1A = 3;
int B_1B = 5;

/*
  초음파센서 연결

  TRIG - D8
  ECHO - D9
*/

int trigPin = 8;
int echoPin = 9;

/*
  서보모터 연결

  OUT - D10
*/

int servoPin = 10;

Servo servo;

/*
  모터 속도 설정
  값 범위: 0 ~ 255
*/

int motorASpeed = 150;
int motorBSpeed = 150;

/*
  장애물 판단 거리 35cm 이내
*/

int obstacleDistance = 35;

void setup() {
  Serial.begin(9600);

  pinMode(A_1A, OUTPUT);
  pinMode(A_1B, OUTPUT);
  pinMode(B_1A, OUTPUT);
  pinMode(B_1B, OUTPUT);

  stopCar();

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  servo.attach(servoPin);

  // 서보모터를 정면으로 설정
  servo.write(90);
  delay(1000);

  Serial.println("RC car start");
}

void loop() {
  float cm = getStableDistanceCM();

  Serial.print("Front distance: ");
  Serial.print(cm);
  Serial.println(" cm");

  if (cm < obstacleDistance) {
    avoidObstacle();
  } else {
    forward();
  }

  delay(50);
}

/*
  장애물 회피 동작
*/

void avoidObstacle() {
  float leftDistance = 0;
  float rightDistance = 0;

  stopCar();
  delay(300);

  // 왼쪽 거리 측정
  servo.write(150);
  delay(500);
  leftDistance = getStableDistanceCM();

  Serial.print("Left distance: ");
  Serial.print(leftDistance);
  Serial.println(" cm");

  delay(200);

  // 오른쪽 거리 측정
  servo.write(30);
  delay(500);
  rightDistance = getStableDistanceCM();

  Serial.print("Right distance: ");
  Serial.print(rightDistance);
  Serial.println(" cm");

  delay(200);

  // 서보모터를 다시 정면으로
  servo.write(90);
  delay(300);

  // 살짝 후진
  back();
  delay(350);
  stopCar();
  delay(200);

  // 더 넓은 쪽으로 회전
  if (leftDistance > rightDistance) {
    left();
    delay(550);
  } else {
    right();
    delay(550);
  }

  stopCar();
  delay(200);
}

/*
  초음파센서 거리 측정 함수

  pulseIn에 timeout을 추가
  허공을 0cm로 착각해서 멈추는 문제 발생 → 신호를 못 받으면 999cm로 처리
*/

float getDistanceCM() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH, 25000);

  if (duration == 0) {
    return 999;
  }

  float distance = duration / 29.0 / 2.0;

  return distance;
}

/*
  안정적인 거리 측정 함수

  여러 번 측정해서 평균 냄
  너무 빠르게 연속 측정하지 않도록 delay 추가
*/

float getStableDistanceCM() {
  float cmSum = 0;
  int sampleCount = 5;

  for (int i = 0; i < sampleCount; i++) {
    cmSum += getDistanceCM();
    delay(20);
  }

  return cmSum / sampleCount;
}

/*
  RC카 전진
*/

void forward() {
  analogWrite(A_1A, motorASpeed);
  analogWrite(A_1B, 0);

  analogWrite(B_1A, motorBSpeed);
  analogWrite(B_1B, 0);
}

/*
  RC카 후진
*/

void back() {
  analogWrite(A_1A, 0);
  analogWrite(A_1B, motorASpeed);

  analogWrite(B_1A, 0);
  analogWrite(B_1B, motorBSpeed);
}

/*
  RC카 좌회전

  오른쪽 모터 전진, 왼쪽 모터 정지 방식
*/

void left() {
  analogWrite(A_1A, motorASpeed);
  analogWrite(A_1B, 0);

  analogWrite(B_1A, 0);
  analogWrite(B_1B, 0);
}

/*
  RC카 우회전

  왼쪽 모터 전진, 오른쪽 모터 정지
*/

void right() {
  analogWrite(A_1A, 0);
  analogWrite(A_1B, 0);

  analogWrite(B_1A, motorBSpeed);
  analogWrite(B_1B, 0);
}

/*
  RC카 정지
  기존 stop() 이름은 Arduino 내부 함수와 헷갈려서 stopCar()로 바꿈
*/

void stopCar() {
  analogWrite(A_1A, 0);
  analogWrite(A_1B, 0);

  analogWrite(B_1A, 0);
  analogWrite(B_1B, 0);
}
