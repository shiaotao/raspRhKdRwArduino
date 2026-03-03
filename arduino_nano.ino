#include <Servo.h>

Servo servo_cir;    //创建一个舵机控制对象,使用Servo类最多可以控制8个舵机
int pos = 0;        // 该变量用与存储舵机角度位置
char var;


void setup() {
  // put your setup code here, to run once:
  servo_cir.attach(5);  // 该舵机由arduino第4脚控制
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()>0) { //当有信号的时候
    var = Serial.read();
    if(var == '3') {  //传过来的是0
      // digitalWrite(13, LOW);
      for(pos = 0; pos < 180;) {  // 从0度到180度运动
        servo_cir.write(pos);   // 指定舵机转向的角度
        delay(15);  // 等待15ms让舵机到达指定位置
        pos += 1;
      }
    }
  }
}




/*void loop()
{
  for(pos = 0; pos < 180; pos += 1)  // 从0度到180度运动
  {
    servo_cir.write(pos);     // 指定舵机转向的角度
    delay(15);                // 等待15ms让舵机到达指定位置
  }
}*/
