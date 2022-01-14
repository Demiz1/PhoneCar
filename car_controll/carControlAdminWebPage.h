char carControlAdminWebPage[] PROGMEM = R"====( <!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>CaseCar Admin</title>
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

    #takeControlButton{
        height: 50px;
        background-color:tomato;
        border-radius:20px;
        text-align: center;
        line-height:50px;
        margin-bottom:10px;
        margin-top:5px;
    }
    
    .posData{
        height: 130px;
    }
    
    .dataPanes{
        float:left;
        width:100px;
    }
    .UserPanel{
        height : 350px;
        background-color:lightblue;
        margin-bottom:10px;
        border-style:solid;
        
        border-radius:20px;
    }
    
    .userList{
        height : 200px;
        background-color:bisque;
        margin-right:5px;
        padding-left:5px;
        overflow-x: hidden; 
        overflow-x: auto; 
        text-align:justify; 
        margin-bottom:5px;
        margin-left:5px;
    }
    .comandPanel{
        height:56px;
    }
    
    .inComandPanel{
        float:left;
        margin-left:5px;
        height:100%;
        text-align: center;
        background-color:yellow;
        line-height:50px;
        padding-left:8px;
        padding-right:8px;
        border-radius:10px;
        margin-top:0px;
    }
    
    #targetUser{
        width:45px;
        border-radius:0px;
        resize: none;
        height:50px;
    }
    
    .topPanel{
        background-color:green;
        margin-right:5px;
        border-radius:10px;
        padding-bottom:7px;
        padding-top:7px;
        padding-left:5px;
        margin-left:5px;
    }
    </style>
</head>
<body>
<div class="mainWrapper">
    <div class="UserPanel">
        <p class="topPanel">Connected users XXXX</p>
        
        <div class="userList">
            <p>exUser #1 </p>
            <p>exUser #2 </p>
            <p>exUser #3 </p>
            <p>exUser #4 DRIVER </p>
            <p>exUser #5 </p>
            <p>exUser #6 </p>
        </div>

        <div class="comandPanel">
            <textarea id="targetUser" class="inComandPanel" rows="1" maxlength="4">user</textarea>
            <p id="transferControlButton" class="inComandPanel" onclick="transferControlButton()">Transfer control</p>
        </div>
    </div>

    <div id="box">
        <div id="ball"></div>
    </div>
    <p id="takeControlButton" onclick="takeControlButton()">Take Control</p>
</div>

</body>
 <script>
        var initX;
        var initY;
        var posX = 50;
        var posY = 50;
        var listeningOnInterface = 0; //keyboard is 0, gyro is 1
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
                intervalFunction = setInterval(sendToSocket, 20);
            }else if(event.data == "control removed"){
                console.log("we are nolonger in control");
                clearInterval(intervalFunction);
            }else if(event.data == "You are now admin"){
                console.log("we are admin");
            }
        });
        
        socket.addEventListener('error', function(event) {
            console.log("socket error");
            clearInterval(sendToSocket);
        });
        
        socket.addEventListener('close', function(event){
            console.log("socket closed");
            clearInterval(sendToSocket);
        });
        
        socket.addEventListener('open', function(event){
            console.log("socket is open");
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
        
        
        function takeControlButton(){
            if (socket.readyState === WebSocket.OPEN) {
                socket.send("takeControl");
            }
        }
        
        function transferControlButton(){
            if (socket.readyState === WebSocket.OPEN) {
                //get value
                var rawValue = document.getElementById("targetUser").value;
                if(!isNaN(rawValue)){
                    document.getElementById("targetUser").value = "";
                    console.log("giveControl" + rawValue);
                    socket.send("giveControl" + rawValue);
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
