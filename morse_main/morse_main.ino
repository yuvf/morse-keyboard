const int buttonPin = 11;     // the number of the pushbutton pin
const int ledPin =  13;      // the number of the LED pin

// variables will change:
int buttonState = 0;         // variable for reading the pushbutton status
int isDown = 0;

void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  Serial.begin(9600);
  Serial.write("Test2");
  buttonState = digitalRead(buttonPin);
  if (buttonState == HIGH)
  {
    Serial.print("HIGH");
  }
  else
  {
    Serial.print("LOW");
  }
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    if (isDown == 0)
    {
      //Serial.write("Button down");
      isDown = 1;
    }
    
    // turn LED on:
    digitalWrite(ledPin, HIGH);
  } else {
    // turn LED off:
    isDown = 0;
    digitalWrite(ledPin, LOW);
  }
}
