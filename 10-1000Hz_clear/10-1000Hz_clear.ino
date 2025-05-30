// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MLX90393
// This code is designed to work with the MLX90393_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products
 
#include<Wire.h>

double x=0;
double y=0;
double z=0;
double Fx=0;
double Fy=0;
double Fz=0;
double xMag=0;
double yMag=0;
double zMag=0;
int t=0;
int flag=0;
int clear_flag=0;
int zt=0;
int xyt =0;
int update_flag=0;
int Rec_Addr=0;
bool stringComplete = false;     // 检查字符串是否接收完整
String part1 = "";
String part2 = "";
int delay_f = 0;
String inchar;

const int window_size = 20;  // 窗口大小
float xreadings[window_size];  // 存储历史数据
float yreadings[window_size];  // 存储历史数据
float zreadings[window_size];  // 存储历史数据
int currentIndex = 0;
int size=0;
float xtotal = 0;
float ytotal = 0;
float ztotal = 0;
float smoothedValueXY = 0;
float smoothedValueX = 0;
float smoothedValueY = 0;
float smoothedValueZ = 0;

bool correction = false;

const int clearwindowsize = 30;  // 窗口大小
double xclearBuffer[clearwindowsize];
double yclearBuffer[clearwindowsize];
double zclearBuffer[clearwindowsize];
double xyclearBuffer[clearwindowsize];
double maxDiffX = 0;
double maxDiffY = 0;
double maxDiffZ = 0;
double maxDiffXY = 0;
int clearbufferIndex = 0;         // 当前滑动窗口索引
bool clearbufferFilled = false;   // 判断滑动窗口是否已满
// 初始化最大最小值和对应索引
double maxX, minX, maxY, minY, maxZ, minZ, maxXY, minXY;

void setup()
{

// Initialise serial communication, set baud rate = 115200
Serial.begin(115200);

for (int i = 0; i < window_size; i++) {
    xreadings[i] = 0;
    yreadings[i] = 0;
    zreadings[i] = 0;
  }

}

void serialEvent() 
{
  while (Serial.available()) 
  {
  
    inchar = Serial.readString();
  
    // 使用indexOf和substring方法分离字符串
    int commaIndex = inchar.indexOf(',');
    if (commaIndex != -1) {
      part1 = inchar.substring(0, commaIndex);
      part2 = inchar.substring(commaIndex + 1);
      // 打印分离后的字符串
      //Serial.print("Part 1:");
      //Serial.println(part1);
      //Serial.print("Part 2:");
      //Serial.println(part2);
    } else {
      //Serial.println("Error: Incorrect format");
    }


    if (part1 == "0x10")
    {
      Rec_Addr=0x10;
      update_flag=1;
    }
    else if(part1 == "0x11")
    {
      Rec_Addr=0x11;
      update_flag=1;
    }
    else if(part1 == "0x12")
    {
      Rec_Addr=0x12;
      update_flag=1;
    }
    else if(part1 == "0x13")
    {
      Rec_Addr=0x13;
      update_flag=1;
    }
    else if(part1 == "0x14")
    {
      Rec_Addr=0x14;
      update_flag=1;
    }
    else if(part1 == "0x15")
    {
      Rec_Addr=0x15;
      update_flag=1;
    }
    else if(part1 == "0x16")
    {
      Rec_Addr=0x16;
      update_flag=1;
    }
    else if(part1 == "0x17")
    {
      Rec_Addr=0x17;
      update_flag=1;
    }
    else if(part1 == "0x18")
    {
      Rec_Addr=0x18;
      update_flag=1;
    }
    else if(part1 == "0x19")
    {
      Rec_Addr=0x19;
      update_flag=1;
    }
    else if(part1 == "0x1A")
    {
      Rec_Addr=0x1A;
      update_flag=1;
    }
    else if(part1 == "0x1B")
    {
      Rec_Addr=0x1B;
      update_flag=1;
    }
    else if(part1 == "0x0C")
    {
      Rec_Addr=0x0C;
      update_flag=1;
    }
    else if(part1 == "0x0D")
    {
      Rec_Addr=0x0D;
      update_flag=1;
    }
    else if(part1 == "0x0E")
    {
      Rec_Addr=0x0E;
      update_flag=1;
    }
    else if(part1 == "0x0F")
    {
      Rec_Addr=0x0F;
      update_flag=1;
    }
    else
    {
      //Serial.println("Error");
      flag=0;
    }

    if (part2 == "10 Hz")
    {
      delay_f=99000;
    }
    else if(part2 == "30 Hz")
    {
      delay_f=32000;
    }
    else if(part2 == "50 Hz")
    {
      delay_f=18400;
    }
    else if(part2 == "100 Hz")
    {
      delay_f=8700;
    }
    else if(part2 == "300 Hz")
    {
      delay_f=2100;
    }
    else if(part2 == "1000 Hz")
    {
      delay_f=1;
    }
    else
    {
      delay_f=1000000;
    }

  }
   
}

 
void loop()
{
serialEvent();
if (update_flag==1)
{
// Initialise I2C communication as MASTER
  Wire.begin(7, 6);

  Wire.beginTransmission(0x70);
  Wire.write(1 << 2);  // Set the appropriate bit for the channel
  Wire.endTransmission();
  delay(5);

// Start I2C Transmission
Wire.beginTransmission(Rec_Addr);
// Select Write register command
Wire.write(0x60);
// Set AH = 0x00, BIST disabled
Wire.write(0x00);
// Set AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5; 0x7c GAIN_SEL = 7
Wire.write(0x1C);
// Select address register, (0x00 << 2)
Wire.write(0x00);
// Stop I2C Transmission
Wire.endTransmission();
// Request 1 byte of data
Wire.requestFrom(Rec_Addr, 1);
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
// // Start I2C Transmission
// Wire.beginTransmission(Rec_Addr);
// // Select Write register command
// Wire.write(0x60);
// Wire.write(0x04);
// Wire.write(0x00);
// Wire.write(0x04);
// // Stop I2C Transmission
// Wire.endTransmission();
// // Request 1 byte of data
// Wire.requestFrom(Rec_Addr, 1);
// // Read status byte
// if(Wire.available() == 1)
// {
// unsigned int c = Wire.read();
// }
// Start I2C Transmission
Wire.beginTransmission(Rec_Addr);
// Select Write register command
Wire.write(0x60);
// Set AH = 0x02
Wire.write(0x02);
// Set AL = 0xB4, RES for magnetic measurement = 01; AH 0x00 AL 0x14  RES for magnetic measurement = 00;Set AL = 0xB4 Fit=5; A8 Fit 2 OSR 0;A9 Fit 2 OSR 1
Wire.write(0xA8);
// Select address register, (0x02 << 2)
Wire.write(0x08);
// Stop I2C Transmission
Wire.endTransmission();
// Request 1 byte of data
Wire.requestFrom(Rec_Addr, 1);
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
delay(300);

// Start I2C Transmission
Wire.beginTransmission(Rec_Addr);
// Start single meaurement mode, ZYX enabled
Wire.write(0x1E);
// Stop I2C Transmission
Wire.endTransmission();
 
// Request 1 byte of data
Wire.requestFrom(Rec_Addr, 1);
 
// Read status byte
if(Wire.available() == 1)
{
unsigned int c = Wire.read();
}
delay(9);

update_flag=0;
flag=1;
size=0;
t=0;
x=0;
y=0;
z=0;
xtotal=0;
ytotal=0;
ztotal=0;
smoothedValueX = 0;
smoothedValueY = 0;
smoothedValueZ = 0;
for (int a = 0; a < window_size; a++) {
    xreadings[a] = 0;
    yreadings[a] = 0;
    zreadings[a] = 0;
}
for (int b = 0; b < clearwindowsize; b++) {
    xclearBuffer[b] = 0;
    yclearBuffer[b] = 0;
    zclearBuffer[b] = 0;
}
clearbufferIndex=0;
currentIndex=0;
maxDiffX = 0;
maxDiffY = 0;
maxDiffZ = 0;
maxX=0;
minX=0;
maxY=0;
minY=0;
maxZ=0;
minZ=0;
clearbufferFilled = false;   // 判断滑动窗口是否已满
clear_flag=0;

}

if (flag==1)
{
unsigned int data[7];
// Start I2C Transmission
Wire.beginTransmission(Rec_Addr);
// Send read measurement command, ZYX enabled
Wire.write(0x4E);
// Stop I2C Transmission
Wire.endTransmission();

// Request 7 bytes of data
Wire.requestFrom(Rec_Addr, 7);
 
// Read 7 bytes of data
// status, xMag msb, xMag lsb, yMag msb, yMag lsb, zMag msb, zMag lsb
if(Wire.available() == 7);
{
data[0] = Wire.read();
data[1] = Wire.read();
data[2] = Wire.read();
data[3] = Wire.read();
data[4] = Wire.read();
data[5] = Wire.read();
data[6] = Wire.read();
}

// Convert the data
int16_t xValue = data[1] * 256 + data[2];
int16_t yValue = data[3] * 256 + data[4];
int16_t zValue = data[5] * 256 + data[6];

xMag = (xValue);
yMag = (yValue);
zMag = (zValue);

t++;
if(t<10)
{}

else if(t>10 && t<=60)
{
  x=x+xMag;
  y=y+yMag;
  z=z+zMag;
  //Serial.print("t: "); Serial.println(t);
}

else if (t==61)
{
  x=x/50;
  y=y/50;
  z=z/50;
}
else if (t>61)
{
    Fx = xMag-x;
    Fy = yMag-y;
    Fz = zMag-z;

if (t > 100)
{
Serial.print(Fx);
Serial.print(" ");
Serial.print(Fy);
Serial.print(" ");
Serial.println(Fz);
// Serial.print(" ");
// Serial.print(x);
// Serial.print(" ");
// Serial.print(y);
// Serial.print(" ");
// Serial.print(z);
// Serial.print(" ");
// Serial.print(xMag);
// Serial.print(" ");
// Serial.print(yMag);
// Serial.print(" ");
// Serial.println(zMag);
delayMicroseconds(delay_f);
t=101;
}
}
}
else
{
  Fx=random(0,255);
  Fy=random(0,255);
  Fz=random(0,255);
  Serial.print(Fx);
  Serial.print("  ");
  Serial.print(Fy);
  Serial.print("  ");
  Serial.println(Fz);
  delay(1000);
}
}
