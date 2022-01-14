bool swapControl(uint8_t from_num, uint8_t to_num){
  //send notListening to 'from'
  if(clientInControl >=0){
    removeControl(from_num);
  }
  
  //send listening on, to 'to'
  if(giveControl(to_num))return true;
  return false;
}

bool giveControl(uint8_t num){
  char log[logMaxLength];
  sprintf(log,"Giving control to %d", num);
  
  displayAndSerialLog(log);
  if(caseCarWebSocket.sendTXT(num, "control gained")){
    clientInControl = num;
    verboseLogg("success");
    return true;
  }
  verboseLogg("failed");
  return false;
}

bool removeControl(uint8_t num){
  char log[logMaxLength];
  sprintf(log,"Removing control from %d", num);
  displayAndSerialLog(log);
  if(caseCarWebSocket.sendTXT(num, "control removed")){
    verboseLogg("success");
    return true;
  }
  clientInControl = -1;
  verboseLogg("failed");
  return false;
}
