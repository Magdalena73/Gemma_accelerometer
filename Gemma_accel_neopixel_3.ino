/********************************************************************************************
 *  The Gemma M0 needs the Adafruit SAMD (32-bits ARM Cortex-M0+ and Cortex-M4) Boards Library.
 *  It uses the AVRISP mkII programmer.
 *  double clik on the Gemma M0 Reset button TWICE before uploading the programm!!
 *  download following libraries: Wire, Adafruit_Sensor, Adafruit_NeoPixel for the NeoPixel Ring
 *  and Adafruit_LIS3DH for the accelerometer
 */

#include <Wire.h>
#include <Adafruit_LIS3DH.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_Sensor.h>
// ************** Define some colors ****************
#define RED 0xff0000
#define YELLOW 0xffff00
#define GREEN 0x00ff00
#define BLUE 0x0000ff
#define VIOLET 0xff00ff
#define WHITE 0xffffff 

#define ACCEL_ADDR 0x18 // Accelerometer I2C address
#define NEO_PIN   1
#define NUM_LEDS 12
#define Array_size 50
uint32_t color  = 0x008080; //teal color
int16_t ac_x,ac_y,ac_z;

int16_t Lis_Read_X[Array_size],Lis_Read_Y[Array_size],Lis_Read_Z[Array_size];
int Status;     
Adafruit_NeoPixel MVpixels = Adafruit_NeoPixel(NUM_LEDS, NEO_PIN);
     
// I2C
Adafruit_LIS3DH accel = Adafruit_LIS3DH();
//********************** SETUP **************************************
void setup(void) {
uint8_t i;
  Wire.setClock(400000); // Run I2C at 400 KHz for faster screen updates
  MVpixels.begin();
  MVpixels.clear();
  MVpixels.setBrightness(20);// set not too bright!
  
// Serial.begin(9600);
 if(!accel.begin(ACCEL_ADDR))  // Init accelerometer.  If it fails LED blinks to alert
   { pinMode(LED_BUILTIN, OUTPUT);    
    for(i=1;;i++)                  
    { digitalWrite(LED_BUILTIN, i & 1); // toggle LED
      delay(250);                       
    }
  }
  accel.setRange(LIS3DH_RANGE_4_G);   // 4G!
  Status = 10; 
}

//************************ FUNCTION find average *************
int16_t find_average(int16_t Readings[])
{ int16_t min_value,max_value, av;
  uint8_t i;  
  max_value = Readings[0];
  min_value = Readings[0];
  for (i=1; i<Array_size; i++){
    if (Readings[i]> max_value)
      max_value = Readings[i];
    if (Readings[i]< min_value)
      min_value = Readings[i];          
  }
  
 av = (max_value + min_value)/2;
 return (av);
}
//************************** MAIN ***********************************
void loop() {
  uint8_t i;
 // accel.read();      // get X Y and Z data at once
 
//******************** average data *****************
 for (i=0; i< Array_size; i++)
  { 
    accel.read();
    Lis_Read_X[i]= accel.x; 
    Lis_Read_Y[i]= accel.y;
    Lis_Read_Z[i]= accel.z;    
  }
  ac_x = find_average(Lis_Read_X);
  ac_y = find_average(Lis_Read_Y);
  ac_z = find_average(Lis_Read_Z);
  /*ac_x = accel.x;
  ac_y = accel.y;
  ac_z = accel.z;*/
 // *************** Transform accelerometer axes ************
  ac_x = -ac_x / 256;     
  ac_y = -ac_y / 256;  
  ac_z = ac_z / 256;    

  //print out data
 /* Serial.print("X:  "); Serial.print(ac_x); 
  Serial.print("  \tY:  "); Serial.print(ac_y);
  Serial.print("  \tZ:  "); Serial.print(ac_z);
  Serial.println();*/
  
//************************* find Status *************************************
//********************* check Status from horizontal position ***************
if ((ac_z < 9)&&(ac_z > -9))
   vertical(ac_x,ac_y);
else
{ 
  if ((Status == 10)&&(ac_x > 7)) 
   Status = 16;
  else if ((Status == 10)&&(ac_x < -7)) 
   Status = 36;  
  else if ((Status == 10)&&(ac_y > 7)) 
   Status = 46; 
  else if ((Status == 10)&&(ac_y < -7)) 
   Status = 26;  
//************************ check Status RIGHT ********************   
  else if ((Status == 16)&&(ac_x <= 7)) 
   Status = 10;
  else if ((Status == 16)&&(ac_x > 11)) 
   Status = 15;

  else if ((Status == 15)&&(ac_x <= 11)) 
   Status = 16;
  else if ((Status == 15)&&(ac_x > 19)) 
   Status = 13; 

  else if ((Status == 13)&&(ac_x <= 19)) 
   Status = 15;
  else if ((Status == 13)&&(ac_x > 25)) 
    Status = 11;  

  else if ((Status == 11)&&(ac_x <= 25)) 
   Status = 13;
//******************** check Status UP ********************
  else if ((Status == 46)&&(ac_y <= 7)) 
   Status = 10;
  else if ((Status == 46)&&(ac_y > 11)) 
   Status = 45;

  else if ((Status == 45)&&(ac_y <= 11)) 
   Status = 46;
  else if ((Status == 45)&&(ac_y > 19)) 
   Status = 43; 

  else if ((Status == 43)&&(ac_y <= 19)) 
   Status = 45;
  else if ((Status == 43)&&(ac_y > 25)) 
    Status = 41;

  else if ((Status == 41)&&(ac_y <= 25)) 
   Status = 43;
//****************** check Status LEFT ********************** 
 else if ((Status == 36)&&(ac_x >= -7)) 
   Status = 10;
  else if ((Status == 36)&&(ac_x < -11)) 
   Status = 35;

  else if ((Status == 35)&&(ac_x >= -11)) 
   Status = 36;
  else if ((Status == 35)&&(ac_x < -19)) 
   Status = 33; 

  else if ((Status == 33)&&(ac_x >= -19)) 
   Status = 35;
  else if ((Status == 33)&&(ac_x < -25)) 
    Status = 31;     

  else if ((Status == 31)&&(ac_x >= -25)) 
   Status = 33;
//***************** check Status DOWN ****************** 
  else if ((Status == 26)&&(ac_y >= -7)) 
   Status = 10;
  else if ((Status == 26)&&(ac_y < -11)) 
   Status = 25;

  else if ((Status == 25)&&(ac_y >= -11)) 
   Status = 26;
  else if ((Status == 25)&&(ac_y < -19)) 
   Status = 23; 

  else if ((Status == 23)&&(ac_y >= -19)) 
   Status = 25;
  else if ((Status == 23)&&(ac_y < -25)) 
    Status = 21; 
    
  else if ((Status == 21)&&(ac_y >= -25)) 
   Status = 23;   
   
  //************* run Status *******************
  run_status(Status);
 }  
//delay(500);
}
// *********************FUNCTIONS *****************************************************

// ********* run status **************************

void run_status(int ac_status)
{
switch (ac_status) {
    case 10:
      horizontal();
      break;
    case 11:
      right_1(); 
      break;
    case 13:
      right_3(); 
      break;
    case 15:
      right_5(); 
      break;
    case 16:
      right_6(); 
      break;
      
    case 21:
      down_1(); 
      break;
    case 23:
      down_3(); 
      break;
    case 25:
      down_5(); 
      break;
    case 26:
      down_6(); 
      break; 
      
    case 31:
      left_1(); 
      break;
    case 33:
      left_3(); 
      break;
    case 35:
      left_5(); 
      break;
    case 36:
      left_6(); 
      break; 
      
    case 41:
      up_1(); 
      break;
    case 43:
      up_3(); 
      break;
    case 45:
      up_5(); 
      break;
    case 46:
      up_6(); 
      break;     
  }
   
}
//**************** turn on all LEDs - Horizontal *************************
void horizontal()
{
  uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, color); 
      }
  MVpixels.show();     
}
//****************  turn on 6 LEDS right *******************
void right_6()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, color);
        if((i>3)&&(i<9))
          MVpixels.setPixelColor(i, 0);
      }
       MVpixels.show(); 
  }
//****************  turn on 5 LEDS right *******************
void right_5()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, color);
        if((i>=3)&&(i<10))
          MVpixels.setPixelColor(i, 0);
      }
       MVpixels.show(); 
  }
//****************  turn on 3 LEDS right *******************
void right_3()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i==0)||(i==1)||(i==11))
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }
//****************  turn on 1 LED right *******************
void right_1()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if(i==0)
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }  
//****************  turn on 6 LEDS left *******************
void left_6()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=3)&&(i<10))
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }  
//**************** turn on 5 LEDS left *******************
void left_5()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=4)&&(i<9))
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }    
//****************  turn on 3 LEDS left *******************
void left_3()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=5)&&(i<8))
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }    
//**************** turn on 1 LED left *******************
void left_1()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if(i==6)
          MVpixels.setPixelColor(i, color);
      }
       MVpixels.show(); 
  }    
//**************** turn on 6 LEDS UP *******************
void up_6()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, color);
        if((i>=1)&&(i<6))
          MVpixels.setPixelColor(i,0);
      }
       MVpixels.show(); 
  }    
//**************** turn on 5 LEDS UP *******************
void up_5()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=7)&&(i<NUM_LEDS))
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }      
//****************  turn on 3 LEDS UP *******************
void up_3()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=8)&&(i<11))
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }
//****************  turn on 1 LED UP *******************
void up_1()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if(i==9)
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }                 
//****************  turn on 6 LEDS DOWN *******************
void down_6()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=0)&&(i<7))
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }       
//****************  turn on 5 LEDS DOWN *******************
void down_5()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=1)&&(i<6))
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }           
//****************  turn on 3 LEDS DOWN *******************
void down_3()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if((i>=2)&&(i<5))
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }         
//**************** turn on 1 LED DOWN *******************
void down_1()
{uint8_t i;
  for (i=0; i<NUM_LEDS; i++)
      { 
        MVpixels.setPixelColor(i, 0);
        if(i==3)
          MVpixels.setPixelColor(i,color);
      }
       MVpixels.show(); 
  }         
//************************* if the board is vertical (Z= 0), turn on only 1 LED at the time ********************
void vertical(int16_t ax, int16_t ay)
{int16_t value_1,value_2,value_3,value_4;

  MVpixels.clear();
  value_1 = 7;
  value_2 = 11;
  value_3 = 19;
  value_4 = 25;
  ax = filter_axis(ax);
  ay = filter_axis(ay);
  if ((ax == 0)&&(ay == value_4)){
     MVpixels.setPixelColor(9,RED);
     MVpixels.show(); 
 }
 if ((ax == value_2)&&(ay == value_3)){
     MVpixels.setPixelColor(10, YELLOW);
     MVpixels.show(); 
 }  
 if ((ax == value_3)&&(ay == value_2)){
     MVpixels.setPixelColor(11, GREEN);
     MVpixels.show();   
 }      
 if ((ax == value_4)&&(ay == 0)){
     MVpixels.setPixelColor(0, BLUE);
     MVpixels.show();
 } 
 if ((ax == value_3)&&(ay == -value_2)){
     MVpixels.setPixelColor(1,VIOLET);
     MVpixels.show();
 }
 if ((ax == value_2)&&(ay == -value_3)){
     MVpixels.setPixelColor(2, WHITE);
     MVpixels.show();
 }  
 if ((ax == 0)&&(ay == -value_4)){
     MVpixels.setPixelColor(3, RED);  
     MVpixels.show();
 }      
 if ((ax == -value_2)&&(ay == -value_3)){
     MVpixels.setPixelColor(4, YELLOW);
     MVpixels.show();
 }
 if ((ax == -value_3)&&(ay == -value_2)){
     MVpixels.setPixelColor(5, GREEN);
     MVpixels.show();
 }
 if ((ax == -value_4)&&(ay == 0)){
     MVpixels.setPixelColor(6,BLUE); 
     MVpixels.show();
 } 
 if ((ax == -value_3)&&(ay == value_2)){
     MVpixels.setPixelColor(7, VIOLET);
     MVpixels.show();
 }
 if ((ax == -value_2)&&(ay == value_3)){
     MVpixels.setPixelColor(8, WHITE);            
     MVpixels.show(); 
 }  
}  
//****************** filter function ***********************************
int16_t filter_axis(int16_t ac)
{
 int16_t L_0, L_1, L_2, L_3, L_4,value_1,value_2,value_3,value_4,acxy;
 
 L_0 = 7;
 L_1 = 9;
 L_2 = 11;
 L_3 = 19;
 L_4 = 25;
 value_1 = 7;
 value_2 = 11;
 value_3 = 19;
 value_4 = 25;
 if((ac >= -L_0)&&(ac <= L_0))
  acxy = 0;
  
 if((ac > L_1)&&(ac <= L_2))
  acxy = value_1;
 if((ac > L_2)&&(ac <= L_3))
  acxy = value_2;
 if ((ac > L_3)&&(ac <= L_4))
  acxy = value_3;
 if (ac > L_4)
  acxy = value_4;

 if((ac > -L_2)&&(ac < -L_1))
  acxy = -1*value_1;
 if((ac > -L_3)&&(ac <= -L_2))
  acxy = -1*value_2;
 if((ac > -L_4)&&(ac <= -L_3))
  acxy = -1*value_3;
 if (ac <=  -L_4)
  acxy = -1*value_4;
  
return (acxy);  
}
