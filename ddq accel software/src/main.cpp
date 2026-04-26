#include <Arduino.h>
#include <variables.h>

/*
  PS3 CALLBACKS
*/

void Handle_Basic(ps3_button_t btn_down) {
  if (btn_down.up) Thwack();
  if (btn_down.right) Right_Shot();
  if (btn_down.down) I_CHALLENGE_YOU();
  if (btn_down.left) Left_Shot();
  if (btn_down.circle) Right_Uppercut();
  if (btn_down.square) Left_Uppercut();
  if (btn_down.r1) Light_Right();
  if (btn_down.l1) Light_Left();
  if (btn_down.r2) Heavy_Right();
  if (btn_down.l2) Heavy_Left();
}

void Handle_Taunt(ps3_button_t btn_down) {
  if (btn_down.up) I_CHALLENGE_YOU();
  if (btn_down.right) REVEL();
  if (btn_down.down) FOR_DULCINEA();
  if (btn_down.left) TO_RIGHT_THE_UNRIGHTABLE_WRONG();
  if (btn_down.circle) Right_Uppercut();
  if (btn_down.square) Left_Uppercut();
  if (btn_down.r1) Light_Right();
  if (btn_down.l1) Light_Left();
  if (btn_down.r2) Heavy_Right();
  if (btn_down.l2) Heavy_Left();
}

void notify() {
  ps3_button_t btn_down = Ps3.data.button;
  ps3_analog_stick_t stick_data = Ps3.data.analog.stick;
  int lx = stick_data.lx;
  int ly = stick_data.ly;
  int rx = stick_data.rx;
  int ry = stick_data.ry;

  // Handle state transitions
  if (Ps3.event.button_down.cross) { locomotion_state = (locomotion_state != CROUCHED) ? CROUCHED : GAUCHO; }
  if (Ps3.event.button_down.triangle) { drive_state = (drive_state != STD) ? STD : OFF; }
  if (Ps3.event.button_down.start) { drive_state = (drive_state != ADV) ? ADV : OFF; }
  if (Ps3.event.button_down.select) { 
    atk_state = (atk_state != TAUNT) ? TAUNT : BASIC; 
    drive_state = OFF;
  }

  // Handle timeouts
  if (Ps3.event.button_down.up) thwack_timeout = millis();
  if (Ps3.event.button_down.circle) right_uppercut_timeout = millis();
  if (Ps3.event.button_down.square) left_uppercut_timeout = millis();
  if (Ps3.event.button_down.down) t3_timeout = millis();

  // Check if battery is low
  float voltage = 0;
  for (int i = 0; i < K; i++) voltage += readings[i];
  voltage /= K;
  if (voltage < 2550) { 
    // Voltage is too low to operate, disable machine
    Idle(); 
  }
  else {
    if (drive_state == OFF) {
      // Check if any buttons were pressed
      if (
        btn_down.l1 || btn_down.l2 || btn_down.r1 ||
        btn_down.r2 || btn_down.up || btn_down.right ||
        btn_down.down || btn_down.left || btn_down.circle ||
        btn_down.square
      ) {
        if (atk_state == BASIC) Handle_Basic(btn_down);
        else Handle_Taunt(btn_down);
      }
      // Else check if the stick movement is above a certain threshold
      else if (abs(lx) > 10 || abs(ly) > 10 || abs(rx) > 10 || abs(ry) > 10) {
        // Check which stick received the stronger signal
        if (abs(ry) + abs(rx) < abs(ly) + abs(lx)) {
          if (abs(ly) > abs(lx)) {
            if (ly < 0) Forward(350);
            else Backward(350);
          }
          else {
            if (lx < 0) Right(350);
            else Left(350);
          }
        }
        else {
          if (rx < 0) Sidestep_Left(250);
          else Sidestep_Right(250);
        }
      }
      // No input, idle
      else Idle();
    }
    else {
      switch (drive_state) {
        case STD:
          if (
              btn_down.l1 || btn_down.l2 || btn_down.r1 ||
              btn_down.r2 || btn_down.up || btn_down.right ||
              btn_down.down || btn_down.left || btn_down.circle ||
              btn_down.square ||
              abs(lx) > 10 || abs(ly) > 10 || abs(rx) > 10 || abs(ry) > 10
          ) {
            Handle_Basic(btn_down);

            // Check if any buttons were released, thus idling upper body
            ps3_button_t btn_released = Ps3.event.button_up;
            if (
              btn_released.l1 || btn_released.l2 || btn_released.r1 ||
              btn_released.r2 || btn_released.up || btn_released.right ||
              btn_released.down || btn_released.left || btn_released.circle ||
              btn_released.square
            ) {
              for (int i = 0; i < 7; i++) servos[i].write(gaucho_pos[i]);
            }

            // Handle Standard Driving
            if (abs(lx) > 10 || abs(ly) > 10 || abs(rx) > 10 || abs(ry) > 10) {
              // Check which stick received the stronger signal
              if (abs(ry) + abs(rx) < abs(ly) + abs(lx)) {
                if (abs(ly) > abs(lx)) {
                  if (ly < 0) Drive_Backward(abs(ly)*2);
                  else Drive_Forward(ly*2);
                }
                else {
                  if (lx < 0) Drive_Left(abs(lx)*2);
                  else Drive_Right(lx*2);
                }
              }
              else {
                Halt_Drive();
                if (rx < 0) Sidestep_Left(250);
                else Sidestep_Right(250);
              }
            }
            else Halt_Drive();
          }
          // No input, idle
          else Idle();
          break;
        case ADV:
          if (
              btn_down.l1 || btn_down.l2 || btn_down.r1 ||
              btn_down.r2 || btn_down.up || btn_down.right ||
              btn_down.down || btn_down.left || btn_down.circle ||
              btn_down.square ||
              abs(lx) > 10 || abs(ly) > 10 || abs(rx) > 10 || abs(ry) > 10
          ) {
            Handle_Basic(btn_down);

            // Check if any buttons were released, thus idling upper body
            ps3_button_t btn_released = Ps3.event.button_up;
            if (
              btn_released.l1 || btn_released.l2 || btn_released.r1 ||
              btn_released.r2 || btn_released.up || btn_released.right ||
              btn_released.down || btn_released.left || btn_released.circle ||
              btn_released.square
            ) {
              for (int i = 0; i < 7; i++) servos[i].write(gaucho_pos[i]);
            }

            // Handle Advance Driving
            if (abs(ly) > 10 || abs(ry) > 10) {
              unsigned ain1 = (ry < 0) ? LOW : HIGH;
              unsigned ain2 = (ry < 0) ? HIGH : LOW;
              unsigned bin1 = (ly < 0) ? LOW : HIGH;
              unsigned bin2 = (ly < 0) ? HIGH : LOW;
              Drive(ain1, ain2, abs(ry)*2, bin1, bin2, abs(ly)*2);
            }
            else Halt_Drive();
          }
          // No input, idle
          else Idle();
          break;
      }
    }
  }
}

void On_Connect() {
  lcd.setRotation(0);
  Ps3.setPlayer(2);
  Idle();
}

void setup() {
  // Motor Initialization
  pinMode(PWMA, OUTPUT);
  pinMode(AIN1, OUTPUT);
  pinMode(AIN2, OUTPUT);
  pinMode(BIN1, OUTPUT);
  pinMode(BIN2, OUTPUT);
  pinMode(PWMB, OUTPUT);

  // Servo Initialization
	for (int i = 0; i < 11; i++) servos[i].attach(servo_pins[i]);

  // Battery Monitoring Initialization
	lcd.begin(SSD1306_SWITCHCAPVCC, 0x3C);
	lcd.clearDisplay();
	pinMode(battery, INPUT);

	lcd.setRotation(1);
	lcd.setTextSize(1);
	lcd.setTextColor(SSD1306_WHITE);

	// Ps3 Initialization
	Ps3.attach(notify);
	Ps3.attachOnConnect(On_Connect);
	Ps3.begin("2c:81:58:3a:93:f7");

  // Animation Ramps Initialization
  t1_rb.go(-135);
  t1_rb.go(-45, 350, LINEAR, FORTHANDBACK);

  t2_rb.go(-135);
  t2_rb.go(-90, 350, LINEAR, FORTHANDBACK);
  t2_lb.go(135);
  t2_lb.go(90, 350, LINEAR, FORTHANDBACK);
  t2_w.go(-100);
  t2_w.go(80, 1500, LINEAR, FORTHANDBACK);

  t4_rs.go(0);
  t4_rs.go(100, 450, LINEAR, FORTHANDBACK);
  t4_ls.go(0);
  t4_ls.go(-100, 450, LINEAR, FORTHANDBACK);
  t4_rb.go(-135);
  t4_rb.go(0, 450, LINEAR, FORTHANDBACK);
  t4_lb.go(135);
  t4_lb.go(0, 450, LINEAR, FORTHANDBACK);
}

unsigned long init_timeout = 0;
unsigned long init_wait_val = 0;
void loop() {
  while(!Ps3.isConnected()) {
    unsigned long ms = millis();

    if (ms > init_timeout + 1000) {
      init_wait_val = (init_wait_val+1)%4;
      init_timeout = ms;
    }

    Waiting_To_Pair(init_wait_val);
  }

  Display_Voltage();
}