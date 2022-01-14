char carControlWebPage[] PROGMEM = R"====( <!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CaseCar User</title>
    <style>
    
    body{
        width:400px;
        background-color:silver;
    }
    
    p{
        user-select: none;
    }
    #box{
        height: 400px;
        border-style: solid;
        background-color: lime;
        border-radius:20px;
    }
    
    #ball{
        width: 50px;
        height: 50px;
        background-color: black;
        border-radius: 50%;
        position:relative;
        left:88%;
        top:20%;
    }

    #wantToDriveButton{
        height: 50px;
        background-color:yellow;
        border-radius:15px;
        text-align: center;
        line-height:50px;
        margin-bottom:10px;
        margin-top:5px;
        margin-left:5px;
        margin-right:5px;
    }
    
    #statusP{
        height: 50px;
        background-color:DodgerBlue;
        border-radius:15px;
        text-align: center;
        line-height:50px;
        margin-bottom:10px;
        margin-top:5px;
        margin-left:5px;
        margin-right:5px;
    }
    
    #clientId{
        height: 50px;
        background-color:Gainsboro;
        border-radius:15px;
        text-align: center;
        line-height:50px;
        margin-bottom:10px;
        margin-top:5px;
        margin-left:5px;
        margin-right:5px;
    }
    
    .topPanel{
        background-color:lightblue;
        margin-bottom:10px;
        border-style:solid;
        border-radius:20px;
    }
    
    .infoPanel{
        height : 200px;
        background-color:bisque;
        border-radius:15px;
        margin-right:5px;
        padding-left:5px;
        padding-right:5px;
        overflow-x: hidden; 
        overflow-x: auto; 
        text-align:justify; 
        margin-bottom:5px;
        margin-left:5px;
        margin-top:5px;
    }
    
    </style>
</head>
<body>
<div class="mainWrapper">
    <div class="topPanel">        
        <div class="infoPanel">
            <p>Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore </p>
        </div>

        <p id="wantToDriveButton" onclick="wantToDriveButton()">I want to drive</p>
        <p id="statusP">Status</p>
        <p id="clientId">Client id</p>
    </div>

    <div id="box">
        <div id="ball"></div>
    </div>
</div>

</body>
 <script>
        var initX;
        var initY;
        var posX = 50;
        var posY = 50;
        var listeningOnInterface = 0; //keyboard is 0, gyro is 1
        var wantToDrive = 0;
        document.getElementById("statusP").innerText = "Not connected";
        let socket = new WebSocket(prompt("Write the target socket", "ws://192.168.1.1:81"));
        var intervalFunction;
        
        var keyPressed = {};
        var lastSpeedUpdate = Date.now();
        if (window.DeviceOrientationEvent) {
            window.addEventListener('deviceorientation',changeDeviceListening,{ capture:true , once:true});
            window.addEventListener('deviceorientation', moveBall_position);
        }
        
        if(window.KeyboardEvent){
            window.addEventListener('keydown', keysPressed);
            window.addEventListener('keyup', keysReleased);
        }
        
        socket.addEventListener('message', function(event) {
            console.log(event.data);
            if(event.data == "control gained"){
                console.log("we are in control");
                document.getElementById("statusP").innerText = "You are in control";
                document.getElementById("wantToDriveButton").innerText = "Click to release control";
                intervalFunction = setInterval(sendToSocket, 20);
            }else if(event.data == "control removed"){
                console.log("we are nolonger in control");
                document.getElementById("statusP").innerText = "You lost the control";
                clearInterval(intervalFunction);
            }else if(event.data.startsWith("client id")){
                document.getElementById("clientId").innerText = event.data;
            }else if(event.data.startsWith("wantToDrive=1")){
                document.getElementById("wantToDriveButton").innerText = "Click if you dont want to drive";
                wantToDrive = 1;
            }else if(event.data.startsWith("wantToDrive=0")){
                wantToDrive = 0;
                document.getElementById("wantToDriveButton").innerText = "Click to drive";
            }
        });
        
        socket.addEventListener('error', function(event) {
            console.log("socket error");
            document.getElementById("statusP").innerText = "There was a comunication error";
            clearInterval(sendToSocket);
        });
        
        socket.addEventListener('close', function(event){
            console.log("socket closed");
            document.getElementById("statusP").innerText = "Communication ended";
            clearInterval(sendToSocket);
        });
        
        socket.addEventListener('open', function(event){
            console.log("socket is open");
            document.getElementById("statusP").innerText = "Communication esablished";
        });
        
        
        function keysPressed(event){
            keyPressed[event.code] = true;
            updateLocalValues();
        }
        
        function keysReleased(event){
            keyPressed[event.code] = false;
            updateLocalValues();
        }
        
        function updateLocalValues(event){
            if(listeningOnInterface == 0){
                //we are listening on the keyboard
                var arr = Object.keys(keyPressed);
                if(!(keyPressed['ArrowLeft'] || keyPressed['ArrowRight'])){
                    posX = 50;
                }
                if(keyPressed['ArrowLeft']){
                    posX = 0;
                }
                if(keyPressed['ArrowRight']){
                    posX = 100;
                }
                
                 if(!(keyPressed['ArrowUp'] || keyPressed['ArrowDown'])){
                    posY= 50;
                }               
                if(keyPressed['ArrowUp']){
                    posY = 0;
                }
                
                if(keyPressed['ArrowDown']){
                    posY = 100;
                }
                
                //console.log("pos);
                updateUI();
                
            }
        }
        
        //we got a orientation event, change to gyro listening
        function changeDeviceListening(event){
            listeningOnInterface = 1;
        }
        
        
        function wantToDriveButton(){
            if (socket.readyState === WebSocket.OPEN) {
                if(wantToDrive == 0){
                    socket.send("wantToDrive");
                }
                if(wantToDrive == 1){
                    socket.send("dontWantToDrive");
                }
            }
        }
        
        function moveBall_position(event){
            posX = (event.gamma * 1.5 + 50);
            posY = (event.beta * 2 - 40 + 50);
            
            updateUI();
        }
        
        function updateUI(){
            
            var localX = posX / (100/88);
            var localY = posY / (100/88);
            
            if(localX > 88) localX = 88;
            if(localY > 88) localY = 88;
            
            if(localX < 0) localX = 0;
            if(localY < 0) localY = 0;
            document.getElementById("ball").style.left = localX + "%";
            document.getElementById("ball").style.top = localY + "%";
            
            console.log("posX: " + posX.toFixed(2) + "  posY: " + posY.toFixed(2));
        }
        
        function sendToSocket(){
            var localX = posX.toFixed(0).toString();
            if(localX.length < 2){
                localX = "0" + localX;
            }
            if(localX.length < 3){
                localX = "0" + localX;
            }
            var localY = (100-posY.toFixed(0)).toString();
            if(localY.length < 2){
                localY = "0" + localY;
            }
            if(localY.length < 3){
                localY = "0" + localY;
            }
            var message = "driveComandD=" + localY + "T=" + localX;
            console.log(message);
            socket.send(message);
        }
    </script>
</html> 
)====";
