#include <Arduino.h>
#include <LiquidViewport.h>
#include <Rotary.h>

#include <LiquidCrystal_I2C.h>
#include <Wire.h>

#define EN_DT_PIN 2
#define EN_SW_PIN 3
#define EN_CLK_PIN 4

#define EN_CLICK_DEBOUNCE 250
#define EN_ROT_DEBOUNCE 5

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define ARROW_CHAR 0

#define ARROW_COL 2
#define ZERO_COL 0
#define ZERO_ROW 0
#define AXIS_COL 7

enum Axis
{
  X,
  Y,
  Z
};

enum Screen
{
  ABOUT,
  ACCELERATION,
  CARD,
  CTRL,
  INFO,
  LASER,
  MAIN,
  MOTION,
  MOVE_AXES,
  MOVE_AXIS,
  MOVE_X,
  MOVE_Y,
  MOVE_Z,
  PREP,
  STEPS,
  VELOCITY
};

const uint8_t TEN_MILLIMETER = 100;
const uint8_t ONE_MILLIMETER = 10;
const uint8_t TENTH_MILLIMETER = 1;

Axis currentAxis;
uint8_t currentUnit;
uint16_t millimeters;

byte ARROW[8] =
{
  0b00000,
  0b00100,
  0b00110,
  0b11111,
  0b11111,
  0b00110,
  0b00100,
  0b00000
};

LCD lcd(LCD_ADDR, LCD_COLS, LCD_ROWS);

LiquidScreen infoScreen;
LiquidScreen aboutScreen;
LiquidScreen moveAxisScreen;

LiquidMenu mainScreen;
LiquidMenu prepScreen;
LiquidMenu ctrlScreen;
LiquidMenu cardScreen;

LiquidMenu moveAxesScreen;
LiquidMenu moveXScreen;
LiquidMenu moveYScreen;
LiquidMenu moveZScreen;

LiquidMenu motionScreen;
LiquidMenu velocityScreen;
LiquidMenu stepsScreen;
LiquidMenu accelScreen;

LiquidViewport viewport(lcd, LCD_COLS, LCD_ROWS);
Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);

void onClicked()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case ABOUT:
    aboutScreenClicked();
    break;
  case ACCELERATION:
    accelerationScreenClicked();
    break;
  case CARD:
    cardScreenClicked();
    break;
  case CTRL:
    ctrlScreenClicked();
    break;
  case INFO:
    infoScreenClicked();
    break;
  case MAIN:
    mainScreenClicked();
    break;
  case MOTION:
    motionScreenClicked();
    break;
  case MOVE_AXIS:
    moveAxisScreenClicked();
    break;
  case MOVE_AXES:
    moveAxesScreenClicked();
    break;
  case MOVE_X:
    moveXScreenClicked();
    break;
  case MOVE_Y:
    moveYScreenClicked();
    break;
  case MOVE_Z:
    moveZScreenClicked();
    break;
  case PREP:
    prepScreenClicked();
    break;
  case STEPS:
    stepsScreenClicked();
    break;
  case VELOCITY:
    velocityScreenClicked();
    break;
  default:
    break;
  }
}

void onRotationCCW()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case ACCELERATION:
  case CTRL:
  case MAIN:
  case MOTION:
  case MOVE_AXES:
  case MOVE_X:
  case MOVE_Y:
  case MOVE_Z:
  case PREP:
  case STEPS:
  case VELOCITY:
    viewport.previousLine();
    viewport.display(false);
    break;
  case MOVE_AXIS:
    // cnc.move(currentAxis, Rotation::POSITIVE, units);
    millimeters += currentUnit;
    viewport.display(true);
    break;
  default:
    break;
  }
}

void onRotationCW()
{
  switch (viewport.getCurrentScreenIndex())
  {
  case ACCELERATION:
  case CTRL:
  case MAIN:
  case MOTION:
  case MOVE_AXES:
  case MOVE_X:
  case MOVE_Y:
  case MOVE_Z:
  case PREP:
  case STEPS:
  case VELOCITY:
    viewport.nextLine();
    viewport.display(false);
    break;
  case MOVE_AXIS:
    // cnc.move(currentAxis, Rotation::NEGATIVE, units);
    millimeters -= currentUnit;
    if (millimeters < 0)
      millimeters = 0;
    viewport.display(true);
    break;
  default:
    break;
  }
}

void onRotation(Rotation direction)
{
  switch (direction)
  {
  case COUNTERCLOCKWISE:
    onRotationCCW();
    break;
  case CLOCKWISE:
    onRotationCW();
    break;
  default:
    break;
  }
}

void aboutScreenClicked()
{
  setScreen(MAIN, true);
}

void accelerationScreenClicked()
{
  switch (viewport.getCurrentLineIndex(ACCELERATION))
  {
  case 0:
    setScreen(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CARD))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  default:
    break;
  }
}

void ctrlScreenClicked()
{
  switch (viewport.getCurrentLineIndex(CTRL))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  case 1:
    setScreen(MOTION);
    break;
  case 2:
    setScreen(LASER);
    break;
  case 3:
    storeSettings();
    break;
  default:
    break;
  }
}

void infoScreenClicked()
{
  setScreen(MAIN);
}

void mainScreenClicked()
{

  switch (viewport.getCurrentLineIndex(MAIN))
  {
  case 0:
    setScreen(INFO);
    break;
  case 1:
    setScreen(PREP);
    break;
  case 2:
    setScreen(CTRL);
    break;
  case 3:
    /*
      setScreen(CARD);
    */
    break;
  case 4:
    setScreen(ABOUT);
    break;
  default:
    break;
  }
}

void motionScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOTION))
  {
  case 0:
    setScreen(CTRL, true);
    break;
  case 1:
    setScreen(ACCELERATION);
    break;
  case 2:
    setScreen(VELOCITY);
    break;
  case 3:
    setScreen(STEPS);
    break;
  default:
    break;
  }
}

void moveAxesScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_AXES))
  {
  case 0:
    setScreen(PREP, true);
    break;
  case 1:
    setScreen(MOVE_X);
    break;
  case 2:
    setScreen(MOVE_Y);
    break;
  case 3:
    setScreen(MOVE_Z);
    break;
  default:
    break;
  }
}

void moveAxisScreenClicked()
{
  switch (currentAxis)
  {
  case X:
    setScreen(MOVE_X, true);
    break;
  case Y:
    setScreen(MOVE_Y, true);
    break;
  case Z:
    setScreen(MOVE_Z, true);
    break;
  default:
    break;
  }
}

void moveXScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_X))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(X, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(X, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(X, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void moveYScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_Y))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(Y, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(Y, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(Y, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void moveZScreenClicked()
{
  switch (viewport.getCurrentLineIndex(MOVE_Z))
  {
  case 0:
    setScreen(MOVE_AXES, true);
    break;
  case 1:
    setMoveAxisScreen(Z, TEN_MILLIMETER);
    break;
  case 2:
    setMoveAxisScreen(Z, ONE_MILLIMETER);
    break;
  case 3:
    setMoveAxisScreen(Z, TENTH_MILLIMETER);
    break;
  default:
    break;
  }
}

void prepScreenClicked()
{
  switch (viewport.getCurrentLineIndex(PREP))
  {
  case 0:
    setScreen(MAIN, true);
    break;
  case 1:
    setScreen(MOVE_AXES);
    break;
  case 2:
    autohome();
    break;
  case 3:
    setHomeOffsets();
    break;
  case 4:
    disableSteppers();
    break;
  default:
    break;
  }
}

void stepsScreenClicked()
{
  switch (viewport.getCurrentLineIndex(STEPS))
  {
  case 0:
    setScreen(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void velocityScreenClicked()
{
  switch (viewport.getCurrentLineIndex(VELOCITY))
  {
  case 0:
    setScreen(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void formatAxisLine(Buffer buf, char *text)
{
  switch (currentAxis)
  {
  case X:
    snprintf(buf, sizeof(buf), text, "X");
    break;
  case Y:
    snprintf(buf, sizeof(buf), text, "Y");
    break;
  case Z:
    snprintf(buf, sizeof(buf), text, "Z");
    break;
  }
}

void formatMilliLine(Buffer buf, char *text)
{
  uint8_t tens = millimeters / 10;
  uint8_t ones = millimeters % 10;
  snprintf(buf, sizeof(buf), text, tens, ones);
}

void autohome()
{
  Serial.println("Auto home");
}

void setHomeOffsets()
{
  Serial.println("Set Home Offsets");
}

void disableSteppers()
{
  Serial.println("Disable Steppers");
}

void storeSettings()
{
  Serial.println("Store Settings");
}

void setScreen(uint8_t screenId)
{
  setScreen(screenId, false);
}

void setScreen(LiquidScreen *screen, uint8_t screenId, bool forcePosition)
{
  uint8_t lineIndex = viewport.getCurrentLineIndex(screenId);
  uint8_t lineCount = screen->getLineCount();

  rotary.setBoundaries(0, lineCount);
  rotary.setPosition(forcePosition ? lineIndex : 0);

  viewport.setCurrentScreen(screenId);
  viewport.display(true);
}

void setScreen(uint8_t screenId, bool forcePosition)
{
  LiquidScreen *screen = viewport.getScreen(screenId);
  
  if (screen)
  {
    setScreen(screen, screenId, forcePosition);
  }
}

void setMoveAxisScreen(Axis axis, uint8_t unit)
{
  setScreen(MOVE_AXIS);

  currentAxis = axis;
  currentUnit = unit;

  rotary.setBoundaries(-100, 1);

  viewport.setCurrentScreen(MOVE_AXIS);
  viewport.display(true);
}

void setup()
{
  Serial.begin(115200);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  rotary.setOnClicked(onClicked);
  rotary.setOnRotation(onRotation);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.createChar(ARROW_CHAR, ARROW);

  LiquidScreen::createLine(infoScreen, ZERO_COL, 0, "Boreal CNC");
  LiquidScreen::createLine(infoScreen, ZERO_COL, 1, "X: 0 Y: 0 Z: 0");
  LiquidScreen::createLine(infoScreen, ZERO_COL, 2, "0%");
  LiquidScreen::createLine(infoScreen, ZERO_COL, 3, "Boreal CNC");

  LiquidScreen::createLine(mainScreen, ARROW_COL, ZERO_ROW, "Info");
  LiquidScreen::createLine(mainScreen, ARROW_COL, ZERO_ROW, "Prepare");
  LiquidScreen::createLine(mainScreen, ARROW_COL, ZERO_ROW, "Control");
  LiquidScreen::createLine(mainScreen, ARROW_COL, ZERO_ROW, "No TF card");
  LiquidScreen::createLine(mainScreen, ARROW_COL, ZERO_ROW, "About CNC ");

  LiquidScreen::createLine(prepScreen, ARROW_COL, ZERO_ROW, "Main");
  LiquidScreen::createLine(prepScreen, ARROW_COL, ZERO_ROW, "Move axes");
  LiquidScreen::createLine(prepScreen, ARROW_COL, ZERO_ROW, "Auto home");
  LiquidScreen::createLine(prepScreen, ARROW_COL, ZERO_ROW, "Set home offsets");
  LiquidScreen::createLine(prepScreen, ARROW_COL, ZERO_ROW, "Disable steppers");

  LiquidScreen::createLine(moveAxesScreen, ARROW_COL, ZERO_ROW, "Prepare");
  LiquidScreen::createLine(moveAxesScreen, ARROW_COL, ZERO_ROW, "Move X");
  LiquidScreen::createLine(moveAxesScreen, ARROW_COL, ZERO_ROW, "Move Y");
  LiquidScreen::createLine(moveAxesScreen, ARROW_COL, ZERO_ROW, "Move Z");

  LiquidScreen::createLine(moveXScreen, ARROW_COL, ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveXScreen, ARROW_COL, ZERO_ROW, "Move X 10mm");
  LiquidScreen::createLine(moveXScreen, ARROW_COL, ZERO_ROW, "Move X 1mm");
  LiquidScreen::createLine(moveXScreen, ARROW_COL, ZERO_ROW, "Move X 0.1mm");

  LiquidScreen::createLine(moveYScreen, ARROW_COL, ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveYScreen, ARROW_COL, ZERO_ROW, "Move Y 10mm");
  LiquidScreen::createLine(moveYScreen, ARROW_COL, ZERO_ROW, "Move Y 1mm");
  LiquidScreen::createLine(moveYScreen, ARROW_COL, ZERO_ROW, "Move Y 0.1mm");

  LiquidScreen::createLine(moveZScreen, ARROW_COL, ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveZScreen, ARROW_COL, ZERO_ROW, "Move Z 10mm");
  LiquidScreen::createLine(moveZScreen, ARROW_COL, ZERO_ROW, "Move Z 1mm");
  LiquidScreen::createLine(moveZScreen, ARROW_COL, ZERO_ROW, "Move Z 0.1mm");

  LiquidScreen::createLine(ctrlScreen, ARROW_COL, ZERO_ROW, "Main");
  LiquidScreen::createLine(ctrlScreen, ARROW_COL, ZERO_ROW, "Motion");
  LiquidScreen::createLine(ctrlScreen, ARROW_COL, ZERO_ROW, "Laser");
  LiquidScreen::createLine(ctrlScreen, ARROW_COL, ZERO_ROW, "Store settings");

  LiquidScreen::createLine(motionScreen, ARROW_COL, ZERO_ROW, "Control");
  LiquidScreen::createLine(motionScreen, ARROW_COL, ZERO_ROW, "Acceleration");
  LiquidScreen::createLine(motionScreen, ARROW_COL, ZERO_ROW, "Velocity");
  LiquidScreen::createLine(motionScreen, ARROW_COL, ZERO_ROW, "Steps/mm");

  LiquidScreen::createLine(stepsScreen, ARROW_COL, ZERO_ROW, "Motion");
  LiquidScreen::createLine(stepsScreen, ARROW_COL, ZERO_ROW, "X steps/mm");
  LiquidScreen::createLine(stepsScreen, ARROW_COL, ZERO_ROW, "Y steps/mm");
  LiquidScreen::createLine(stepsScreen, ARROW_COL, ZERO_ROW, "Z steps/mm");

  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Motion");
  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Vel");
  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Travel");
  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Vmax X");
  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Vmax Y");
  LiquidScreen::createLine(velocityScreen, ARROW_COL, ZERO_ROW, "Vmax Z");

  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Motion");
  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Accel");
  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Travel");
  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Amax X");
  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Amax Y");
  LiquidScreen::createLine(accelScreen, ARROW_COL, ZERO_ROW, "Amax Z");

  LiquidScreen::createLine(cardScreen, ARROW_COL, ZERO_ROW, "Main");
  LiquidScreen::createLine(aboutScreen, ARROW_COL, ZERO_ROW, "Main");

  LiquidScreen::createFormattedLine(moveAxisScreen, AXIS_COL, 1, "Move %s"); // e.g "Move X"
  LiquidScreen::createFormattedLine(moveAxisScreen, AXIS_COL, 2, "+%03d.%d"); // e.g "+000.0"

  viewport.addScreen(INFO, infoScreen);
  viewport.addScreen(MAIN, mainScreen);
  viewport.addScreen(PREP, prepScreen);
  viewport.addScreen(CTRL, ctrlScreen);
  viewport.addScreen(CARD, cardScreen);
  viewport.addScreen(ABOUT, aboutScreen);

  viewport.addScreen(MOVE_AXES, moveAxesScreen);
  viewport.addScreen(MOVE_AXIS, moveAxisScreen);
  viewport.addScreen(MOVE_X, moveXScreen);
  viewport.addScreen(MOVE_Y, moveYScreen);
  viewport.addScreen(MOVE_Z, moveZScreen);

  viewport.addScreen(MOTION, motionScreen);

  viewport.addScreen(ACCELERATION, accelScreen);
  viewport.addScreen(VELOCITY, velocityScreen);
  viewport.addScreen(STEPS, stepsScreen);

  viewport.setCurrentScreen(INFO);
  viewport.display(true);
}

void loop()
{
  rotary.tick();
}