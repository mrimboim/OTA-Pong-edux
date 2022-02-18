#include <otaConfig.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <string>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define UP_BUTTON 12
#define DOWN_BUTTON 14

const unsigned long PADDLE_RATE = 40;
const unsigned long BALL_RATE = 0;
const uint8_t PADDLE_HEIGHT = 18;

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void drawCourt()
{
  display.drawRect(0, (64-48), 128, 48, WHITE);
}



uint8_t ball_x = 64, ball_y = (48/2);
uint8_t ball_dir_x = 1, ball_dir_y = 1;
unsigned long ball_update;

unsigned long paddle_update;
const uint8_t CPU_X = 12;
uint8_t cpu_y = 17;
uint8_t cpu_score = 0;

const uint8_t PLAYER_X = 115;
uint8_t player_y = 17;
uint8_t player_score = 0;

void drawScore()
{
  display.fillRect(1, 1, 126,15, BLACK);
  display.setTextSize(1.5);
	display.setTextColor(WHITE);
	display.setCursor(5, 5);
	display.print("CPU:");
  //display.print((std::to_string(cpu_score)).c_str());
  display.print(cpu_score);

	display.setCursor(68, 5);
	display.print("Player:");
  //display.print((std::to_string(cpu_score)).c_str());
  display.print(player_score);

}

void setup()
{
  Serial.begin(115200);
  Serial.println("Booting");
  ArduinoOTA.setHostname("firstsketch");
  setupOTA();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }

  pinMode(UP_BUTTON, INPUT);
  pinMode(DOWN_BUTTON, INPUT);

  display.clearDisplay();
  drawCourt();
  display.drawRect(0, 0, 128, 17, WHITE);//score box
  drawScore();

 // while (millis() - start < 2000);

  display.display();

  ball_update = millis();
  paddle_update = ball_update;
}

// the loop function runs over and over again forever
void loop()
{
  ArduinoOTA.handle();

  bool update = false;
  bool end = false;
  unsigned long time = millis();

  static bool up_state = false;
  static bool down_state = false;

  up_state = (digitalRead(UP_BUTTON) == LOW);
  down_state = (digitalRead(DOWN_BUTTON) == LOW);

  if (time > ball_update)
  {
    uint8_t new_x = ball_x + ball_dir_x;
    uint8_t new_y = ball_y + ball_dir_y;

    // Check if we hit the vertical walls
    if (new_x == 0 || new_x == 127)
    {
      if(new_x == 127){
        cpu_score++;
      }else{
        player_score++;
      }
      ball_dir_x = -ball_dir_x;
      new_x += 2*(ball_dir_x);
      drawScore();
      if(player_score > 9 || cpu_score > 9)
      {
        end = true;
      }

    }

    // Check if we hit the horizontal walls.
    if (new_y == ((64 - 48)) || new_y == 63)
    {
      ball_dir_y = -ball_dir_y;
      new_y +=  2*(ball_dir_y);
    }

    // Check if we hit the CPU paddle
    if (new_x == CPU_X && new_y >= cpu_y && new_y <= cpu_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += 2*(ball_dir_x);
    }

    // Check if we hit the player paddle
    if (new_x == PLAYER_X && new_y >= player_y && new_y <= player_y + PADDLE_HEIGHT)
    {
      ball_dir_x = -ball_dir_x;
      new_x += 2*(ball_dir_x);
    }

    display.drawPixel(ball_x, ball_y, BLACK);
    display.drawPixel(new_x, new_y, WHITE);
    ball_x = new_x;
    ball_y = new_y;

    ball_update += BALL_RATE;
    update = true;
  }

  if (time > paddle_update)
  {
    paddle_update += PADDLE_RATE;

    // CPU paddle
    display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, BLACK);
    const uint8_t half_paddle = PADDLE_HEIGHT >> 1;
    if (cpu_y + half_paddle > ball_y)
    {
      cpu_y -= 1;
    }
    if (cpu_y + half_paddle < ball_y)
    {
      cpu_y += 1;
    }
    if (cpu_y < 17)
      cpu_y = 17;
    if (cpu_y + PADDLE_HEIGHT > 63)
      cpu_y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(CPU_X, cpu_y, PADDLE_HEIGHT, WHITE);

    // Player paddle
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, BLACK);
    if (up_state)
    {
      player_y -= 1;
    }
    if (down_state)
    {
      player_y += 1;
    }
    up_state = down_state = false;
    if (player_y < 17)
      player_y = 17;
    if (player_y + PADDLE_HEIGHT > 63)
      player_y = 63 - PADDLE_HEIGHT;
    display.drawFastVLine(PLAYER_X, player_y, PADDLE_HEIGHT, WHITE);

    update = true;
  }

  // if(end){//later on make a end animation that does some type of wipe
 
 
  //   display.clearDisplay()
     
  // }

  if(update)  {
    display.display();
  }

}