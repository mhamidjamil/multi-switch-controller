//latest Update 23-7-22
// #include<arduino.h>
// 74HC595 related
byte the_big_one[9][9] = {0}; // main array to control the switching
int button_number = 0;
byte status = 0;
byte room_number = 0;
byte multi = 0;

// configuring pins of 74HC595 pins
const byte dataPin = 3;
////Pin connected to Data in (DS) of 74HC595
const byte latchPin = 5;
// Pin connected to latchPin pin (ST_CP) of 74HC595 (Storage Register clock input)
const byte clockPin = 6;
// Pin connected to clock pin (SH_CP) of 74HC595 (Shift Register clock input)
byte output_voltage = 120; //0 to 5 mapped to 0 to 255;
byte installed_ICs = 2;
byte relay_1 = 25;
byte relay_2 = 26;
bool relay_1_done = 0;
bool relay_2_done = 0;

unsigned int dumy_current_seconds = 0;
bool alfa = 0;
bool beta = 0;
byte current_loop = 0;
int divider = 0;
bool repetition = false;
// looping+++++++++++++++++++++++++++++++++++++++++++++
//  int loop_starter = 4;
byte looper = 4;
byte looping = 0;
bool looping_in_process = false;
byte loop_switch_number = 0;
byte loop_room_number = 0;
unsigned loop_on_seconds = 0;
unsigned loop_off_seconds = 0;
int loop_global_variable = 0; // it will be sent to main_array_modifier()
unsigned int dumy_on_seconds = 0;
unsigned int dumy_on_seconds_main = 0;

unsigned int dumy_off_seconds = 0;
unsigned int dumy_off_seconds_main = 0;
bool on_hoa = false;

//+++++++++++++++++++++++++++++++++++++++++++++++++++++

//*******************************
// double time = 0;
unsigned int seconds = 0;
// int sec_by_ten = 0;
int sec_by_hundred = 0;
int sec_by_thousand = 0;
// int sec_by_micro = 0;
unsigned int req_sec = 0;
unsigned long int delay_switching[10][5] = {0};
// int flag = 0;
bool clean = false;
unsigned int required_time_for_relay_1 = 0;
unsigned int required_time_for_relay_2 = 0;
//*******************************

// 5 and 6 are free pins
void setup() // heart ♥ of code
{
    Serial.begin(9600); // Initiate a serial communication

    Serial.println("New run");

    //--------------------------------------------//
    // Set all the pins of 74HC595 as OUTPUT
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    reseter(); // reset the previous data stored in 74HC595IC
    main_array_modifier(111);
    switch_control();
    //--------------------------------------------//
}

void loop()
{
    time_control_1000_of_sec(1);
    switch_timing_checker(); // chek if any switch have to change state acording to time
    if (looping_in_process == false)
    {
        seconds_reseter(); // reset time if all switches are time independant
    }
    else
    {
        switch_loop_control(0, 0, 0, 0);
    }
    if (seconds > 3)
    {
        clock_control(0, 0);
    }
    // Serial.println("Closing Door");
    if (Serial.available() >= 1) // is a number available?
    {
        unsigned int numbri = Serial.parseInt(); // get the number
        if (numbri > 0)
        {
            if (numbri == 2 || looper < 4)
            {
                time_control_100_of_sec(2);
                if (looper == 4)
                {
                    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
                    Serial.println("Looping of switches starting...");
                    looping_in_process = true;
                    Serial.print("Enter Room Number : ");
                    looper--; // looper = 3
                }

                else if (looper == 3)
                {

                    loop_room_number = numbri;
                    Serial.println(loop_room_number);

                    Serial.print("Enter number of switch : ");
                    looper--; // looper = 2
                }
                else if (looper == 2)
                {
                    loop_switch_number = numbri;
                    Serial.println(loop_switch_number);
                    Serial.print("Enter On time in seconds : ");
                    looper--; // looper = 1
                }
                else if (looper == 1)
                {
                    loop_on_seconds = numbri;
                    Serial.println(loop_on_seconds);
                    Serial.print("Enter Off time in seconds : ");
                    looper--; // looper = 1
                }
                else
                {
                    loop_off_seconds = numbri;
                    Serial.println(loop_off_seconds);
                    Serial.println("Leaving loop body and sending data to loop function with detail :");

                    Serial.print("Room Number : ");
                    Serial.print(loop_room_number);

                    Serial.print(" Switch number : ");
                    Serial.print(loop_switch_number);

                    Serial.print(" seconds On time : ");
                    Serial.print(loop_on_seconds);

                    Serial.print(" seconds Off time : ");
                    Serial.println(loop_off_seconds);

                    Serial.println("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$");
                    switch_loop_control(loop_room_number, loop_switch_number, loop_on_seconds, loop_off_seconds);
                    looping_in_process = true;
                    looper = 4;
                }
            }
            else if (numbri == 3)
            {
                looping_in_process = false;
                Serial.println("``````````````Cancling looping process`````````````");
            }
            else if (numbri == 5)
            {

                Serial.println("________________________________________");
                Serial.println("Reseting shift register memory : 10");
                Serial.println("Time update : 15");
                Serial.println("Reseting Both timing relays : 20 ");
                Serial.println("looping process : 2 ");
                Serial.println("to stop looping : 3");
                Serial.println("________________________________________");

                // Serial.println("")
            }
            else if (numbri == 10)
            {
                Serial.println("Reseting shift register memory");
                reseter();
                seconds = 0;
            }
            else if (numbri == 15)
            {
                Serial.print("**** Updated time : ");
                Serial.print(seconds);
                Serial.println(" ****");
            }
            else if (numbri == 20)
            {
                Serial.println("Reseting Both timing relays");
                clock_control(1, 1);
            }
            else if (numbri >= 101)
            {
                Serial.print("working on : ");
                Serial.println(numbri);
                main_array_modifier(numbri); // 1st of all number will be send to array_modifire which arange switches
                switch_control();            // then this function will execute the data of array_modifier...
            }
            // time_control_100_of_sec(3);
            numbri = 0;
        }
    }
}
void main_array_modifier(unsigned long int inputer) // control the input by user...brain of code
{

    int nDigits = floor(log10(abs(inputer))) + 1; // to get the lenght of inputed number
    int temp_data_holder = 0;
    temp_data_holder = nDigits - 2;
    int power_holder = 10;
    int i = 0;
    for (i = 1; i < temp_data_holder; i++)
    {
        power_holder *= 10;
    }
    button_number = (inputer % power_holder);
    int remining_data_holder = 0;
    remining_data_holder = (inputer / power_holder);

    status = (remining_data_holder % 10);
    room_number = (remining_data_holder / 10);
    byte ctr = 0;
    int holder = 0;
    int new_num = button_number;
    if (status != 0 && status != 1)
    {
        timer(room_number, status, button_number);
        button_number = 0;
    }
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

void switch_control() // backbone of Complete Switching
{
    int i = 0;
    int j = 0;
    // int status = 0;
    for (i = installed_ICs; i >= 0; i--) // control room number
    {
        for (j = 1; j < 9; j++) // control switch and status
        {
            if (the_big_one[i][j] == 1)
            {
                analogWrite(dataPin, output_voltage);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
                analogWrite(clockPin, output_voltage);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
                digitalWrite(dataPin, LOW);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
                digitalWrite(clockPin, LOW);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
            }
            else
            {
                digitalWrite(dataPin, LOW);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
                analogWrite(clockPin, output_voltage);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
                digitalWrite(clockPin, LOW);
                // time_control_1000000_of_sec(1);
                time_control_1000_of_sec(1);
            }
        }
    }
    // digitalWrite(latchPin, HIGH);
    analogWrite(latchPin, output_voltage);
    // time_control_1000000_of_sec(1);
    time_control_1000_of_sec(1);
    digitalWrite(latchPin, LOW);
    // time_control_1000000_of_sec(1);
    time_control_1000_of_sec(1);
}

void reseter()
{
    int i = 0, j = 0;
    digitalWrite(dataPin, LOW);
    digitalWrite(latchPin, LOW);
    digitalWrite(clockPin, LOW);

    // digitalWrite (dataPin, LOW);
    time_control_1000_of_sec(1);
    for (j = installed_ICs; j >= 0; j--)
    {
        for (i = 0; i < 8; i++)
        {
            the_big_one[j][i] = 0;
            digitalWrite(clockPin, HIGH);
            time_control_1000_of_sec(1);
            digitalWrite(clockPin, LOW);
            time_control_1000_of_sec(1);
        }
    }
    digitalWrite(latchPin, HIGH);
    time_control_1000_of_sec(2);
    digitalWrite(latchPin, LOW);
    time_control_1000_of_sec(1);
}

//______________________________________________//
// delay provider & control the seconds backbone of time_switching()
void time_control_complete_sec(int timer) // provides 1 second delay or delay(1000)
{
    for (; timer > 0; timer--)
    {
        delay(1000);
        seconds++;
    }
}
// void time_control_10_of_sec(int timer)
// { //provides 10 of second or (1/10) or delay(100)
//     for (; timer > 0; timer--)
//     {
//         delay(100);
//         sec_by_ten++;
//     }
//     if (sec_by_ten >= 10)
//     {
//         seconds++;
//         sec_by_ten = 0;
//     }
// }
void time_control_100_of_sec(int timer)
{ // provides 100 of second or (1/100) or delay(10)
    for (; timer > 0; timer--)
    {
        delay(10);
        sec_by_hundred++;
    }
    if (sec_by_hundred >= 100)
    {
        seconds++;
        sec_by_hundred = 0;
    }
}
void time_control_1000_of_sec(int timer)
{
    for (; timer > 0; timer--)
    {
        delay(1);
        sec_by_thousand++;
    }

    if (sec_by_thousand >= 1000)
    {
        sec_by_thousand = 0;
        seconds++;
    }
}
// void time_control_1000000_of_sec(int timer)
// {
//     for (; timer > 0; timer--)
//     {
//         delayMicroseconds(1);
//         sec_by_micro++;
//     }

//     if (sec_by_micro >= 18000)
//     {
//         sec_by_micro = 0;
//         seconds++;
//     }
// }
//_________________________________________________//

void timer(int room_number, int status, int switch_number)
{
    byte swh = 0;
    int temp_sn = switch_number;
    { // better output to see record
        Serial.println("//------------------------------------------------------//");
        Serial.print(" Switch Number : ");
        switch_number /= 100;
        Serial.print(switch_number);
        Serial.print(" Room number : ");
        Serial.print(room_number);
        if (status == 4 || status == 6 || status == 2)
        {
            Serial.print(" ON after : ");
            swh = 1;
        }
        else
        {
            Serial.print(" OFF after :  ");
            swh = 0;
        }

        if (status == 2 || status == 3)
        {
            Serial.print(temp_sn % 100);
            Serial.print(" Seconds");
            req_sec = temp_sn % 100;
        }
        else if (status == 4 || status == 5)
        {
            Serial.print(temp_sn % 100);
            Serial.print(" Minutes");
            req_sec = 60 * (temp_sn % 100);
        }
        else if (status == 6 || status == 7)
        {
            Serial.print(temp_sn % 100);
            Serial.print(" Hours");
            req_sec = 60 * 60 * (temp_sn % 100);
        }
        Serial.println();
        Serial.println("//------------------------------------------------------//");
        Serial.print("Total require seconds are : ");
        Serial.println(req_sec);
        timing_for_switch(((room_number * 10) + switch_number),
                          ((((room_number * 10) + swh) * 10) + switch_number), req_sec);
    }
}
void timing_for_switch(int room_number_AND_switch_number, int command, unsigned int req_seconds)
{
    // Serial.print("======Required status : ");
    // Serial.print(room_number_AND_switch_number);
    // Serial.println(" ======");
    if (room_number_AND_switch_number == relay_1 || room_number_AND_switch_number == relay_2)
    {
        if (room_number_AND_switch_number == relay_1)
        {
            clock_control(req_seconds, 0);
        }
        else
        {
            clock_control(0, req_seconds);
        }
    }
    int IK = 0;
    bool mil_jata_hay = false;
    if (repetition == false)
    {
        for (IK = 0; IK < 10; IK++)
        {
            if (delay_switching[IK][1] == room_number_AND_switch_number)
            { // if inputed Room_number and switch number are already have a record
                Serial.println("1 Match founded so over writing array");
                delay_switching[IK][0] = 1;
                delay_switching[IK][2] = command;
                delay_switching[IK][3] = req_seconds;
                delay_switching[IK][4] = seconds;
                mil_jata_hay = true;
                IK = 99;
                clean = true;
            }
        }
    }
    bool stored = false;
    if (mil_jata_hay == false)
    { // if inputed Room_number and switch number are not in the record
        for (IK = 0; IK < 10; IK++)
        {
            if (delay_switching[IK][0] == 0) // if 1st element of array (from 0 to 9) then we can store data
            {
                if (stored == false) // if data is not stored yet
                {
                    Serial.println("NO Match found so Creating new data");
                    delay_switching[IK][0] = 1;
                    delay_switching[IK][1] = room_number_AND_switch_number;
                    delay_switching[IK][2] = command;
                    delay_switching[IK][3] = req_seconds;
                    delay_switching[IK][4] = seconds;
                    stored = true; // data is now stored this conditoin will not run in this loop
                    IK = 99;
                    clean = true;
                }
            }
        }
    }
}

void switch_timing_checker()
{ // check all the switches which are supposed to on or off after a specific time
    int IK = 0;
    for (IK = 0; IK < 10; IK++)
    {
        if (delay_switching[IK][0] == 1) // if there is any data stored
        {
            int total_time = 0;
            total_time = delay_switching[IK][3] + delay_switching[IK][4];
            if ((total_time - 1) <= seconds) // if the required time is reached then switch will on/off
            {
                int command;
                delay_switching[IK][0] = 0; // data will clerad from array as it is not usefull after requird time
                delay_switching[IK][1] = 0; // cleaing data
                command = delay_switching[IK][2];
                Serial.print("Working on : ");
                Serial.println(command);
                Serial.print("Required time was :");
                Serial.print(delay_switching[IK][3]);
                Serial.println(" Seconds");
                // if the required time is reached then the array_modifier and switch_control will work as normal
                main_array_modifier(command);
                switch_control();

                delay_switching[IK][2] = 0; // cleaing data
                delay_switching[IK][3] = 0; // cleaing data
                delay_switching[IK][4] = 0; // cleaing data
            }
        }
    }
}

void seconds_reseter()
{ // set seconds to 0 if seconds are not in use so it will saves the process storage and overflow of varibale
    int IK = 0;
    bool data_parra_hay = false;
    for (IK = 0; IK < 10; IK++) // check the delay_switching if data found then data_parra_hay will becomes true
    {
        if (delay_switching[IK][0] == 1)
        {
            data_parra_hay = true;
        }
    }
    if (data_parra_hay == false) // if there is no data in the delay_switching
    {
        seconds = 0;
        if (clean == true) // it will print bellow command just for 1 time after when ever delay swiching is used
        {
            Serial.println("!!!....There is no data in seconds variable so cleaing seconds....!!!");
            clean = false;
        }
    }
}

void clock_control(unsigned int req_1, unsigned int req_2) // 2-1-5 and 2-1-6 are the relay switches
{                                                          // i have to control  room 1 (1 to 8 switches) and room 2 (1 to 4 switches)
    int current_deling_relay = 0;
    { // managing new incoming value
        if (req_1 == 1)
        {
            required_time_for_relay_1 = 0;
        }
        else if (req_2 == 1)
        {
            required_time_for_relay_2 = 0;
        }
        else if (req_1 > 1 || req_2 > 1)
        {
            // reset_clock();
            if (req_1 > 1)
            {
                required_time_for_relay_1 = req_1;
                alfa = 0;
            }
            if (req_2 > 1)
            {
                required_time_for_relay_2 = req_2;
                beta = 0;
            }
        }
        // else if (alfa == 1 && beta == 1)
        // {
        //     return 0;
        // }
    }
    { // clock lights control
        if ((relay_1_done == 0) && (alfa == 0) && required_time_for_relay_1 > 1)
        {
            divider = (required_time_for_relay_1) / 12;
            dumy_current_seconds = seconds;
            current_deling_relay = 1;
            alfa = 1;
        }
        else if (beta == 0 && required_time_for_relay_2 > 1 && relay_1_done == 1)
        {
            dumy_current_seconds = seconds;
            divider = (required_time_for_relay_2 - required_time_for_relay_1) / 12;
            current_deling_relay = 2;
            beta = 1;
        }
        // Serial.print("dumy value is : ");
        // Serial.print(dumy_current_seconds);
        // Serial.print(" next updated time : ");
        // Serial.print(seconds - dumy_current_seconds);
        // Serial.print("  divider : ");
        // Serial.println(divider);
        if (divider * 1 > (seconds - dumy_current_seconds))
        {
            reset_clock();
            main_array_modifier(111);

            switch_control();
        }
        else if (divider * 2 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(112);
            switch_control();
        }
        else if (divider * 3 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(113);
            switch_control();
        }
        else if (divider * 4 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(114);
            switch_control();
        }
        else if (divider * 5 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(115);
            switch_control();
        }
        else if (divider * 6 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(116);
            switch_control();
        }
        else if (divider * 7 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(117);
            switch_control();
        }
        else if (divider * 8 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(118);
            switch_control();
        }
        else if (divider * 9 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(211);
            switch_control();
        }
        else if (divider * 10 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(212);
            switch_control();
        }
        else if (divider * 11 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(213);
            switch_control();
        }
        else if (divider * 12 > (seconds - dumy_current_seconds))
        {
            main_array_modifier(214);
            switch_control();
            relay_1_done = 1;
            current_loop++;
            if (current_loop == 2)
            {
                relay_1_done = 0;
                current_loop = 0;
                main_array_modifier(1136);
                main_array_modifier(2114);
                switch_control();
            }
            time_control_complete_sec(divider / 2);
            reset_clock();

            divider = 0;
        }
        // else
        // {
        //     Serial.println(" ### non off abve are selected ### ");
        // }
    }
}
void reset_clock()
{
    main_array_modifier(10123);
    main_array_modifier(10456);
    main_array_modifier(1078);
    main_array_modifier(20123);
    main_array_modifier(204);
    switch_control();
}

void switch_loop_control(int room_number, int switch_number, int on_time, int off_time)
{
    // dumy_on_seconds = seconds;
    // dumy_off_seconds = seconds;

    if (room_number == 0)
    {
        if (dumy_on_seconds + dumy_on_seconds_main < (seconds + 1) && on_hoa == false)
        {
            main_array_modifier(loop_global_variable + 10);
            // Serial.println(loop_global_variable + 10);
            switch_control();
            dumy_off_seconds = seconds;
            on_hoa = true;
        }
        if (dumy_off_seconds + dumy_off_seconds_main < (seconds + 1) && on_hoa == true)
        {
            main_array_modifier(loop_global_variable);
            switch_control();
            // Serial.println(loop_global_variable);
            // Serial.println("Enter in off time loop");
            dumy_on_seconds = seconds;
            on_hoa = false;
        }
    }
    else
    {
        // Serial.println("Maintaiend value is : ");
        dumy_on_seconds = seconds;
        dumy_off_seconds = seconds;
        loop_global_variable = ((room_number * 100) + switch_number);
        dumy_off_seconds_main = on_time;

        dumy_on_seconds_main = off_time;
        // Serial.print("recived on and  off time : ");
        // Serial.println(loop_global_variable);
        // Serial.print(on_time);
        // Serial.print(off_time);
    }
}
