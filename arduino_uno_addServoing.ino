/*
NoBombCar_findwayOnly.ino
排爆小车程序
@Jerry_316
2021.04.23
v0.9.16
*/

#include <stdio.h>
#include <Servo.h>


//*defence variables
const int power_left = 5;       // 马达引脚编号
const int power_right = 6;
const int power_back_h = 9;
const int power_back_b = 10;

const int gray_middle_left_pin = A1;    // 灰度传感器引脚编号
const int gray_middle_right_pin = A2;
const int gray_left_side_pin = A3;
const int gray_right_side_pin = A4;

int gray_middle_left;       // 灰度传感器输入值（赋值变量）
int gray_middle_right;
int gray_left_side;
int gray_right_side;
const int gray_middle_left_point = 49;     // 灰度传感器阈值（255）
const int gray_middle_right_point = 31;
const int gray_left_side_point = 64;
const int gray_right_side_point = 64;

Servo servo_pick;   // 定义舵机对象


// put your setup code here, to run once:
void setup() {
    pinMode(power_left, OUTPUT);
    pinMode(power_right, OUTPUT);
    pinMode(power_back_h, OUTPUT);
    pinMode(power_back_b, OUTPUT);
    pinMode(gray_middle_left_pin, INPUT);
    pinMode(gray_middle_right_pin, INPUT);
    pinMode(gray_left_side_pin, INPUT);
    pinMode(gray_right_side_pin, INPUT);
    delay(3000);    // 开机延时3s启动
}


// put your main code here, to run repeatedly:
void loop() {
    gray_middle_left = gray_read_map(gray_middle_left_pin);
    gray_middle_right = gray_read_map(gray_middle_right_pin);
    gray_left_side = gray_read_map(gray_left_side_pin);
    gray_right_side = gray_read_map(gray_right_side_pin);
    if((gray_middle_left < gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side < gray_left_side_point) && (gray_right_side < gray_right_side_point)) {
        power_run_full(0);
    } else if((gray_middle_left < gray_middle_left_point) && (gray_middle_right > gray_middle_right_point) && (gray_left_side < gray_left_side_point) && (gray_right_side < gray_right_side_point)) {
        // 向右微偏，向左微调
        power_run_full(1);
    } else if((gray_middle_left > gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side < gray_left_side_point) && (gray_right_side < gray_right_side_point)) {
        // 向左微偏，向右微调
        power_run_full(2);
    } else if((gray_middle_left < gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side < gray_left_side_point) && (gray_right_side > gray_right_side_point)) {
        // 严重右偏情况
        power_run_full(10);
    } else if((gray_middle_left < gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side > gray_left_side_point) && (gray_right_side < gray_right_side_point)) {
        // 严重左偏情况
        power_run_full(20);
    } else {
        //power_off_all();    //! the car will stop
        power_run_full(0);
    }
}


void gray_read_map(int gray_pin)
// 灰度传感器模拟输入映射到0至255
{
    int gray_in;
    // 获取灰度传感器模拟输入（1023）循迹
    gray_in = analogRead(gray_pin);
    gray_in = map(gray_in, 0, 1023, 0, 255);    // map it to the range of the analog out
    return gray_in;
}

void Color_Count()
// 脉冲计数（中断函数）
{
  color_count++;
}

void power_off_all()
// 刹车后停车（0.01s）
{
    digitalWrite(power_left, LOW);
    digitalWrite(power_right, LOW);
    digitalWrite(power_back_h, HIGH);
    digitalWrite(power_back_b, HIGH);
    delay(10);
    digitalWrite(power_back_h, LOW);
    digitalWrite(power_back_b, LOW);
}

void power_run_full(int turn)
// 全速运行函数，0 -> 直行、1 -> 向左、2 -> 向右
//TODO 大调整延时
{
    digitalWrite(power_back_h, HIGH);
    digitalWrite(power_back_b, LOW);
    if(turn == 1) {
        digitalWrite(power_left, LOW);
        analogWrite(power_right, 196);
        delay(10);
    } else if(turn == 10) {
        digitalWrite(power_left, LOW);
        analogWrite(power_right, 196);
        delay(60);
    } else if(turn == 2) {
        digitalWrite(power_right, LOW);
        analogWrite(power_left, 196);
        delay(10);
    } else if(turn == 20) {
        digitalWrite(power_right, LOW);
        analogWrite(power_left, 196);
        delay(60);
    } else {
        analogWrite(power_left, 196);
        analogWrite(power_right, 130);
        delay(10);
    }
}

void pick_bomb(int angle)
//启动舵机机械臂排爆，指定角度
{
    int pos = 0;
    for(pos = 0; pos < angle;) {
        servo_pick.write(pos);   // 指定舵机转向的角度
        delay(15);              // 等待15ms让舵机到达指定位置
        pos += 1;
    }
    for(pos = angle; pos > 0;) {
        servo_pick.write(pos);
        delay(15);
        pos -= 1;
    }
}