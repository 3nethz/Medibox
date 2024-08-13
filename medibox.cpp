#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHTesp.h>
#include <WiFi.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C // See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
#define buzzer 5
#define LED_1 15
#define LED_2 16
#define PB_CANCEL 34
#define PB_UP 32
#define PB_DOWN 14
#define PB_OK 35
#define DHT 13


Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);   //Declaring the display
DHTesp dhtSensor;  //Declaring the DHT sensor

//Global variables
long time_zone_hour = 0;
long time_zone_minute = 0;

//NTP server
const char* NTP_SERVER   =  "pool.ntp.org";
long UTC_OFFSET    = 0;
int UTC_OFFSET_DST = 0;

int date = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

//Alarm variables
bool alarm_enabled = true;
int n_alarms = 3;
int alarm_hours[] = {0,0,0};
int alarm_minutes[] = {0,0,0};
bool alarm_triggered[] = {false, false,false};

int C = 262;
int D = 294;
int E = 330;
int F = 349;
int G = 392;
int A = 440;
int B = 494;
int C_H = 523;
int n_notes = 8;
int notes[] = {C,D,E,F,G,A,B,C_H};

//Menu variables
int current_mode = 0;
int max_modes = 5;
String modes[] = {"1 - Set Time Zone", "2 - Set Alarm 1", "3 - Set Alarm 2", "4- Set Alarm 3", "5 - Cancel Alarms"};

void print_line(String text, int column, int row, int size){
    //display a custom message
    display.setTextSize(size);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(column, row);
    display.println(text);
    display.display(); 
}

void print_time(void){
  display.clearDisplay();
  print_line("Time: " + String(date) + ":" + String(hours) + ":" + String(minutes) + ":" + String(seconds), 0, 0, 1);
}

void ring_alarm(){
    //show meesasage on display
    display.clearDisplay();
    print_line("Medicine Time!", 10, 20, 2);

    //light up LED
    digitalWrite(LED_1, HIGH);

    bool break_happened = false;

    //ring the buzzer
    while(break_happened == false && digitalRead(PB_CANCEL) == HIGH){
    for (int i = 0; i < n_notes; i++){
        if(digitalRead(PB_CANCEL) == LOW){
            delay(200);
            break_happened = true;
            break;
        }
        tone(buzzer, notes[i]);
        delay(500);
        noTone(buzzer);
        delay(2);
        }
    
    //turn off LED
    digitalWrite(LED_1 , LOW);
    display.clearDisplay();
    }
} 

void update_time(){
  struct tm timeinfo;
  getLocalTime(&timeinfo);

  char timeHour[3];
  strftime(timeHour, 3, "%H", &timeinfo);
  hours = atoi(timeHour);

  char timeMinutes[3];
  strftime(timeMinutes, 3, "%M", &timeinfo);
  minutes = atoi(timeMinutes);

  char timeSeconds[3];
  strftime(timeSeconds, 3, "%S", &timeinfo);
  seconds = atoi(timeSeconds);

  char timeDate[3];
  strftime(timeDate, 3, "%d", &timeinfo);
  date = atoi(timeDate);
}

void update_time_with_check_alarm(){
    update_time();
    print_time();

    if (alarm_enabled){
        //iterate through all alarms
        for(int i = 0; i < n_alarms; i++){
            if (alarm_hours[i] == hours && alarm_minutes[i] == minutes &&  !alarm_triggered[i]){
                ring_alarm();
                alarm_triggered[i] = true;
            }
        }
    }
}

void update_UTC_OFFSET(){
  UTC_OFFSET = time_zone_hour * 3600 + time_zone_minute * (long)60;
}

int wait_for_butten_press(){
  while (true){
    if (digitalRead(PB_CANCEL) == LOW){
      delay(200);
      return PB_CANCEL;
    }
    else if (digitalRead(PB_UP) == LOW){
      delay(200);
      return PB_UP;
    }
    else if (digitalRead(PB_DOWN) == LOW){
      delay(200);
      return PB_DOWN;
    }
    else if (digitalRead(PB_OK) == LOW){
      delay(200);
      return PB_OK;
    }

    update_time();
  }
}

void set_alarm(int alarm){
  int temp_hour = alarm_hours[alarm];
  while(true){
    display.clearDisplay();
    print_line("Enter Hour:" + String(temp_hour), 0, 0, 2);

    int pressed = wait_for_butten_press();
    if (pressed == PB_UP){
      temp_hour = (temp_hour + 1) % 24;
    }
    else if(pressed == PB_DOWN){
      temp_hour = (temp_hour - 1);
      if(temp_hour < 0){
        temp_hour = 23;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      alarm_hours[alarm] = temp_hour;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }

  int temp_minutes = alarm_minutes[alarm];
  while(true){
    display.clearDisplay();
    print_line("Enter Minutes:" + String(temp_minutes), 0, 0, 2);

    int pressed = wait_for_butten_press();
    if (pressed == PB_UP){
      temp_minutes = (temp_minutes + 1) % 60;
    }
    else if(pressed == PB_DOWN){
      temp_minutes = (temp_minutes - 1);
      if(temp_minutes < 0){
        temp_minutes = 59;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      alarm_minutes[alarm] = temp_minutes;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
  display.clearDisplay();
  print_line("Alarm is set!", 10, 20, 2);
  delay(1000);
}

void set_time_zone(){
  int temp_time_zone_hour = time_zone_hour;
  while(true){
    display.clearDisplay();
    print_line("Enter Hour:" + String(temp_time_zone_hour), 0, 0, 2);

    int pressed = wait_for_butten_press();
    if (pressed == PB_UP){
      temp_time_zone_hour = (temp_time_zone_hour + 1) % 15;
    }
    else if(pressed == PB_DOWN){
      temp_time_zone_hour = (temp_time_zone_hour - 1) % 13;
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      time_zone_hour = temp_time_zone_hour;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }

  int temp_time_zone_minutes = time_zone_minute;
  while(true){
    display.clearDisplay();
    print_line("Enter Minutes:" + String(temp_time_zone_minutes), 0, 0, 2);

    int pressed = wait_for_butten_press();
    if (pressed == PB_UP){
      temp_time_zone_minutes = (temp_time_zone_minutes + 15) % 60;
    }
    else if(pressed == PB_DOWN){
      temp_time_zone_minutes = (temp_time_zone_minutes - 15);
      if(temp_time_zone_minutes < 0){
        temp_time_zone_minutes = 59;
      }
    }
    else if (pressed == PB_OK)
    {
      delay(200);
      time_zone_minute = temp_time_zone_minutes;
      break;
    }
    else if (pressed == PB_CANCEL)
    {
      delay(200);
      break;
    }
  }
    update_UTC_OFFSET();
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);
    Serial.println(UTC_OFFSET);
    Serial.println(time_zone_minute);
  
  display.clearDisplay();
  print_line("Time Zone is set!", 10, 20, 2);
  delay(1000);

}

void run_mode(int mode){
  if (mode == 0){
    set_time_zone();
  }
  else if (mode == 1){
    set_alarm(0);
  }
  else if (mode == 2){
    set_alarm(1);
  }
  else if (mode == 3){
    set_alarm(2);
  }
  else if (mode == 4){
    alarm_enabled = false;
    display.clearDisplay();
    print_line("Alarms Disabled", 10, 20, 2);
    delay(2000);
  }
}

void go_to_menu(){
  while (digitalRead(PB_CANCEL) == HIGH){
    display.clearDisplay();
    print_line(modes[current_mode], 0, 0, 2);

    int pressed = wait_for_butten_press();

    if(pressed == PB_UP){
      current_mode = (current_mode + 1) % max_modes;
    }
    else if (pressed == PB_DOWN){
      current_mode = (current_mode - 1);
      if(current_mode < 0){
        current_mode = max_modes - 1;
      }
    }
    else if (pressed == PB_OK){
      run_mode(current_mode);
    }
    else if (pressed == PB_CANCEL){
      delay(200);
      break;
    }
  }
}

void check_temp_and_humidity(){
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  bool all_good = true;
  if (data.temperature > 32){
    all_good = false;
    display.clearDisplay();
    digitalWrite(LED_2, HIGH);
    print_line("Temperature is too high!", 0, 40, 1);
    delay(2000);
  }
  if (data.temperature < 26){
    all_good = false;
    display.clearDisplay();
    digitalWrite(LED_2, HIGH);
    print_line("Temperature is too low!", 0, 40, 1);
    delay(2000);
  }
  if (data.humidity > 80){
    all_good = false;
    display.clearDisplay();
    digitalWrite(LED_2, HIGH);
    print_line("Humidity is too high!", 0, 50, 1);
    delay(2000);
  }
  if (data.humidity < 60){
    all_good = false;
    display.clearDisplay();
    digitalWrite(LED_2, HIGH);
    print_line("Humidity is too low!", 0, 50, 1);
    delay(2000);
  }
  if (all_good){
    digitalWrite(LED_2, LOW);
  }
}

void setup() {
    pinMode(buzzer, OUTPUT);
    pinMode(LED_1, OUTPUT);
    pinMode(LED_2, OUTPUT);
    pinMode(PB_CANCEL, INPUT);
    pinMode(PB_UP, INPUT);
    pinMode(PB_DOWN, INPUT);
    pinMode(PB_OK, INPUT);

    //initiate DHT sensor
    dhtSensor.setup(DHT, DHTesp::DHT22);
    //initiate Serial Monitor and OLED display
    Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }
    //initiate WiFi
    WiFi.begin("Wokwi-GUEST", "", 6);
    while (WiFi.status() != WL_CONNECTED) {
      delay(250);
      display.clearDisplay();
      print_line("Connecting to WiFi", 10, 20, 1);
    }
    display.clearDisplay();
    print_line("Connected to WiFi", 10, 20, 1);
    
    configTime(UTC_OFFSET, UTC_OFFSET_DST, NTP_SERVER);

    //turn on OLED display
    display.display();
    delay(2000);
    
    //clear OLED display
    display.clearDisplay();

    print_line("Welcome to medibox!", 10, 20, 2);
    delay(1000);
    display.clearDisplay();
}

void loop() {
  //check if the user wants to go to the menu
  if(digitalRead(PB_OK) == LOW){
    delay(200);
    go_to_menu();
  }
  //update time and check for alarms
  update_time_with_check_alarm();
  delay(10);
  check_temp_and_humidity();
}