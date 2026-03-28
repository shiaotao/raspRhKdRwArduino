#include <Servo.h>

Servo servo_cir;    //创建一个舵机控制对象,使用Servo类最多可以控制8个舵机
Servo servo_put;
//int pos = 0;        // 该变量用与存储舵机角度位置
char var;


void setup() {
  // put your setup code here, to run once:
  servo_cir.attach(5);  // 该舵机由arduino第5脚控制
  servo_put.attach(6);
  Serial.begin(9600);
  while(Serial.available()>0)
  Serial.read();
}

void loop() {
  // put your main code here, to run repeatedly:
  while(Serial.available()==1) {   //当有信号的时候
    var = Serial.read();
    if(var == '0') {
      put_in(0);
    }else if(var == '1') {
      put_in(80);
    }else if(var == '2') {
      put_in(130);
    }else if(var == '3') {
      put_in(180);
    }

}
  //while(Serial.available()>0)
    //Serial.read();
}



void put_in(int anagle) {
  int pos = 0;
  for(pos = 0; pos < anagle;) {  // 从0度到180度运动
        servo_cir.write(pos);   // 指定舵机转向的角度
        delay(15);  // 等待15ms让舵机到达指定位置
        pos += 1;
  }
  for(pos = 90; pos > 0;) {  // 从0度到180度运动
        servo_put.write(pos);   // 指定舵机转向的角度
        delay(15);  // 等待15ms让舵机到达指定位置
        pos -= 1;
  }
  for(pos = 0; pos < 90;) {  // 从0度到180度运动
        servo_put.write(pos);   // 指定舵机转向的角度
        delay(15);  // 等待15ms让舵机到达指定位置
        pos += 1;
  }
  delay(900);
  for(pos = anagle; pos > 0;) {  // 从0度到180度运动
        servo_cir.write(pos);   // 指定舵机转向的角度
        delay(15);  // 等待15ms让舵机到达指定位置
        pos -= 1;
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
