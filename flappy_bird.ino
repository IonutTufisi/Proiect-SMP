#include <LiquidCrystal.h>

#define JoyX 2
#define PIN_AUTOPLAY 1
#define PIN_READWRITE 10
#define PIN_CONTRAST 12

#define SPRITE_RUN1 1
#define SPRITE_RUN2 2
#define SPRITE_SARE 3
#define SPRITE_SARE_SUS '.'         // Folosesc caracterul '.' pt cap
#define SPRITE_SARE_JOS 4
#define SPRITE_TEREN_LIBER ' '      // Folossc caraterul ' '
#define SPRITE_TEREN_SOLID 5
#define SPRITE_TEREN_SOLID_DREAPTA 6
#define SPRITE_TEREN_SOLID_STANGA 7

#define OM_POZ_ORZ 1    // Pozitia orizontala a omuletului p ecran

#define TEREN_WIDTH 16
#define TEREN_LIBER 0
#define TEREN_JOS_BLOCK 1
#define TEREN_SUS_BLOCK 2

#define OM_POZ_OFF 0          // Omul este invizibil
#define OM_POZ_RUN_JOS_1 1  // Omul alearga pe o line de jos (pose 1)
#define OM_POZ_RUN_JOS_2 2  //                              (pose 2)

#define OM_POZ_SARE_1 3       // Sare
#define OM_POZ_SARE_2 4       // Half-way up
#define OM_POZ_SARE_3 5       // SARE is on SUS row
#define OM_POZ_SARE_4 6       // SARE is on SUS row
#define OM_POZ_SARE_5 7       // SARE is on SUS row
#define OM_POZ_SARE_6 8       // SARE is on SUS row
#define OM_POZ_SARE_7 9       // Half-way down
#define OM_POZ_SARE_8 10      // About to land

#define OM_POZ_RUN_SUS_1 11 // boy is running on SUS row (pose 1)
#define OM_POZ_RUN_SUS_2 12 //                              (pose 2)

LiquidCrystal lcd(11, 9, 6, 5, 4, 3);
static char TERENSUS[TEREN_WIDTH + 1];
static char TERENJOS[TEREN_WIDTH + 1];
static bool buttonPushed = false;

void initializeGraphics(){
  static byte graphics[] = {
    // Poz de alergat 1
    B01100,
    B01100,
    B00000,
    B01110,
    B11100,
    B01100,
    B11010,
    B10011,
    // Poz de alergat 2
    B01100,
    B01100,
    B00000,
    B01100,
    B01100,
    B01100,
    B01100,
    B01110,
    // Saritura
    B01100,
    B01100,
    B00000,
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    // Saritura in jos
    B11110,
    B01101,
    B11111,
    B10000,
    B00000,
    B00000,
    B00000,
    B00000,
    // Pamant
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    B11111,
    // Pamant la dreapta
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    B00011,
    // Pamant la stanga
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
    B11000,
  };
  
  int i;
  
  for (i = 0; i < 7; ++i) {
    lcd.createChar(i + 1, &graphics[i * 8]);
  }
  for (i = 0; i < TEREN_WIDTH; ++i) {
    TERENSUS[i] = SPRITE_TEREN_LIBER;
    TERENJOS[i] = SPRITE_TEREN_LIBER;
  }
}

void avansTEREN(char* TEREN, byte nouTEREN){
  for (int i = 0; i < TEREN_WIDTH; ++i) {
    char current = TEREN[i];
    char urmator = (i == TEREN_WIDTH-1) ? nouTEREN : TEREN[i+1];
    switch (current){
      case SPRITE_TEREN_LIBER:
        TEREN[i] = (urmator == SPRITE_TEREN_SOLID) ? SPRITE_TEREN_SOLID_DREAPTA : SPRITE_TEREN_LIBER;
        break;
      case SPRITE_TEREN_SOLID:
        TEREN[i] = (urmator == SPRITE_TEREN_LIBER) ? SPRITE_TEREN_SOLID_STANGA : SPRITE_TEREN_SOLID;
        break;
      case SPRITE_TEREN_SOLID_DREAPTA:
        TEREN[i] = SPRITE_TEREN_SOLID;
        break;
      case SPRITE_TEREN_SOLID_STANGA:
        TEREN[i] = SPRITE_TEREN_LIBER;
        break;
    }
  }
}

  bool traseazaOmulet(byte position, char* TERENSUS, char* TERENJOS, unsigned int score) {
  bool collide = false;
  char SUSSave = TERENSUS[OM_POZ_ORZ];
  char JOSSave = TERENJOS[OM_POZ_ORZ];
  byte SUS, JOS;
  switch (position) {
    case OM_POZ_OFF:
      SUS = JOS = SPRITE_TEREN_LIBER;
      break;
    case OM_POZ_RUN_JOS_1:
      SUS = SPRITE_TEREN_LIBER;
      JOS = SPRITE_RUN1;
      break;
    case OM_POZ_RUN_JOS_2:
      SUS = SPRITE_TEREN_LIBER;
      JOS = SPRITE_RUN2;
      break;
    case OM_POZ_SARE_1:
    case OM_POZ_SARE_8:
      SUS = SPRITE_TEREN_LIBER;
      JOS = SPRITE_SARE;
      break;
    case OM_POZ_SARE_2:
    case OM_POZ_SARE_7:
      SUS = SPRITE_SARE_SUS;
      JOS = SPRITE_SARE_JOS;
      break;
    case OM_POZ_SARE_3:
    case OM_POZ_SARE_4:
    case OM_POZ_SARE_5:
    case OM_POZ_SARE_6:
      SUS = SPRITE_SARE;
      JOS = SPRITE_TEREN_LIBER;
      break;
    case OM_POZ_RUN_SUS_1:
      SUS = SPRITE_RUN1;
      JOS = SPRITE_TEREN_LIBER;
      break;
    case OM_POZ_RUN_SUS_2:
      SUS = SPRITE_RUN2;
      JOS = SPRITE_TEREN_LIBER;
      break;
  }
  if (SUS != ' ') {
    TERENSUS[OM_POZ_ORZ] = SUS;
    collide = (SUSSave == SPRITE_TEREN_LIBER) ? false : true;
  }
  if (JOS != ' ') {
    TERENJOS[OM_POZ_ORZ] = JOS;
    collide |= (JOSSave == SPRITE_TEREN_LIBER) ? false : true;
  }
  
  byte digits = (score > 9999) ? 5 : (score > 999) ? 4 : (score > 99) ? 3 : (score > 9) ? 2 : 1;
  
  // Draw the scene
  TERENSUS[TEREN_WIDTH] = '\0';
  TERENJOS[TEREN_WIDTH] = '\0';
  char temp = TERENSUS[16-digits];
  TERENSUS[16-digits] = '\0';
  lcd.setCursor(0,0);
  lcd.print(TERENSUS);
  TERENSUS[16-digits] = temp;  
  lcd.setCursor(0,1);
  lcd.print(TERENJOS);
  
  lcd.setCursor(16 - digits,0);
  lcd.print(score);

  TERENSUS[OM_POZ_ORZ] = SUSSave;
  TERENJOS[OM_POZ_ORZ] = JOSSave;
  return collide;
}

// Handle the button push as an interrupt
void buttonPush() {
  buttonPushed = true;
}

void setup(){
  pinMode(PIN_READWRITE, OUTPUT);
  digitalWrite(PIN_READWRITE, LOW);
  pinMode(PIN_CONTRAST, OUTPUT);
  digitalWrite(PIN_CONTRAST, LOW);
  pinMode(JoyX, INPUT);
  digitalWrite(JoyX, HIGH);
  pinMode(PIN_AUTOPLAY, OUTPUT);
  digitalWrite(PIN_AUTOPLAY, HIGH);
  
  // Digital pin 2 maps to interrupt 0
  attachInterrupt(0/*PIN_BUTTON*/, buttonPush, FALLING);
  
  initializeGraphics();
  
  lcd.begin(16, 2);
}

void loop(){
  static byte boyPos = OM_POZ_RUN_JOS_1;
  static byte nouTERENTip = TEREN_LIBER;
  static byte nouTERENDurata = 1;
  static bool playing = false;
  static bool blink = false;
  static unsigned int distance = 0;
  
  if (!playing) {
    traseazaOmulet((blink) ? OM_POZ_OFF : boyPos, TERENSUS, TERENJOS, distance >> 3);
    if (blink) {
      lcd.setCursor(0,0);
      lcd.print("Press Start");
    }
    delay(250);
    blink = !blink;
    if (buttonPushed) {
      initializeGraphics();
      boyPos = OM_POZ_RUN_JOS_1;
      playing = true;
      buttonPushed = false;
      distance = 0;
    }
    return;
  }

  // Shift the TEREN to the STANGA
  avansTEREN(TERENJOS, nouTERENTip == TEREN_JOS_BLOCK ? SPRITE_TEREN_SOLID : SPRITE_TEREN_LIBER);
  avansTEREN(TERENSUS, nouTERENTip == TEREN_SUS_BLOCK ? SPRITE_TEREN_SOLID : SPRITE_TEREN_LIBER);
  
  // Make nou TEREN to enter on the DREAPTA
  if (--nouTERENDurata == 0) {
    if (nouTERENTip == TEREN_LIBER) {
      nouTERENTip = (random(3) == 0) ? TEREN_SUS_BLOCK : TEREN_JOS_BLOCK;
      nouTERENDurata = 2 + random(10);
    } else {
      nouTERENTip = TEREN_LIBER;
      nouTERENDurata = 10 + random(10);
    }
  }
    
  if (buttonPushed) {
    if (boyPos <= OM_POZ_RUN_JOS_2) boyPos = OM_POZ_SARE_1;
    buttonPushed = false;
  }  

  if (traseazaOmulet(boyPos, TERENSUS, TERENJOS, distance >> 3)) {
    playing = false; // The boy collided with something. Too bad.
  } else {
    if (boyPos == OM_POZ_RUN_JOS_2 || boyPos == OM_POZ_SARE_8) {
      boyPos = OM_POZ_RUN_JOS_1;
    } else if ((boyPos >= OM_POZ_SARE_3 && boyPos <= OM_POZ_SARE_5) && TERENJOS[OM_POZ_ORZ] != SPRITE_TEREN_LIBER) {
      boyPos = OM_POZ_RUN_SUS_1;
    } else if (boyPos >= OM_POZ_RUN_SUS_1 && TERENJOS[OM_POZ_ORZ] == SPRITE_TEREN_LIBER) {
      boyPos = OM_POZ_SARE_5;
    } else if (boyPos == OM_POZ_RUN_SUS_2) {
      boyPos = OM_POZ_RUN_SUS_1;
    } else {
      ++boyPos;
    }
    ++distance;
    
    digitalWrite(PIN_AUTOPLAY, TERENJOS[OM_POZ_ORZ + 2] == SPRITE_TEREN_LIBER ? HIGH : LOW);
  }
  delay(100);
}
