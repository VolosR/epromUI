
#include "Rotary-encoder-easyC-SOLDERED.h"
#include "fonts.h"
#include "largeFonts.h"
#include <TFT_eSPI.h>
#include <EEPROM.h>
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);
TFT_eSprite set = TFT_eSprite(&tft);

#define EEPROM_SIZE 81

Rotary rotary;
int chosen=1;
int inc=0;
int element=0;
int increments[3]={1,3,5};
uint8_t dd[80]={39,20,20,0,200,200,200,
                  18,50,0,200,200,200,
                  76,20,0,200,200,200,
                  104,53,1,180,180,180,
                  186,20,1,160,160,160,
                  10,80,20,6,190,190,190,
                  10,90,20,6,20,190,20,
                  10,100,20,6,90,190,90,
                  10,110,20,6,190,90,90,
                  10,120,20,6,90,90,120,
                  10,130,20,6,90,100,190,
                  10,140,20,6,90,120,90,};

uint8_t xpos[79]={0};   
uint8_t ypos[79]={0};                   

bool setR=0;
bool addOrValue=0;
int deb=0;
int deb2=0;
int blink=0;


char lbl[6]={'X','Y','F','R','G','B'};
char lbl2[7]={'X','Y','W','H','R','G','B'};

unsigned short lblC[6]={TFT_SILVER,TFT_SILVER,TFT_SILVER,TFT_RED,TFT_GREEN,0x5E7D};
unsigned short  lbl2C[7]={TFT_SILVER,TFT_SILVER,TFT_SILVER,TFT_SILVER,TFT_RED,TFT_GREEN,0x5E7D};

String txt[5]={"77.12","TMP","86%","OCTOBER 12","WIND 120m/s"};


void epromWrite()
{
     for(int i=0;i<80;i++){
     EEPROM.write(i, dd[i]);
     EEPROM.commit();
    }
}


void setup() {

   pinMode(15,OUTPUT);
   digitalWrite(15,1);

  pinMode(0,INPUT_PULLUP);
  pinMode(14,INPUT_PULLUP);
  
   Wire.begin(43,44);
   rotary.begin();

   
   EEPROM.begin(EEPROM_SIZE);

   if(EEPROM.read(0)!=39)
   {
     epromWrite();
   }

   for(int i=0;i<80;i++)
   dd[i]=EEPROM.read(i);
 

  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  sprite.createSprite(255,170);
  sprite.setTextDatum(4);
  set.createSprite(65,170);

     ledcSetup(0, 10000, 8);
     ledcAttachPin(38, 0);
     ledcWrite(0, 120);

  int n=0;

for(int j=0;j<5;j++)
for(int i=0;i<6;i++)
{xpos[n]=1+(i*9); ypos[n]=30+(j*7); n++;}

for(int j=0;j<7;j++)
for(int i=0;i<7;i++)
{xpos[n]=1+(i*9); ypos[n]=98+(j*7); n++;}

}

void draw()
{
  sprite.fillSprite(TFT_BLACK);
  

       // drawing 7 shper, rectangles/ lines
    for(int i=0;i<7;i++)
    {
    if(setR==1 && element==i+6 && blink<12)  
    sprite.fillRect(dd[31+(i*7)],dd[32+(i*7)],dd[33+(i*7)],dd[34+(i*7)],TFT_BLACK);
    else
    sprite.fillRect(dd[31+(i*7)],dd[32+(i*7)],dd[33+(i*7)],dd[34+(i*7)],tft.color565(dd[35+(i*7)], dd[36+(i*7)], dd[37+(i*7)]));
    }
    
    for(int i=0;i<5;i++)
    {
    sprite.setTextColor(tft.color565(dd[(i*6)+4], dd[(i*6)+5], dd[(i*6)+6]));  
    choseFont(dd[(i*6)+3]);
    if(setR==1 && element==i+1 && blink<12)
    sprite.setTextColor(TFT_BLACK); 
    else
    sprite.setTextColor(tft.color565(dd[(i*6)+4], dd[(i*6)+5], dd[(i*6)+6])); 
    sprite.drawString(txt[i],dd[(i*6)+1],dd[(i*6)+2]);
    sprite.unloadFont();
    }
   
 

        if(setR)
        sprite.pushSprite(65,0);  
        else
        sprite.pushSprite(32,0); 
    }

void drawS()
{
findElement();  
set.fillSprite(0x0127);
set.drawLine(64,0,64,170,0x630C);
set.setTextColor(0xB596,0x0127);
set.setTextDatum(0);

set.drawString("TEXT",2,2,2);
set.drawString("SHAPES",2,70,2);

for(int j=0;j<6;j++)
{
set.setTextColor(lblC[j],0x0127);
set.drawString(String(lbl[j]),4+(j*9),19);
}
for(int j=0;j<7;j++)
{
  set.setTextColor(lbl2C[j],0x0127);
set.drawString(String(lbl2[j]),4+(j*9),87);
}

set.setTextColor(TFT_SILVER,0x0127);

for(int i=0;i<79;i++)
set.drawRect(xpos[i],ypos[i],8,8,TFT_SILVER);

set.fillRect(xpos[chosen-1]+2,ypos[chosen-1]+2,4,3,TFT_ORANGE);

  set.setTextDatum(4);
  set.drawString(String(dd[chosen]),48,158,2);
  set.drawString("INC"+String(increments[inc]),50,9);
  //set.drawString(String(element),54,10);

  set.drawString(String(chosen),16,158,2);
  set.fillRect(1+(addOrValue*32),168,32,2,TFT_WHITE);
  set.pushSprite(0,0);
}


void checkRotary()
{
    int state = rotary.getState();
    if (state != ROTARY_IDLE)
    {
        if (state == ROTARY_CW)
        {
          if(addOrValue){
          dd[chosen]=dd[chosen]+increments[inc];
          if(dd[chosen]>255) dd[chosen]=0;}
          else
          {
            chosen++;
            if(chosen>79) chosen=1;
          }
        }
         if (state == ROTARY_CCW)
        {
         if(addOrValue){
          dd[chosen]=dd[chosen]-increments[inc];
          if(dd[chosen]<0) dd[chosen]=255;}
          else
          {
            chosen--;
            if(chosen<1) chosen=79;
          }
        }
         if (state == BTN_CLICK)
        {
         addOrValue=!addOrValue;
        }
    }   
}

void checkButtons()
{
  if(digitalRead(0)==0)
  {
  if(deb==0){deb=1;  inc++; if(inc==3) inc=0;}
  }else deb=0;

  if(digitalRead(14)==0)
  {
  if(deb2==0){deb2=1; 
   if(setR==1) epromWrite();
   setR=!setR; tft.fillScreen(TFT_BLACK);}
  }else deb2=0;
}

void choseFont(int f)
{
  switch(f)
    {
    case 0: sprite.loadFont(font25); break;
    case 1: sprite.loadFont(font30); break;
    case 2: sprite.loadFont(font35); break;
    case 3: sprite.loadFont(font40); break;
    case 4: sprite.loadFont(font45); break;
    case 5: sprite.loadFont(font50); break;
    case 6: sprite.loadFont(font60); break;
    case 7: sprite.loadFont(font70); break;
    case 8: sprite.loadFont(font80); break;
    case 9: sprite.loadFont(font100); break;

    default: sprite.loadFont(font25);
    }
}

void findElement()
{
  if(chosen>=1 && chosen<7) element=1;
  if(chosen>=7 && chosen<13) element=2;
  if(chosen>=13 && chosen<19) element=3;
  if(chosen>=19 && chosen<25) element=4;
  if(chosen>=25 && chosen<31) element=5;

  if(chosen>=31 && chosen<38) element=6;
  if(chosen>=38 && chosen<45) element=7;
  if(chosen>=45 && chosen<52) element=8;
  if(chosen>=52 && chosen<59) element=9;
  if(chosen>=59 && chosen<66) element=10;
  if(chosen>=66 && chosen<73) element=11;
  if(chosen>=73 && chosen<80) element=12;
}

void loop() {

 blink++; if(blink==30) blink=0; 
 
 if(setR)
 {checkRotary(); drawS();}
 checkButtons();
 draw();
}
