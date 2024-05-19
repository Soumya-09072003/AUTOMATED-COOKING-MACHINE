//International Market Product PoC on ACM
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include "soc/rtc.h"
#include "HX711.h"

// BlYNK APP connection to ESP32
char BLYNK_TEMPLATE_ID[] = "TMPL3AkaNDoWc";
char BLYNK_TEMPLATE_NAME[] = "ACM V2";
char auth[] = "MEG";

//ESP32 WiFI Connection
char ssid[] = "12345678";
char pass[] = "J4_oQJriUxGdcJDTP4tQSTn8oc-ArMoU";

// RLY connection to ESP32 as per PoC_ACM_Schematic_V1
#define RLY_IND_CKR1 19
#define RLY_IND_CKR2
#define RLY_SOLND_MOVE_1_Down 18
#define RLY_SOLND_MOVE_1_Up 5
#define RLY_SOLND_MOVE_2_Down
#define RLY_SOLND_MOVE_2_Up
#define RLY_Temp_IND_CKR1_Plus
#define RLY_Temp_IND_CKR1_Minus
#define RLY_Temp_IND_CKR2_Plus
#define RLY_Temp_IND_CKR2_Minus

// LoadCell connection to ESP32 as per PoC_ACM_Schematic_V1
#define LoadCell_Water_DT 26
#define LoadCell_Water_SCK 25
#define LoadCell_Rice_DT 24
#define LoadCell_Rice_SCK 23
#define LoadCell_Dal_DT
#define LoadCell_Dal_SCK
//Declaring scale variable for HX711
HX711 scale;

// Proximity Switch connection to ESP32 as per PoC_ACM_Schematic_V1
#define PS_POT_IND_CKR1 23
#define PS_POT_DIS_RAC1 21
#define PS_POT_IND_CKR2
#define PS_POT_DIS_RAC2

// LED simulating Output of PoC ACM
#define LED_Water_VLV 32
#define LED_Rice_VLV 33
#define LED_Dal_VLV
#define LED_LID1_Open 25
#define LED_LID1_Close 26
#define LED_LID1_PAR_Open 27
#define LED_Strrier_1 14

#define LED_LID2_Open
#define LED_LID2_Close
#define LED_LID2_PAR_Open
#define LED_Strrier_2

#define LED_RICE_Selection 12
#define LED_Dal_Selection 13

// Pushbutton Switch Output of PoC ACM
#define PB1_PoC_ACM 20
#define PB2_PoC_ACM 21
#define PB3_PoC_ACM 22

//Analog IO of PoC ACM
#define ANA_Person_Selection 3

//Temp Variable will change:
int Person_Selection;

//For water in rice making
int Discharged_Water_for_Rice;
int Initial_Water_weight;
int Current_Water_weight;
int One_Glass_Water = 300;  //Amount of water required to cook 100gm ( for 1 person) rice i.e. 300gm

//For water in dal making
int Discharged_Water_for_Dal;
int One_Glass_Water = 300;  //Amount of water required to cook 100gm ( for 1 person) dal i.e. 300gm

//For rice in rice making
int Discharged_Rice_for_Rice;
int Initial_Rice_weight;
int Current_Rice_weight;
int One_Scoop_Rice = 100;  //Amount of rice required to cook rice for 1 person i.e. 100gm

//For dal in dal making
int Discharged_Dal_for_Dal;
int Initial_Dal_weight;
int Current_Dal_weight;
int One_Scoop_Dal = 100;  //Amount of dal required to cook dal for 1 person i.e. 100gm

// Temp variables will change:
int PS_POT_IND_CKR1_ButtonState = 0;  // variable for reading the Proximity Switch status
int PS_POT_IND_CKR2_ButtonState = 0;  // variable for reading the Proximity Switch status

void setup() {
  Serial.begin(115200);

  //Bly
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);

  // Attempt WiFi connection
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1200);  // Wait for suggested comeback time
    Serial.println("Connecting to WiFi...");

    //Load Cells Scaling and Initialzation for water
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    scale.begin(LoadCell_Water_DT, LoadCell_Water_SCK);
    Serial.println("Initializing the scale");
    scale.set_scale(CALIBRATION_FACTOR);  // this value is obtained by calibrating the scale with known weights; see the README for details

    //Load Cells Scaling and Initialzation for rice
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);
    scale.begin(LoadCell_Rice_DT, LoadCell_Rice_SCK);
    Serial.println("Initializing the scale");
    scale.set_scale(CALIBRATION_FACTOR);  // this value is obtained by calibrating the scale with known weights; see the README for details


    //Load Cells Scaling and Initialzation
    rtc_clk_cpu_freq_set(RTC_CPU_FREQ_80M);

    // initialize the  pin as an input/output:
    pinMode(PS_POT_IND_CKR1, INPUT);
    pinMode(PS_POT_DIS_RAC1, INPUT);

    pinMode(RLY_IND_CKR1, OUTPUT);
    pinMode(RLY_SOLND_MOVE_1_Down, OUTPUT);
    pinMode(RLY_SOLND_MOVE_1_Up, OUTPUT);
    pinMode(RLY_Temp_IND_CKR1_Plus, OUTPUT);
    pinMode(RLY_Temp_IND_CKR1_Minus, OUTPUT);

    pinMode(LED_Water_VLV, OUTPUT);
    pinMode(LED_Rice_VLV, OUTPUT);
    pinMode(LED_LID1_Open, OUTPUT);
    pinMode(LED_LID1_Close, OUTPUT);
    pinMode(LED_LID1_PAR_Open, OUTPUT);
    pinMode(LED_Strrier_1, OUTPUT);
    pinMode(LED_RICE_Selection, OUTPUT);

    pinMode(RLY_IND_CKR2, OUTPUT);
    pinMode(RLY_SOLND_MOVE_2_Down, OUTPUT);
    pinMode(RLY_SOLND_MOVE_2_Up, OUTPUT);
    pinMode(RLY_Temp_IND_CKR2_Plus, OUTPUT);
    pinMode(RLY_Temp_IND_CKR2_Minus, OUTPUT);
    pinMode(LED_Dal_Selection, OUTPUT);
    pinMode(LED_LID2_Open, OUTPUT);
    pinMode(LED_LID2_Close, OUTPUT);
    pinMode(LED_LID2_PAR_Open, OUTPUT);
    pinMode(LED_Strrier_2, OUTPUT);
    pinMode(LED_Dal_VLV, OUTPUT);
  }

  void loop() {
    Blynk.run();
#== == == == == == == == == == == == == == == == == == == == == == == == == == *** RICE START *** == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
    if (LED_RICE_Selection == HIGH) {
      Serial.println("User Selected Recipe --> Rice");
      // read the value from the sensor:
      Person_Selection = analogRead(ANA_Person_Selection);

      if (Person_Selection == 1) {
        Serial.println("Selected_Person = 1");

        // read the state of the proximity_switch_POT value:
        PS_POT_IND_CKR1_ButtonState = digitalRead(PS_POT_IND_CKR1);

        // check if the proximity_switch_POT is sensed. If it is, the proximity_switch_POT_buttonState is HIGH:
        if (PS_POT_IND_CKR1 == HIGH) {
          // Send message regarding POT presence:
          Serial.println("Ind_Cooker_POT present");
          Serial.println("Start -Cooking Process");

#== == == == == == == == == == == == == == == == == == == == == == == == = Pouring of Water for cleaning rice Started == == == == == == == == == == == == == == == == == == == == == =

          //Pour/Discharge the required water for rice_cooking
          digital.write(LED_Water_VLV, HIGH);

          while (LED_Water_VLV == HIGH) {
            scale.begin(LoadCell_Water_DT, LoadCell_Water_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Water... In Progress");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Water done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Water_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of water: ");
              Serial.println(Water_Load_Cell_reading);
              if (Water_Load_Cell_reading != Water_Load_Cell_lastReading) {
                Serial.print(Water_Load_Cell_reading);
              }
              Water_Load_Cell_lastReading = Water_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of water load cell not found.");
            }

            Discharged_Water_for_Rice = 0;
            Initial_Water_weight = Water_Load_Cell_lastReading;
            Current_Water_weight = round(scale.get_units());
            Discharged_Water_for_Rice = Initial_Water_weight - Current_Water_weight;

            //Set Water quantity as per person selection & rice or ingradient i.e. here 1 person = 1 glass of water
            if (Discharged_Water_for_Rice == One_Glass_Water) {
              digital.write(LED_Water_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Water for cleaning rice End == == == == == == == == == == == == == == == == == == == == == == == =
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Rice Start == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

          //Pour/Discharge the required rice for rice cooking
          digital.write(LED_Rice_VLV, HIGH);
          while (LED_Rice_VLV == HIGH) {
            scale.begin(LoadCell_Rice_DT, LoadCell_Rice_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Rice... remove any weights from the scale.");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Rice done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Rice_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of rice: ");
              Serial.println(Rice_Load_Cell_reading);
              if (Rice_Load_Cell_reading != Rice_Load_Cell_lastReading) {
                Serial.print(Rice_Load_Cell_reading);
              }
              Rice_Load_Cell_lastReading = Rice_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of rice load cell not found.");
            }

            Discharged_Rice_for_Rice = 0;
            Initial_Rice_weight = Rice_Load_Cell_lastReading;
            Current_Rice_weight = round(scale.get_units());
            Discharged_Rice_for_Rice = Initial_Rice_weight - Current_Rice_weight;

            if (Discharged_Rice_for_Rice == One_Scoop_Rice) {
              digital.write(LED_Rice_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Rice End == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
#== == == == == == == == == == == == == == == == == == == == == == == == Cleaning of Rice Start == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

          //ON Strrier in in cleaning process of rice
          digital.write(LED_Strrier, HIGH);
          delay(10000);

          //OFF Strrier in in cleaning process of rice
          digital.write(LED_Strrier, LOW);
          delay(10000);

          //Close the lid to clean the rice
          digital.write(LED_LID1_Close, HIGH);
          delay(10000);

          //Solenoid move down to pour water at the time of cleaning rice
          digital.write(RLY_SOLND_MOVE_Down, HIGH);
          delay(10000);

          //Solenoid move up to pour water at the time of cleaning rice
          digital.write(RLY_SOLND_MOVE_Up, HIGH);
          delay(10000);

          //Close the lid to clean the rice
          digital.write(LED_LID1_Open, HIGH);
          delay(10000);
#== == == == == == == == == == == == == == == == == == == == == == == == Cleaning of Rice End == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==
#== == == == == == == == == == == == == == == == == == == == == == == == = Pouring of Water for cooking Started == == == == == == == == == == == == == == == == == == == == == == ==

          //Pour/Discharge the required water for rice_cooking
          digital.write(LED_Water_VLV, HIGH);

          while (LED_Water_VLV == HIGH) {
            scale.begin(LoadCell_Water_DT, LoadCell_Water_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Water... In Progress");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Water done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Water_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of water: ");
              Serial.println(Water_Load_Cell_reading);
              if (Water_Load_Cell_reading != Water_Load_Cell_lastReading) {
                Serial.print(Water_Load_Cell_reading);
              }
              Water_Load_Cell_lastReading = Water_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of water load cell not found.");
            }

            Discharged_Water_for_Rice = 0;
            Initial_Water_weight = Water_Load_Cell_lastReading;
            Current_Water_weight = round(scale.get_units());
            Discharged_Water_for_Rice = Initial_Water_weight - Current_Water_weight;

            //Set Water quantity as per person selection & rice or ingradient i.e. here 1 person = 1 glass of water
            if (Discharged_Water_for_Rice == One_Glass_Water) {
              digital.write(LED_Water_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == = Pouring of Water for cooking End == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == == == == == == == == == == == = Cooking rice start = == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

          //Start Induction cooker-1 for cooking rice

          digital.write(RLY_IND_CKR1, HIGH);
          Serial.println("Induction Cooker ON for Cooking RICE ");

          //Increase Temperature using TEMP++

          //press 1st time to increase 100 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          //press 2nd time to increase 200 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          //press 3rd time to increase 300 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          //press 4th time to increase 400 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          //press 5th time to increase 500 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          //press 6th time to increase 600 deg celcius
          digital.write(RLY_Temp_IND_CKR1_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR1_Plus, LOW);
          delay(1000);

          digital.write(LED_LID1_Close, HIGH);
          delay(10000);

          digital.write(LED_LID1_Open, HIGH);
          delay(20000);

          digital.write(LED_LID1_Close, HIGH);
          delay(10000);

          digital.write(LED_LID1_Open, HIGH);
          delay(20000);

          digital.write(LED_LID1_Close, HIGH);
          delay(10000);

          digital.write(LED_LID1_Open, HIGH);
          delay(20000);

#== == == == == == == == == == == == == == == == == == == == == == == == = Cooking rice end = == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == Removing excess water from rice process start == == == == == == == == == == == == == == == == == == == == == == == == == == == == =

          //Close the lid to reomove excess water from rice
          digital.write(LED_LID1_Close, HIGH);
          delay(10000);

          //Solenoid move down to pour excess water from rice
          digital.write(RLY_SOLND_MOVE_Down, HIGH);
          delay(10000);

          //Solenoid move up after pour excess water from rice
          digital.write(RLY_SOLND_MOVE_Up, HIGH);
          delay(10000);

          Serial.println("HI MASTER!!\nFOOD IS READY\n THANK YOU FOR USING ACM.... :)");

#== == == == == == == == == == == == == == Removing excess water from rice process end == == == == == == == == == == == == == == == == == == == == == == == == == == == == =

          //Open the lid to take rice out
          digital.write(LED_LID1_Open, HIGH);

          //
          //
          //
          //
          //
        } else {
          Serial.println("Ind_Cooker_POT Absent");
          Serial.println("Cooking can't be started");
          Serial.println("Please select recipes after putting POT");
        }
      }

      if (Person_Selection == 2)

      {
        Serial.println("Selected_Person = 2");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 3)

      {
        Serial.println("Selected_Person = 3");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 4)

      {
        Serial.println("Selected_Person = 4");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 5)

      {
        Serial.println("Selected_Person = 5");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 6)

      {
        Serial.println("Selected_Person = 6");
        //
        //
        //
        //
        //
      }
    }
#== == == == == == == == == == == == == == == == == == == == == == == == == == *** RICE END *** == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == == == == == == == == == == == == == *** DAL START *** == == == == == == == == == == == == == == == == == == == == == == == == == == == == =

    if (LED_DAL_Selection == HIGH) {
      Serial.println("User Selected Recipe --> Rice");
      // read the value from the sensor:
      Person_Selection = analogRead(ANA_Person_Selection);

      if (Person_Selection == 1) {
        Serial.println("Selected_Person = 1");

        // read the state of the proximity_switch_POT value:
        PS_POT_IND_CKR2_ButtonState = digitalRead(PS_POT_IND_CKR);

        // check if the proximity_switch_POT is sensed. If it is, the proximity_switch_POT_buttonState is HIGH:
        if (PS_POT_IND_CKR2 == HIGH) {
          // Send message regarding POT presence:
          Serial.println("Ind_Cooker_POT present");
          Serial.println("Start -Cooking Process");
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Water for cleaning Dal Start == == == == == == == == == == == == == == == == == == == == == =

          //Pour/Discharge the required water for rice_cooking
          digital.write(LED_Water_VLV, HIGH);

          while (LED_Water_VLV == HIGH) {
            scale.begin(LoadCell_Water_DT, LoadCell_Water_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Water... In Progress");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Water done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Water_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of water: ");
              Serial.println(Water_Load_Cell_reading);
              if (Water_Load_Cell_reading != Water_Load_Cell_lastReading) {
                Serial.print(Water_Load_Cell_reading);
              }
              Water_Load_Cell_lastReading = Water_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of water load cell not found.");
            }

            Discharged_Water_for_Dal = 0;
            Initial_Water_weight = Water_Load_Cell_lastReading;
            Current_Water_weight = round(scale.get_units());
            Discharged_Water_for_Dal = Initial_Water_weight - Current_Water_weight;

            //Set Water quantity as per person selection & rice or ingradient i.e. here 1 person = 1 glass of water
            if (Discharged_Water_for_Dal == One_Glass_Water) {
              digital.write(LED_Water_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Water for cleaning dal End == == == == == == == == == == == == == == == == == == == == == == == =
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of dal Start == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

          //Pour/Discharge the required rice for rice cooking
          digital.write(LED_Dal_VLV, HIGH);
          while (LED_Dal_VLV == HIGH) {
            scale.begin(LoadCell_Dal_DT, LoadCell_Dal_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Dal... remove any weights from the scale.");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Dal done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Dal_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of dal: ");
              Serial.println(Dal_Load_Cell_reading);
              if (Dal_Load_Cell_reading != Dal_Load_Cell_lastReading) {
                Serial.print(Rice_Load_Cell_reading);
              }
              Dal_Load_Cell_lastReading = Dal_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of dal load cell not found.");
            }

            Discharged_Dal_for_Dal = 0;
            Initial_Dal_weight = Dal_Load_Cell_lastReading;
            Current_Dal_weight = round(scale.get_units());
            Discharged_Dal_for_Dal = Initial_Dal_weight - Current_Dal_weight;

            if (Discharged_Dal_for_Dal == One_Scoop_Dal) {
              digital.write(LED_Dal_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == Pouring of Dal End == == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == == == == == == == == == == == = Pouring of Water for cooking Start == == == == == == == == == == == == == == == == == == == == == == ==

          //Pour/Discharge the required water for rice_cooking
          digital.write(LED_Water_VLV, HIGH);

          while (LED_Water_VLV == HIGH) {
            scale.begin(LoadCell_Water_DT, LoadCell_Water_SCK);
            //Tare LoadCell_Water
            if (scale.is_ready()) {
              scale.set_scale();
              Serial.println("Tare_LoadCell_Water... In Progress");
              delay(5000);
              scale.tare();
              Serial.println("Tare_LoadCell_Water done...");
              delay(5000);
            }

            if (scale.wait_ready_timeout(200)) {
              Water_Load_Cell_reading = round(scale.get_units());
              Serial.print("Weight of water: ");
              Serial.println(Water_Load_Cell_reading);
              if (Water_Load_Cell_reading != Water_Load_Cell_lastReading) {
                Serial.print(Water_Load_Cell_reading);
              }
              Water_Load_Cell_lastReading = Water_Load_Cell_reading;
            }

            else {
              Serial.println("HX711 of water load cell not found.");
            }

            Discharged_Water_for_Dal = 0;
            Initial_Water_weight = Water_Load_Cell_lastReading;
            Current_Water_weight = round(scale.get_units());
            Discharged_Water_for_Dal = Initial_Water_weight - Current_Water_weight;

            //Set Water quantity as per person selection & rice or ingradient i.e. here 1 person = 1 glass of water
            if (Discharged_Water_for_Dal == One_Glass_Water) {
              digital.write(LED_Water_VLV, LOW);
            }
          }
#== == == == == == == == == == == == == == == == == == == == == == == == = Pouring of Water for cooking End == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == == == == == == == == == == == = Cooking Dal start = == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

          //Start Induction cooker-1 for cooking rice

          digital.write(RLY_IND_CKR2, HIGH);
          Serial.println("Induction Cooker ON for Cooking DAL ");

          //Increase Temperature using TEMP++

          //press 1st time to increase 100 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          //press 2nd time to increase 200 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          //press 3rd time to increase 300 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          //press 4th time to increase 400 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          //press 5th time to increase 500 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          //press 6th time to increase 600 deg celcius
          digital.write(RLY_Temp_IND_CKR2_Plus, HIGH);
          delay(1000);
          digital.write(RLY_Temp_IND_CKR2_Plus, LOW);
          delay(1000);

          digital.write(LED_LID2_Close, HIGH);
          delay(10000);
          Serial.println("HI MASTER!!\nFOOD IS READY\n THANK YOU FOR USING ACM.... :)");

          //Open the lid to take rice out
          digital.write(LED_LID1_Open, HIGH);

          //
          //
          //
          //
          //
        } else {
          Serial.println("Ind_Cooker_POT Absent");
          Serial.println("Cooking can't be started");
          Serial.println("Please select recipes after putting POT");
        }
      }

      if (Person_Selection == 2)

      {
        Serial.println("Selected_Person = 2");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 3)

      {
        Serial.println("Selected_Person = 3");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 4)

      {
        Serial.println("Selected_Person = 4");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 5)

      {
        Serial.println("Selected_Person = 5");
        //
        //
        //
        //
        //
      }

      if (Person_Selection == 6)

      {
        Serial.println("Selected_Person = 6");
        //
        //
        //
        //
        //
      }
    }
#== == == == == == == == == == == == == == == == == == == == == == == == == == *** DAL END *** == == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

#== == == == == == == == == == == == == == == == == == == == == == == == = Cooking Dal end = == == == == == == == == == == == == == == == == == == == == == == == == == == == ==

    /*  if (LED_Veg_Fry_Selection == HIGH)
    {
     Serial.println("User Selected Recipe --> Rice");
     {

     }
    }

    if (LED_Veg_Curry_Selection == HIGH)
    {
     Serial.println("User Selected Recipe --> Rice");
     {

     }
    }
*/