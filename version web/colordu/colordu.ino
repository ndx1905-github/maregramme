/*
Pour programmer le colorduino, utiliser une arduino uno dont on a enlevé la puce
utiliser les branchements 
5V --> VDD
GND --> GND
0(RX) --> RXD
1(TX) --> TXD
RST --> DTR 


*/

#include <Colorduino.h>
#include <Wire.h> 

const int marnage=8; //marnage -> amplitude affichage (nombre de leds)
float dureeMaree=372.616667*719/720;  // duree -> estimation durée moyenne d'un cycle en minutes entre PM et BM et correction dérive 1 minute toutes les 12h
int deltaMinutes=255*4 ;
int UpdateTime=0 ;

typedef struct
{
  unsigned char r;
  unsigned char g;
  unsigned char b;
} ColorRGB;

typedef struct 
{
  unsigned char h;
  unsigned char s;
  unsigned char v;
} ColorHSV;

unsigned char plasma[ColorduinoScreenWidth][ColorduinoScreenHeight];
long paletteShift;


void HSVtoRGB(void *vRGB, void *vHSV) 
{
  float r, g, b, h, s, v;
  float f, p, q, t;
  int i;
  ColorRGB *colorRGB=(ColorRGB *)vRGB;
  ColorHSV *colorHSV=(ColorHSV *)vHSV;

  h = (float)(colorHSV->h / 256.0);
  s = (float)(colorHSV->s / 256.0);
  v = (float)(colorHSV->v / 256.0);

  if(s == 0.0) {
    b = v;
    g = b;
    r = g;
  }

  else
  {
    h *= 6.0; 
    i = (int)(floor(h));
    f = h - i;

    p = (float)(v * (1.0 - s));
    q = (float)(v * (1.0 - (s * f)));
    t = (float)(v * (1.0 - (s * (1.0 - f))));

    switch(i)
    {
      case 0: r=v; g=t; b=p; break;
      case 1: r=q; g=v; b=p; break;
      case 2: r=p; g=v; b=t; break;
      case 3: r=p; g=q; b=v; break;
      case 4: r=t; g=p; b=v; break;
      case 5: r=v; g=p; b=q; break;
      default: r = g = b = 0; break;
    }
  }
  
  colorRGB->r = (int)(min(r,g) * 15.0);           //  A bidouiller
  colorRGB->g = (int)(g * 45.0);                  //  pour modifier
  colorRGB->b = (int)(max(max(r,g),b) * 155.0);   //  la couleur de la mer
}

unsigned int RGBtoINT(void *vRGB)
{
  ColorRGB *colorRGB=(ColorRGB *)vRGB;

  return (((unsigned int)colorRGB->r)<<16) + (((unsigned int)colorRGB->g)<<8) + (unsigned int)colorRGB->b;
}


float
dist(float a, float b, float c, float d) 
{
  return sqrt((c-a)*(c-a)+(d-b)*(d-b));
}


void
plasma_morph()
{
  unsigned char x,y;
  float value;
  ColorRGB colorRGB;
  ColorHSV colorHSV;

  for(y = 0; y < ColorduinoScreenHeight; y++)
    for(x = 0; x < ColorduinoScreenWidth; x++) {
      {
  value = sin(dist(x + paletteShift, y, 128.0, 128.0) / 8.0)
    + sin(dist(x, y, 64.0, 64.0) / 8.0)
    + sin(dist(x, y + paletteShift / 7, 192.0, 64) / 7.0)
    + sin(dist(x, y, 192.0, 100.0) / 8.0);
  colorHSV.h=(unsigned char)((value) * 128)&0xff;
  colorHSV.s=255; 
  colorHSV.v=255;
  HSVtoRGB(&colorRGB, &colorHSV);
  
  Colorduino.SetPixel(x, y, colorRGB.r, colorRGB.g, colorRGB.b);
      }
  }
  paletteShift++;

}

void ColorFill(unsigned char R,unsigned char G,unsigned char B)
{
  PixelRGB *p = Colorduino.GetPixel(0,0);
  for (unsigned char y=0;y<ColorduinoScreenWidth;y++) {
    for(unsigned char x=0;x<ColorduinoScreenHeight;x++) {
      p->r = R;
      p->g = G;
      p->b = B;
      p++;
    }
  }
  
}

void setup()
{
 
  Wire.begin(0x09);                  // Start the I2C Bus with address 9 in decimal (= 0x09 in hexadecimal) / innitialisation du bus I2C avec comme adresse 9 en decimal soit 0x09 en hexadecimal

  Wire.onReceive(receiveEvent);   // Attach a function to trigger when something is received / Attacher une fonction a declencher lorsque quelque chose est recu

  Serial.begin(9600);
  Colorduino.Init();  

  unsigned char whiteBalVal[3] = {1,4,4}; // balance des blancs {14,63,63}
  Colorduino.SetWhiteBal(whiteBalVal);
  
  
  paletteShift=128000;
  unsigned char bcolor;

  for(unsigned char y = 0; y < ColorduinoScreenHeight; y++)
    for(unsigned char x = 0; x < ColorduinoScreenWidth; x++)
    {

      bcolor = (unsigned char)
      (
            128.0 + (128.0 * sin(x*8.0 / 16.0))
          + 128.0 + (128.0 * sin(y*8.0 / 16.0))
      ) / 2;
      plasma[x][y] = bcolor;
    }

Serial.print("code a l'adresse http://ndef.free.fr/maregramme.ino");
    
}

void receiveEvent() {
  deltaMinutes = Wire.read()*4 ;                // read one character from the I2C / lire le caractere recu sur le bus I2C
  UpdateTime = int(millis()/60000) ;
}

void loop()
{

//deltaMinutes = deltaUpdate ; //+ int(millis()/60000) ; 
 
  
plasma_morph();
plage(deltaMinutes); 

delay (160);

if ( deltaMinutes == 255*4 || abs ( UpdateTime - int ( millis () / 60000 ) > 60) ) {
    for (int j = 0 ; j < 8 ; j++) { 
        for (int i = 0 ; i < 8 ; i++) {Colorduino.SetPixel(i,j,30,30,30);} // colonne de séparation
        }
    }

Colorduino.FlipPage(); 

}

int hauteur(int deltaMinutes)
{
  return round(8-marnage*sq(sin(radians(90*deltaMinutes/dureeMaree))));
}

void plage(int deltaMinutes)
{

//d'abord colonne de gauche i=0
for (int j = hauteur(deltaMinutes+60*0) ; j < 8 ; j++) {
   Colorduino.SetPixel(0,j,255,255,25); // jaune foncé
    }

//puis i=1  neutralisé en colonne noire
for (int j = 0 ; j < 8 ; j++) { Colorduino.SetPixel(1,j,0,0,0);} // colonne de séparation

//et enfin i=2 à 7 qui affiche en réalité i=1 à 6
for (int i = 1 ; i < 7 ; i++) {
  if (hauteur(deltaMinutes+60*i)==constrain(hauteur(deltaMinutes+60*i),0,7))
  {
  for (int j = hauteur(deltaMinutes+60*i) ; j < 8 ; j++) {
    Colorduino.SetPixel(i+1,j,255,255,25); // jaune foncé
    }
  } 
}

}
