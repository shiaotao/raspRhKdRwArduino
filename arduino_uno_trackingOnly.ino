/*
NoBombCar_findwayOnly.ino
排爆小车程序（循迹测试）
@Jerry_316
2021.04.23
v0.9.1
*/

#include <stdio.h>

//*defence variables
const int power_left_h = 5;     //马达引脚编号
const int power_left_b = 6;     //马达引脚编号
const int power_right_h = 10;
const int power_right_b = 9;
const int power_back_h = 7;
const int power_back_b = 8;

const int green_left_pin = A2;  //灰度传感器引脚编号
const int green_right_pin = A3;

int gray_left;      //灰度传感器输入值（赋值变量）
int gray_right;
const int gray_left_point = 49;     //灰度传感器阈值（255）
const int gray_right_point = 31;

// put your setup code here, to run once:
void setup() {
    pinMode(power_left_h, OUTPUT);
    pinMode(power_left_b, OUTPUT);
    pinMode(power_right_h, OUTPUT);
    pinMode(power_right_b, OUTPUT);
    pinMode(power_back_h, OUTPUT);
    pinMode(power_back_b, OUTPUT);
    pinMode(green_left_pin, INPUT);
    pinMode(green_right_pin, INPUT);
    delay(3000);    //开机延时3s启动
}

// put your main code here, to run repeatedly:
void loop() {
    //获取灰度传感器模拟输入（1023）循迹
    gray_left = analogRead(green_left_pin);
    gray_left = map(gray_left, 0, 1023, 0, 255);    // map it to the range of the analog out
    gray_right = analogRead(green_right_pin);
    gray_right = map(gray_right, 0, 1023, 0, 255);
    if((gray_left < gray_left_point) && (gray_right < gray_right_point)) {
        power_run_full(0);
    } else if((gray_left < gray_left_point) && (gray_right > gray_right_point)) {
        power_run_full(1);
    } else if((gray_left > gray_left_point) && (gray_right < gray_right_point)) {
        power_run_full(2);
    } else{
        power_off_all();    //! the car will stop
    }
}

void power_off_all()
//停车指令
{
    digitalWrite(power_left_h, HIGH);
    digitalWrite(power_left_b, HIGH);
    digitalWrite(power_right_h, HIGH);
    digitalWrite(power_right_b, HIGH);
    digitalWrite(power_back_h, HIGH);
    digitalWrite(power_back_b, HIGH);
    delay(100);
    digitalWrite(power_left_h, LOW);
    digitalWrite(power_left_b, LOW);
    digitalWrite(power_right_h, LOW);
    digitalWrite(power_right_b, LOW);
    digitalWrite(power_back_h, LOW);
    digitalWrite(power_back_b, LOW);
}

void power_run_full(int turn)
//全速运行函数，0 -> 直行、1 -> 向左、2 -> 向右
{
    digitalWrite(power_back_h, HIGH);
    digitalWrite(power_back_b, LOW);
    if(turn == 1) {
        analogWrite(power_left_h, 196);
        digitalWrite(power_left_b, LOW);
        analogWrite(power_right_h, 255);
        digitalWrite(power_right_b, LOW);
    } else if(turn == 2) {
        analogWrite(power_right_h, 196);
        digitalWrite(power_right_b, LOW);
        analogWrite(power_left_h, 255);
        digitalWrite(power_left_b, LOW);
    } else{
        analogWrite(power_left_h, 255);
        digitalWrite(power_left_b, LOW);
        analogWrite(power_right_h, 255);
        digitalWrite(power_right_b, LOW);
    }
    delay(10);
}
