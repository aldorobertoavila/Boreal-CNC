#include <Cartesian.h>
#include <Command.h>
#include <Process.h>
#include <LimitSwitch.h>
#include <LiquidViewport.h>
#include <Rotary.h>
#include <StepperMotor.h>

#include <LiquidCrystal_I2C.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>

#include <queue>
#include <memory>

using namespace std;

#define CMD_QUEUE_SIZE 10

#define EN_CLICK_DEBOUNCE 250
#define EN_ROT_DEBOUNCE 5

#define LCD_ADDR 0x27
#define LCD_COLS 20
#define LCD_ROWS 4

#define LCD_ARROW_CHAR 0
#define LCD_ARROW_COL 2
#define LCD_ZERO_COL 0
#define LCD_ZERO_ROW 0

#define LSR_MAX_POWER 255

#define EN_CLK_PIN 4
#define EN_DT_PIN 2
#define EN_SW_PIN 17

#define LSR_PIN 13

#define SW_X_PIN 36
#define SW_Y_PIN 39
#define SW_Z_PIN 34

#define CS_RS1_PIN 27
#define CS_RS2_PIN 26
#define CS_RS3_PIN 25
#define CS_RS4_PIN 33

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

SPIClass spi(VSPI);

ShiftRegisterMotorInterface SHIFT_REGISTER_X1(spi, CS_RS1_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_X2(spi, CS_RS2_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Y(spi, CS_RS3_PIN, MSBFIRST);
ShiftRegisterMotorInterface SHIFT_REGISTER_Z(spi, CS_RS4_PIN, MSBFIRST);

BilateralMotorInterface BILATERAL_X(SHIFT_REGISTER_X1, SHIFT_REGISTER_X2);

StepperMotor DRIVER_X(BILATERAL_X);
StepperMotor DRIVER_Y(SHIFT_REGISTER_Y);
StepperMotor DRIVER_Z(SHIFT_REGISTER_Z);

LimitSwitch SW_X(SW_X_PIN);
LimitSwitch SW_Y(SW_Y_PIN);
LimitSwitch SW_Z(SW_Z_PIN);

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

Cartesian CARTESIAN;
Laser laser(LSR_PIN);

const uint8_t TEN_MILLIMETER = 100;
const uint8_t ONE_MILLIMETER = 10;
const uint8_t TENTH_MILLIMETER = 1;

byte ARROW[8] =
    {
        0b00000,
        0b00100,
        0b00110,
        0b11111,
        0b11111,
        0b00110,
        0b00100,
        0b00000};

queue<std::shared_ptr<Command>> cmdQueue;
queue<std::shared_ptr<Process>> procQueue;
std::shared_ptr<Command> cmd;
std::shared_ptr<Process> proc;

/*
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

void setScreen(uint8_t screenId)
{
  setScreen(screenId, false);
}

void setMoveAxisScreen(Axis axis, uint8_t unit)
{
  setScreen(MOVE_AXIS);

  rotary.setBoundaries(-100, 1);

  viewport.setCurrentScreen(MOVE_AXIS);
  viewport.display(true);
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
    setScreen(Screen::LASER);
    break;
  case 3:
    // storeSettings();
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
    // setScreen(CARD);
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
    // autohome();
    break;
  case 3:
    // setHomeOffsets();
    break;
  case 4:
    // disableSteppers();
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
  // uint8_t tens = millimeters / 10;
  // uint8_t ones = millimeters % 10;
  // snprintf(buf, sizeof(buf), text, tens, ones);
}

void clickCallback()
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
    viewport.display(true);
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
    viewport.display(true);
    break;
  default:
    break;
  }
}

void rotateCallback(Rotation direction)
{

  if (direction == Rotation::CLOCKWISE)
  {
    onRotationCW();
  }
  else
  {
    onRotationCCW();
  }
}
*/

float parseNumber(String line, int index, char arg, float val)
{
  if (index < 0)
  {
    return val;
  }

  String sub = line.substring(index + 1);
  index = sub.indexOf(' ');
  sub = sub.substring(0, index);

  return atof(sub.c_str());
}

float parseNumber(String line, char arg, float val)
{
  return parseNumber(line, line.indexOf(arg), arg, val);
}

void setup()
{
  Serial.begin(115200);

  // wait until serial
  while (!Serial)
    ;

  spi.begin();

  Serial.print("SD ");

  if (!SD.begin(SS))
  {
    Serial.println("failed to mount!");
    return;
  }

  Serial.println("mount!");

  /*
  lcd.init();
  Wire.setClock(400000);
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.createChar(LCD_ARROW_CHAR, ARROW);
  */

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  pinMode(CS_RS1_PIN, OUTPUT);
  pinMode(CS_RS2_PIN, OUTPUT);
  pinMode(CS_RS3_PIN, OUTPUT);
  pinMode(CS_RS4_PIN, OUTPUT);

  DRIVER_X.setMaxSpeed(600);
  DRIVER_Y.setMaxSpeed(600);
  DRIVER_Z.setMaxSpeed(600);

  DRIVER_X.setAcceleration(300);
  DRIVER_Y.setAcceleration(300);
  DRIVER_Z.setAcceleration(300);

  laser.setMaxPower(LSR_MAX_POWER);

  /*
  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);
  */

  SW_X.setDebounceTime(10);
  SW_Y.setDebounceTime(10);
  SW_Z.setDebounceTime(10);

  CARTESIAN.setStepperMotor(Axis::X, DRIVER_X);
  CARTESIAN.setStepperMotor(Axis::Y, DRIVER_Y);
  CARTESIAN.setStepperMotor(Axis::Z, DRIVER_Z);

  CARTESIAN.setLimitSwitch(Axis::X, SW_X);
  CARTESIAN.setLimitSwitch(Axis::Y, SW_Y);
  CARTESIAN.setLimitSwitch(Axis::Z, SW_Z);

  CARTESIAN.setMinStepsPerMillimeter(Axis::X, 5);
  CARTESIAN.setMinStepsPerMillimeter(Axis::Y, 5);
  CARTESIAN.setMinStepsPerMillimeter(Axis::Z, 25);

  CARTESIAN.setStepsPerMillimeter(Axis::X, 10);
  CARTESIAN.setStepsPerMillimeter(Axis::Y, 10);
  CARTESIAN.setStepsPerMillimeter(Axis::Z, 50);

  CARTESIAN.setDimension(Axis::X, 400);
  CARTESIAN.setDimension(Axis::Y, 420);
  CARTESIAN.setDimension(Axis::Z, 95);

  /*
  LiquidScreen::createLine(infoScreen, LCD_ZERO_COL, 0, "Boreal CNC");
  LiquidScreen::createLine(infoScreen, LCD_ZERO_COL, 1, "X: 0 Y: 0 Z: 0");
  LiquidScreen::createLine(infoScreen, LCD_ZERO_COL, 2, "0%");
  LiquidScreen::createLine(infoScreen, LCD_ZERO_COL, 3, "Boreal CNC");

  LiquidScreen::createLine(mainScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Info");
  LiquidScreen::createLine(mainScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare");
  LiquidScreen::createLine(mainScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Control");
  LiquidScreen::createLine(mainScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "No TF card");
  LiquidScreen::createLine(mainScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "About CNC ");

  LiquidScreen::createLine(prepScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
  LiquidScreen::createLine(prepScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move axes");
  LiquidScreen::createLine(prepScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Auto home");
  LiquidScreen::createLine(prepScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Set home offsets");
  LiquidScreen::createLine(prepScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Disable steppers");

  LiquidScreen::createLine(moveAxesScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare");
  LiquidScreen::createLine(moveAxesScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X");
  LiquidScreen::createLine(moveAxesScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y");
  LiquidScreen::createLine(moveAxesScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z");

  LiquidScreen::createLine(moveXScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveXScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 10mm");
  LiquidScreen::createLine(moveXScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 1mm");
  LiquidScreen::createLine(moveXScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 0.1mm");

  LiquidScreen::createLine(moveYScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveYScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 10mm");
  LiquidScreen::createLine(moveYScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 1mm");
  LiquidScreen::createLine(moveYScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 0.1mm");

  LiquidScreen::createLine(moveZScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  LiquidScreen::createLine(moveZScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 10mm");
  LiquidScreen::createLine(moveZScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 1mm");
  LiquidScreen::createLine(moveZScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 0.1mm");

  LiquidScreen::createLine(ctrlScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
  LiquidScreen::createLine(ctrlScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  LiquidScreen::createLine(ctrlScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Laser");
  LiquidScreen::createLine(ctrlScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Store settings");

  LiquidScreen::createLine(motionScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Control");
  LiquidScreen::createLine(motionScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Acceleration");
  LiquidScreen::createLine(motionScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Velocity");
  LiquidScreen::createLine(motionScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Steps/mm");

  LiquidScreen::createLine(stepsScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  LiquidScreen::createLine(stepsScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "X steps/mm");
  LiquidScreen::createLine(stepsScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Y steps/mm");
  LiquidScreen::createLine(stepsScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Z steps/mm");

  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Vel");
  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel");
  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax X");
  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Y");
  LiquidScreen::createLine(velocityScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Z");

  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Accel");
  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel");
  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax X");
  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Y");
  LiquidScreen::createLine(accelScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Z");

  LiquidScreen::createLine(cardScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
  LiquidScreen::createLine(aboutScreen, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");

  LiquidScreen::createFormattedLine(moveAxisScreen, LCD_ZERO_ROW, 1, "Move %s");  // e.g "Move X"
  LiquidScreen::createFormattedLine(moveAxisScreen, LCD_ZERO_ROW, 2, "+%03d.%d"); // e.g "+000.0"

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
  */

  procQueue.push(std::make_shared<Process>(SD, "/test1.gcode"));
  // procQueue.push(std::make_shared<Process>(SD, "/test2.gcode"));
  // procQueue.push(std::make_shared<Process>(SD, "/test3.gcode"));

  // cmdQueue.push(std::make_shared<AutohomeCommand>(CARTESIAN, laser));
  // cmdQueue.push(std::make_shared<DwellCommand>(5000));
  // cmdQueue.push(std::make_shared<LinearMoveCommand>(CARTESIAN, laser, 10, 10, 2, 0));
  // cmdQueue.push(std::make_shared<DwellCommand>(5000));
  // cmdQueue.push(std::make_shared<LinearMoveCommand>(CARTESIAN, laser, 20, 20, 2, 0));
}

void loop()
{
  // rotary.tick();

  if (!procQueue.empty() && !proc)
  {
    proc = procQueue.front();
    procQueue.pop();

    if (proc)
    {
      proc->start();
    }
  }

  if (proc)
  {
    Status status = proc->status();

    switch (status)
    {
    case Status::COMPLETED:
      proc = nullptr;
      break;

    case Status::CONTINUE:
      if (cmdQueue.size() < CMD_QUEUE_SIZE)
      {
        String line = proc->readNextLine();

        if (line.isEmpty())
        {
          return;
        }

        int code = parseNumber(line, 'G', -1);
        // TODO: ignore comments

        switch (code)
        {
        case 0:
        case 1:
          cmdQueue.push(std::make_shared<LinearMoveCommand>(CARTESIAN, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'X', 0), parseNumber(line, 'S', 0)));
          break;
        case 2:
        case 3:

          if (line.indexOf('I') > 0 || line.indexOf('J') > 0)
          {
            // TODO: invalid arc
            if (line.indexOf('R') > 0)
            {
              return;
            }

            cmdQueue.push(std::make_shared<ArcMoveCommand>(CARTESIAN, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'I', 0), parseNumber(line, 'J', 0), parseNumber(line, 'K', 0), parseNumber(line, 'S', 0)));
          }
          else
          {
            // TODO: invalid circle
            if (line.indexOf('X') < 0 && line.indexOf('Y') < 0)
            {
              return;
            }

            cmdQueue.push(std::make_shared<CircleMoveCommand>(CARTESIAN, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'R', 0), parseNumber(line, 'S', 0)));
          }

          break;
        case 4:
          if (line.indexOf('S') > 0)
          {
            cmdQueue.push(std::make_shared<DwellCommand>(parseNumber(line, 'S', 0) * 1000));
          }
          else
          {
            cmdQueue.push(std::make_shared<DwellCommand>(parseNumber(line, 'P', 0)));
          }

          break;

        case 28:
          cmdQueue.push(std::make_shared<AutohomeCommand>(CARTESIAN, laser));
          break;
        default:
          break;
        }
      }
      break;

    case Status::ERROR:
      // TODO: create report
      proc = nullptr;
      break;
    default:
      break;
    }
  }

  if (!cmdQueue.empty() && !cmd)
  {
    cmd = cmdQueue.front();
    cmdQueue.pop();

    if (cmd)
    {
      cmd->start();
    }
  }

  if (cmd)
  {
    cmd->execute();

    Status status = cmd->status();

    switch (status)
    {
    case Status::COMPLETED:
      cmd->finish();
      cmd = nullptr;
      break;

    case Status::ERROR:
      // TODO: create report
      cmd = nullptr;
      break;
    default:
      break;
    }
  }
}