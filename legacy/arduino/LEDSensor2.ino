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
// What we are going to do is apply a positive voltage at digital2 and
// a low voltage at digital3. This is backwards for the LED, current will
// not flow and light will not come out, but we will charge up the 
// capacitance of the LED junction and the Arduino pin.
//
// Then we are going to disconnect the output drivers from digital2 and
// count how long it takes the stored charge to bleed off through the 
// the LED. The brighter the light, the faster it will bleed away to 
// digital3.
//
// Then just to be perverse we will display the brightness back on the 
// same LED by turning it on for a millisecond. This happens more often
// with brighter lighting, so the LED is dim in a dim room and brighter 
// in a bright room. Quite nice.
//
// (Though a nice idea, this implementation is flawed because the refresh
// rate gets too long in the dark and it flickers disturbingly.)
//
#define LED_N_SIDE 11
#define LED_P_SIDE 12

#define MAX_COUNT 180

#define RED 3
#define GREEN 6
#define BLUE 5


void setup()
{
  pinMode(13,OUTPUT);
  digitalWrite(13,LOW);
  Serial.begin(9600);
  
  
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
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

  // Isolate the pin 2 end of the diode
  pinMode(LED_N_SIDE,INPUT);
  digitalWrite(LED_N_SIDE,LOW);  // turn off internal pull-up resistor
}

void paintColor( byte colorId )
{
  int redValue, greenValue, blueValue;
  switch ( colorId )
  {
    case 0:
      // black    00 00 00
      redValue = 0;
      greenValue = 0;
      blueValue = 0;
      break;
    case 1:
      // gray     80 80 80
      redValue = 128;
      greenValue = 128;
      blueValue = 128;
      break;
    case 2:
      // maroon   80 00 00
      redValue = 128;
      greenValue = 0;
      blueValue = 0;
      break;
    case 3:
      // red      FF 00 00
      redValue = 255;
      greenValue = 0;
      blueValue = 0;
      break;
    case 4:
      // green    00 80 00
      redValue = 0;
      greenValue = 128;
      blueValue = 0;
      break;
    case 5:
      // lime     00 FF 00
      redValue = 0;
      greenValue = 255;
      blueValue = 0;
      break;
    case 6:
      // olive    80 80 00
      redValue = 128;
      greenValue = 128;
      blueValue = 0;
    case 7:
      // yellow   FF FF 00
      redValue = 255;
      greenValue = 255;
      blueValue = 0;
      break;
    case 8:
      // navy     00 00 80
      redValue = 0;
      greenValue = 0;
      blueValue = 128;
      break;
    case 9:
      // blue     00 00 FF
      redValue = 0;
      greenValue = 0;
      blueValue = 255;
      break;
    case 10:
      // purple   80 00 80
      redValue = 128;
      greenValue = 0;
      blueValue = 128;
      break;
    case 11:
      // fuchsia  FF 00 FF
      redValue = 255;
      greenValue = 0;
      blueValue = 255;
      break;
    case 12:
      // teal     00 80 80
      redValue = 0;
      greenValue = 128;
      blueValue = 128;
      break;
    case 13:
      // aqua     00 FF FF
      redValue = 0;
      greenValue = 255;
      blueValue = 255;
      break;
    case 14:
      // silver   C0 C0 C0
      redValue = 192;
      greenValue = 192;
      blueValue = 192;
      break;
    case 15:
      // white    FF FF FF
      redValue = 255;
      greenValue = 255;
      blueValue = 255;
      break;
  }
  
  analogWrite(RED, redValue);
  analogWrite(GREEN, greenValue);
  analogWrite(BLUE, blueValue);
}

boolean nonZero = true;

byte ons[16];
byte counter;

void loop()
{
  if (nonZero)
  {
    lightMode();
    delay(5);
    sensorMode();
  }

  nonZero = true;
  for( byte j = 0; j < MAX_COUNT; j++ )
  {
    if ( digitalRead(LED_N_SIDE) == 0 ) 
    {
      nonZero = false;
      break;
    }
  }
  
  if ( nonZero )
  {
    digitalWrite(13,LOW);
  }
  else
  {
    digitalWrite(13,HIGH);
  }
  
  counter +=1;
  if ( counter > 16 ) counter = 0;
  ons[ counter ] = nonZero;
  
  byte sum = 0;
  for( byte j = 0; j < 16; j++ )
  {
    sum += ons[ j ];
  }
  
  paintColor( sum );
  //Serial.print( sum, DEC );
  //unsigned int j;

  // Apply reverse voltage, charge up the pin and led capacitance
  //pinMode(LED_N_SIDE,OUTPUT);
  //pinMode(LED_P_SIDE,OUTPUT);
  //digitalWrite(LED_N_SIDE,HIGH);
  //digitalWrite(LED_P_SIDE,LOW);

  // Isolate the pin 2 end of the diode
  //pinMode(LED_N_SIDE,INPUT);
  //digitalWrite(LED_N_SIDE,LOW);  // turn off internal pull-up resistor

  // Count how long it takes the diode to bleed back down to a logic zero
  //Read the amount of Light coming into the LED sensor
  //long darkness=0;
  //int lightLevel=0;

  //while((digitalRead(LED_N_SIDE)!=0) && darkness < max_darkness){
  //   darkness++;
  //}

  //lightLevel=((max_darkness-darkness)+1)/80;
  //Serial.println(lightLevel);

  // You could use 'j' for something useful, but here we are just using the
  // delay of the counting.  In the dark it counts higher and takes longer, 
  // increasing the portion of the loop where the LED is off compared to 
  // the 1000 microseconds where we turn it on.

  // Turn the light on for 1000 microseconds
  //pinMode(LED_N_SIDE,OUTPUT);
  //pinMode(LED_P_SIDE,OUTPUT);
  //digitalWrite(LED_P_SIDE,HIGH);
  //digitalWrite(LED_N_SIDE,LOW);


  //delayMicroseconds(1000);
  // we could turn it off, but we know that is about to happen at the loop() start
}


