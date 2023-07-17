// ----
// esp32 devkit c
// ---

// headers
const int HEADER = 0xff;
const int NUM_CH = 16;

// for shift registers (optocoupler switching)
const int SR_SER = 32;  // Keigo's = SER (DI)
const int SR_RCLK = 33; // Keigo's = CLK (CK)
const int SR_CLR = 25;  // Keigo's = RCLK (RC)
const int SR_CLK = 26;  // Keigo's = 26
const int SR_OE = 27;

const int STATE_OPEN = 0;
const int STATE_GND = 1;
const int STATE_HIGH = 2;

// change this when you changed the number of boards!!
const int channel_num = 16;
const int register_num = (channel_num * 2) * 1;
int register_state[register_num] = {0};

int channel_state[channel_num] = {STATE_OPEN};
// -----

// ------ electrical stimulation -----
void set_all_channel()
{
  digitalWrite(SR_CLK, LOW);
  digitalWrite(SR_RCLK, LOW);

  for (int i = 0; i < channel_num; i++)
  {
    if (channel_state[i] == STATE_OPEN)
    {
      // LOW
      digitalWrite(SR_SER, LOW);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
      // LOW
      digitalWrite(SR_SER, LOW);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
    }
    else if (channel_state[i] == STATE_HIGH)
    {
      // LOW
      digitalWrite(SR_SER, LOW);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
      // HIGH
      digitalWrite(SR_SER, HIGH);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
    }
    else if (channel_state[i] == STATE_GND)
    {
      // HIGH
      digitalWrite(SR_SER, HIGH);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
      // LOW
      digitalWrite(SR_SER, LOW);
      digitalWrite(SR_CLK, HIGH);
      digitalWrite(SR_CLK, LOW);
    }
  }

  digitalWrite(SR_SER, LOW);
  digitalWrite(SR_RCLK, HIGH);
  digitalWrite(SR_RCLK, LOW);
}

// ------

void setup()
{
  // --- for shift registers
  pinMode(SR_SER, OUTPUT);
  pinMode(SR_CLK, OUTPUT);
  pinMode(SR_RCLK, OUTPUT);
  pinMode(SR_CLR, OUTPUT);
  pinMode(SR_OE, OUTPUT);
  digitalWrite(SR_OE, LOW);
  digitalWrite(SR_CLR, HIGH);

  for (int i = 0; i < channel_num; i++)
  {
    channel_state[i] = STATE_OPEN;
  }
  set_all_channel();

  //  Serial.begin(921600);
  Serial.begin(1500000);
}

void loop()
{
  // each channel (N byte) + header (1 byte)
  if (Serial.available() >= NUM_CH)
  {
    int header = Serial.read();
    if (header == HEADER)
    {
      byte state = 0;
      for (int i = 0; i < channel_num; i++)
      {
        state = Serial.read();
        if (state < 0 || state > 2)
        {
          // out of range. something went wrong
          continue;
        }
        channel_state[i] = state;
      }
      set_all_channel();
    }
  }
}
