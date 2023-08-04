// ----
// esp32 devkit c
// ---

// headers
enum {
    REQUEST_CHANGE_STATE = 0xff,
    REQUEST_CHECK_STATE = 0xfe,
    REQUEST_CHANGE_ECHO_MODE = 0xfd
};
const int NUM_CH_USED = 12;

// for shift registers (optocoupler switching)
const int SR_SER    = 32;   // Keigo's = SER (DI)
const int SR_CLK    = 33;   // on board, it is written RCK but it's shift register's clk //26;  // Keigo's = 26
const int SR_CLR    = 25;   // Keigo's = RCLK (RC)
const int SR_RCLK   = 26;   // on board, it is written CK but it's actually a ratch's clk //33; // Keigo's = CLK (CK)
const int SR_OE     = 27;   // not used

const int STATE_OPEN = 0;
const int STATE_GND = 1;
const int STATE_HIGH = 2;

// change this when you changed the number of boards!!
const int board_num = 1;
const int channel_num = 16;
const int register_num = (channel_num * 2) * board_num;
int register_state[register_num] = {0};
int channel_state[channel_num] = {STATE_OPEN};
enum {
    REACT_ON_EVERY_REQUEST = 0,
    ECHO_ONLY_ON_ECHO_REQUEST = 1,
    NO_ECHO = 2
};
int echo_mode = NO_ECHO;
// -----

// ------ electrical stimulation -----
void set_all_channel()
{
    digitalWrite(SR_CLK, LOW);
    digitalWrite(SR_RCLK, LOW);

    for (int i = channel_num - 1; i >= 0 ; i--)
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
    static int header = 0;
    if (Serial.available()) {
        header = Serial.read();
        if (echo_mode == REACT_ON_EVERY_REQUEST) {
            Serial.print("ECHO Header: ");
            Serial.print(header);
            Serial.print("\n");
        }
        if (header == REQUEST_CHANGE_ECHO_MODE)
        {
            if (Serial.available())
            {
                echo_mode = Serial.read();
            }
            if (echo_mode != NO_ECHO)
            {
                Serial.print("Echo mode has been changed to: ");
                Serial.print(echo_mode);
                Serial.print("\n");
            }
        }
        else if (header == REQUEST_CHECK_STATE)
        {
            Serial.print("Channel status: ");
            for (int i = 0; i < channel_num; i++) {
                Serial.print(channel_state[i]);
                Serial.print(" ");
            }
            Serial.print("\n");
        }
        else if (header == REQUEST_CHANGE_STATE)
        {
            if (Serial.available() >= NUM_CH_USED)
            {
                byte state = 0;
                for (int i = 0; i < channel_num; i++)
                {
                    channel_state[i] = STATE_OPEN;
                }
                for (int i = 0; i < NUM_CH_USED; i++)
                {
                    state = Serial.read();
                    if (state < 0 || 2 < state)
                    {
                        // out of range. something went wrong
                        continue;
                    }
                    channel_state[i] = state;
                }
                set_all_channel();

                if (echo_mode == REACT_ON_EVERY_REQUEST)
                {
                    Serial.print("all channels has been set\n");
                }
            }
            else {
                if (echo_mode == REACT_ON_EVERY_REQUEST)
                {
                    Serial.print("ERROR: The received data size was small\n");
                }

            }
        }
    }
}
