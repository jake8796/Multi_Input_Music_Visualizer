#include <MIDI.h>
#include <FastLED.h>
#include <RotaryEncoder.h>

#define debug
#define ARRAY_SIZE(array) ((sizeof(array)) / (sizeof(array[0])))
#define ON_BIT_MASK 0x80

// Rotary Encoder pins
#define PIN_A 18
#define PIN_B 21
#define PIN_SW 19

// Definitions for the LED matrix
/***********************************************************
The LED matrix is structured in a snaking pattern as shown below
0 | 15 | 16 |...
1 | 14 | 17 |...
. | .  | .  |...
7 | 8  | 23 |...
************************************************************/
#define NUM_OF_ROWS 64 // Number of Rows on LED Matrix
#define SIZE_OF_ROW 8  // Number of columns
#define NUM_LEDS NUM_OF_ROWS *SIZE_OF_ROW
#define LED_DATA_PIN 2 // Data pin of the individually addressable LED Matrix
// Debug LED
#define ON_BOARD_LED 17
// Pin for ADC
#define ADC_PIN 34
// Selection definitions
#define MIDI_SELECT 0
#define AUX_SELECT 1
// MIDI instantiation
MIDI_CREATE_INSTANCE(HardwareSerial, Serial2, midi2);





// Interrupt Service Routines
void IRAM_ATTR Rotation();
void IRAM_ATTR ButtonPressed();
void IRAM_ATTR MyHandleNoteOn(byte channel, byte pitch, byte velocity);
void IRAM_ATTR MyHandleNoteOff(byte channel, byte pitch, byte velocity);

// Global Objects
CRGB leds[NUM_LEDS];
CRGB showFrameLeds[NUM_LEDS];
QueueHandle_t xLedQueue;
RotaryEncoder rotEncode(PIN_A, PIN_B, PIN_SW);
Dot select(25, 3, 1);

// Globals for thread processes
uint8_t selection = 0;
int selectFlag = 0;
uint8_t lastRotationState;
unsigned long timer1 = 0;
static const BaseType_t pro_cpu = 0; // Assign pro_cpu to core 0
static const BaseType_t app_cpu = 1; // Assign app_cpu to core 1
static TaskHandle_t setFrameTask = NULL;
static QueueHandle_t msg_queue;
enum
{
  MSG_QUEUE_LEN = 20
}; // Number of slots in message queue
static SemaphoreHandle_t mutex; // Mutex for the byte queue
// Delays
static const uint32_t showFrameDelay = 100;
static const uint32_t setFrameDelay = 50;
static const uint32_t showFrameRoutineDelay = 10;
int firstLedRow[NUM_OF_ROWS];
int ledRowsQueue[NUM_OF_ROWS];

int adcValue;
int ledYValue;
int numberOfADCReadings;
int counter = 0;
int rowBuffer[NUM_OF_ROWS] = {0};

static int ledMap[8][64] = {{0, 15, 16, 31, 32, 47, 48, 63, 64, 79, 80, 95, 96, 111, 112, 127, 128, 143, 144, 159, 160, 175, 176, 191, 192, 207, 208, 223, 224, 239, 240, 255, 256, 271, 272, 287, 288, 303, 304, 319, 320, 335, 336, 351, 352, 367, 368, 383, 384, 399, 400, 415, 416, 431, 432, 447, 448, 463, 464, 479, 480, 495, 496, 511},
                            {1, 14, 17, 30, 33, 46, 49, 62, 65, 78, 81, 94, 97, 110, 113, 126, 129, 142, 145, 158, 161, 174, 177, 190, 193, 206, 209, 222, 225, 238, 241, 254, 257, 270, 273, 286, 289, 302, 305, 318, 321, 334, 337, 350, 353, 366, 369, 382, 385, 398, 401, 414, 417, 430, 433, 446, 449, 462, 465, 478, 481, 494, 497, 510},
                            {2, 13, 18, 29, 34, 45, 50, 61, 66, 77, 82, 93, 98, 109, 114, 125, 130, 141, 146, 157, 162, 173, 178, 189, 194, 205, 210, 221, 226, 237, 242, 253, 258, 269, 274, 285, 290, 301, 306, 317, 322, 333, 338, 349, 354, 365, 370, 381, 386, 397, 402, 413, 418, 429, 434, 445, 450, 461, 466, 477, 482, 493, 498, 509},
                            {3, 12, 19, 28, 35, 44, 51, 60, 67, 76, 83, 92, 99, 108, 115, 124, 131, 140, 147, 156, 163, 172, 179, 188, 195, 204, 211, 220, 227, 236, 243, 252, 259, 268, 275, 284, 291, 300, 307, 316, 323, 332, 339, 348, 355, 364, 371, 380, 387, 396, 403, 412, 419, 428, 435, 444, 451, 460, 467, 476, 483, 492, 499, 508},
                            {4, 11, 20, 27, 36, 43, 52, 59, 68, 75, 84, 91, 100, 107, 116, 123, 132, 139, 148, 155, 164, 171, 180, 187, 196, 203, 212, 219, 228, 235, 244, 251, 260, 267, 276, 283, 292, 299, 308, 315, 324, 331, 340, 347, 356, 363, 372, 379, 388, 395, 404, 411, 420, 427, 436, 443, 452, 459, 468, 475, 484, 491, 500, 507},
                            {5, 10, 21, 26, 37, 42, 53, 58, 69, 74, 85, 90, 101, 106, 117, 122, 133, 138, 149, 154, 165, 170, 181, 186, 197, 202, 213, 218, 229, 234, 245, 250, 261, 266, 277, 282, 293, 298, 309, 314, 325, 330, 341, 346, 357, 362, 373, 378, 389, 394, 405, 410, 421, 426, 437, 442, 453, 458, 469, 474, 485, 490, 501, 506},
                            {6, 9, 22, 25, 38, 41, 54, 57, 70, 73, 86, 89, 102, 105, 118, 121, 134, 137, 150, 153, 166, 169, 182, 185, 198, 201, 214, 217, 230, 233, 246, 249, 262, 265, 278, 281, 294, 297, 310, 313, 326, 329, 342, 345, 358, 361, 374, 377, 390, 393, 406, 409, 422, 425, 438, 441, 454, 457, 470, 473, 486, 489, 502, 505},
                            {7, 8, 23, 24, 39, 40, 55, 56, 71, 72, 87, 88, 103, 104, 119, 120, 135, 136, 151, 152, 167, 168, 183, 184, 199, 200, 215, 216, 231, 232, 247, 248, 263, 264, 279, 280, 295, 296, 311, 312, 327, 328, 343, 344, 359, 360, 375, 376, 391, 392, 407, 408, 423, 424, 439, 440, 455, 456, 471, 472, 487, 488, 503, 504}};

static int menuScreen[][2] = {{2, 2}, {2, 5}, {3, 2}, {3, 3}, {3, 4}, {3, 5}, {4, 2}, {4, 5}, {6, 3}, {6, 4}, {6, 5}, {7, 3}, {8, 3}, {8, 4}, {8, 5}, {10, 3}, {10, 4}, {10, 5}, {10, 6}, {11, 3}, {11, 5}, {12, 3}, {12, 4}, {12, 5}, {14, 3}, {14, 4}, {14, 5}, {15, 5}, {16, 3}, {16, 4}, {16, 5}, {18, 3}, {19, 2}, {19, 3}, {19, 4}, {19, 5}, {20, 3}, {22, 3}, {22, 5}, {28, 3}, {28, 4}, {28, 5}, {29, 2}, {30, 3}, {30, 4}, {30, 5}, {31, 2}, {32, 3}, {32, 4}, {32, 5}, {34, 2}, {34, 3}, {34, 4}, {34, 5}, {36, 2}, {36, 3}, {36, 4}, {36, 5}, {37, 2}, {37, 5}, {38, 3}, {38, 4}, {40, 2}, {40, 3}, {40, 4}, {40, 5}, {47, 3}, {47, 4}, {47, 5}, {48, 2}, {48, 4}, {49, 3}, {49, 4}, {49, 5}, {51, 2}, {51, 3}, {51, 4}, {51, 5}, {52, 5}, {53, 2}, {53, 3}, {53, 4}, {53, 5}, {55, 2}, {55, 3}, {55, 5}, {56, 4}, {57, 2}, {57, 3}, {57, 5}};

void Enqueue(int queue[], int sizeOfBuffer, int item)
{

  for (int i = sizeOfBuffer; i >= 0; i--)
  {
    queue[i] = queue[i - 1];
  }
  queue[0] = item;
}

int enqueueBit(int byteQueue, int thing)
{
  // Value must be 1 or zero
  if (thing == 1)
  {
    byteQueue = byteQueue << 1;
    byteQueue = byteQueue | thing;
  }
  else
  {
    byteQueue = byteQueue << 1;
  }
  return byteQueue;
}

void showFrame(void *parameters)
{

  while (1)
  {

    FastLED.show();
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
      for (int row = 0; row < NUM_OF_ROWS; row++)
      {
        // Enqueue the first led row into the queue
        ledRowsQueue[row] = enqueueBit(ledRowsQueue[row], firstLedRow[row]);
        int bitMask = 1;
        for (int i = 0; i < SIZE_OF_ROW; i++)
        {
          // Set led to red if bit is 1 and set led to black if bit is 0
          if ((bitMask & ledRowsQueue[row]) == bitMask)
          {
            coord point{row, i};
            Serial.println(row);
            int index = MatrixToLedString(point, SIZE_OF_ROW);
            leds[index] = CRGB::Red;
          }
          else
          {
            coord point{row, i};
            int index = MatrixToLedString(point, SIZE_OF_ROW);
            leds[index] = CRGB::Black;
          }
          bitMask = bitMask << 1;
        }
      }
      xSemaphoreGive(mutex);
    }

    vTaskDelay(showFrameDelay / portTICK_PERIOD_MS);
  }
}

void setFrame(void *parameters)
{
  while (1)
  {
    int pitchToIndex;
    coord point;
    if (xSemaphoreTake(mutex, 0) == pdTRUE)
    {
      for (int i = 0; i < uxQueueMessagesWaiting(msg_queue); i++)
      {
        if (xQueueReceive(msg_queue, (void *)&pitchToIndex, 10) == pdTRUE)
        {
          // Set led to red if an on note is sent
          // Serial.println(pitchToIndex);
          if ((pitchToIndex & ON_BIT_MASK) == ON_BIT_MASK)
          {
            firstLedRow[(pitchToIndex & (~ON_BIT_MASK))] = 1; // remove "ON" bit component
          }
          else
          {
            firstLedRow[pitchToIndex] = 0;
          }
        }
      }
      xSemaphoreGive(mutex);
    }
    vTaskDelay(setFrameDelay / portTICK_PERIOD_MS);
  }
}

void setup()
{

  // put your setup code here, to run once:
  for (int i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CRGB::Black;
  }
  FastLED.addLeds<NEOPIXEL, LED_DATA_PIN>(leds, NUM_LEDS); // GRB ordering is assumed
  Serial.begin(115200);
  for (int i = 0; i < ARRAY_SIZE(menuScreen); i++)
  {
    int x = menuScreen[i][0];
    int y = (menuScreen[i][1]);
    int z = ledMap[y][x];
    Serial.println(z);
    leds[z] = CRGB::Red;
  }
  SetLedsFromDotCorner(select.corners);
  Serial.println("ESP32 Started");
  attachInterrupt(PIN_A, Rotation, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_SW), ButtonPressed, CHANGE);
  while (selectFlag != 1)
  {
    if (millis() - timer1 > 200)
    {
      timer1 = millis();
      Serial.print("Last Rotation = ");
      if (rotEncode.GetLastRotationState() == CLOCKWISE)
      {
        Serial.println("Clockwise");
      }
      else
      {
        Serial.println("CounterClockwise");
      }
      Serial.print("Rotary counter = ");
      Serial.println(rotEncode.GetRotationCounter());
      if (lastRotationState != rotEncode.GetLastRotationState())
      {
        if (rotEncode.GetLastRotationState() == COUNTERCLOCKWISE)
        {
          select.MoveDot(44, 3);
          selection = AUX_SELECT;
        }
        else
        {
          selection = MIDI_SELECT;
          select.MoveDot(25, 3);
        }
        lastRotationState = rotEncode.GetLastRotationState();
      }
      SetLedsFromDotCorner(select.corners);
      ClearLedsFromDotCorner(select.lastCorners);
      FastLED.show();
    }
  }
  if (selection == MIDI_SELECT)
  {
    mutex = xSemaphoreCreateMutex();
    msg_queue = xQueueCreate(MSG_QUEUE_LEN, sizeof(int));
    midi2.begin(MIDI_CHANNEL_OMNI);          // Listen to all incoming messages
    midi2.setHandleNoteOn(MyHandleNoteOn);   // Sets ISR for a Note ON message
    midi2.setHandleNoteOff(MyHandleNoteOff); // Sets ISR for a Note OFF message
    xTaskCreatePinnedToCore(
        showFrame,    /* Task function. */
        "Show Frame", /* name of task. */
        10000,        /* Stack size of task */
        NULL,         /* parameter of the task */
        1,            /* priority of the task */
        NULL,         /* Task handle to keep track of created task */
        0);           /* pin task to core 0 */
    xTaskCreatePinnedToCore(
        setFrame,    /* Task function. */
        "Set Frame", /* name of task. */
        10000,       /* Stack size of task */
        NULL,        /* parameter of the task */
        2,           /* priority of the task */
        NULL,        /* Task handle to keep track of created task */
        0);          /* pin task to core 0 */
  }
  else
  {
    pinMode(ADC_PIN, INPUT);
    analogReadResolution(12);
    xLedQueue = xQueueCreate(10, sizeof(leds));
    if (xLedQueue == NULL)
    {
      Serial.println("Queue not created");
    }
    xTaskCreatePinnedToCore(
        showFrameRoutine, /* Task function. */
        "Show Frame",     /* name of task. */
        10000,            /* Stack size of task */
        NULL,             /* parameter of the task */
        2,                /* priority of the task */
        NULL,             /* Task handle to keep track of created task */
        0);               /* pin task to core 0 */
  }
}

void loop()
{
  // put your main code here, to run repeatedly:
  if (selection == MIDI_SELECT)
  {
    midi2.read();
  }
  else
  {
    // int timer = micros();

    adcValue = analogRead(ADC_PIN);
#ifdef debug
    Serial.print("ADC_VALUE:");
    Serial.print(adcValue);
    Serial.print(",");
#endif
    ledYValue = adcValue * SIZE_OF_ROW;
    ledYValue = ledYValue / 4096;
    if (ledYValue >= SIZE_OF_ROW)
      ledYValue = SIZE_OF_ROW - 1;
#ifdef debug
    Serial.print("LED_VALUE:");
    Serial.println(ledYValue);
// Serial.print(",");
#endif
    Enqueue(rowBuffer, NUM_OF_ROWS, ledYValue);
    for (int i = 0; i < NUM_OF_ROWS; i++)
    {
      int yValue = rowBuffer[i];
      for (int j = 0; j <= yValue; j++)
      {
        int ledIndex = ledMap[j][i];
        showFrameLeds[ledIndex] = CRGB::Blue;
      }
      for (int j = SIZE_OF_ROW - 1; j > yValue; j--)
      {
        int ledIndex = ledMap[j][i];
        showFrameLeds[ledIndex] = CRGB::Black;
      }
    }
    xQueueSend(xLedQueue, (void *)&showFrameLeds, 1);
  }
}

void IRAM_ATTR Rotation()
{
  rotEncode.Rotate();
}
void IRAM_ATTR ButtonPressed()
{

  unsigned long timePressed = rotEncode.SwitchChanges();
  //   Serial.print("Start Time = ");
  //   Serial.println(rotEncode._swPressed);
  //   Serial.print("End Time = ");
  //   Serial.println(rotEncode._swReleased);
  selectFlag = 1;
  if (timePressed >= 3000)
  {
    ESP.restart();
    selectFlag = 0;
  }
}

void IRAM_ATTR MyHandleNoteOn(byte channel, byte pitch, byte velocity)
{
  // digitalWrite(ON_BOARD_LED,HIGH);  //Turn LED on
  // Serial.println(pitch);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;

  int pitchToIndex = pitch - 36;
  // add on bit so there is a difference between on and off messages
  pitchToIndex = pitchToIndex | ON_BIT_MASK;
  // Serial.println(pitchToIndex);
  if (xQueueSendFromISR(msg_queue, (void *)&pitchToIndex, &xHigherPriorityTaskWoken) != pdTRUE)
  {
    Serial.println("Queue Full");
  }
}
void IRAM_ATTR MyHandleNoteOff(byte channel, byte pitch, byte velocity)
{
  // digitalWrite(ON_BOARD_LED,LOW);  //Turn LED off
  // Serial.println(pitch);
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  byte pitchToIndex = pitch - 36;
  if (xQueueSendFromISR(msg_queue, (void *)&pitchToIndex, &xHigherPriorityTaskWoken) != pdTRUE)
  {
    Serial.println("Queue Full");
  }
}

void showFrameRoutine(void *parameters)
{

  while (1)
  {
    if (xQueueReceive(xLedQueue, (void *)&leds, 0) == pdTRUE)
    {
      FastLED.show();
    }
    vTaskDelay(showFrameRoutineDelay / portTICK_PERIOD_MS);
  }
}

int MatrixToLedString(coord coord, int length)
{

  int index;
  index = length * coord.x;
  if ((coord.x % 2) == 0)
  {
    index += coord.y;
  }
  else
  {
    index = (length - 1) - coord.y + index;
  }
  return index;
}

void LedStripToMatrix(int index, coord *coord)
{
  if ((index / 8) % 2 == 0)
  {
    coord->y = index % 8;
  }
  else
  {
    coord->y = 7 - (index % 8);
  }
  coord->x = index / 8;
}
