// code is working fine but have max delay;
#include <math.h>

#include <SoftwareSerial.h>
int the_big_one[9][9] = {0}; // main array to control the switching
int button_number = 0;
int status = 0;
int room_number = 0;
int multi = 0;

const int dataPin = 3;
const int latchPin = 5;
// Pin connected to clock pin (SH_CP) of 74HC595
const int clockPin = 6;
int globe_switching_delay = 1; // 1 for 1/1000, or 1000th of a sec 
////Pin connected to Data in (DS) of 74HC595

unsigned int num = 0;
void setup()
{
    // Set all the pins of 74HC595 as OUTPUT
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    delay(1000);
    all_blinker();
    delay(500);
    reseter();
    delay(500);
    Serial.begin(9600);
    //--------------------------------------------//
    num = 11127;
    main_array_modifier(num);
    //--------------------------------------------//
    delay(100);
    switch_control();
}

void loop()
{
    if (Serial.available() >= 1) // is a number available?
    {
        unsigned int numbri = Serial.parseInt(); // get the number
        if (numbri > 0)
        {
            main_array_modifier(numbri);
            Serial.println("working on : " + String(numbri));
            delay(50);
            switch_control();
        }
        else
        {
            // Serial.println("Not a number");
        }
    }
}
void reseter()
{
    int i = 0;
    digitalWrite(dataPin, LOW);
    digitalWrite(latchPin, LOW);
    digitalWrite(clockPin, LOW);

    // digitalWrite (dataPin, LOW);
    delay(10);
    for (i = 0; i < 8; i++)
    {
        digitalWrite(clockPin, HIGH);
        delay(10);
        digitalWrite(clockPin, LOW);
        delay(10);
    }
    digitalWrite(latchPin, HIGH);
    delay(10);
    digitalWrite(latchPin, LOW);
    delay(10);
}
void all_blinker()
{
    digitalWrite(latchPin, LOW);
    digitalWrite(clockPin, LOW);
    digitalWrite(dataPin, HIGH);

    int i = 0;
    // digitalWrite (dataPin, LOW);
    delay(100);
    for (i = 0; i < 8; i++)
    {
        digitalWrite(clockPin, HIGH);
        delay(100);
        digitalWrite(clockPin, LOW);
        delay(10);
    }
    digitalWrite(latchPin, HIGH);
    delay(10);
    digitalWrite(latchPin, LOW);
    delay(10);
}
void main_array_modifier(unsigned int inputer)
{

    int nDigits = floor(log10(abs(inputer))) + 1; // to get the lenght of inputed number
    int temp_data_holder = 0;
    temp_data_holder = nDigits - 2;
    int i = 0;
    int power_holder = 10;
    for (i = 1; i < temp_data_holder; i++)
    {
        power_holder *= 10;
    }
    button_number = (inputer % power_holder);
    int remining_data_holder = 0;
    remining_data_holder = (inputer / power_holder);
    status = (remining_data_holder % 10);
    room_number = (remining_data_holder / 10);
    int ctr = 0;
    int holder = 0;
    int new_num = button_number;
    the_big_one[room_number - 1][0] = status;                        // assign status to array
    for (ctr = 0; ctr < floor(log10(abs(button_number))) + 1; ctr++) // switches on off control
    {
        holder = new_num % 10;
        if (holder > 0)
        {
            if (the_big_one[room_number - 1][holder] == 0 && (status == 1))
            {
                the_big_one[room_number - 1][holder] = 1;
            }
            else if (the_big_one[room_number - 1][holder] == 1 && (status) == 0)
            {
                the_big_one[room_number - 1][holder] = 0;
            }
        }
        new_num = new_num / 10;
    }
}

void switch_control()
{
    int i = 0;
    int j = 0;
    int status = 0;
    for (i = 9; i >= 0; i--) // control room number
    {
        for (j = 1; j < 9; j++) // control switch and status
        {
            if (the_big_one[i][j] == 1)
            {
                digitalWrite(dataPin, HIGH);
                delay(globe_switching_delay);
                digitalWrite(clockPin, HIGH);
                delay(globe_switching_delay);

                digitalWrite(dataPin, LOW);
                delay(globe_switching_delay);
                digitalWrite(clockPin, LOW);
                delay(globe_switching_delay);
            }
            else
            {
                digitalWrite(dataPin, LOW);
                delay(globe_switching_delay);

                digitalWrite(clockPin, HIGH);
                delay(globe_switching_delay);
                digitalWrite(clockPin, LOW);
                delay(globe_switching_delay);
            }
        }
    }
    digitalWrite(latchPin, HIGH);
    delay(globe_switching_delay);
    digitalWrite(latchPin, LOW);
    delay(globe_switching_delay);
}
