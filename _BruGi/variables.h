
/*************************/
/* Config Structure      */
/*************************/

struct config_t
{
  
  int32_t gyroPitchKp; 
  int32_t gyroPitchKi;   
  int32_t gyroPitchKd;
  int32_t gyroRollKp;
  int32_t gyroRollKi;
  int32_t gyroRollKd;
  int16_t accTimeConstant;
  int16_t angleOffsetPitch;   // angle offset, deg*100
  int16_t angleOffsetRoll;
  int8_t dirMotorPitch;
  int8_t dirMotorRoll;
  uint8_t motorNumberPitch;
  uint8_t motorNumberRoll;
  uint8_t maxPWMmotorPitch;
  uint8_t maxPWMmotorRoll;
  uint16_t refVoltageBat;    // Ubat reference, unit = volts*100
  uint16_t cutoffVoltage;    // Ubat cutoff, unit = volts*100
  bool motorPowerScale;
  bool rcAbsolutePitch;
  bool rcAbsoluteRoll;
  int8_t maxRCPitch;
  int8_t maxRCRoll;
  int8_t minRCPitch;
  int8_t minRCRoll;
  int16_t rcGainPitch;
  int16_t rcGainRoll;
  int16_t rcLPFPitch;        // low pass filter for RC absolute mode, units=1/10 sec
  int16_t rcLPFRoll;         
  bool rcModePPMPitch;       // RC mode, true=common RC PPM channel, false=separate RC channels 
  bool rcModePPMRoll;
  bool rcModePPMAux;
  bool rcModePPMFpvPitch;
  bool rcModePPMFpvRoll;

  uint8_t rcPinModeCH0;      // 0=Channel OFF, 1=use digital PWM/PPM control, 2=use analog control
  uint8_t rcPinModeCH1;      // 
  uint8_t rcPinModeCH2;      // 
 
  int8_t rcChannelPitch;     // input channel for pitch
  int8_t rcChannelRoll;      // input channel for roll
  int8_t rcChannelAux;       // input channel for auxiliary functions
  int8_t rcChannelFpvPitch;  // input channel for fpv channel pitch
  int8_t rcChannelFpvRoll;   // input channel for fpv channel roll
  
  int8_t fpvGainPitch;       // gain of FPV channel pitch
  int8_t fpvGainRoll;        // gain of FPV channel roll
  
  int16_t rcLPFPitchFpv;     // low pass filter in FPV mode
  int16_t rcLPFRollFpv;      // low pass filter in FPV mode
  
  int16_t rcMid;             // rc channel center ms
  
   
  bool enableGyro;           // enable gyro attitude update
  bool enableACC;            // enable acc attitude update
  bool axisReverseZ;
  bool axisSwapXY;
  
  bool fpvFreezePitch;
  bool fpvFreezeRoll;
  
  uint8_t maxPWMfpvPitch;    // motor PWM power in FPV freeze mode
  uint8_t maxPWMfpvRoll;
  
  int8_t fpvSwPitch;         // fpv switch pitch: -1=always on, 0=off, 1=auxSW1, 2=auxSW2 
  int8_t fpvSwRoll;          // fpv switch roll: -1=alwas on, 0=off, 1=auxSW1, 2=auxSW2 
  int8_t altSwAccTime;       // switch alternate Acc time: -1=always on, 0=off, 1=auxSW1, 2=auxSW2
  int16_t accTimeConstant2;  // alternate constant
  
  bool gyroCal;              // gyro calibration at startup 
  int16_t  gyrOffsetX;       // gyyro calibration offsets
  int16_t  gyrOffsetY;
  int16_t  gyrOffsetZ;
  
  int16_t  accOffsetX;       // acc calibration offsets
  int16_t  accOffsetY;
  int16_t  accOffsetZ;
  
  uint8_t crc8;
} config;

void setDefaultParameters()
{
 
 
  config.dirMotorPitch = 1;
  config.dirMotorRoll = -1;
 

  config.maxPWMmotorPitch = 80;
  config.maxPWMmotorRoll = 80;
  config.refVoltageBat = 800;
  config.cutoffVoltage = 600;
  config.motorPowerScale = 0;
 
  config.maxPWMfpvPitch=80;
  config.maxPWMfpvRoll=80;

  config.crc8 = 0;  
}


typedef struct PIDdata {
  int32_t   Kp, Ki, Kd;
} PIDdata_t;

PIDdata_t pitchPIDpar,rollPIDpar;







/*************************/
/* Variables             */
/*************************/



// motor drive

int8_t pwmSinMotor[256];

int currentStepMotor0 = 0;
int currentStepMotor1 = 0;
bool motorUpdate = false; 

int8_t pitchDirection = 1;
int8_t rollDirection = 1;

uint8_t freqCounter = 0;

int pitchMotorDrive = 0;
int rollMotorDrive = 0;

// control motor update in ISR
bool enableMotorUpdates = false;

uint8_t pwm_a_motor0 = 128;
uint8_t pwm_b_motor0 = 128;
uint8_t pwm_c_motor0 = 128;



// battery voltage
float voltageBat = 0;
float uBatValue_f = 0;


//scaled Motor Power
uint16_t maxPWMmotorPitchScaled;
uint16_t maxPWMmotorRollScaled;

// Variables for MPU6050
float gyroPitch;
float gyroRoll; //in deg/s

float resolutionDevider;
int16_t x_val;
int16_t y_val;
int16_t z_val;

float PitchPhiSet = 0;
float RollPhiSet = 0;
static float pitchAngleSet=0;
static float rollAngleSet=0;

static float qLPPitch[3] = {0,0,0};
static float qLPRoll[3] = {0,0,0};


int count=0;

// RC control

struct rcData_t
{
 uint32_t microsRisingEdge;
 uint32_t microsLastUpdate;
 uint16_t rx;
 uint16_t rx1;
 bool     update;
 bool     valid;
 float    rcSpeed;
 float    setpoint;
 bool     rcAuxSwitch1;
 bool     rcAuxSwitch2;
};



float rcLPFPitch_tc = 1.0;
float rcLPFRoll_tc = 1.0;

float rcLPFPitchFpv_tc = 1.0;
float rcLPFRollFpv_tc = 1.0;

// Gimbal State
enum gimStateType {
 GIM_IDLE=0,      // no PID
 GIM_UNLOCKED,    // PID on, fast ACC
 GIM_LOCKED,      // PID on, slow ACC
 GIM_ERROR        // error condition
};

gimStateType gimState = GIM_IDLE;
int stateCount = 0;

// rc fpv mode
bool fpvModePitch = false;
bool fpvModeRoll = false;

bool fpvModeFreezePitch = false;
bool fpvModeFreezeRoll = false;

// rc alternate ACC time
bool altModeAccTime = false;

//*************************************
//
//  IMU
//
//*************************************
enum axisDef {
  ROLL,
  PITCH,
  YAW
};

typedef struct fp_vector {
  float X;
  float Y;
  float Z;
} t_fp_vector_def;

typedef union {
  float   A[3];
  t_fp_vector_def V;
} t_fp_vector;



//********************
// sensor orientation
//********************
typedef struct sensorAxisDef {
  char idx;
  int  dir;
} t_sensorAxisDef;

typedef struct sensorOrientationDef {
  t_sensorAxisDef Gyro[3];
  t_sensorAxisDef Acc[3];
} t_sensorOrientationDef;

t_sensorOrientationDef sensorDef = { 
    {{0, 1}, {1, 1}, {2, 1}},    // Gyro
    {{0, 1}, {1, 1}, {2, 1}}     // Acc
  };


static float gyroScale=0;

static int16_t gyroADC[3];
static int16_t accADC[3];

static t_fp_vector EstG;

static float accLPF[3] = {0.0,0.0,0.0};
static float accMag = 0;
static bool disableAccGtest = false;

static float AccComplFilterConst = 0;  // filter constant for complementary filter

static int16_t acc_25deg = 25;      //** TODO: check

static int32_t angle[2]    = {0,0};  // absolute angle inclination in multiple of 0.01 degree    180 deg = 18000

static float angleOffsetRoll_f = 0;
static float angleOffsetPitch_f = 0;

static int32_t angleOffsetRoll = 0;
static int32_t angleOffsetPitch = 0;


// DEBUG only
uint32_t stackTop = 0xffffffff;
uint32_t stackBottom = 0;

uint32_t heapTop = 0;
uint32_t heapBottom = 0xffffffff;

