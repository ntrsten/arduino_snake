#include <Arduino.h>
#include <MeOrion.h>

Me4Button btn(PORT_6);

#define DIM 10
#define MAX_X 320/DIM
#define MAX_Y 240/DIM
#define SPEED 15

enum smjerovi {DESNO = 0, DOLJE, LIJEVO, GORE, NEMA};

//jedna od dviju fja koju prima orion pločica (arduino)
void setup() 
{
  Serial.begin(9600);
  randomSeed(analogRead(0));
  Serial.println("CLS(0);");
}

//globalne varijable, nebi trebale biti, al bar nisu u loop
int x, y, smjer;
int zx, zy, zsmjer;
int ix, iy;

bool turn = false;

int hx, hy;
bool pojeo = true;

int t = 0;

bool ended = false;

bool provjera_kraja(int x, int y){
  if (x == MAX_X || x == -1)
    return true;
  if (y == MAX_Y || y == -1)
    return true;
  return false;
}
//druga funkcija koju prima orion pločica, konstantno vrti, ali ne koristimo za igru jer želimo dodatnu funkcionalnost petlje za igru
void loop() 
{
  //buffer za serijsku komunikaciju s ekranom
  char buffer[50];
  
  uint8_t polja[32][32];
  //popunjavanje matrice polja bez smjera
  for (ix = 0; ix < 32; ix++){
    for (iy = 0; iy < 32; iy++){
      polja[ix][iy] = NEMA;
    }
  }
  
  x = random(2,28); 
  y = random(2,20);
  smjer = random(0,3);
  
  zx = x;
  zy = y;
  zsmjer = smjer;

  while (!ended){
    t++;
    if (t == SPEED){
      turn = false;
      t = 0;
      polja[x][y]=smjer;
      switch(smjer){
        case DESNO:
          x++;
          break;
        case DOLJE:
          y++;
          break;
        case LIJEVO:
          x--;
          break;
        case GORE:
          y--;
          break;
        default:
          break;
      }
      //granice polja
      if (provjera_kraja(x, y)){
        Serial.println("DS64(80,30,'GAME OVER',1);");
        ended = true;
      }
      //sama pojela sebe
      if (polja[x][y] != NEMA){
        Serial.println("DS64(80,30,'GAME OVER',1);");
        ended = true;
      }
      //pojedena hrana
      //treba nam varijabla pojeo zbog početne duljine
      if (x == hx && y == hy){
         pojeo = true;
      }
      if (pojeo){
        pojeo = false;
        hx = random(0,31); 
        hy = random(0,23);
        //tražimo polje koje nije na zmiji
        while(polja[hx][hy] != NEMA){
          hx = random(0,31); 
          hy = random(0,23);
        }
        sprintf(buffer, "BOXF(%d,%d,%d,%d,2);", hx*10, hy*10, (hx+1)*10,(hy+1)*10);
        Serial.println(buffer);
      }
      //ako nije pojeo, briše se zadnji segment
      else{
        sprintf(buffer, "BOXF(%d,%d,%d,%d,0);", zx*10, zy*10, (zx+1)*10,(zy+1)*10);
        Serial.println(buffer);
        polja[zx][zy] = NEMA;
        switch(zsmjer){
          case DESNO:
            zx++;
            break;
          case DOLJE:
            zy++;
            break;
          case LIJEVO:
            zx--;
            break;
          case GORE:
            zy--;
            break;
          default:
            break;
        }
        zsmjer = polja[zx][zy];
      }
     //crtanje vodećeg segmenta
     sprintf(buffer, "BOXF(%d,%d,%d,%d,7);", x*10, y*10, (x+1)*10,(y+1)*10);
     Serial.println(buffer);
  
    }
    //input izvan petlje, osiguravamo frekventnjiji inputcheck, manji latency
     switch(btn.pressed() ){
      case 1:
        if (smjer != LIJEVO && !turn)
          smjer = DESNO;
        turn = true;
        break;
      case 2:
        if (smjer != GORE && !turn)
          smjer = DOLJE;
        turn = true;
        break;
      case 3:
        if (smjer != DESNO && !turn)
          smjer = LIJEVO;
        turn = true;
        break;
      case 4:
        if (smjer != DOLJE && !turn)
          smjer = GORE;
        turn = true;
        break;
      default:
        break;
     } 
  delay(10);
  }
}


