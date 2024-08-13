#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET    -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

//Global variables
int date = 0;
int hours = 0;
int minutes = 0;
int seconds = 0;

unsigned long timeNow = 0;
unsigned long timeLast = 0;

void setup() {
  // put your setup code here, to run once:

  //initiate Serial Monitor and OLED display
  Serial.begin(115200);
    if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println(F("SSD1306 allocation failed"));
        for(;;);
    }

    //turn on OLED display
    display.display();
    delay(2000);
    
    //clear OLED display
    display.clearDisplay();

    print_line("Welcome to medibox!", 10, 20, 2);
    display.clearDisplay();

}

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

void update_time(){
  timeNow = millis()/1000;
  seconds = timeNow - timeLast;

  if (seconds >= 60)
  {
    minutes += 1;
    timeLast += 60;
  } 
  if (minutes >= 60)
  {
    hours += 1;
    minutes = 0;
  } 
  if (hours >= 24)
  {
    date += 1;
    hours = 0;
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  update_time();
  print_time();
  delay(10);
}