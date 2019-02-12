#include <Bounce2.h>

const int UNIT = 300;

enum separatorType
{
  space=UNIT*7,
  dot=UNIT*1,
  line=UNIT*3,
  part_letter=UNIT*1,
  letter=UNIT*3,
  error=0,
};

const int button_pin = 11;     // the number of the pushbutton pin
const int led_pin =  13;      // the number of the LED pin

// variables will change:
Bounce morse_key = Bounce(button_pin, 10);
separatorType separator = space;
int button_state = 0;         // variable for reading the pushbutton status
unsigned long last_time = 0;
unsigned long gap_time = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(led_pin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(button_pin, INPUT);
  Serial.begin(9600);
  Serial.println("Starting...");
  last_time = millis();
}

void loop() {
  if (morse_key.update())
  {
    if (morse_key.risingEdge())
    {
      key_up();
    }
    else if (morse_key.fallingEdge())
    {
      key_down();
    }
  }
}

void key_down()
{
  Serial.print("Down time: ");
  Serial.print(millis() - last_time);
  Serial.println();
  last_time = millis();
  
  // turn LED on:
  digitalWrite(led_pin, LOW);

  separator = check_separation();
  Serial.println(separator);
}

separatorType check_separation()
{
  gap_time = millis() - last_time;
  Serial.println(space);
  if (gap_time >= space)
  {
    return space;
  }
  if (gap_time >= letter)
  {
    return letter;
  }
  if (gap_time >= part_letter)
  {
    return part_letter;
  }
  return error;
}

void key_up()
{
  Serial.print("Up time: ");
  Serial.print(millis() - last_time);
  Serial.println();
  last_time = millis();

  // turn LED off:
  digitalWrite(led_pin, HIGH);
}
