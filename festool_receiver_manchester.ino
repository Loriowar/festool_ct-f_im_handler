#define RX_PIN 4
#define ACTION_PIN 8 // for relay connection
#define INDICATOR_PIN 13 // for visialization
#define BIT_TIME_USEC 500 // bit impulse length
#define BITS_TO_READ 45 // full uint64_t size is too much here; in fact should be enough 39-40

// Encoding of signal front
//  Manchester v1 by Tomas: 0 encoded by 0->1 (RISE), 1 encoded by 1->0 (DOWN)
//  Manchester v2 IEEE 802.4 and IEEE 802.3: 0 encoded by 1->0 (DOWN), 1 encoded by 0->1 (RISE)
#define RISE 0
#define DOWN 1
#define END_OF_READ 3

// Festool command codes
#define START_CODE 0x177F
#define STOP_CODE 0x17CA
// Unknown commands
// 0x47 - on receiver power on
// 0x3B00 - to full speed?
// 0x3B7FFF - reduce speed with rate?
// 0x3BFF - reduce speed to zero?
// 0x2FF - unknown (between reduce speed with rate and reduce speed to zero)
// 0x17CA - unknown
//
// Messages sample (power-on and subsequent buttons push with some delay):
// Received value hex (bin) = 47 (1000111)
// 
// Received value hex (bin) = 177F (1011101111111)
// Received value hex (bin) = 3B00 (11101100000000)
// Received value hex (bin) = 177F (1011101111111)

// Received value hex (bin) = 17CA (1011111001010)
// Received value hex (bin) = 3B7FFF (1110110111111111111111)
// Received value hex (bin) = 0 (0)
// Received value hex (bin) = 0 (0)
// Received value hex (bin) = 2FF (1011111111)
// Received value hex (bin) = 3BFF (11101111111111)

// Received value hex (bin) = 177F (1011101111111)
// Received value hex (bin) = 3B00 (11101100000000)
// Received value hex (bin) = 177F (1011101111111)

// Received value hex (bin) = 17CA (1011111001010)
// Received value hex (bin) = 3B7FFF (1110110111111111111111)
// Received value hex (bin) = 0 (0)
// Received value hex (bin) = 2FF (1011111111)
// Received value hex (bin) = 0 (0)
// Received value hex (bin) = 3BFF (11101111111111)

void setup() {
  // Serial.begin(9600);
  pinMode(RX_PIN, INPUT);
  pinMode(ACTION_PIN, OUTPUT);
  pinMode(INDICATOR_PIN, OUTPUT);

  digitalWrite(ACTION_PIN, LOW);
  digitalWrite(INDICATOR_PIN, LOW);
}

// Do not use Serial.print for debug. This consume plenty of time, break a timing and the logic can't read all bits as expected.
int readManchesterBit() {
  waitNextBit();
  int first = ReadBit();
  waitNextBit();
  int last = ReadBit();

  if (first == last)
    return END_OF_READ;

  return (first < last) ? RISE : DOWN;
}

uint32_t readAllManchesterBits() {
  uint32_t r = 0;
  int current = 0;
  while(true) {
    current = readManchesterBit();
    if (current == END_OF_READ)
      break;

    r *= 2;
    r += current;
  }

  return r;
}

// union serial_data {
//   uint64_t data;
//   struct {
//     uint32_t last;
//     uint32_t first;
//   };
// };

// serial_data rxSequence;

void waitNextBit() {
  return delayMicroseconds(BIT_TIME_USEC);
}

int ReadBit() {
  return digitalRead(RX_PIN);
}

void doAction(uint32_t message) {
  switch (message) {
    case START_CODE:
      digitalWrite(ACTION_PIN, HIGH);
      digitalWrite(INDICATOR_PIN, HIGH);
      break;
    case STOP_CODE:
      digitalWrite(ACTION_PIN, LOW);
      digitalWrite(INDICATOR_PIN, LOW);
      break;
  }
  
  return;
}

void loop() {
  // rxSequence.data = 0;
  // Wait for the start bit. Without that sync the timing will be wrong because we have no clock.
  while (ReadBit() == 0) {}

  // To measure pulse somewhere in the middle, but to keep reserve for arduino loop execution time
  delayMicroseconds(BIT_TIME_USEC / 4);

  uint32_t message = 0;
  message = readAllManchesterBits();

  doAction(message);
  // Serial.print("Received value hex (bin) = ");
  // Serial.print(message, HEX);
  // Serial.print(" (");
  // Serial.print(message, BIN);
  // Serial.println(")");
}
