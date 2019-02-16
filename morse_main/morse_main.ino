/* File: morse_main.ino
 * Author: Yuval Fisher
 * 
 * Morse key as a keyboard, for typing in English and in Hebrew
 */

/*
 * Details:
 * Morse key connected in pin 11 as a button
 * Made up sequences - "......" for Enter, "......." for Backspace, "........" for changing language
 */

#include <HashMap.h>
#include <Bounce2.h>
#include <Keyboard.h>


const int UNIT = 100;

enum separatorType
{
  space = UNIT * 7 ,
  dot = UNIT * 1,
  line = UNIT * 3,
  part_letter = UNIT * 1,
  letter = UNIT * 3,
  pause = UNIT * 12,
  error = 0,
};

// This struct contains a singal key and it's symbols. An array of structs should be created of every langauge
typedef struct
{
  String symbols;
  char key;
} Key;

// pins
const int button_pin = 11;     // the number of the morse key input
const int led_pin =  13;      // the number of the LED pin

// global variables
unsigned long DOWN_START_TIME = 0; // saves the start time when the key is down
unsigned long DOWN_TIME = 0; // saves the calculated time of the down time
unsigned long UP_START_TIME = 0; // saves the start time when the key is up
unsigned long UP_TIME = 0; // saves the calculated time of the up key
String CURRENT_CHAR = ""; // saves the dots and lines of the current written char
bool WORD_END = true; // False - space hasn't been written yet. True - space was written
bool IS_DOWN = false; // A bool to check if the key is down, used to check if the key is up enough time to write the buffer or space

Bounce morse_key = Bounce(button_pin, 10); // The object of the morse key. Fixes bounce issues of metal first contact

// Create an array of keys mapped to symbols
Key ENG_KEYS[] = {{".-", 'a'}, {"-...", 'b'}, {"-.-.", 'c'}, {"-..", 'd'}, {".", 'e'}, {"..-.", 'f'}, {"--.", 'g'}, {"....", 'h'}, {"..", 'i'},
              {".---", 'j'}, {"-.-", 'k'}, {".-..", 'l'}, {"--", 'm'}, {"-.", 'n'}, {"---", 'o'}, {".--.", 'p'}, {"--.-", 'q'}, {".-.", 'r'},
              {"...", 's'}, {"-", 't'}, {"..-", 'u'}, {"...-", 'v'}, {".--", 'w'}, {"-..-", 'x'}, {"-.--", 'y'}, {"--..", 'z'}, {".----", '1'},
              {"..---", '2'}, {"...--", '3'}, {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}, {"-----", '0'},
              {"......", 10}, // Enter
              {".......", 8},  // Backspace
              };

Key HEB_KEYS[] = {{".-", 't'}, {"-...", 'c'}, {"-.-.", 'x'}, {"-..", 's'}, {".", 'u'}, {"--.", 'd'}, {"....", 'j'}, {"..", 'h'},
              {".---", 'g'}, {"-.-", 'f'}, {".-..", 'k'}, {"--", 'n'}, {"-.", 'b'}, {"---", 'v'}, {".--.", 'p'}, {"--.-", 'e'},
              {".-.", 'r'}, {"...", 'a'}, {"-", ','}, {"..-", 'y'}, {".--", 'm'}, {"--..", 'z'}, {".----", '1'}, {"..---", '2'},
              {"...--", '3'}, {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}, {"-----", '0'},
              {"......", 10}, // Enter
              {".......", 8},  // Backspace
              };

String CHANGE_LANG = "........"; // For changing langauge
Key * CURRENT_LANG = NULL;
size_t CURRENT_LANG_SIZE = 0;


void setup() {
  // initialize the LED pin as an output:
  pinMode(led_pin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(button_pin, INPUT);

  Serial.begin(9600);
  Serial.println("Starting...");

  // Set the start of the up time to the beginning of the run
  UP_START_TIME = millis();

  // Initialize the keyboard
  Keyboard.begin();

  // Set current language
  CURRENT_LANG = &HEB_KEYS[0];
  CURRENT_LANG_SIZE = sizeof(HEB_KEYS);
}

void loop() {
  // Call update to check if chars should be written
  update();

  // if there is change of state (up or down)
  if (morse_key.update())
  {
    if (morse_key.risingEdge())
    {
      // morse key is now down. Call key down function
      key_down();
    }
    else if (morse_key.fallingEdge())
    {
      // morse key is now up. Call key up function
      key_up();
    }
  }
}

void key_up()
{
  UP_START_TIME = millis();
  DOWN_TIME = UP_START_TIME - DOWN_START_TIME;
  IS_DOWN = false;
  //Serial.print("Down time: ");
  //Serial.println(DOWN_TIME);

  // turn LED off:
  digitalWrite(led_pin, LOW);

  add_input(DOWN_TIME);
}

void add_input(unsigned long down_time)
{
  if (down_time >= line)
  {
    //Serial.print("LINE ");
    CURRENT_CHAR += "-";
  }
  //else if (down_time >= dot)
  else
  {
    //Serial.print("DOT ");
    CURRENT_CHAR += ".";
  }
}

void key_down()
{
  DOWN_START_TIME = millis();
  IS_DOWN = true;

  //Serial.print("Up time: ");
  //Serial.println(UP_TIME);

  // turn LED on:
  digitalWrite(led_pin, HIGH);


}

void update()
{
  /* This function writes the correct chars if the morse key is "up" long enough
     The morse key can be is "up" state for a long time, like when the user finishes to type.
     That's why we should call a different function to check if something needs to be written,
     and to not wait of the user to change the state to "down".
  */
  if (!IS_DOWN)
  {
    UP_TIME = millis() - UP_START_TIME;
    calculate_separator(UP_TIME);
  }
}

void calculate_separator(unsigned long up_time)
{
  if (CURRENT_CHAR.length() != 0)
  {
    if (up_time >= letter)
    {
      calculate_letter();
      //Serial.print(" CHAR ");

      CURRENT_CHAR = "";
      WORD_END = false;
    }
  }
  else if (!WORD_END && up_time >= space)
  {
    write_letter(' ');

    WORD_END = true;
  }
}

void write_letter(char to_write)
{
  Keyboard.print(to_write);
}

void change_lang()
{
  if (CURRENT_LANG == &ENG_KEYS[0])
  {
    CURRENT_LANG = &HEB_KEYS[0];
    CURRENT_LANG_SIZE = sizeof(HEB_KEYS);
  }
  else
  {
    CURRENT_LANG = &ENG_KEYS[0];
    CURRENT_LANG_SIZE = sizeof(ENG_KEYS);
  }
  Keyboard.press(KEY_LEFT_SHIFT);
  Keyboard.press(KEY_LEFT_ALT);
  Keyboard.release(KEY_LEFT_SHIFT);
  Keyboard.release(KEY_LEFT_ALT);
}

void calculate_letter()
{
  if (CURRENT_CHAR.equals(CHANGE_LANG))
  {
    change_lang();
    return;
  }
  for (unsigned int i=0; i<(CURRENT_LANG_SIZE/sizeof(Key)); i++)
  {
    if (CURRENT_CHAR.equals(CURRENT_LANG[i].symbols))
    {
      write_letter(CURRENT_LANG[i].key);
      return;
    }
  }
  Serial.print(CURRENT_CHAR);
}
