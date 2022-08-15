#include <Cartesian.h>
#include <Command.h>
#include <Process.h>
#include <LimitSwitch.h>
#include <LiquidScreen.h>
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

enum ScreenID
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

LiquidScreen INFO_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen ABOUT_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);
LiquidScreen MOVE_AXIS_SCREEN = LiquidScreen(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MAIN_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu PREP_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CTRL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu CARD_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu LASER_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOVE_AXES_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_X_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Y_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu MOVE_Z_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidMenu MOTION_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu VELOCITY_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu STEPS_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);
LiquidMenu ACCEL_SCREEN = LiquidMenu(lcd, LCD_COLS, LCD_ROWS);

LiquidScreen SCREENS[] = {
    ABOUT_SCREEN,
    ACCEL_SCREEN,
    CARD_SCREEN,
    CTRL_SCREEN,
    INFO_SCREEN,
    LASER_SCREEN,
    MAIN_SCREEN,
    MOTION_SCREEN,
    MOVE_AXES_SCREEN,
    MOVE_AXIS_SCREEN,
    MOVE_X_SCREEN,
    MOVE_Y_SCREEN,
    MOVE_Z_SCREEN,
    PREP_SCREEN,
    STEPS_SCREEN,
    VELOCITY_SCREEN
};

Rotary rotary(EN_DT_PIN, EN_CLK_PIN, EN_SW_PIN);
Cartesian cartesian;
Laser laser(LSR_PIN);

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

ScreenID currentScreen;

void display(ScreenID screenIndex, bool forcePosition)
{
  LiquidScreen screen = SCREENS[screenIndex];

  uint8_t lineIndex = screen.getCurrentLineIndex();
  uint8_t lineCount = screen.getLineCount();

  rotary.setBoundaries(0, lineCount);
  rotary.setPosition(forcePosition ? lineIndex : 0);

  currentScreen = screenIndex;
  screen.display();
}

void setMoveAxisScreen(Axis axis, uint8_t unit)
{
  rotary.setBoundaries(-100, 1);

  display(ScreenID::MOVE_AXIS, false);
}

void autohome()
{
}

void setHomeOffsets()
{
}

void disableSteppers()
{
}

void aboutScreenClicked()
{
  display(ScreenID::MAIN, true);
}

void storeSettings()
{
}

void accelerationScreenClicked()
{
  switch (ACCEL_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void cardScreenClicked()
{
  switch (CARD_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MAIN, true);
    break;
  default:
    break;
  }
}

void ctrlScreenClicked()
{
  switch (CTRL_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MAIN, true);
    break;
  case 1:
    display(ScreenID::MOTION, false);
    break;
  case 2:
    display(ScreenID::LASER, false);
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
  display(ScreenID::MAIN, false);
}

void mainScreenClicked()
{
  switch (MAIN_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::INFO, true);
    break;
  case 1:
    display(ScreenID::PREP, false);
    break;
  case 2:
    display(ScreenID::CTRL, false);
    break;
  case 3:
    display(ScreenID::CARD, false);
    break;
  case 4:
    display(ScreenID::ABOUT, false);
    break;
  default:
    break;
  }
}

void motionScreenClicked()
{
  switch (MOTION_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::CTRL, true);
    break;
  case 1:
    display(ScreenID::ACCELERATION, false);
    break;
  case 2:
    display(ScreenID::VELOCITY, false);
    break;
  case 3:
    display(ScreenID::STEPS, false);
    break;
  default:
    break;
  }
}

void moveAxesScreenClicked()
{
  switch (MOVE_AXES_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(PREP, true);
    break;
  case 1:
    display(ScreenID::MOVE_X, false);
    break;
  case 2:
    display(ScreenID::MOVE_Y, false);
    break;
  case 3:
    display(ScreenID::MOVE_Z, false);
    break;
  default:
    break;
  }
}

void moveAxisScreenClicked()
{
}

void moveXScreenClicked()
{
}

void moveYScreenClicked()
{
}

void moveZScreenClicked()
{
}

void prepScreenClicked()
{
  switch (PREP_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(ScreenID::MAIN, true);
    break;
  case 1:
    display(MOVE_AXES, false);
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
  switch (STEPS_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void velocityScreenClicked()
{
  switch (VELOCITY_SCREEN.getCurrentLineIndex())
  {
  case 0:
    display(MOTION, true);
    break;
  case 1:
    break;
  default:
    break;
  }
}

void clickCallback()
{
  switch (currentScreen)
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

void rotateCallback(Rotation direction)
{
  LiquidScreen screen = SCREENS[currentScreen];

  if (currentScreen == ScreenID::MOVE_AXIS)
  {
    screen.display(true);
    return;
  }

  if (direction == Rotation::CLOCKWISE)
  {
    screen.nextLine();
  }
  else
  {
    screen.previousLine();
  }

  screen.display(false);
}

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

void createLine(LiquidScreen &screen, uint8_t row, uint8_t col, String text)
{
  screen.append(std::make_shared<LiquidLine>(row, col, text));
}

void createFormattedLine(LiquidScreen &screen, uint8_t row, uint8_t col, String text)
{
  // screen.append(std::make_shared<LiquidFormattedLine>());
  screen.append(std::make_shared<LiquidLine>(row, col, text));
}

void setupAccelScreen()
{
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Accel");
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel");
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax X");
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Y");
  createLine(ACCEL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Amax Z");
}

void setupAboutScreen()
{
  createLine(ABOUT_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
}

void setupCardScreen()
{
  createLine(CARD_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
}

void setupCtrlScreen()
{
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Laser");
  createLine(CTRL_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Store settings");
}

void setupInfoScreen()
{
  createLine(INFO_SCREEN, LCD_ZERO_COL, 0, "Boreal CNC");
  createLine(INFO_SCREEN, LCD_ZERO_COL, 1, "X: 0 Y: 0 Z: 0");
  createLine(INFO_SCREEN, LCD_ZERO_COL, 2, "0%");
}

void setupMainScreen()
{
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Info");
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare");
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Control");
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "No TF card");
  createLine(MAIN_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "About CNC");
}

void setupMoveAxesScreen()
{

  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Prepare");
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X");
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y");
  createLine(MOVE_AXES_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z");
}

void setupMoveAxisScreen()
{
  createFormattedLine(MOVE_AXIS_SCREEN, LCD_ZERO_ROW, 1, "Move %s");  // e.g "Move X"
  createFormattedLine(MOVE_AXIS_SCREEN, LCD_ZERO_ROW, 2, "+%03d.%d"); // e.g "+000.0"
}

void setupMoveXScreen()
{

  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 10mm");
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 1mm");
  createLine(MOVE_X_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move X 0.1mm");
}

void setupMoveYScreen()
{

  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 10mm");
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 1mm");
  createLine(MOVE_Y_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Y 0.1mm");
}
void setupMoveZScreen()
{
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Axes");
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 10mm");
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 1mm");
  createLine(MOVE_Z_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move Z 0.1mm");
}

void setupPrepScreen()
{

  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Main");
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Move axes");
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Auto home");
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Set home offsets");
  createLine(PREP_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Disable steppers");
}

void setupMotionScreen()
{
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Control");
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Acceleration");
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Velocity");
  createLine(MOTION_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Steps/mm");
}
void setupStepsScreen()
{
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "X steps/mm");
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Y steps/mm");
  createLine(STEPS_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Z steps/mm");
}
void setupVelocityScreen()
{
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Motion");
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vel");
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Travel");
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax X");
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Y");
  createLine(VELOCITY_SCREEN, LCD_ARROW_COL, LCD_ZERO_ROW, "Vmax Z");
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

  Wire.begin();
  Wire.setClock(3400000); // High Speed Mode at 3.4 Mbps

  pinMode(CS_RS1_PIN, OUTPUT);
  pinMode(CS_RS2_PIN, OUTPUT);
  pinMode(CS_RS3_PIN, OUTPUT);
  pinMode(CS_RS4_PIN, OUTPUT);

  pinMode(EN_CLK_PIN, INPUT);
  pinMode(EN_DT_PIN, INPUT);
  pinMode(EN_SW_PIN, INPUT_PULLUP);

  pinMode(SDA, PULLUP);
  pinMode(SCL, PULLUP);

  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.createChar(LCD_ARROW_CHAR, ARROW);

  DRIVER_X.setMaxSpeed(600);
  DRIVER_Y.setMaxSpeed(600);
  DRIVER_Z.setMaxSpeed(600);

  DRIVER_X.setAcceleration(300);
  DRIVER_Y.setAcceleration(300);
  DRIVER_Z.setAcceleration(300);

  laser.setMaxPower(LSR_MAX_POWER);

  rotary.setOnClicked(clickCallback);
  rotary.setOnRotation(rotateCallback);

  rotary.setClickDebounceTime(EN_CLICK_DEBOUNCE);
  rotary.setRotationDebounceTime(EN_ROT_DEBOUNCE);

  SW_X.setDebounceTime(10);
  SW_Y.setDebounceTime(10);
  SW_Z.setDebounceTime(10);

  cartesian.setStepperMotor(Axis::X, DRIVER_X);
  cartesian.setStepperMotor(Axis::Y, DRIVER_Y);
  cartesian.setStepperMotor(Axis::Z, DRIVER_Z);

  cartesian.setLimitSwitch(Axis::X, SW_X);
  cartesian.setLimitSwitch(Axis::Y, SW_Y);
  cartesian.setLimitSwitch(Axis::Z, SW_Z);

  cartesian.setMinStepsPerMillimeter(Axis::X, 5);
  cartesian.setMinStepsPerMillimeter(Axis::Y, 5);
  cartesian.setMinStepsPerMillimeter(Axis::Z, 25);

  cartesian.setStepsPerMillimeter(Axis::X, 10);
  cartesian.setStepsPerMillimeter(Axis::Y, 10);
  cartesian.setStepsPerMillimeter(Axis::Z, 50);

  cartesian.setDimension(Axis::X, 400);
  cartesian.setDimension(Axis::Y, 420);
  cartesian.setDimension(Axis::Z, 95);

  setupAccelScreen();
  setupAboutScreen();
  setupCardScreen();
  setupCtrlScreen();
  setupInfoScreen();
  setupMainScreen();
  setupMotionScreen();
  setupMoveAxesScreen();
  setupMoveAxisScreen();
  setupMoveXScreen();
  setupMoveYScreen();
  setupMoveZScreen();
  setupPrepScreen();
  setupStepsScreen();
  setupVelocityScreen();

  INFO_SCREEN.display();
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
          cmdQueue.push(std::make_shared<LinearMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'X', 0), parseNumber(line, 'S', 0)));
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

            cmdQueue.push(std::make_shared<ArcMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'I', 0), parseNumber(line, 'J', 0), parseNumber(line, 'K', 0), parseNumber(line, 'S', 0)));
          }
          else
          {
            // TODO: invalid circle
            if (line.indexOf('X') < 0 && line.indexOf('Y') < 0)
            {
              return;
            }

            cmdQueue.push(std::make_shared<CircleMoveCommand>(cartesian, laser, parseNumber(line, 'X', 0), parseNumber(line, 'Y', 0), parseNumber(line, 'Z', 0), parseNumber(line, 'R', 0), parseNumber(line, 'S', 0)));
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
          cmdQueue.push(std::make_shared<AutohomeCommand>(cartesian, laser));
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