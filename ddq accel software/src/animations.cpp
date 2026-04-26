#include <variables.h>

/*
  SERVO VARIABLES
*/

int servo_pins[] = { 15, 2, 4, 5, 18, 19, 13, 12, 14, 26, 27 };

Servo s_rs;
Servo s_rd;
Servo s_rb;

Servo s_ls;
Servo s_ld;
Servo s_lb;

Servo s_w;

Servo s_rh;
Servo s_rf;

Servo s_lh;
Servo s_lf;

Servo servos[] = { 
  s_rs, s_rd, s_rb, 
  s_ls, s_ld, s_lb, 
  s_w,
  s_rh, s_rf,
  s_lh, s_lf
};

int std_pos[] = {80, 6, 135, 100, 180, 45, 100, 90, 90, 100, 110};
int gaucho_pos[] = {80, 6, 135, 100, 180, 45, 100, 113, 113, 77, 87};
int crouch_pos[] = {80, 6, 135, 100, 180, 45, 100, 163, 163, 27, 37};

/*
    ANIMATION FUNCTIONS
*/

void Move(int servo, int delta, int* pos_arr) { servos[servo].write(pos_arr[servo]+delta); }

// Movement States
enum Drive_State drive_state = OFF;

enum Locomotion_State locomotion_state = GAUCHO;

enum Atk_State atk_state = BASIC;

void Fix_Rest(bool (*In_Use)(int)) {
  if (drive_state != OFF) {
    for (int i = 0; i < 6; i++) { if (!In_Use(i)) Move(i, 0, gaucho_pos); }
  }

  switch (locomotion_state) {
    case GAUCHO:
      for (int i = 0; i < 11; i++) { if (!In_Use(i)) Move(i, 0, gaucho_pos); }
      break;
    case CROUCHED:
      for (int i = 0; i < 11; i++) { if (!In_Use(i)) Move(i, 0, crouch_pos); }
      break;
  }
}

void Halt_Drive() {
  digitalWrite(AIN1, LOW);
	digitalWrite(AIN2, LOW);
	digitalWrite(BIN1, LOW);
	digitalWrite(BIN2, LOW);
}

void Idle() {
    switch (locomotion_state) {
        case CROUCHED:
           for (int i = 0; i < 11; i++) servos[i].write(crouch_pos[i]);
           break;
        case GAUCHO:
           for (int i = 0; i < 11; i++) servos[i].write(gaucho_pos[i]);
           break;
    }
    Halt_Drive();
}

bool In_Use_Feet(int i) { return i == lf || i == rf || i == w; }

void Left(int spd) {
    unsigned long delta = millis() % spd;
    if (delta < spd/2) {
        // Raise Body and Reorient
        Move(lf, -20, gaucho_pos);
        Move(rf, 20, gaucho_pos);
        Move(w, 0, gaucho_pos);
    }
    else {
        // Lower Body and Turn Left
        Move(lf, 0, gaucho_pos);
        Move(rf, 0, gaucho_pos);
        Move(w, 80, gaucho_pos);
    }

    // Fix Rest of Body
    Fix_Rest(In_Use_Feet);
}

void Right(int spd) {
  unsigned long delta = millis() % spd;
  if (delta < spd/2) {
    // Raise Body and Reorient
    Move(lf, -20, gaucho_pos);
    Move(rf, 20, gaucho_pos);
    Move(w, 0, gaucho_pos);
  }
  else {
    // Lower Body and Turn Right
    Move(lf, 0, gaucho_pos);
    Move(rf, 0, gaucho_pos);
    Move(w, -80, gaucho_pos);
  }

  // Fix Rest of Body
  Fix_Rest(In_Use_Feet);
}

bool In_Use_Sidestep(int i) { return (i == rh || i == rf || i == lh || i == lf); }

void Sidestep_Left(int spd) {
  unsigned long delta = millis() % spd;
  if (delta < spd/2) {
    // Thrust
    Move(rh, 20, std_pos);
    Move(rf, -20, std_pos);
    // Catch
    Move(lh, -20, std_pos);
    Move(lf, -20, std_pos);
  }
  else {
    // Reset
    Move(rh, 0, std_pos);
    Move(rf, 0, std_pos);
    Move(lh, 0, std_pos);
    Move(lf, 0, std_pos);
  }

  // Fix Rest of Body
  Fix_Rest(In_Use_Sidestep);
}

void Sidestep_Right(int spd) {
  unsigned long delta = millis() % spd;
  if (delta < spd/2) {
    // Thrust
    Move(lh, -20, std_pos);
    Move(lf, 20, std_pos);
    // Catch
    Move(rh, 20, std_pos);
    Move(rf, 20, std_pos);
  }
  else {
    // Reset 
    Move(lh, 0, std_pos);
    Move(lf, 0, std_pos);
    Move(rh, 0, std_pos);
    Move(rf, 0, std_pos);
  }

  // Fix Rest of Body
  Fix_Rest(In_Use_Sidestep);
}

bool In_Use_Walking(int i) { return (i == lf || i == rf || i == w || i == rh || i == lh); }
void Forward(int spd) {
  unsigned long delta = millis() % spd;

  if (delta < spd/2) {
    // Shift Mass Left and Rotate Left
    Move(lf, 25, std_pos);
    Move(rf, 25, std_pos);
    Move(lh, 0, std_pos);
    Move(rh, 0, std_pos);
    Move(w, 45, std_pos);
  }
  else {
    // Shift Mass Right and Rotate Right
    Move(lf, -25, std_pos);
    Move(rf, -25, std_pos);
    Move(lh, 0, std_pos);
    Move(rh, 0, std_pos);
    Move(w, -45, std_pos);
  }

  // Fix Rest of Body
  Fix_Rest(In_Use_Walking);
}

void Backward(int spd) {
  unsigned long delta = millis() % spd;

  if (delta < spd/2) {
    // Shift Mass Left and Rotate Right
    Move(lf, 25, std_pos);
    Move(rf, 25, std_pos);
    Move(lh, 0, std_pos);
    Move(rh, 0, std_pos);
    Move(w, -45, std_pos);
  }
  else {
    // Shift Mass Right and Rotate Left
    Move(lf, -25, std_pos);
    Move(rf, -25, std_pos);
    Move(lh, 0, std_pos);
    Move(rh, 0, std_pos);
    Move(w, 45, std_pos);
  }

  // Fix Rest of Body
  Fix_Rest(In_Use_Walking);
}

void Drive(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb) {
  // Raise body and drive
  Move(lf, -20, gaucho_pos);
  Move(rf, 20, gaucho_pos);

  digitalWrite(AIN1, ain1);
  digitalWrite(AIN2, ain2);
  analogWrite(PWMA, pwma);

  digitalWrite(BIN1, bin1);
  digitalWrite(BIN2, bin2);
  analogWrite(PWMB, pwmb);

  // Fix Hips
  Move(lh, 0, gaucho_pos);
  Move(rh, 0, gaucho_pos);
}

void Drive_Left(int spd) { Drive(LOW, HIGH, spd, HIGH, LOW, spd); }

void Drive_Right(int spd) { Drive(HIGH, LOW, spd, LOW, HIGH, spd); }

void Drive_Forward(int spd) { Drive(HIGH, LOW, spd, HIGH, LOW, spd); }

void Drive_Backward(int spd) { Drive(LOW, HIGH, spd, LOW, HIGH, spd); }

// Attack Functions

bool In_Use_Heavy(int i) { return (i == ld || i == lb || i == rd || i == rb || i == w); }

void Heavy_Right() {
  Move(ld, -90, gaucho_pos);
  Move(lb, 135, gaucho_pos);
  Move(rd, 90, gaucho_pos);
  Move(rb, -135, gaucho_pos);
  Move(w, 80, gaucho_pos);
  Fix_Rest(In_Use_Heavy);
}

void Heavy_Left() {
  Move(ld, -90, gaucho_pos);
  Move(lb, 135, gaucho_pos);
  Move(rd, 90, gaucho_pos);
  Move(rb, -135, gaucho_pos);
  Move(w, -100, gaucho_pos);
  Fix_Rest(In_Use_Heavy);
}

bool In_Use_Light_Right(int i) { return (i == w || i == rs || i == rd || i == rb); }
void Light_Right() {
  Move(w, 65, gaucho_pos);
  Move(rs, 100, gaucho_pos);
  Move(rd, 65, gaucho_pos);
  Move(rb, -135, gaucho_pos);
  Fix_Rest(In_Use_Light_Right);
}

bool In_Use_Light_Left(int i) { return (i == w || i == ls || i == ld || i == lb); }
void Light_Left() {
  Move(w, -65, gaucho_pos);
  Move(ls, -100, gaucho_pos);
  Move(ld, -65, gaucho_pos);
  Move(lb, 135, gaucho_pos);
  Fix_Rest(In_Use_Light_Left);
}

unsigned long right_uppercut_timeout = 0;
bool In_Use_Right_Uppercut(int i) { return (i == rb || i == w || i == rs); }
void Right_Uppercut() {
  if (millis() < right_uppercut_timeout + 350) {
    // Orient
    Move(w, 0, gaucho_pos);
    Move(rs, 0, gaucho_pos);
    Move(rb, -90, gaucho_pos);
    // Crouch if not in drive mode
    if (drive_state == OFF) locomotion_state = CROUCHED;
  }
  else {
    // Thrust
    Move(w, 45, gaucho_pos);
    Move(rs, 100, gaucho_pos);
    Move(rb, -45, gaucho_pos);
    // Stand if not in drive mode
    if (drive_state == OFF) locomotion_state = GAUCHO;
  }
  Fix_Rest(In_Use_Right_Uppercut);
}

unsigned long left_uppercut_timeout = 0;
bool In_Use_Left_Uppercut(int i) { return (i == lb || i == w || i == ls); }
void Left_Uppercut() {
  if (millis() < left_uppercut_timeout + 350) {
    // Orient
    Move(w, 0, gaucho_pos);
    Move(ls, 0, gaucho_pos);
    Move(lb, 90, gaucho_pos);
    // Crouch if not in drive mode
    if (drive_state == OFF) locomotion_state = CROUCHED;
  }
  else {
    // Thrust
    Move(w, -45, gaucho_pos);
    Move(ls, -100, gaucho_pos);
    Move(lb, 45, gaucho_pos);
    // Stand if not in drive mode
    if (drive_state == OFF) locomotion_state = GAUCHO;
  }
  Fix_Rest(In_Use_Left_Uppercut);
}

bool In_Use_Right_Shot(int i) { return (i == rb || i == rd); }
void Right_Shot() {
  Move(rb, -90, gaucho_pos);
  Move(rd, 90, gaucho_pos);
  Fix_Rest(In_Use_Right_Shot);
}

bool In_Use_Left_Shot(int i) { return (i == lb || i == ld); }
void Left_Shot() {
  Move(lb, 90, gaucho_pos);
  Move(ld, -90, gaucho_pos);
  Fix_Rest(In_Use_Left_Shot);
}

unsigned long thwack_timeout = 0;
bool In_Use_Thwack(int i) { return (i == rs || i == ls || i == rd || i == ld || i == w || i == rb || i == lb); }
void Thwack() {
    // Orient
    if (millis() < thwack_timeout + 350) {
        Move(rs, -80, gaucho_pos);
        Move(ls, 80, gaucho_pos);
        Move(rd, 90, gaucho_pos);
        Move(ld, -90, gaucho_pos);
        Move(w, -100, gaucho_pos);
        Move(rb, -45, gaucho_pos);
        Move(lb, 45, gaucho_pos);
    }
    // Thrust
    else {
        Move(rs, -80, gaucho_pos);
        Move(ls, 80, gaucho_pos);
        Move(rd, 90, gaucho_pos);
        Move(ld, -90, gaucho_pos);
        Move(w, -100, gaucho_pos);
        Move(rb, -135, gaucho_pos);
        Move(lb, 135, gaucho_pos);
    }
    Fix_Rest(In_Use_Thwack);
}

// Taunt Functions
rampInt t1_rb;
bool In_Use_t1(int i) { return (i == w || i == rs || i == rd || i == rb || i == ls || i == ld || i == lb); }
void I_CHALLENGE_YOU() {
  Move(w, 65, gaucho_pos);
  Move(rs, 80, gaucho_pos);
  Move(rd, 65, gaucho_pos);
  Move(rb, t1_rb.update(), gaucho_pos);
  Move(ls, -45, gaucho_pos);
  Move(ld, -45, gaucho_pos);
  Move(lb, 45, gaucho_pos);
  Fix_Rest(In_Use_t1);
}

rampInt t2_rb;
rampInt t2_lb;
rampInt t2_w;
bool In_Use_t2(int i) { return (i == rs || i == ls || i == rd || i == ld || i == rb || i == lb || i == w); };
void REVEL() {
    Move(rs, 100, gaucho_pos);
    Move(ls, -100, gaucho_pos);
    Move(rd, 90, gaucho_pos);
    Move(ld, -90, gaucho_pos);
    Move(rb, t2_rb.update(), gaucho_pos);
    Move(lb, t2_lb.update(), gaucho_pos);
    Move(w, t2_w.update(), gaucho_pos);
    Fix_Rest(In_Use_t2);
}

unsigned long t3_timeout = 0;
bool In_Use_t3(int i) { return (i == rs || i == rb || i == ld || i == lb); }
void FOR_DULCINEA() {
    if (millis() < t3_timeout + 500) {
        locomotion_state = CROUCHED;
        Idle();
    }
    else {
        locomotion_state = GAUCHO;
        Move(rs, 100, gaucho_pos);
        Move(rb, -45, gaucho_pos);
        Move(ld, -45, gaucho_pos);
        Move(lb, 135, gaucho_pos);
        Fix_Rest(In_Use_t3);
    }
}

rampInt t4_rs;
rampInt t4_ls;
rampInt t4_rb;
rampInt t4_lb;
bool In_Use_t4(int i) { return (i == ld || i == rd || i == rs || i == ls || i == rb || i == lb); }
void TO_RIGHT_THE_UNRIGHTABLE_WRONG() {
    Move(ld, -45, gaucho_pos);
    Move(rd, 45, gaucho_pos);
    Move(rs, t4_rs.update(), gaucho_pos);
    Move(ls, t4_ls.update(), gaucho_pos);
    Move(rb, t4_rb.update(), gaucho_pos);
    Move(lb, t4_lb.update(), gaucho_pos);
    Fix_Rest(In_Use_t4);
}