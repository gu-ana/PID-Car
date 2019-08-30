// to get motor to turn, define 6 constants
//left
const int left_nslp_pin = 31; //digital write as long as this is high, then the other two commands would work
const int left_dir_pin = 29; //digital write
const int left_pwm_pin = 40; //analog write from 0,255
const int right_nslp_pin = 11; //as long as this is high, then the other two commands would work
const int right_dir_pin = 30; 
const int right_pwm_pin = 39;
//pins for the photoresistors
const int reflect0= 65;
const int reflect1= 48;
const int reflect2= 64;
const int reflect3= 47;
const int reflect4= 52;
const int reflect5= 68;
const int reflect6= 53;
const int reflect7= 69;
//pin for LED
const int LED = 51; //yellow
const int LED2 = 57; //Red black
//IR cameras
const int oddIR = 45;
const int evenIR = 61;
//stopCounter for the end
int stopCounter = 0;
//initial conditions
int leftSpeed = 70;
int rightSpeed = 70;
double TOTAL = 0;
int errorPrior = 0;

bool turn = false;
double position =0;
bool inSchoolZone = false;


void setup() {
  // put your setup code here, to run once:
Serial.begin(9600);
pinMode(left_nslp_pin, OUTPUT);
pinMode(right_nslp_pin, OUTPUT);
pinMode(left_dir_pin, OUTPUT);
pinMode(right_dir_pin, OUTPUT);
pinMode(left_pwm_pin,OUTPUT);
pinMode(right_pwm_pin,OUTPUT);
pinMode(reflect0, INPUT);
pinMode(reflect1, INPUT);
pinMode(reflect2, INPUT);
pinMode(reflect3, INPUT);
pinMode(reflect4, INPUT);
pinMode(reflect5, INPUT);
pinMode(reflect6, INPUT);
pinMode(reflect7, INPUT);
pinMode(oddIR, OUTPUT);
pinMode(evenIR, OUTPUT);
pinMode(LED, OUTPUT); 
pinMode(LED2,OUTPUT);
digitalWrite(LED,LOW);
digitalWrite(LED2,LOW);

analogWrite(left_pwm_pin, leftSpeed);
analogWrite(right_pwm_pin,rightSpeed);

}

void callReflectOutput()
{
  pinMode(reflect0, OUTPUT);
  pinMode(reflect1, OUTPUT);
  pinMode(reflect2, OUTPUT);
  pinMode(reflect3, OUTPUT);
  pinMode(reflect4, OUTPUT);
  pinMode(reflect5, OUTPUT);
  pinMode(reflect6, OUTPUT);
  pinMode(reflect7, OUTPUT);
}

void digitalWriteReflect()
{
  digitalWrite(reflect0, HIGH); 
  digitalWrite(reflect1, HIGH); 
  digitalWrite(reflect2, HIGH); 
  digitalWrite(reflect3, HIGH); 
  digitalWrite(reflect4, HIGH); 
  digitalWrite(reflect5, HIGH); 
  digitalWrite(reflect6, HIGH); 
  digitalWrite(reflect7, HIGH); 
}

void callReflectInput()
{
  pinMode(reflect0, INPUT);
  pinMode(reflect1, INPUT);
  pinMode(reflect2, INPUT);
  pinMode(reflect3, INPUT);
  pinMode(reflect4, INPUT);
  pinMode(reflect5, INPUT);
  pinMode(reflect6, INPUT);
  pinMode(reflect7, INPUT);
}

void motorControl(double position) {
  // get reflect values
  // compute current position
  // define Kp, Ki, and Kd
  double error = position;
  int Kp = 2, Kd = 38;
  int motorSpeed = (Kp*error) + (Kd*(error-errorPrior));
  
  errorPrior = error;

  int changeRightSpd = rightSpeed - motorSpeed;
  int changeLeftSpd = leftSpeed + motorSpeed;

  if (changeRightSpd > 255)
  {
    changeRightSpd = 255;
  }
  if ( changeRightSpd < 0)
  {
    changeRightSpd = 0;
  }
  if (changeLeftSpd > 255)
  {
    changeLeftSpd = 255;
  }
  if (changeLeftSpd < 0)
  {
    changeLeftSpd =0;
  }
  
  // write new speeds
  analogWrite(left_pwm_pin, changeLeftSpd);
  analogWrite(right_pwm_pin, changeRightSpd);
  
}

void pinStatus(int refFFR, int refFR, int refR,int refMidR,int refMidL,int refL,int refFL,int refFFL)
{
  if(refFFR && refFR && refR && refMidR && refMidL && refL && refFL && refFFL && digitalRead(left_nslp_pin))
  {
    stopCounter++;
  } 
}
double CalculateTotal(double FFR, double FR, double R, double midR, double midL, double L, double FL, double FFL)
{
  double total = 0;
  FFR = FFR * 4;
  FR = FR * 2;
  R = R * 1;
  midR = midR * .25;
  midL = midL * -.25;
  L = L * -1;
  FL = FL * -2;
  FFL = FFL * -4;

  total = FFR + FR + R + midR + midL + L + FL + FFL;
  return total;
}
void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(oddIR, HIGH);
  digitalWrite(evenIR, HIGH);
  //this is for the 8 direction sensors
  callReflectOutput();
  digitalWriteReflect();
  delayMicroseconds(5);
  callReflectInput();
  //delay(2);
  delayMicroseconds(1500);
  int refFFL = digitalRead(reflect7);
  int refFL = digitalRead(reflect6);
  int refL = digitalRead(reflect5);
  int refMidL = digitalRead(reflect4);
  int refMidR = digitalRead(reflect3);
  int refR = digitalRead(reflect2);
  int refFR = digitalRead(reflect1);
  int refFFR = digitalRead(reflect0); 
  pinStatus(refFFR, refFR, refR,refMidR,refMidL,refL,refFL,refFFL);
  //take an average of the last 5 positions recorded.
  int numTaken = 5;
  double errorArray[numTaken] = {0,0,0,0,0};
  for(int i =0; i < numTaken; i++)
  {
    double temp = CalculateTotal(refFFR,refFR,refR, refMidR, refMidL,refL, refFL, refFFL);
    errorArray[i] = temp;
  }
  for(int i =0; i< numTaken; i++)
  {
    position += errorArray[i];
  }
  position = position / numTaken; 
  
  motorControl(position);
  if (stopCounter == 1 && !turn)
    {
      digitalWrite(LED2, HIGH);
      digitalWrite(left_dir_pin,HIGH);
      analogWrite(left_pwm_pin,200);
      analogWrite(right_pwm_pin,200);
      //delayMicroseconds(7000);
      delay(250);
      turn = true;
      analogWrite(left_pwm_pin,0);
      analogWrite(right_pwm_pin,0);
      delayMicroseconds(100);
      digitalWrite(left_dir_pin,LOW);
      analogWrite(left_pwm_pin,200);
      analogWrite(right_pwm_pin,200);
      delay(20);
    }
  else if (stopCounter >= 2 && turn)
    {
      digitalWrite(LED2,LOW);
      digitalWrite(left_nslp_pin,LOW);
      digitalWrite(right_nslp_pin,LOW);
    }
  else
  {
  digitalWrite(left_nslp_pin, HIGH); //off
  digitalWrite(right_nslp_pin, HIGH); //off
  digitalWrite(left_dir_pin, LOW);
  digitalWrite(right_dir_pin,LOW);
  }

 
  

  }
  
