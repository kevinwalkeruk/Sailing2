var WriteLog = false;
var watchId;
var LastUpdate = new Date();
var OldLat = 50.789756;
var OldLong = -1.722025;
var NewLong = -1.722025;
var NewLat =  50.789756;
var TotalDistance = 0;
var FirstGPS = 0;
var Speed = 0;
var LastSpeed = 0;
var NewSpeed = 0;
var MaxSpeed = 0;
var MaxSpeedValue = 0;
var SpeedTolerance = 10;
var Distance = 0;
//Averages
var Samples =20;
var AngleToWindArray = [];
var i=0;
var AlertText = "No Alert";

var Bearing = 0;
var WindAngle = 180;
var AngleToWindValue = 0;
var SendWindAngle = 999;
var AngleToWind = 0;
var Shift = 0;

var AverageAngleToWind = 0;
var TackAngle = 0;
var DefaultTackAngle = 90;
var PortStarboard = "";
var PortStarboardOld = "";
var PortStarboardShort = "";
var ShiftAlert = 5;
var Tacking = false;
var TackStartAngle = 0;
var LastAngleToWind = 0;
var Marks = 0;
var OldMarks = 0;
var BoatDirection = "";
var Direction = "";
var LastBoatDirection = "";
var AngleToWindSet = false;
//var SendDataType = "N";
//var SendData = 0;

// Choose options about the data returned
var options = {
  enableHighAccuracy: true,
  maximumAge: 0,
  timeout: 10000
};

Pebble.addEventListener("ready", function(e) {
  console.log("PebbleKit JS ready!");
});

Pebble.addEventListener("appmessage", function(e){
  SendDataToWatch();
  console.log("Data received from Watch");
  if (e.payload.AppKeyWatchCommand){
    var Command = e.payload.AppKeyWatchCommand;
    console.log("Watch Cammand Received");    
    console.log(e.payload.AppKeyWatchCommand);
    if (Command == "RSTMaxSpd"){
      MaxSpeed = 0;
      console.log("Max Speed Reset");
    }
    else if (Command == "RaceSt"){
      Marks = 0;
      OldMarks = 0;
      MaxSpeed = 0;
      Distance = 0;
      console.log("Race Started");
    }
    else if (Command == "RaceFi"){
      console.log("Race Finished");
    }
    else if (Command == "SetWind"){
      console.log("True Wind Angle = " + WindAngle);
      console.log("Bearing = " + (Bearing).toFixed(0));
      console.log("Default Tack Angle = " + DefaultTackAngle);
      if (Bearing >= 0 & Bearing <= 360){
        WindAngle = Bearing + (DefaultTackAngle / 2);
        //console.log("True Wind Angle = " + (TrueWindAngle).toFixed(0));
        if(WindAngle >= 360){
          WindAngle = WindAngle - 360;
        }
        //console.log("New True Wind Angle = " + (WindAngle).toFixed(0));
      }
      //AlertText = "Wind Set " + (WindAngle).toFixed(0);
      //SendDataType = "WindAngle";
      //SendData = TrueWindAngle;
      console.log("New True Wind Angle = " + (WindAngle).toFixed(0));
      SendWindAngle = WindAngle;
      AngleToWindSet = true;
    }
    else if (Command.substring(0,7) == "SetTack"){
      DefaultTackAngle = Command.substring(7);
      console.log("Tack Angle Set = " + DefaultTackAngle);
    }
    //else if (Command.substring(0,7) == "SetWind"){
    //  WindAngle = Command.substring(7);
    //  console.log("Wind Angle Set = " + WindAngle);
    //}
    /*

    else if (Command.substring(0,8) == "SetShift"){
      ShiftAlert = Command.substring(8);
      console.log("Shift Alert Set = " + ShiftAlert);
    }
    else if (Command.substring(0,8) == "MaxSpeed"){
      MaxSpeed = Command.substring(8);
      MaxSpeed = MaxSpeed / 10;
      console.log("Max Speed Set = " + MaxSpeed);
    }
    */
    else{
      console.log("Unknown Command = " + Command);
    }
  }
});

function MyRadians(MyAngle){
  //2xPI radians = 360 degrees, PI radians = 180 degrees, radian = 180 degrees / PI
  //console.log('radians = ' + MyAngle*Math.PI/180);
  return MyAngle*Math.PI/180;
}

function firstGPS(pos) {
  console.log("First GPS Data Received!");
  LastUpdate = new Date();
  OldLat = pos.coords.latitude;
  OldLong = pos.coords.longitude;
  console.log("First Lat = " + OldLat + ", First Long = " + OldLong + " @ " + LastUpdate);
  //var intervalId = setInterval(GetGPS, 2000);
  watchId = navigator.geolocation.watchPosition(success, error, options);
}

function CalculateAngleToWind(){
  //Angle to Wind
  if (Speed > 0.5){
    if (Bearing > WindAngle){
      AngleToWind = Bearing - WindAngle;
    }
    else{
      AngleToWind = 360 - WindAngle + Bearing;
    }
    if (AngleToWind > 180){
      AngleToWind = AngleToWind - 360;
    }
    
    if (AngleToWind < 0){
      AngleToWind = 0 - AngleToWind;
      PortStarboard = "Star";
      PortStarboardShort = "S";
    }
    else{
      PortStarboard = "Port";
      PortStarboardShort = "P";
    }
    if (PortStarboard != PortStarboardOld){
      if (AngleToWind < 80){
        //AlertText = "Tack " + PortStarboard;
        //console.log("Tack " + PortStarboard);
        Tacking = true;
        TackStartAngle = AverageAngleToWind;
      }
      else if (AngleToWind > 100 & Tacking != true){
        //AlertText = "Gybe " + PortStarboard;
      }
      PortStarboardOld = PortStarboard;
    }
    //Averages///////////////////////////////////////////////
    //Load Array
    for (i = 0; i < Samples; i++){
      AngleToWindArray[i] = AngleToWindArray[i + 1];
    }
    AngleToWindArray[Samples] = AngleToWind;
  
    //Add Array Values
    AverageAngleToWind = 0;
    for (i = 0; i < Samples; i++){
      AverageAngleToWind += AngleToWindArray[i];
    }
    //Divide by quantity
    AverageAngleToWind = AverageAngleToWind/Samples;
    ////////////////////////////////////////////////////////
  
    //Check Speed
    if (AngleToWind <90 & Tacking == false){
      //Calculate Shift
      Shift = AngleToWind - AverageAngleToWind;  
      if (Shift > ShiftAlert){
        AlertText = "Shift " + (Shift).toFixed(0) + " " + PortStarboardShort + (AngleToWind).toFixed(0);
      }
    }
    else{
      Shift = 0;
    }
    if (Tacking == true){
      //Tack Finished
      if (AngleToWind < (LastAngleToWind + 1) & AngleToWind > (LastAngleToWind - 1)){
        Tacking = false;
        TackAngle = TackStartAngle + AngleToWind;
        AlertText = "Tack " + PortStarboard + " " + (TackAngle).toFixed(0);
        //console.log('Alert Text = ' + AlertText);
      }
      LastAngleToWind = AngleToWind;
    }
    if (AverageAngleToWind < 80){
      BoatDirection = "UpWind";
    }
    else if (AverageAngleToWind > 110){
      BoatDirection = "DownWind";
    }
    if (LastBoatDirection != BoatDirection){
      LastBoatDirection = BoatDirection;
      Marks ++;
      if (Marks > 9){
        Marks = 0;
      }
    }
  }
}

function CalculateBearing(latStart, longStart, latFinish ,longFinish){
  var φ1 = MyRadians(latStart), φ2 = MyRadians(latFinish); // gives d in metres
  var λ1=MyRadians(longStart), λ2=MyRadians(longFinish);
  var y = Math.sin(λ2-λ1) * Math.cos(φ2);
  var x = Math.cos(φ1)*Math.sin(φ2) - Math.sin(φ1)*Math.cos(φ2)*Math.cos(λ2-λ1);
  //var Bearing = Math.atan2(y, x).toDegrees();
  var BearingData = Math.atan2(y, x);
  BearingData = 180/Math.PI*BearingData;
  if(BearingData < 0){
    BearingData = 360+BearingData;
  }
  return BearingData;
}

function success(pos) {
  NewLat = pos.coords.latitude;
  NewLong = pos.coords.longitude;
  
  //var NewLat = 51.903245;
  //var NewLong = -1.175287;

  //Calculate distance
  var latStart = OldLat, latFinish = NewLat, longStart = OldLong, longFinish=NewLong;
  var φ1 = MyRadians(latStart), φ2 = MyRadians(latFinish), Δλ = MyRadians((longFinish-longStart)), R = 6371e3; // gives d in metres
  Distance = Math.acos( Math.sin(φ1)*Math.sin(φ2) + Math.cos(φ1)*Math.cos(φ2) * Math.cos(Δλ) ) * R; 
  Distance = Distance/1609.344; //1609.344 to get miles

  CalculateSpeed();

  if (Speed > 0.5){
    //CALCULATE BEARING
    Bearing = CalculateBearing(latStart, longStart, latFinish ,longFinish);
    //console.log('Calculate Bearing = ' + Bearing);
  
    CalculateAngleToWind();
    //console.log('Calculate Angle to Wind = ' + AngleToWind);
  
    //CalculateAverageSpeed();
    //SpeedDifference = Speed - AverageSpeed;
  }

  //if (Distance > 0){
  //  TotalDistance = TotalDistance + Distance;
  //}//Total Distance
  
  if (FirstGPS === 0){
    TotalDistance =0;
    FirstGPS =1;
  }
   
  SendDataToWatch();
  
  OldLat = NewLat;
  OldLong = NewLong;
}

function CalculateSpeed(){
  var d = new Date();
  var UpdateDuration = (d-LastUpdate)/1000;
  LastUpdate = d;
    // Calculate Speed - 3600 seconds per hour
  NewSpeed = Distance/UpdateDuration*3600;
  if (NewSpeed < 100){
    if (NewSpeed < (LastSpeed + SpeedTolerance) && NewSpeed > (LastSpeed - SpeedTolerance)){
      Speed = NewSpeed;
    }
    else if (WriteLog == true){
      console.log('Speed Error = ' + (NewSpeed).toFixed(1));
    }
    LastSpeed = NewSpeed;
    //console.log('Max Speed = ' + (MaxSpeed).toFixed(1));
    if (Speed > MaxSpeed && Speed < 100){
      MaxSpeed = Speed;
      AlertText = "Max Spd " + (MaxSpeed).toFixed(1);
      //AlertType = 1;
      console.log('New Max Speed = ' + (MaxSpeed).toFixed(1));
    }
  }
  
}

function SendDataToWatch(){
  //MaxSpeedValue = (MaxSpeed).toFixed(1);
  //MaxSpeedValue = MaxSpeedValue * 10;
  var SpeedValue = Speed * 10;
  var MaxSpeedValue = MaxSpeed * 10;
  var ShiftValue = Shift * 10;
  var ShiftType = "Shift";
  var ATWType = "ATW";
  if (AngleToWindSet){
    if (ShiftValue > 20){
      ATWType = "Head";
    }
    else if (ShiftValue < -20){
      ATWType = "Lift";
    }
    if (ShiftValue > 0){
      ShiftType = "Head";
    }
    else if (ShiftValue < 0){
      ShiftValue =ShiftValue * -1;
      ShiftType = "Lift";
    }
    else if (ShiftValue == "nan"){
      ShiftValue =0;
    }
    if (ShiftValue > 999){
      ShiftValue = 999;
    }
    if (AngleToWind > 0){
      AngleToWindValue = AngleToWind;// * 10;
    }
    else{
      AngleToWindValue = 0;
    }  
    if (AngleToWindValue > 90){
      AngleToWindValue = 180 - AngleToWindValue;
    }
    AngleToWindValue = AngleToWindValue * 10;
  }
  else{
    AngleToWindValue =999;
    Shift = 999;
  }
  
  if (AngleToWind <= 90){
    Direction = PortStarboardShort + "U";
  }
  else{
    Direction = PortStarboardShort + "D";
  }
  
    
  Pebble.sendAppMessage({
    'AppKeyAlert': "" + AlertText,
    'AppKeySpeed': "" + (SpeedValue).toFixed(0),
    'AppKeyMaxSpeed': "" + (MaxSpeedValue).toFixed(0),
    'AppKeyMarks': "" + (Marks).toFixed(0),
    'AppKeyAngleToWind': "" + (AngleToWindValue).toFixed(0),
    'AppKeyATWType': "" + ATWType,
    'AppKeyBearing': "" + (Bearing).toFixed(0),
    'AppKeyDirection':"" + Direction,
    'AppKeyShift':"" + (ShiftValue).toFixed(0),
    'AppKeyShiftType':"" + ShiftType,
 });
  
  AlertText = "No Alert";
  //SendDataType = "N";
  //SendData = 0;
  SendWindAngle = 999;
}

function error(err) {
  console.log('location error (' + err.code + '): ' + err.message);
}

function GetGPS() {
  navigator.geolocation.getCurrentPosition(success, error, options);
}

navigator.geolocation.getCurrentPosition(firstGPS, error, options);








