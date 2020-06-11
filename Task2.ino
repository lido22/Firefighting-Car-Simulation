unsigned long prevMillis = 0, alarmCallingTime, motorStepTime=-5;
char curStep = 0, rightMotor[] = {4, 1, 3, 0}, leftMotor[]={8, 6, 7, 5}, fanMotor[]={A0, A2, A1, A3}, dir=0, curBackStep = 15, numTurning=0; //0=front, 1=right, 2=left, 3=back;
bool ledState = false, fanOn = false;
bool motorSteps[] = {HIGH, LOW, LOW, LOW, LOW, LOW, HIGH, LOW, LOW, HIGH, LOW, LOW, LOW, LOW, LOW, HIGH};
void setup(){
  DDRD = B11111011; //0 => 7
  DDRC = B00001111;
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(8, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(2), operateFanMotor, CHANGE);
}

void loop() {
  if(digitalRead(A5)){
    if(dir == 0 || dir == 3){
      if(detectObstacles(9) == false){
        if(detectObstacles(11) == false){
          if(detectObstacles(10) == false){
            dir = 3;
          }
          else{
            dir = 2;
          }
        }
        else{
          dir = 1;
        }
      }
      else{
        dir = 0;
      }
    }
    unsigned long curMillis = millis();
    if(curMillis - prevMillis >= 1000){
      ledState = !ledState;
      digitalWrite(13, ledState);
      prevMillis = millis();
    }
    if((millis()-motorStepTime>=31)){
      if(dir == 3 && !fanOn){
        moveBack();
      }
      else{
        moveForward(dir);
        if(numTurning == 64){
          dir = 0;
          numTurning =0;
        }
      }
    }
  }
}
void operateFanMotor(){
  if(digitalRead(2)){
    alarmCallingTime = millis();
    fanOn=false;
  }
  else{
    if(millis() - alarmCallingTime >=500 && !fanOn){
      fanOn = true;
      dir = 0;
    }
  }
}
void moveForward(int dir){
  if(dir == 1 || dir == 2){
    numTurning++;
  }
  if(curStep == 16){
    curStep = 0;
  }
  char i = 0;
  while(i<4){
    if(fanOn == false){
      if(dir!=2){
        digitalWrite(leftMotor[i], motorSteps[curStep]);
      }
      if(dir!=1){
        digitalWrite(rightMotor[i], motorSteps[curStep]);
      }
    }
    else{
        digitalWrite(fanMotor[i], motorSteps[curStep]);
    }
    i++;
    curStep++;
  }
  motorStepTime = millis();
  
}
void moveBack(){
  if(curBackStep == -1){
    curBackStep = 15;
  }
  if(fanOn == false){
    char i=3;
    while(i>=0){
      digitalWrite(leftMotor[i], motorSteps[curBackStep]);
      digitalWrite(rightMotor[i], motorSteps[curBackStep]);
      i--;
      curBackStep--;
    }
  } 
  motorStepTime = millis();
}
bool detectObstacles(char pinNum){
  unsigned long sonicTime;
  int distance;
  bool done = false;
  // Sets the trigPin on HIGH state for 10 micro seconds
  digitalWrite(12, HIGH);
  sonicTime = micros();
  while(!done){
    if(micros()-sonicTime >= 10){
      digitalWrite(12, LOW);
      distance = pulseIn(pinNum, HIGH, 2000)*0.017;    
      done = true; 
    }
  }
  return distance<=20 && distance!=0;
}
