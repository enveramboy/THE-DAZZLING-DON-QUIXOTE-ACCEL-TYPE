#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Servo.h>
#include <Ramp.h>
#include <Ps3Controller.h>

/*
  BATTERY MONITORING VARIABLES
*/

#define battery 35
#define K 100
extern float readings[K];
extern int reading_idx;
extern Adafruit_SSD1306 lcd;

/*
  BATTERY DISPLAY FUNCTIONS
*/


/**
 * @brief Displays current voltage on connected OLED display.
 * 
 * Reads the current voltage of the battery
 * and depending on which bin it lands in, displays a graphic
 * corresponding to charge on the OLED display.
 * [3050, 4095]: full charge graphic,
 * [2800, 3050): two bar charge graphic,
 * [2550, 2800): one bar charge graphic (blinking),
 * [0, 2550): empty charge graphic (blinking).
 * Filtering of size K is used, such that readings are 
 * stored on an array of size K and the average of the
 * array is taken as the output value.
*/
void Display_Voltage();

/**
 * @brief Animate pairing message on connected OLED display.
 * 
 * Prints to the OLED display "Waiting to pair"
 * followed by an amount of periods equal to the passed in 
 * phase value.
 * 
 * @param phase The input phase (0-3)
*/
void Waiting_To_Pair(int phase);

// Animation Variables

/*
    MOTOR VARIABLES
*/

#define PWMA 32
#define AIN1 33
#define AIN2 25
#define BIN1 16
#define BIN2 23
#define PWMB 17

/*
    SERVO VARIABLES
*/

#define rs 0
#define rd 1
#define rb 2

#define ls 3
#define ld 4
#define lb 5

#define w 6

#define rh 7
#define rf 8
#define lh 9
#define lf 10

extern Servo s_rs;
extern Servo s_rd;
extern Servo s_rb;

extern Servo s_ls;
extern Servo s_ld;
extern Servo s_lb;

extern Servo s_w;

extern Servo s_rh;
extern Servo s_rf;

extern Servo s_lh;
extern Servo s_lf;

extern Servo servos[11];

extern int servo_pins[];

extern int std_pos[11];
extern int gaucho_pos[11];
extern int crouch_pos[11];

/*
    ANIMATION FUNCTIONS
*/

/**
 * @brief Move servo.
 * 
 * Orient servo delta degrees from a reference position.
 * 
 * @param servo numerical value assigned to servo for indexing.
 * @param delta displacement from reference in degrees.
 * @param pos_arr array of reference angles.
*/
void Move(int servo , int delta, int* pos_arr);

enum Drive_State {
  STD,
  ADV,
  OFF
};

extern enum Drive_State drive_state;

enum Locomotion_State {
    GAUCHO,
    CROUCHED
};

extern enum Locomotion_State locomotion_state;

enum Atk_State {
  BASIC,
  TAUNT
};

extern enum Atk_State atk_state;

/**
 * @brief Fix the rest of body excluding used servos.
 * 
 * Takes a In_Use function, which checks if the currently
 * accessed servo is a used servo, if so returns true.
 * 
 * @param In_Use function to check if servo is in use.
*/
void Fix_Rest(bool (*In_Use)(int));

/**
 * @brief Disable Motors
 * 
 * Writes low to each of the motor terminals.
*/
void Halt_Drive();

/**
 * @brief Idle machine.
 * 
 * Set servo positions to either crouched or resting orientations,
 * then write low to dc motor terminals to halt movement.
*/
void Idle();

/**
 * @brief Turn left via legs.
 * 
 * Turn in place left via a two beat pattern. 
 * 
 * First beat raises the body via the feet 
 * and returns the waist to the original 
 * position. 
 * 
 * Second beat lowers the body to the default
 * position while turning the waist left.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Left(int spd);

/**
 * @brief Turn right via legs.
 * 
 * Turn in place right via a two beat pattern. 
 * 
 * First beat raises the body via the feet 
 * and returns the waist to the original 
 * position. 
 * 
 * Second beat lowers the body to the default
 * position while turning the waist right.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Right(int spd);

/**
 * @brief Move left via legs.
 * 
 * Move left via a two beat pattern.
 * 
 * First beat moves the right hip and foot
 * outward and upward, thrusting the body in
 * the left direction. The left hip and foot
 * are extended outward as well but to stabilize
 * the body.
 * 
 * Second beat returns the body to default position.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Sidestep_Left(int spd);

/**
 * @brief Move right via legs.
 * 
 * Move right via a two beat pattern.
 * 
 * First beat moves the left hip and foot
 * outward and upward, thrusting the body in
 * the right direction. The right hip and foot
 * are extended outward as well but to stabilize
 * the body.
 * 
 * Second beat returns the body to default position.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Sidestep_Right(int spd);

/**
 * @brief Move forward via legs.
 * 
 * Move forward via a two beat pattern.
 * 
 * First beat shifts the center of mass towards the left
 * via the feet, then rotates the waist left.
 * 
 * Second beat shifts the center of mass towards the right
 * via the feet,  then rotates the waist right.
 * 
 * Thus forward movement is achieved by turning the waist
 * towards where the center of mass is currently shifted to.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Forward(int spd);

/**
 * @brief Move backward via legs.
 * 
 * Move backward via a two beat pattern.
 * 
 * First beat shifts the center of mass towards the left
 * via the feet, then rotates the waist right.
 * 
 * Second beat shifts the center of mass towards the right
 * via the feet,  then rotates the waist left.
 * 
 * Thus backward movement is achieved by turning the waist
 * away from where the center of mass is currently shifted to.
 * 
 * @param spd The speed in milliseconds to cycle 
 * through all the phases.
*/
void Backward(int spd);

/**
 * @brief General drive function
 * 
 * Turn the inners of the feet inward in order for wheels to contact ground.
 * Leave upper body unfixed.
 * 
 * @param ain1 AIN1 value (Right +).
 * @param ain2 AIN2 value (Right -).
 * @param pwma Right motor pwm.
 * @param bin1 BIN1 value (Left +)
 * @param bin2 BIN2 value (Left -).
 * @param pwmb Left motor pwm.
*/
void Drive(int ain1, int ain2, int pwma, int bin1, int bin2, int pwmb);

/**
 * @brief Turn left via wheels.
 * 
 * Turn left utilizing differential drive. Turn the 
 * inners of the feet inward in order for wheels to 
 * contact ground.
 * 
 * @param spd PWM signal to send to motors (0, 256)
*/
void Drive_Left(int spd);

/**
 * @brief Turn right via wheels.
 * 
 * Turn right utilizing differential drive. Turn the 
 * inners of the feet inward in order for wheels to 
 * contact ground.
 * 
 * @param spd PWM signal to send to motors (0, 256)
*/
void Drive_Right(int spd);

/**
 * @brief Move forward via wheels.
 * 
 * Move forward by driving the wheels forward.
 * Turn the inners of the feet inward in order 
 * for wheels to contact ground.
 * 
 * @param spd PWM signal to send to motors (0, 256)
*/
void Drive_Forward(int spd);

/**
 * @brief Move backward via wheels.
 * 
 * Move backward by driving the wheels backward.
 * Turn the inners of the feet inward in order 
 * for wheels to contact ground.
 * 
 * @param spd PWM signal to send to motors (0, 256)
*/
void Drive_Backward(int spd);

/**
 * @brief Wide right attack.
 * 
 * Extends both arms out and turn waist left.
*/
void Heavy_Right();

/**
 * @brief Wide left attack.
 * 
 * Extends both arms out and turn waist right.
*/
void Heavy_Left();

/**
 * @brief Light right attack.
 * 
 * Extend right arm forward in a punch.
*/
void Light_Right();

/**
 * @brief Light left attack.
 * 
 * Extend left arm forward in a punch.
*/
void Light_Left();

extern unsigned long right_uppercut_timeout;
/**
 * @brief Right uppercut attack.
 * 
 * Orient right arm downward (350ms) then thrust upward. 
 * If not in drive mode also thrust legs for added power.
*/
void Right_Uppercut();

extern unsigned long left_uppercut_timeout;
/**
 * @brief Left uppercut attack.
 * 
 * Orient left arm downward (350ms) then thrust upward. 
 * If not in drive mode also thrust legs for added power.
*/
void Left_Uppercut();

/**
 * @brief Right side attack.
*/
void Right_Shot();

/**
 * @brief Left side attack.
*/
void Left_Shot();

extern unsigned long thwack_timeout;
/**
 * @brief Push attack.
 * 
 * Turn waist right, orient arms downward then sweep them upward.
*/
void Thwack();

extern rampInt t1_rb;
/**
 * @brief Beckon the opposing knight forward.
 * 
 * Orient arm out and oscillate it in a way that appears as if it is calling
 * the opponent forward.
*/
void I_CHALLENGE_YOU();

extern rampInt t2_rb;
extern rampInt t2_lb;
extern rampInt t2_w;
/**
 * @brief Soak in the crowd's cheers
 * 
 * Extend arms out then slowly bring them inward while rotating around waist.
*/
void REVEL();

extern unsigned long t3_timeout;
/**
 * @brief Thrust fist in air in triumph.
 * 
 * Move bicep to form 90 degree angle with arm.
 * Move from crouch to gaucho for extra oomph.
*/
void FOR_DULCINEA();

extern rampInt t4_rs;
extern rampInt t4_ls;
extern rampInt t4_rb;
extern rampInt t4_lb;
/**
 * @brief Use both arms to beckon the wrongs forward.
 * 
 * Oscillate between arms at side to arms out in front.
*/
void TO_RIGHT_THE_UNRIGHTABLE_WRONG();