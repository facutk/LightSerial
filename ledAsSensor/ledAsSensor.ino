//
// This example shows one way of using an LED as a light sensor.
// You will need to wire up your components as such:
//
//           + digital11
//           |
//           <
//           > 180 ohm resistor
//           <
//           |
//           |
//         -----
//          / \  LED, maybe a 5mm, clear plastic is good
//         -----
//           |
//           |
//           + digital12
//
// What we are going to do is apply a positive voltage at LED N Side and
// a low voltage at LED P Side. This is backwards for the LED, current will
// not flow and light will not come out, but we will charge up the 
// capacitance of the LED junction and the Arduino pin.
//
// Then we are going to disconnect the output drivers from LED P Side and
// count how long it takes the stored charge to bleed off through the 
// the LED. The brighter the light, the faster it will bleed away to 
// LED N Side.
//
 
#define LED_N_SIDE 11
#define LED_P_SIDE 12
#define MAX_COUNT 180

#define MAX_STATES 16

int states[ MAX_STATES ];
int currentState;
 
void setup()
{
  Serial.begin(9600);

  for(currentState = 0; currentState < MAX_STATES; currentState++) {
    states[currentState] = 0;
  }
  currentState = 0;
}
 
void lightMode()
{
  // Turn the light on
  pinMode(LED_N_SIDE,OUTPUT);
  pinMode(LED_P_SIDE,OUTPUT);
  digitalWrite(LED_P_SIDE,HIGH);
  digitalWrite(LED_N_SIDE,LOW);
}
 
void sensorMode()
{
  // Apply reverse voltage, charge up the pin and led capacitance
  pinMode(LED_N_SIDE,OUTPUT);
  pinMode(LED_P_SIDE,OUTPUT);
  digitalWrite(LED_N_SIDE,HIGH);
  digitalWrite(LED_P_SIDE,LOW);
 
  // Isolate the LED N Side of the diode
  // Turn off internal pull-up resistor
  pinMode(LED_N_SIDE,INPUT);
  digitalWrite(LED_N_SIDE,LOW);
}
 
boolean isDark = true;


 
void loop()
{
  if (currentState >= MAX_STATES) {
    String output = "";
    for(int i = 0; i < MAX_STATES; i++) {
      String tmp = String( states[i] );
      output = String( output + tmp );
    }
    
    Serial.print(output);
    currentState = 0;
  } else {
    states[ currentState ] = isDark;
    currentState++;
  }
  //if (isDark)
  //{
    lightMode();
    delay(25);
    sensorMode();
  //}
 
  isDark = true;
  for( byte j = 0; j < MAX_COUNT; j++ )
  {
    if ( digitalRead(LED_N_SIDE) == 0 ) 
    {
      isDark = false;
      break;
    }
  }
}
