void carInit(){
  pinMode(steringServoPin, OUTPUT);
  pinMode(driveServoPin, OUTPUT);
  driveServo.attach(driveServoPin);
  turnServo.attach(steringServoPin);
  carUpdateInterupt.attach_ms(slowestUpdateTime, updateCarMovement);
  displayAndSerialLog("car initiated");
}

void ICACHE_RAM_ATTR updateCarMovement(){
  if(newValuesTime > lastestCarWriteTime){
    //there is a new drive comand, update the car!
    writeToCar(drive,turn);
    lastestCarWriteTime = millis();
  }else if(millis() > lastestCarWriteTime + slowestUpdateTime){
    //we havent gotten a new drive comand for 50ms, stop the car!
    writeToCar(50,50);
    drive = 50;
    turn = 50;
  }
}

void writeToCar(int driveInput, int turnInput){
  map(driveInput, 0, 100, 0, 180);
  map(turnInput, 0, 100, 0, 180);
  driveServo.write(driveInput);
  turnServo.write(turnInput);
}
