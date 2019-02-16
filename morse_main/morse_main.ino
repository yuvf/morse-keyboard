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

#include <Bounce2.h>
#include <Keyboard.h>


const char BACKSPACE = 8;
const char ENTER = 10;
const unsigned int UNIT = 70;

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

// This struct contains a letter in Hebrew, and the corresponding final letter
typedef struct
{
  char key;
  char final_key;
} FinalKey;

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
              {"......", ENTER}, // Enter
              {".......", BACKSPACE},  // Backspace
              };

Key HEB_KEYS[] = {{".-", 't'}, {"-...", 'c'}, {"-.-.", 'x'}, {"-..", 's'}, {".", 'u'}, {"--.", 'd'}, {"....", 'j'}, {"..", 'h'},
              {".---", 'g'}, {"-.-", 'f'}, {".-..", 'k'}, {"--", 'n'}, {"-.", 'b'}, {"---", 'v'}, {".--.", 'p'}, {"--.-", 'e'},
              {".-.", 'r'}, {"...", 'a'}, {"-", ','}, {"..-", 'y'}, {".--", 'm'}, {"--..", 'z'}, {".----", '1'}, {"..---", '2'},
              {"...--", '3'}, {"....-", '4'}, {".....", '5'}, {"-....", '6'}, {"--...", '7'}, {"---..", '8'}, {"----.", '9'}, {"-----", '0'},
              {"......", ENTER}, // Enter
              {".......", BACKSPACE},  // Backspace
              };

FinalKey HEB_FINALS[] = {{'n', 'o'}, {'f', 'l'}, {'b', 'i'}, {'p', ';'}, {'m', '.'}};

String CHANGE_LANG = "........"; // For changing langauge
Key * CURRENT_LANG = NULL;
size_t CURRENT_LANG_SIZE = 0;
char LAST_CHAR;


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

  // turn LED off:
  digitalWrite(led_pin, LOW);

  add_input(DOWN_TIME);
}

void add_input(unsigned long down_time)
{
  if (down_time >= line)
  {
    CURRENT_CHAR += "-";
  }
  else
  {
    CURRENT_CHAR += ".";
  }
}

void key_down()
{
  DOWN_START_TIME = millis();
  IS_DOWN = true;

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

void write_letter(char to_write)
{
  Keyboard.print(to_write);
}

void replace_last_char(char new_char)
{
  write_letter(BACKSPACE);
  write_letter(new_char);
}

void change_final_letter()
/*
 * This function checks if the last letter that was written was a Hebrew final letter
 * If it is, it changes it to the corresponding final letter
 */
{
  if (CURRENT_LANG == &HEB_KEYS[0])
  {
    for (unsigned int i = 0; i<sizeof(HEB_FINALS)/sizeof(HEB_FINALS[0]); i++)
    {
      if (LAST_CHAR == HEB_FINALS[i].key)
      {
        replace_last_char(HEB_FINALS[i].final_key);
        return;
      }
    }
  }
}

void calculate_separator(unsigned long up_time)
{
  if (CURRENT_CHAR.length() != 0)
  {
    if (up_time >= letter)
    {
      calculate_letter();

      CURRENT_CHAR = "";
      WORD_END = false;
    }
  }
  else if (!WORD_END && up_time >= space)
  {
    change_final_letter();
    write_letter(' ');

    WORD_END = true;
  }
}

void change_lang()
{
  /*
   * Change language between Hebrew and English
   * Change by changing the pointer to the current language, and by changing the
   * system language with Ctrl+Alt
   */
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
      LAST_CHAR = CURRENT_LANG[i].key;
      return;
    }
  }
}
