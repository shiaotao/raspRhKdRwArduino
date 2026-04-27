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

#define S0 7        // 颜色识别传感器
#define S1 8
#define S2 11
#define S3 12
#define OUT A5
//#define OE 3

volatile float color_count = 0;               //脉冲计数器（中断函数变量声明为volatile型）
float r_coe = 0.11, g_coe = 0.11, b_coe = 0.08;   //白平衡系数    //TODO 调整白平衡系数

#define N_D_COLOR   0   // 颜色返回值定义
#define RED_COLOR   1
#define GREEN_COLOR 2
#define BLUE_COLOR  3


// put your setup code here, to run once:
void setup() {
    pinMode(power_left, OUTPUT);
    pinMode(power_right, OUTPUT);
    pinMode(power_back_h, OUTPUT);
    pinMode(power_back_b, OUTPUT);
    // 灰度传感器初始化
    pinMode(gray_middle_left_pin, INPUT);
    pinMode(gray_middle_right_pin, INPUT);
    pinMode(gray_left_side_pin, INPUT);
    pinMode(gray_right_side_pin, INPUT);
    // TCS230初始化
    pinMode(S0, OUTPUT);
    pinMode(S1, OUTPUT);
    pinMode(S2, OUTPUT);
    pinMode(S3, OUTPUT);
    pinMode(OUT, INPUT);
    digitalWrite(S0, HIGH);  // 缩放输出率为20%
    digitalWrite(S1, LOW);
    digitalWrite(S2, HIGH);  // 不选择滤波器
    digitalWrite(S3, LOW);
    delay(333);
    // 运行舵机测试
    Servo_move_above(0, 160);
    delay(999);    // 开机延时1s启动
}


// put your main code here, to run repeatedly:
void loop() {
    gray_middle_left = Gray_read_map(gray_middle_left_pin);
    gray_middle_right = Gray_read_map(gray_middle_right_pin);
    gray_left_side = Gray_read_map(gray_left_side_pin);
    gray_right_side = Gray_read_map(gray_right_side_pin);
    if((gray_middle_left < gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side > gray_left_side_point) && (gray_right_side > gray_right_side_point)) {
        power_run_full(0);
    } else if((gray_middle_left < gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side < gray_left_side_point) && (gray_right_side < gray_right_side_point)) {
        // 跨越障碍物过程
        power_run_full(0);
    } else if((gray_middle_left < gray_middle_left_point) && (gray_middle_right > gray_middle_right_point) && (gray_left_side > gray_left_side_point) && (gray_right_side > gray_right_side_point)) {
        // 向右微偏，向左微调
        power_run_full(1);
    } else if((gray_middle_left > gray_middle_left_point) && (gray_middle_right < gray_middle_right_point) && (gray_left_side > gray_left_side_point) && (gray_right_side > gray_right_side_point)) {
        // 向左微偏，向右微调
        power_run_full(2);
    } else if((gray_middle_right > gray_middle_right_point) && (gray_right_side < gray_right_side_point)) {
        // 严重右偏情况
        power_run_full(10);
    } else if((gray_middle_left > gray_middle_left_point) && (gray_left_side < gray_left_side_point)) {
        // 严重左偏情况
        power_run_full(20);
    } else {
        //power_off_all();      //! the car will stop
        power_run_full(0);
    }
    //判断颜色传感器输入信号，确定case值
    //! the back number of color need to be redefence
    if(Color_Test() = 1) {
        power_off_all();
    } else if(Color_Test() = 2) {
        power_off_all();
    } else if(Color_Test() = 3) {
        power_off_all();
    } else {}
}


int Color_Test()
// TCS230颜色识别函数
{
    float rcount = 0, gcount = 0, bcount = 0;  // 脉冲计数
    float r_convert = 0, g_convert = 0, b_convert = 0;  //校准结果
    unsigned long nowtime = 0;
    int i;
    for(i = 0; i < 3;) {
        //* 脉冲计数检测，重复测算3次
        // 红色脉冲；-> R
        color_count = 0;
        digitalWrite(S2, LOW);      // 开启红色滤波器
        digitalWrite(S3, LOW);
        attachInterrupt(digitalPinToInterrupt(OUT), Color_Count, RISING);
        nowtime = millis() + 200;
        while(millis() < nowtime);  //检测0.2秒
        rcount = rcount + color_count;
        detachInterrupt(digitalPinToInterrupt(OUT));//关中断
        // 绿色脉冲；-> G
        color_count = 0;
        digitalWrite(S2, HIGH);
        digitalWrite(S3, HIGH);
        attachInterrupt(digitalPinToInterrupt(OUT), Color_Count, RISING);
        nowtime = millis() + 200;
        while(millis() < nowtime);
        gcount = gcount + color_count;
        detachInterrupt(digitalPinToInterrupt(OUT));
        // 蓝色脉冲；-> B
        color_count = 0;
        digitalWrite(S2, LOW);
        digitalWrite(S3, HIGH);
        attachInterrupt(digitalPinToInterrupt(OUT), Color_Count, RISING);
        nowtime = millis() + 200;
        while(millis() < nowtime);
        bcount = bcount + color_count;
        detachInterrupt(digitalPinToInterrupt(OUT));
        i += 1;
    }
    // 用白平衡系数转换脉冲值
    r_convert = r_coe * (rcount/3);
    g_convert = g_coe * (gcount/3);
    b_convert = b_coe * (bcount/3);
    //* 与阈值比较判断颜色
    //TODO 实验找出白色和黑色的分界点
    if((r_convert>180)&&(g_convert>180)&&(b_convert>180)) {
        // 判断为红色 -> 1
        return RED_COLOR;
    } else if((r_convert<80)&&(g_convert<80)&&(b_convert<80)) {
        // 绿色 -> 2
        return GREEN_COLOR;
    } else if((r_convert > g_convert)&&(r_convert > b_convert)) {
        // 蓝色 -> 3
        return BLUE_COLOR;
    } else {
        return N_D_COLOR;
    }
}

void Gray_read_map(int gray_pin)
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

void Servo_move_above(int angle_0, int angle_1)
// 启动舵机servo_pick，转动指定角度0至1
{
    int pos;
    for(pos = angle_0; pos < angle_1;) {
        servo_pick.write(pos);  // 指定舵机转向的角度
        delay(12);              // 等待15ms让舵机到达指定位置
        pos += 1;
    }
    for(pos = angle_1; pos > angle_0;) {
        servo_pick.write(pos);
        delay(12);
        pos -= 1;
    }
}

void Pick_bomb(int color_case)
// 找到对应气球，舵机机械臂完成排爆
{
    Servo_move_above(0, 60);

}
