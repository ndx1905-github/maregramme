
#include <Colorduino.h>
#include <math.h>

#include <Wire.h> // (code esclave)

#define SDA_PIN 4 // (code esclave)
#define SCL_PIN 5 // (code esclave)

const int16_t I2C_MASTER = 0x42;  //(code esclave)
const int16_t I2C_SLAVE = 0x08;   //(code esclave)

  Wire.begin(SDA_PIN, SCL_PIN, I2C_SLAVE); // code esclave new syntax: join i2c bus (address required for slave)
  Wire.onReceive(receiveEvent); // code esclave register event

double latestHighTide , now ;
int graph[] = {7,7,7,6,6,5,5,3,3,1,1,0,0,0,0,1,1,3,3,5,5,6,6,7,7,7,7,6,6,5,5,3,3,1,1,0,0,0,0,1,1,3,3,5,5,6,6,7};
int level[] = {7,7,7,6,6,5,4,3,2,1,1,0,0,0,0,1,1,2,3,4,5,6,6,7,7,7,7,6,6,5,4,3,2,1,1,0,0,0,0,1,1,2,3,4,5,6,6,7};
int maree ;

typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorRGB;


void niveauMaree(unsigned char L)
{
  PixelRGB *p = Colorduino.GetPixel(0,0);
  for (unsigned char y=0;y<ColorduinoScreenWidth;y++) {
    for(unsigned char x=0;x<L;x++) {
      p->r = 0;
      p->g = 127;
      p->b = 255;
      p++;
    }
  }
  for (unsigned char y=0;y<ColorduinoScreenWidth;y++) {
    for(unsigned char x=L;x<ColorduinoScreenHeight;x++) {
      p->r = 255;
      p->g = 204;
      p->b = 0;
      p++;
    }
  }
 // Colorduino.FlipPage();
}

void setup()
{
  Serial.begin(115200);           // start serial for output (code esclave)

  Colorduino.Init(); // initialize the board
  
  // compensate for relative intensity differences in R/G/B brightness
  // array of 6-bit base values for RGB (0~63)
  // whiteBalVal[0]=red
  // whiteBalVal[1]=green
  // whiteBalVal[2]=blue
  unsigned char whiteBalVal[3] = {36,63,63}; // for LEDSEE 6x6cm round matrix
  Colorduino.SetWhiteBal(whiteBalVal);
 // to adjust white balance you can uncomment this line
 // and comment out the plasma_morph() in loop()
 // and then experiment with whiteBalVal above
 // ColorFill(255,255,255);

}

void loop()
{

latestHighTide = 17,7;
now = latestHighTide + 1;  

maree = level [  int ( 2 * fmod ( now - latestHighTide , 24 ) )] ; 
niveauMaree(maree);
  
 for (int i = 0 ; i < 8 ; i++) {
  maree = graph [ int ( 2 * fmod ( now - latestHighTide + i , 24 ) )] ;
  Colorduino.SetPixel(i,maree,255,255,255);
  if (maree == 3 || maree == 5) {Colorduino.SetPixel(i,maree-1,255,255,255);}
   
  }
  
Colorduino.FlipPage();


}

// function that executes whenever data is received from master
// this function is registered as an event, see setup() code esclave
void receiveEvent(size_t howMany) {

  (void) howMany;
  while (1 < Wire.available()) { // loop through all but the last
    int x = Wire.read(); // receive byte as a character
    //Serial.print(c);         // print the character
  }
  int y = Wire.read();    // receive byte as an integer
  Serial.println(x);         // print the integer
}