//here we write all functions for the wifimodule

void initWifi(){
  //we need to start the AP and set up the webserver to allow for connections
  if(initAP() && initwebServer() && initsocketServer()){
      displayAndSerialLog("WebInterface successfully Started");
  }
}

bool initAP(){
  
  //scan for netowrks if there is going to be conflicting ssid
  WiFi.scanDelete();
  verboseLogg("Scanning networks");
  WiFi.scanNetworks(true, false);
  verboseLogg("scanning...");
  unsigned long currentMillis = millis();
  while(WiFi.scanComplete() < 0 && millis() > currentMillis + 100){
    currentMillis = millis();
  }
  char log[logMaxLength];
  snprintf(log,logMaxLength, "found %d networks" , (WiFi.scanComplete() +1));
  verboseLogg(log);
  int ssidCount = 0;
  for(int i = 0; i < WiFi.scanComplete(); i++){
    if(ssidCount){
      if(WiFi.SSID(i) == ssid + String(ssidCount)) ssidCount++;
    }else{
      if(WiFi.SSID(i) == ssid) ssidCount++;
    }
  }
  String currentSsid;
  if(ssidCount){
    currentSsid = ssid + String(ssidCount);
  }else{
    currentSsid = ssid;
  }
  
  char ssidArr[currentSsid.length() +1];
  currentSsid.toCharArray(ssidArr, currentSsid.length() +1);
  sprintf(log,"The ssid will be %s", ssidArr); 
  verboseLogg(log);
  
  if(WiFi.getMode() != WIFI_AP){
    verboseLogg("changing wifi mode");
    WiFi.mode(WIFI_AP);
    while(WiFi.getMode() != WIFI_AP) {
     verboseLogg(" ...");
      delay(5);
    }
  }
  IPAddress local_ip(192,168,1,1);
  IPAddress gateway(192,168,1,0);
  IPAddress subnet(255,255,255,0);
  // 
  if(WiFi.softAP(currentSsid) && WiFi.softAPConfig(local_ip,gateway,subnet)){
    displayAndSerialLog("AP is started with ssid " + currentSsid + " with ip " + WiFi.softAPIP().toString());
    return true;
  }else{
    displayAndSerialLog("failed to start AP with ssid " + currentSsid);
  }
  return false;
}

bool initwebServer(){

  caseCarWebServer.getServer().setRSACert(new BearSSL::X509List(serverCert), new BearSSL::PrivateKey(serverKey));
  
  
  caseCarWebServer.onNotFound(notFoundFunction);

  //add captive portals here https://github.com/tzapu/WiFiManager
  caseCarWebServer.on("/carAdmin", carControlAdminWebpage);
  caseCarWebServer.on("/", carControlWebsite);
  caseCarWebServer.on("/teapot", teaPotWebpage);
  verboseLogg("Webpage handelers added");
  caseCarWebServer.begin();
  verboseLogg("WebServer started");
  
  return true;
}

void notFoundFunction(){
  //build webpage for this
  char log[logMaxLength];
  IPAddress clientIp = caseCarWebServer.client().remoteIP();
  snprintf(log,logMaxLength, "webClient: %d.%d.%d.%d No URL", clientIp[0], clientIp[1], clientIp[2], clientIp[3]);
  verboseLogg(log);
  caseCarWebServer.send(404, "text/plain", "sorry no webpage on this url");
}

void teaPotWebpage(){
  //build webpage for this
  char log[logMaxLength];
  IPAddress clientIp = caseCarWebServer.client().remoteIP();
  snprintf(log,logMaxLength, "webClient: %d.%d.%d.%d tea-pot reques", clientIp[0], clientIp[1], clientIp[2], clientIp[3]);
  verboseLogg(log);
  caseCarWebServer.send(418, "text/plain", "Im a actually a teapot, not a car, shhh");
}

void carControlWebsite(){
  char log[logMaxLength];
  IPAddress clientIp = caseCarWebServer.client().remoteIP();
  snprintf(log,logMaxLength, "webClient: %d.%d.%d.%d carControl request", clientIp[0], clientIp[1], clientIp[2], clientIp[3]);
  verboseLogg(log);
  caseCarWebServer.send(200, "text/html", carControlWebPage);
}

void carControlAdminWebpage(){
  char log[logMaxLength];
  IPAddress clientIp = caseCarWebServer.client().remoteIP();
  snprintf(log,logMaxLength, "webClient: %d.%d.%d.%d admin page request", clientIp[0], clientIp[1], clientIp[2], clientIp[3]);
  verboseLogg(log);
  caseCarWebServer.send(200, "text/html", carControlAdminWebPage);
}

bool initsocketServer(){
  caseCarWebSocket.begin();
  caseCarWebSocket.onEvent(caseCarWebSocketEvent);
  return true;
}

void caseCarWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      displayAndSerialLog("Client " + String(num) + " disconnected");
      if(clientInControl == num){
        if(num != admin){
          swapControl(clientInControl, admin);
        }else{
          removeControl(num);
        }
      }
      if(num == admin) admin = -1;
      break;
    case WStype_CONNECTED:
      displayAndSerialLog("New client connected with id " + String(num));
      caseCarWebSocket.sendTXT(num, "client id " + String(num));
      break;
    case WStype_TEXT: 
      char log[logMaxLength];
      //displayAndSerialLog("Message recived");
      snprintf(log,logMaxLength, "From client %d Recived %s", num, payload);
      //verboseLogg(log);
  
      char comp1[] = "takeControl";
      char comp2[] = "wantToDrive";
      char comp3[] = "dontWantToDrive";
      char comp4[] = "giveControl";
      char comp5[] = "iAmAdmin";
      char comp6[] = "whoIsInControl";
      char comp7[] = "whoIsAdmin";
      char comp8[] = "driveComand"; //"driveComandD=XXXT=XXX"

      if(strncmp((char*)payload, comp8, strlen(comp8)) == 0){
        if(strlen((const char*)payload) == 21){
          if(isDigit(payload[strlen(comp8) + 2]) && isDigit(payload[strlen(comp8) +7])){      
            int localdrive = atoi((const char*) payload + strlen(comp8) + 2);
            int localturn = atoi((const char*) payload + strlen(comp8) + 7);
            if(0 <= localdrive <= 100 && 0<= localturn <= 100){
              //drive comand is ok.
              drive = localdrive;
              turn = localturn;
              newValuesTime = millis();
            }
          }
        }
      } else if(strncmp((char*)payload, comp2, strlen(comp2)) == 0){
        //match
        driveRequests[num] = true;
        snprintf(log,logMaxLength, "Client %d Wants to drive", num);
        displayAndSerialLog(log);
        caseCarWebSocket.sendTXT(num, "wantToDrive=1");
      } else if(strncmp((char*)payload, comp3, strlen(comp3)) == 0){
        //match
        if(clientInControl == num){
          //the user is the one driving the car, give control to admin
          if(admin > -1){
            swapControl(clientInControl, admin);
          }else{
            //admin not present, remove control
            removeControl(num);
          }
        }
        driveRequests[num] = false;
        caseCarWebSocket.sendTXT(num, "wantToDrive=0");
        snprintf(log,logMaxLength, "Client %d released from driving", num);
        displayAndSerialLog(log);
      } else if(strncmp((char*)payload, comp4, strlen(comp4)) == 0){
        //match
        //check if client is connected, check if client wants control
        if(strlen((const char*)payload) > strlen(comp4)){
          if(isDigit(payload[strlen(comp4)])){
            int targetUser = atoi((const char*) payload + strlen(comp4));
            if(targetUser<= WEBSOCKETS_SERVER_CLIENT_MAX){
              if(driveRequests[targetUser]){
                if(caseCarWebSocket.clientIsConnected(targetUser)){
                  swapControl(clientInControl,targetUser);
                  snprintf(log,logMaxLength, "Transfering drive control", num);
                  displayAndSerialLog(log);
                }else{
                  caseCarWebSocket.sendTXT(num, "User not connected");
                }
              }else{
                caseCarWebSocket.sendTXT(num, "User dont Want To drive");
              }
            }else{
              caseCarWebSocket.sendTXT(num, "User number is too high");
            }
          }
        }
      } else if(strncmp((char*)payload, comp5, strlen(comp5)) == 0){
        //match
        admin = num;
        loggScreenApend((const char*) "admin changed!");
        displayAndSerialLog((const char*) "admin changed!");
        caseCarWebSocket.sendTXT(num, "You are now admin");
      } else if(strncmp((char*)payload, comp6, strlen(comp6)) == 0){
        //match
        admin = num;
        snprintf(log,logMaxLength, "%d is in control", clientInControl);
        caseCarWebSocket.sendTXT(num, log);
      } else if(strncmp((char*)payload, comp7, strlen(comp7)) == 0){
        //match
        snprintf(log,logMaxLength, "admin is %d", admin);
        caseCarWebSocket.sendTXT(num, log);
      } else if(strncmp((char*)payload, comp1, strlen(comp1)) == 0){
        //match
        if(swapControl(clientInControl, num)){
          caseCarWebSocket.sendTXT(num, "DriveControl transfered");
          displayAndSerialLog((const char*) "DriveControl transfered");
        }else{
          caseCarWebSocket.sendTXT(num, "control swap failed");
        }
      }  
      break;
  }
}

void wifiLoopHandler(){
  caseCarWebServer.handleClient();
  caseCarWebSocket.loop();
}
