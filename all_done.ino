#include <math.h>
#include <SoftwareSerial.h>

//RFID NFC control
#include <SPI.h>
#include <MFRC522.h>
#define SS_PIN 10
#define RST_PIN 9
#define ACCESS_DELAY 2000
#define DENIED_DELAY 1000
MFRC522 mfrc522(SS_PIN, RST_PIN); // Create MFRC522 instance.

//servo moter contol
#include <Servo.h>
#define SERVO_PIN 3
Servo myservo;
int close = 180;
int open = 0;

////IR control
//#include <IRremote.h>
//const int RECV_PIN = 4;
//IRrecv irrecv(RECV_PIN);
//decode_results results; //results: store the outcome of IRsignal
//unsigned long key_value = 0;
//bool ready_check = false;
//int IRsender = 0;

//door opening with phone contol
int phone_pin = A0; //moniter the changes in A0 to open the gate if get defined value
int globle_A0_value = 0;

//74HC595 related
int the_big_one[9][9] = {0}; //main array to control the switching
int button_number = 0;
int status = 0;
int room_number = 0;
int multi = 0;

//configuring pins of 74HC595 pins
const int dataPin = 2;
////Pin connected to Data in (DS) of 74HC595
const int latchPin = 7;
//Pin connected to latchPin pin (ST_CP) of 74HC595 (Storage Register clock input)
const int clockPin = 8;
//Pin connected to clock pin (SH_CP) of 74HC595 (Shift Register clock input)

//*******************************
double time = 0;
unsigned long int seconds = 0;
// -3000 +2999 3001
//0 6000
double sec_by_ten = 0;
double sec_by_hundred = 0;
unsigned long int req_sec = 0;
unsigned long int delay_switching[10][5] = {0};
int flag = 0;
bool clean = false;
//*******************************

//5 and 6 are free pins
void setup() //heart ♥ of code
{
    Serial.begin(9600); // Initiate a serial communication

    SPI.begin();        // Initiate  SPI bus
    mfrc522.PCD_Init(); // Initiate MFRC522

    myservo.attach(SERVO_PIN);
    myservo.write(close);
    delay(500);
    myservo.write(close);

    Serial.println("Put your card to the reader...");
    Serial.println();

//    irrecv.enableIRIn();
    //--------------------------------------------//
    // Set all the pins of 74HC595 as OUTPUT
    pinMode(latchPin, OUTPUT);
    pinMode(dataPin, OUTPUT);
    pinMode(clockPin, OUTPUT);
    reseter(); //reset the previous data stored in 74HC595IC
    main_array_modifier(111);
    switch_control();
    //--------------------------------------------//
}

void loop()
{
    switch_timing_checker();
    seconds_reseter();
    time_control_10_of_sec(1);

    if (Serial.available() > 1) // is a number available?
    {
        unsigned long int numbri = Serial.parseInt(); // get the number
        if (numbri == 998)                            //direct door close
        {
            Serial.println("Closing Door");
            myservo.write(close);
        }
        else if (numbri == 999) //direct door open for 3 seconds
        {
            Serial.println("Opeing Door for 3 seconds");
            //            myservo.write(open);
            Door_open(3);
            myservo.write(close);
        }
        else
        {
            Serial.print("working on : ");
            Serial.println(numbri);
            main_array_modifier(numbri); //1st of all number will be send to array_modifire which arange switches
            switch_control();            //then this function will execute the data of array_modifier...
        }
        time_control_100_of_sec(3);
    }
//    IR_function();                           //check if the IR signal is present
    int sensorValue = analogRead(phone_pin); //read the data of A0 and stor in the variable for further use...
    {
        if (sensorValue > 400)
        {
            Serial.print("      Incoming call (opening Gate)  ");
            Serial.println(sensorValue);
            Door_open(3);
            // time_control_complete_sec(5); //Door will open for 5 seconds...
        }
        else if (sensorValue < 400)
        {
            myservo.write(close); //close the door by default
        }
    }
    // Look for new cards
    if (seconds < 3)
    {
        Card_cheker();
    }
//}
//void IR_function()
//{
//    if (irrecv.decode(&results))
//    {
//
//        if (results.value == 0XFFFFFFFF)
//            results.value = key_value;
//
//        switch (results.value)
//        {
//        case 0xFFA25D:
//            Serial.println("CH-");
//            break;
//        case 0xFF629D:
//            Serial.println("CH");
//            Serial.println("Input is cleared");
//            IRsender = 0;
//            time_control_10_of_sec(5);
//            break;
//        case 0xFFE21D:
//            Serial.println("CH+");
//            break;
//        case 0xFF22DD:
//            Serial.println("|<<");
//            Door_open(3);
//           // time_control_complete_sec(5);
//            break;
//        case 0xFF02FD:
//            Serial.println(">>|");
//            myservo.write(close);
//            break;
//        case 0xFFC23D:
//            Serial.println(">||");
//            ready_check = true;
//            break;
//        case 0xFFE01F:
//            Serial.println("-");
//            break;
//        case 0xFFA857:
//            Serial.println("+");
//            break;
//        case 0xFF906F:
//            Serial.println("EQ");
//            time_control_10_of_sec(5);
//            // IRsender /= 10;
//            if (ready_check == true)
//            {
//                Serial.print("sending ");
//                Serial.println(IRsender);
//                main_array_modifier(IRsender);
//                switch_control();
//                ready_check = false;
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute command---!!!");
//                time_control_10_of_sec(2);
//            }
//            IRsender = 0;
//            break;
//        case 0xFF6897:
//            Serial.println("0");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 0;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF9867:
//            Serial.println("100+");
//            break;
//        case 0xFFB04F:
//            Serial.println("200+");
//            break;
//        case 0xFF30CF:
//            Serial.println("1");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 1;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF18E7:
//            Serial.println("2");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 2;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF7A85:
//            Serial.println("3");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 3;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF10EF:
//            Serial.println("4");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 4;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF38C7:
//            Serial.println("5");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 5;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF5AA5:
//            Serial.println("6");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 6;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF42BD:
//            Serial.println("7");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 7;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF4AB5:
//            Serial.println("8");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 8;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        case 0xFF52AD:
//            Serial.println("9");
//            if (ready_check == true)
//            {
//                time_control_10_of_sec(2);
//                IRsender *= 10;
//                IRsender += 9;
//                Serial.print("New Value : ");
//                Serial.println(IRsender);
//            }
//            else
//            {
//                Serial.println("!!!---Enable to execute the command---!!!");
//                time_control_10_of_sec(2);
//            }
//            break;
//        }
//        key_value = results.value;
//        irrecv.resume();
//    }
//}

void main_array_modifier(unsigned long int inputer) //control the input by user...brain of code
{

    int nDigits = floor(log10(abs(inputer))) + 1; //to get the lenght of inputed number
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
    int ctr = 0;
    int holder = 0;
    int new_num = button_number;
    if (status != 0 && status != 1)
    {
        timer(room_number, status, button_number);
        button_number = 0;
    }
    the_big_one[room_number - 1][0] = status;                        //assign status to array
    for (ctr = 0; ctr < floor(log10(abs(button_number))) + 1; ctr++) //switches on off control
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

void switch_control() //backbone of Complete Switching
{
    int i = 0;
    int j = 0;
    int status = 0;
    for (i = 9; i >= 0; i--) //control room number
    {
        for (j = 1; j < 9; j++) //control switch and status
        {
            if (the_big_one[i][j] == 1)
            {
                digitalWrite(dataPin, HIGH);
                time_control_100_of_sec(1); //delay(10);
                digitalWrite(clockPin, HIGH);
                time_control_100_of_sec(1); //delay(10);

                digitalWrite(dataPin, LOW);
                time_control_100_of_sec(1); //delay(10);
                digitalWrite(clockPin, LOW);
                time_control_100_of_sec(1); //delay(10);
            }
            else
            {
                digitalWrite(dataPin, LOW);
                time_control_100_of_sec(1); //delay(10);

                digitalWrite(clockPin, HIGH);
                time_control_100_of_sec(1); //delay(10);
                digitalWrite(clockPin, LOW);
                time_control_100_of_sec(1); //delay(10);
            }
        }
    }
    digitalWrite(latchPin, HIGH);
    time_control_100_of_sec(1);
    digitalWrite(latchPin, LOW);
    time_control_100_of_sec(1);
}

void reseter()
{
    int i = 0;
    digitalWrite(dataPin, LOW);
    digitalWrite(latchPin, LOW);
    digitalWrite(clockPin, LOW);

    // digitalWrite (dataPin, LOW);
    time_control_100_of_sec(1);
    for (i = 0; i < 8; i++)
    {
        digitalWrite(clockPin, HIGH);
        time_control_100_of_sec(1);
        digitalWrite(clockPin, LOW);
        time_control_100_of_sec(1);
    }
    digitalWrite(latchPin, HIGH);
    time_control_100_of_sec(1);
    digitalWrite(latchPin, LOW);
    time_control_100_of_sec(1);
}

//______________________________________________//
//delay provider & control the seconds backbone of time_switching()
void time_control_complete_sec(int timer) //provides 1 second delay or delay(1000)
{
    int alfa;
    for (alfa = 0; alfa < timer; alfa++)
    {
        int sop = 0;
        for (sop = 0; sop < 10; sop++)
        {
            delay(100);
        }
        seconds++;
        seconds_holder();
    }
}
void time_control_10_of_sec(int timer)
{ //provides 10 of second or (1/10) or delay(100)
    int alfa;
    for (alfa = 0; alfa < timer; alfa++)
    {
        int sop = 0;
        for (sop = 0; sop < 10; sop++)
        {
            delay(10);
        }
        sec_by_ten++;
        seconds_holder();
    }
}
void time_control_100_of_sec(int timer)
{ //provides 100 of second or (1/100) or delay(10)
    int alfa;
    for (alfa = 0; alfa < timer; alfa++)
    {
        delay(10);
        sec_by_hundred++;
        seconds_holder();
    }
}
//_________________________________________________//

//-------------------------------------------------//
//increment in seconds
void seconds_holder()
{
    if (sec_by_hundred >= 100)
    {
        seconds++;
        sec_by_hundred = 0;
    }
    if (sec_by_ten >= 10)
    {
        seconds++;
        sec_by_ten = 0;
    }
}

//-------------------------------------------------//

void timer(int room_number, int status, int switch_number)
{
    int swh = 0;
    int temp_sn = switch_number;
    { //better output to see record
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
void timing_for_switch(int room_number_AND_switch_number, int command, int req_seconds)
{
    int IK = 0;
    bool mil_jata_hay = false;
    for (IK = 0; IK < 10; IK++)
    {
        if (delay_switching[IK][1] == room_number_AND_switch_number)
        { //if inputed Room_number and switch number are already have a record
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
    bool stored = false;
    if (mil_jata_hay == false)
    { //if inputed Room_number and switch number are not in the record
        for (IK = 0; IK < 10; IK++)
        {
            if (delay_switching[IK][0] == 0) //if 1st element of array (from 0 to 9) then we can store data
            {
                if (stored == false) //if data is not stored yet
                {
                    Serial.println("NO Match found so Creating new data");
                    delay_switching[IK][0] = 1;
                    delay_switching[IK][1] = room_number_AND_switch_number;
                    delay_switching[IK][2] = command;
                    delay_switching[IK][3] = req_seconds;
                    delay_switching[IK][4] = seconds;
                    stored = true; //data is now stored this conditoin will not run in this loop
                    IK = 99;
                    clean = true;
                }
            }
        }
    }
}

void switch_timing_checker()
{ //check all the switches which are supposed to on or off after a specific time
    int IK = 0;
    for (IK = 0; IK < 10; IK++)
    {
        if (delay_switching[IK][0] == 1) //if there is any data stored
        {
            int total_time = 0;
            total_time = delay_switching[IK][3] + delay_switching[IK][4];
            if ((total_time + 2) <= seconds) //if the required time is reached then switch will on/off
            {
                int command;
                delay_switching[IK][0] = 0; //data will clerad from array as it is not usefull after requird time
                delay_switching[IK][1] = 0; //cleaing data
                command = delay_switching[IK][2];
                Serial.print("Working on : ");
                Serial.println(command);
                Serial.print("Required time was :");
                Serial.print(delay_switching[IK][3]);
                Serial.println(" Seconds");
                //if the required time is reached then the array_modifier and switch_control will work as normal
                main_array_modifier(command);
                switch_control();

                delay_switching[IK][2] = 0; //cleaing data
                delay_switching[IK][3] = 0; //cleaing data
                delay_switching[IK][4] = 0; //cleaing data
            }
        }
    }
}

void seconds_reseter()
{ //set seconds to 0 if seconds are not in use so it will saves the process storage and overflow of varibale
    int IK = 0;
    bool data_parra_hay = false;
    for (IK = 0; IK < 10; IK++) //check the delay_switching if data found then data_parra_hay will becomes true
    {
        if (delay_switching[IK][0] == 1)
        {
            data_parra_hay = true;
        }
    }
    if (data_parra_hay == false) //if there is no data in the delay_switching
    {
        seconds = 0;
        if (clean == true) //it will print bellow command just for 1 time after when ever delay swiching is used
        {
            Serial.println("!!!....There is no data in seconds variable so cleaing seconds....!!!");
            clean = false;
        }
    }
}

void Card_cheker()
{
    {
        if (!mfrc522.PICC_IsNewCardPresent())
        {
            return;
        }
        // Select one of the cards
        if (!mfrc522.PICC_ReadCardSerial())
        {
            return;
        }
        //Show UID on serial monitor
        Serial.print("UID tag :");
        String content = "";
        byte letter;
        for (byte i = 0; i < mfrc522.uid.size; i++)
        {
            Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
            Serial.print(mfrc522.uid.uidByte[i], HEX);
            content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
            content.concat(String(mfrc522.uid.uidByte[i], HEX));
        }
        Serial.println();
        Serial.print("Message : ");
        content.toUpperCase();
        if (content.substring(1) == "B9 AE 50 A2") //change here the UID of the card
        {
            Serial.println("Authorized access");
            Serial.println();
            Door_open(3);
           // time_control_complete_sec(5);
            myservo.write(close);
        }
        else
        {
            Serial.println(" Access denied");
            time_control_complete_sec(1);
        }
    }
}

void Door_open(int wait_time)
{
    int angle = 0;
    for (angle = 180; angle >= 110; angle -= 5) // command to move from 180 degrees to 0 degrees
    {
        myservo.write(angle); //command to rotate the servo to the specified angle
        delay(10);
    }
    time_control_complete_sec(wait_time);
}
