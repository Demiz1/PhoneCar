void initDislpay(){
  display.init();
  display.flipScreenVertically();
  display.setFont(ArialMT_Plain_10);
  displayAndSerialLog("Screen initiated");
}

void updateDisplay(){
  if(clientInControl > -1){
    screenMode = display_drive;
  }else{
    screenMode = display_Init;
  }
  display.clear();

  if(screenMode == display_Init){
    display.drawString(0,0,rows[0]);
    display.drawString(0,10,rows[1]);
    display.drawString(0,20,rows[2]);
    display.drawString(0,30,rows[3]);
    display.drawString(0,40,rows[4]);
  }else if(screenMode == display_drive){
    display.drawString(0,0,"Drive:");
    display.drawProgressBar(40,1,80,10,drive);
    display.drawString(0,11,"Turn:");
    display.drawProgressBar(40,10,80,10,turn);
    display.drawString(0,30,rows[3]);
    display.drawString(0,40,rows[4]);
  }
  display.display();
}

void loggScreenApend(String message){
  String newRows[maxRows];
  int rowCount = 0;
  String cuttedString = message;
  
  while(cuttedString.length()){
    int lengthCount = 0;
    int letterCount = 0;
    int cutPosition = 0;
    bool endOfString = false;
    for(int i = 0; i < cuttedString.length() && lengthCount < maxRowLength; i++){
      String mycurrentSubstring = cuttedString.substring(0,i);
      lengthCount = display.getStringWidth(mycurrentSubstring);
      if(cuttedString.charAt(i) == ' ') cutPosition = i+1;
      if(cuttedString.charAt(i) == '\n') lengthCount = maxRowLength;
      letterCount++;
      if(!(letterCount < cuttedString.length())) endOfString = true; //end of message
    }
    String newRow;
    if(cutPosition && !endOfString){
      newRow = cuttedString.substring(0, cutPosition);
      if(cuttedString.length() - cutPosition){
        cuttedString = cuttedString.substring(cutPosition, cuttedString.length());
      }else{
        cuttedString = "";
      }
    }else{
      newRow = cuttedString.substring(0, letterCount);
      if(cuttedString.length() - letterCount){
        cuttedString = cuttedString.substring(letterCount, cuttedString.length());
      }else{
        cuttedString = "";
      }
    }
    //Serial.println("The string is " + String(cuttedString.length()) + " long, lettercount is " + String(letterCount) + " ROW STRING :" + newRow); 
    
    newRows[rowCount % (maxRows-1)] = newRow;
    rowCount++;
  }
  int rowsToDisplay = rowCount % maxRows;
  //Serial.println("Done formating, now writing to display buffer; message was " + String(rowCount) + " rows long, displaying last " + String(rowsToDisplay) + " rows");
  //shifting data
  int rowsToKeep = maxRows - rowCount % maxRows;
  if(rowsToKeep){
    for(int i = 0;i< rowsToKeep;i++){
      rows[i] = rows[maxRows - rowsToKeep + i];
    }
  }
  //Serial.println("done shifting");
  for(int i = 0; i < (rowCount % maxRows);i++){
    //Serial.println("Adding String on index " + String(i + rowsToKeep) + " content is: " + newRows[i]);
    rows[i + rowsToKeep] = newRows[i];    
  }
  updateDisplay();
}

void loggScreenApend(const char* message){
  String newRows[maxRows];
  int rowCount = 0;
  String cuttedString = message;
  
  while(cuttedString.length()){
    int lengthCount = 0;
    int letterCount = 0;
    int cutPosition = 0;
    bool endOfString = false;
    for(int i = 0; i < cuttedString.length() && lengthCount < maxRowLength; i++){
      String mycurrentSubstring = cuttedString.substring(0,i);
      lengthCount = display.getStringWidth(mycurrentSubstring);
      if(cuttedString.charAt(i) == ' ') cutPosition = i+1;
      if(cuttedString.charAt(i) == '\n') lengthCount = maxRowLength;
      letterCount++;
      if(!(letterCount < cuttedString.length())) endOfString = true; //end of message
    }
    String newRow;
    if(cutPosition && !endOfString){
      newRow = cuttedString.substring(0, cutPosition);
      if(cuttedString.length() - cutPosition){
        cuttedString = cuttedString.substring(cutPosition, cuttedString.length());
      }else{
        cuttedString = "";
      }
    }else{
      newRow = cuttedString.substring(0, letterCount);
      if(cuttedString.length() - letterCount){
        cuttedString = cuttedString.substring(letterCount, cuttedString.length());
      }else{
        cuttedString = "";
      }
    }
    //Serial.println("The string is " + String(cuttedString.length()) + " long, lettercount is " + String(letterCount) + " ROW STRING :" + newRow); 
    
    newRows[rowCount % (maxRows-1)] = newRow;
    rowCount++;
  }
  int rowsToDisplay = rowCount % maxRows;
  //Serial.println("Done formating, now writing to display buffer; message was " + String(rowCount) + " rows long, displaying last " + String(rowsToDisplay) + " rows");
  //shifting data
  int rowsToKeep = maxRows - rowCount % maxRows;
  if(rowsToKeep){
    for(int i = 0;i< rowsToKeep;i++){
      rows[i] = rows[maxRows - rowsToKeep + i];
    }
  }
  //Serial.println("done shifting");
  for(int i = 0; i < (rowCount % maxRows);i++){
    //Serial.println("Adding String on index " + String(i + rowsToKeep) + " content is: " + newRows[i]);
    rows[i + rowsToKeep] = newRows[i];    
  }
  updateDisplay();
}
