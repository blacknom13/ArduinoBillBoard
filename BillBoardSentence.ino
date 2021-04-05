#include "MemoryFree.h"
//1588AS
PROGMEM const byte asRows[] = {19, 11, 9, 16, 2, 8, 3, 6};
PROGMEM const byte asCols[] = {10, 4, 5, 18, 7, 17, 12, 13};
//1588BS
PROGMEM const byte bsRows[] = {2, 7, 13, 5, 19, 12, 18, 10};
PROGMEM const byte bsCols[] = {6, 17, 16, 3, 11, 4, 8, 9};

PROGMEM const char codeBook[][6][5][20] = {
  { //Cap English
    {"-ACGJOQS",       "-HKLMNUVWXY",  "-BHKLMNUVWX",    "+EFHJKNUVXZ",   "+MW"},
    {"-IJTZ",         "+IMT",        "+BJKY",          "-BEFIJKLTW",    "+MW"},
    {"-IJSTXZ",        "+BEFHIKNSTX",   "+BEFHJMWXYZ",    "+ADHNOPQRUV",   "+MW"},
    {"-ISTYZ",        "+AIKPRTYZ",   "+AGJKNPQRSW",    "+ABDGHNOQUVX",  "+MW"},
    {"-ITVY",          "+ITVQY",         "+JVW",         "-EFTJLMPTVWYZ", "+MW"},
    {"-CGJOQSTUVWY",    "-AFHKMNPRX",   "+BCDEGILOQSUVZ", "+AEHKNRWXZ",   "+M"},
  },
  {
    //Small English
    {"+bhklpy",        "+fgjpqt",      "+dgqy",         "+",       "+"},
    {"-acdejos",       "+aceostz",     "-bhijlmnowx",  "+xz",     "+w"},
    {"-adgijtyz",     "+bdghjmnrty",   "+gdknpqouvyz",    "+acemx",  "+w"},
    {"-agjqstxyz",    "+aefjkpqstxz", "-cfijklnprtz",   "+aen",     "+mw"},
    {"-jqstv",         "+jtv",         "+bdghkmoquwy", "+acnsx",   "+mw"},
    {"+bfhijklmnprsxz", "-fhijklmnpqrx", "+acehkmqstz",  "+adenuwxz", "+mw"}
  },
  { //Cap Russian
    {"-ADZLOCFJ", "-DGIQKMNYXS46WU", "-IKMNYX4U",     "-AVGZMORCTS6W7", "+GM6W"},
    {"-DGLT7",    "+DGLMT",         "+GKFS6WU",       "-BE3KTFS6WU",   "+MF6WU"},
    {"-DGZLTX7",  "+BVDE3KLNTX4",   "-A5DKLOPRCTY7J",  "+ADIQLNOPRY47J", "+MF6WU"},
    {"-DGZLTY47J", "+ADGIQKLRTY7UJ",  "+AGKMRYFS6W7UJ", "-5E3KMRCTFS6WU", "+MF6WU"},
    {"-LTY47",    "+DLT",          "+DGFS6WU",       "-5E3GMRTFS6U",  "+DGMF6WU"},
    {"-ZOCTF4",   "+BVE3ZOCTYFS6W7", "+BVE3GZOCYFS6W7", "-BV5DGZMORCTY7", "+DGM6W"}
  },
  { // Small Russian
    {"+ry",       "+r",               "+y",            "+",             "+"},
    {"-adlocfj", "+abv5e3zlopctf27j", "-vziqkmox29h7u", "+diqklfxu",   "+gm6w9"},
    {"-adgzlt27", "+dglmt2",         "+vgzopknry4sf6w7uj", "+adgiqlmcx",  "+mf6w9u"},
    {"-adgltyx2",  "-5giqmopcfs6wj",   "-v5dzklrct9h",   "+adiql",     "+mf6w9u"},
    {"-gzltf427j", "+dgiqltf2j",       "-a5e3iqlmrctx2", "+adgiqlcfxw2", "+dm6w9u"},
    {"-aotcyf42",  "-5dgiqklmnprfx4uj", "+abe3gnpcf4s6w2j",  "+aiqklxs6wu", "+dgm6w9"},
  },
  { //Punctuation Marks
    {"+!)",     "+?(",  ""},
    {"+?!(",    "+)",   "+?"},
    {"+!:;(",   "+)",   "+?"},
    {"+!-(",    "+?-)", ""},
    {"+,:;(",  "+)",   ""},
    {"-?:(-", "+?_(", "+_"}
  }
};

PROGMEM const char widthParams[][2][4][15] = {
  { //Cap English Width
    {"3ILTY", "5MW", "4*"},
    //Small English Width
    {"1il", "4acednusxz", "5mw", "3*"}
  },
  { //Cap Russian Width
    {"5DGM6WUF", "3T", "4*"},
    //Small Russian Width
    {"5dgm6w9uf", "4aiqklcxs2", "3*"}
  },
  { //Punctuation Width
    {"2()-", "3?_", "1*"}
  }
};
PROGMEM const char offSetParams[][2][2][6] {
  { //Cap English Offset From Top of the Dot Matrix
    {"1*"},
    //Small English Offset From Top of the Dot Matrix
    {"2gjpqy", "1*"}
  },
  { //Cap Russian Offset From Top of the Dot Matrix
    {"1*"},
    //Small Russian Offset From Top of the Dot Matrix
    {"2yr", "1*"}
  },
  {//Punctuation Marks Offset From Top of the Dot Matrix
    {"1*"}
  }
};

PROGMEM const char codeNameToInd[][2] = {"en", "ru"};
PROGMEM const byte codeBookSize = 5;

struct charLen {
  byte len;
  charLen * next;
};

byte *rows;
byte *cols;
bool **dat;
bool *window[12];
int sentenceLength;
int sentenceFullLength;
byte constructedSentenceLen;
byte nextCharLen;
int currIndex;
byte numOfMoves;
byte codeBookInd_g;

unsigned long start;
byte elapsed;
byte row;
byte offSet;
char matId = 'a'; // Defines dot matrix type: 'a' if 1588AS, 'b' if 1588BS.

PROGMEM const char sentence[] = "^en{Good morning} ^ru{ZZ!}  ";


byte height = 6;
byte offSetFromTop = 1;
charLen * charListFst;
charLen * charListLst;

void setup() {
  Serial.begin(9600);
  Serial.println(freeMemory());
  // put your setup code here, to run once:
  rows = new byte [8];
  cols = new byte [8];
  if (matId == 'a') {
    for (byte i = 0; i < 8; i++) {
      rows[i] = pgm_read_byte(&asRows[i]);
      cols[i] = pgm_read_byte(&asCols[i]);
    }
  } else {
    for (byte i = 0; i < 8; i++) {
      rows[i] = pgm_read_byte(&bsRows[i]);
      cols[i] = pgm_read_byte(&bsCols[i]);
    }
  }
  for (byte i = 0; i < 8; i++) {
    pinMode(rows[i], OUTPUT);
    pinMode(cols[i], OUTPUT);
    if (matId == 'a') {
      digitalWrite(rows[i], HIGH);
      digitalWrite(cols[i], LOW);
    } else {
      digitalWrite(rows[i], LOW);
      digitalWrite(cols[i], HIGH);
    }
  }

  row = 0;
  offSet = 0;
  numOfMoves = 0;
  currIndex = 0;
  initData(dat);
  calculateSentenceLengthes(sentenceLength, sentenceFullLength);
  Serial.println(sentenceFullLength);
  //Serial.println(sentenceLength);
  constructSenctenceMat(true);
  delay(1000);
  start = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  for (byte j = 0; j < 8; j++) {
    if (window[j][row] == 1) {
      digitalWrite(rows[row], HIGH);
      digitalWrite(cols[j], LOW);
    }
  }
  row = (row + 1) % 8;

  elapsed = millis() - start;
  if (elapsed > 200 && row == 7) {
    start += elapsed;
    if (numOfMoves >= charListFst->len) {
      constructSenctenceMat(false);
      numOfMoves = 0;
      offSet = 0;
    }
    move();
    numOfMoves++;
  }
  for (byte j = 0; j < 8; j++) {
    if (window[j][row] == 1) {
      digitalWrite(rows[row], LOW);
      digitalWrite(cols[j], HIGH);
    }
  }
}

void initData(bool **&mat) {
  mat = new bool * [16];
  for (byte i = 0; i < 16; i++) {
    mat[i] = new bool [8];
    for (byte j = 0; j < 8; j++) {
      mat[i][j] = 0;
    }
  }
}

void calculateSentenceLengthes(int & len, int & fullLen) {
  int i = 0;
  //byte codeBookInd = 0;
  byte ctrlInd = 0;
  int ctrlChrs = 0;
  char ctrlWord[15];
  int total = 0;
  char temp = (char)pgm_read_byte(&sentence[0]);
  bool ignore = false;
  bool ctrlStart = false;
  //Serial.println("here");
  while (temp != '\0') {
    if (temp == '^' || temp == '*') {
      ignore = true;
      ctrlStart = true;
    }
    if (temp == '}') {
      ctrlInd = -1;
    }
    if (ignore) {
      if (temp == '{' || isdigit(temp)) {
        ignore = false;
        codeBookInd_g = assignCodeBook(ctrlWord);
        memset(ctrlWord, 0, 15);
      } else {
        if (!ctrlStart) {
          ctrlWord[ctrlInd] = temp;
          ctrlInd++;
        }
      }
      ctrlChrs++;
      ctrlStart = false;
    }
    i++;
    temp = (char)pgm_read_byte(&sentence[i]);
  }
  fullLen = i;
  len = i - 1 - ctrlChrs;
}

void constructSenctenceMat(bool first) {
  int temp = 0;
  int len = 0;
  byte currLen = 0;
  byte lastCharOnScreen = 0;
  byte deletedCharLen = 0;
  bool **letMat;
  charLen * pointer;
  char currChar;

  if (!first) {
    byte i = 1;
    pointer = charListFst;
    charListFst = charListFst->next;
    deletedCharLen = pointer->len;
    pointer->next = NULL;
    delete pointer;
    charListLst->next = new charLen;
    pointer = charListLst->next;
    lastCharOnScreen = currIndex;
    for (byte i = 0; i < deletedCharLen; i++) {
      for (byte j = 0; j < 8; j++)
        dat[i][j] = dat[i + deletedCharLen][j];
    }
    for (byte i = deletedCharLen; i < constructedSentenceLen - deletedCharLen; i++) {
      for (byte j = 0; j < 8; j++) {
        dat[i][j] = dat[i + deletedCharLen][j];
      }
    }
    for (byte i = constructedSentenceLen - deletedCharLen; i < 16; i++) {
      for (byte j = 0; j < 8; j++) {
        dat[i][j] = 0;
      }
    }
    temp = constructedSentenceLen - deletedCharLen;
  }

  currIndex = handleCtrl(currIndex);
  currChar = (char)pgm_read_byte(&sentence[currIndex % sentenceFullLength]);
  currLen = charDotLength(currChar, currIndex);

  if (first) {
    charListFst = new charLen;
    charListLst = charListFst;
    pointer = charListFst;
  }
  while (temp + (currLen + 1) <= 16) {

    createCharMat(letMat, currChar, currIndex);
    if (!isspace(currChar)) {
      for (byte i = 0; i < height; i++) {
        for (byte j = 0; j < currLen; j++) {
          dat[j + temp][i + offSetFromTop] = letMat[j][i];
        }
      }
      for (byte i = 0; i < currLen; i++) {
        delete[] letMat[i];
      }
      delete letMat;
    }
    temp += currLen + 1;
    charListLst = pointer;
    pointer->len = currLen + 1;
    pointer->next = new charLen;
    pointer = pointer->next;
    pointer->next = NULL;
    //chars++;
    currIndex = (currIndex + 1) % sentenceFullLength;
    currIndex = handleCtrl(currIndex);
    currChar = (char)pgm_read_byte(&sentence[currIndex]);
    currLen = charDotLength(currChar, currIndex);
  }
  charListLst->next = NULL;
  delete pointer;
  pointer = NULL;
  constructedSentenceLen = temp;
}

void move() {
  for (byte i = 0; i < 12; i++)
    window[i] = dat[(i + offSet)];
  offSet++;
}

byte createCharMat(bool **&mat, char c, int ind) {
  byte len = charDotLength(c, ind);
  byte buff = 18;
  if (isspace(c)) {
    return 1;
  }
  char t[buff];
  mat = new bool* [len];
  for (byte i = 0; i < len; i++) {
    mat[i] = new bool[height];
  }
  for  (byte j = 0; j < len; j++) {
    for (byte i = 0; i < height; i++) {
      if (isUpper(c, ind))
        for (byte o = 0; o < buff; o++) {
          t[o] = (char)pgm_read_byte(&codeBook[codeBookInd_g * 2][i][j][o]);
        }
      else if (isLower(c, ind))
        for (byte o = 0; o < buff; o++) {
          t[o] = (char)pgm_read_byte(&codeBook[codeBookInd_g * 2 + 1][i][j][o]);
        }
      //Serial.println(t);
      bool inv = t[0] == '+' ? false : true;
      bool notBroken = true;
      for (byte k = 1; t[k] != '\0'; k++) {
        if (c == t[k]) {
          mat[j][i] = !inv;
          notBroken = false;
          break;
        }
      }
      if (notBroken) {
        mat[j][i] = inv;
      }
    }
  }
  return len;
}



byte charDotLength(char c, int ind) {
  byte i = 0;
  byte width = 0;
  char cmpr;
  if (isUpper(c, ind)) {
    i = 0;
  } else if (isLower(c, ind)) {
    i = 1;
  } else if (isspace(c))
    return 1;
  for (byte j = 0; j < 4; j++) {
    width = (char)pgm_read_byte(&widthParams[codeBookInd_g][i][j][0]) - '0';
    for (byte k = 1; k < 15; k++) {
      cmpr = (char)pgm_read_byte(&widthParams[codeBookInd_g][i][j][k]);
      if (cmpr == '\0') break;
      if (c == cmpr || cmpr == '*') {
        j = 5;
        break;
      }
    }
  }
  for (int j = 0; j < 2; j++) {
    offSetFromTop = (char)pgm_read_byte(&offSetParams[codeBookInd_g][i][j][0]) - '0';
    for (int k = 1; k < 6; k++) {
      cmpr = (char)pgm_read_byte(&offSetParams[codeBookInd_g][i][j][k]);
      //Serial.println(cmpr);
      if (cmpr == '\0') break;
      if (c == cmpr || cmpr == '*') {
        j = 5;
        break;
      }
    }
  }
  return width;
}


byte assignCodeBook(char code[15]) {
  int c, i = 0;
  char temp[15];
  bool completeMatch;
  //Serial.println("Well Entered");
  byte bookSize = pgm_read_byte(&codeBookSize);
  if (code[0] == '\0')
    return bookSize - 1;
  for (; i < bookSize; i++) {
    completeMatch = true;
    c = 0;
    for (int j = 0; j < 2; j++) {
      temp[j] = (char)pgm_read_byte(&codeNameToInd[i][j]);
      if (code[j] != temp[j] || (code[j] == '\0' && temp[j] != '\0') || (code[j] != '\0' && temp[j] == '\0')) {
        completeMatch = false;
        break;
      }
    }
    if (completeMatch)
      break;
  }
  if (completeMatch)
    return i;
  else
    return bookSize - 1;
  return 0;
}

byte isCtrl(char c) {
  if (c == '{' || c == '}') {
    return 1;
  } else if (c == '^' || c == '*') {
    return 2;
  } else
    return 0;
}

int handleCtrl(int ind) {
  char c = (char)pgm_read_byte(&sentence[ind % sentenceFullLength]);
  byte ctrlType = isCtrl(c);
  char ctrlWord[15];
  switch (ctrlType) {
    case 0:
      return ind;
    case 1:
      return (ind + 1) % sentenceFullLength;
    case 2:
      ind = (ind + 1) % sentenceFullLength;;
      char temp = (char)pgm_read_byte(&sentence[ind]);
      if (isdigit(temp))
        return ind;
      else {
        byte i = 0;
        while (temp != '{') {
          ctrlWord[i] = temp;
          i++;
          temp = (char)pgm_read_byte(&sentence[(ind + i) % sentenceFullLength]);
        }
        codeBookInd_g = assignCodeBook(ctrlWord);
        return (ind + i + 1) % sentenceFullLength;;
      }
  }
}

bool isUpper(char c, int i) {
  if (isalpha(c))
    return isupper(c);
  else if (isDigit(c)) {
    if ((char)pgm_read_byte(&sentence[i - 1]) == '^')
      return true;
    else
      return false;
  } else
    return false;
}

bool isLower(char c, int i) {
  if (isalpha(c))
    return islower(c);
  else if (isDigit(c)) {
    if ((char)pgm_read_byte(&sentence[i - 1]) == '*')
      return true;
    else
      return false;
  } else
    return false;
}

/*bool isExtra() {

}*/

/*void makeFullMat(char x) {
  bool** temp = NULL;
  int len;
  dat = new bool* [8];
  for (int i = 0; i < 8; i++) {
    dat[i] = new bool[8];
  }
  for (int i = 0; i < 8; i++) {
    for (int j = 0; j < 8; j++) {
      dat[i][j] = 0;
    }
  }
  len = createCharMat(temp, x);
  for (int i = 0; i < height; i++) {
    for (int j = 0; j < len; j++) {
      dat[j][i + offSetFromTop] = temp[j][i];
    }
  }
  }*/
