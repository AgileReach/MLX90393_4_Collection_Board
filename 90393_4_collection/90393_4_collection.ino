// Distributed with a free-will license.
// Use it any way you want, profit or free, provided it fits in the licenses of its associated works.
// MLX90393
// This code is designed to work with the MLX90393_I2CS I2C Mini Module available from ControlEverything.com.
// https://www.controleverything.com/products

#include <Wire.h>

// Define I2C pins
#define I2C_SDA 7
#define I2C_SCL 6

// MLX90393 I2C Address is 0x0C(12)
#define MLX_ADDR 0x0C
// PCA9546 I2C multiplexer address
#define PCA9546_ADDR 0x70

// Function to select I2C channel on PCA9546
void selectChannel(uint8_t channel) {
  if (channel > 3) return;  // PCA9546 has 4 channels (0-3)
  Wire.beginTransmission(PCA9546_ADDR);
  Wire.write(1 << channel);  // Set the appropriate bit for the channel
  Wire.endTransmission();
  // Serial.println("Channe l selected");
  // Serial.println(channel);
  delay(2); // Small delay after switching channels
}

double x = 0;
double y = 0;
double z = 0;
double Fx = 0;
double Fy = 0;
double Fz = 0;
int t = 0;

void init_mlx() {
  // Start I2C Transmission
  Wire.beginTransmission(MLX_ADDR);
  Wire.write(0x60); // Write register command
  Wire.write(0x00); // AH = 0x00, BIST disabled
  Wire.write(0x1C); // AL = 0x5C, Hall plate spinning rate = DEFAULT, GAIN_SEL = 5
  Wire.write(0x00); // address register
  Wire.endTransmission();
  Wire.requestFrom(MLX_ADDR, 1);
  if (Wire.available() == 1) Wire.read();

  Wire.beginTransmission(MLX_ADDR);
  Wire.write(0x60);
  Wire.write(0x02); // AH = 0x02
  Wire.write(0xA8); // AL = 0xB4, RES for magnetic measurement = 0
  Wire.write(0x08); // address register
  Wire.endTransmission();
  Wire.requestFrom(MLX_ADDR, 1);
  if (Wire.available() == 1) Wire.read();
  delay(300);
  // Start I2C Transmission
  Wire.beginTransmission(0x0C);
  // Start single meaurement mode, ZYX enabled
  Wire.write(0x1E);
  // Stop I2C Transmission
  Wire.endTransmission();
}

void setup() {
  // Initialize I2C with specific pins
  Serial.begin(115200);
  delay(100);
  Serial.println("MLX90393 x4 with PCA9546 multiplexer test");

  // Initialize all MLX90393 sensors
  for (int board = 2; board < 4; board++) {
    selectChannel(board);
    delay(5);
    init_mlx();
    Serial.print("Initialized MLX90393 on board ");
    Serial.println(board);
  }
  Serial.println("All sensors initialized");
}

void loop() {
  for (int board = 2; board < 4; board++) {
    selectChannel(board);
    delay(5);

    uint8_t data[7];
    // Start I2C Transmission
    // Wire.beginTransmission(MLX_ADDR);
    // Wire.write(0x3E); // Start single measurement mode, ZYX enabled
    // Wire.endTransmission();
    // Wire.requestFrom(MLX_ADDR, 1);
    // if (Wire.available() == 1) Wire.read();
    // delay(10);

    

    Wire.beginTransmission(MLX_ADDR);
    Wire.write(0x4E); // Send read measurement command, ZYX enabled
    Wire.endTransmission();
    Wire.requestFrom(MLX_ADDR, 7);
    if (Wire.available() == 7) {
      data[0] = Wire.read();
      data[1] = Wire.read();
      data[2] = Wire.read();
      data[3] = Wire.read();
      data[4] = Wire.read();
      data[5] = Wire.read();
      data[6] = Wire.read();
    } else {
      Serial.print("Board "); Serial.print(board); Serial.println(": MLX90393 read error");
      continue;
    }

    int xMag = data[1] * 256 + data[2];
    int yMag = data[3] * 256 + data[4];
    int zMag = data[5] * 256 + data[6];

    Serial.print(xMag); Serial.print(",");
    Serial.print(yMag); Serial.print(",");
    Serial.print(zMag); 
    if( board != 3)
      Serial.print(",");
    else
      Serial.print("\r\n");

    delay(10);
  }
  delay(100); // Small delay between reading cycles
}

