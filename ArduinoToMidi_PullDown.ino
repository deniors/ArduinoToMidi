#define NUM_ROWS 5
#define NUM_COLS 12

#define NOTE_ON_CMD 0x90
#define NOTE_OFF_CMD 0x80
#define NOTE_VELOCITY 127

//MIDI baud rate
#define SERIAL_RATE 31250

// Pin Definitions

// LED output pins
const int statLedPin = 7;

// Row input pins
const int row1Pin = 0;
const int row2Pin = 1;
const int row3Pin = 2;
const int row4Pin = 3;
const int row5Pin = 4;

// 74HC595 pins
const int dataPin = 11;
const int latchPin = 12;
const int clockPin = 13;

boolean keyPressed[NUM_ROWS][NUM_COLS];
uint8_t keyToMidiMap[NUM_ROWS][NUM_COLS];

// bitmasks for scanning columns
int bits[] =
{ 
  B00000001,
  B00000010,
  B00000100,
  B00001000,
  B00010000,
  B00100000,
  B01000000,
  B10000000
};

void setup()
{
  int note = 48;

  for(int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    for(int rowCtr = 0; rowCtr < NUM_ROWS; ++rowCtr)
    {
      keyPressed[rowCtr][colCtr] = false;
      keyToMidiMap[rowCtr][colCtr] = note;
      note++;
    }
  }

  // setup pins output/input mode
  pinMode(dataPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(latchPin, OUTPUT);

  pinMode(row1Pin, INPUT);
  pinMode(row2Pin, INPUT);
  pinMode(row3Pin, INPUT);
  pinMode(row4Pin, INPUT);
  pinMode(row5Pin, INPUT);
  
  pinMode(statLedPin, OUTPUT);

  Serial.begin(SERIAL_RATE);
}

void loop()
{
  for (int colCtr = 0; colCtr < NUM_COLS; ++colCtr)
  {
    //scan next column
    scanColumn(colCtr);

    //get row values at this column
    int rowValue[NUM_ROWS];
    rowValue[0] = digitalRead(row1Pin);
    rowValue[1] = digitalRead(row2Pin);
    rowValue[2] = digitalRead(row3Pin);
    rowValue[3] = digitalRead(row4Pin);
    rowValue[4] = digitalRead(row5Pin);

    // process keys pressed
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] != 0 && !keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = true;
        noteOn(rowCtr,colCtr);
      }
    }

    // process keys released
    for(int rowCtr=0; rowCtr<NUM_ROWS; ++rowCtr)
    {
      if(rowValue[rowCtr] == 0 && keyPressed[rowCtr][colCtr])
      {
        keyPressed[rowCtr][colCtr] = false;
        noteOff(rowCtr,colCtr);
      }
    }
  }
}

void scanColumn(int colNum)
{
  digitalWrite(latchPin, LOW);

  if(0 <= colNum && colNum <= 7)
  {
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000); //right sr
    shiftOut(dataPin, clockPin, MSBFIRST, bits[colNum]); //left sr
  }
  else
  {
    shiftOut(dataPin, clockPin, MSBFIRST, bits[colNum-8]); //right sr
    shiftOut(dataPin, clockPin, MSBFIRST, B00000000); //left sr
  }
  digitalWrite(latchPin, HIGH);
}

void noteOn(int row, int col)
{
  Serial.write(NOTE_ON_CMD);
  Serial.write(keyToMidiMap[row][col]);
  Serial.write(NOTE_VELOCITY);
  digitalWrite(statLedPin, HIGH);
}

void noteOff(int row, int col)
{
  Serial.write(NOTE_OFF_CMD);
  Serial.write(keyToMidiMap[row][col]);
  Serial.write(NOTE_VELOCITY);
  digitalWrite(statLedPin, LOW);
}


