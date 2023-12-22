#include <Servo.h>



#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
Servo gateServo;
void closeDoor()
 {
  gateServo.write(160);   // 0 degrees is an example angle, adjust as needed
  delay(500);         
}

void openDoor() {
  gateServo.write(0);  // 90 degrees is an example angle, adjust as needed
  delay(500);         
}
const char *ssid = "AG";
const char *password = "123456780";

const int irPin1 = D4;  // IR sensor 1 pin
const int irPin2 = D3;  // IR sensor 2 pin
const int irPin3 = D0;
const int ledPin1 = D2;
const int ledPin2 = D1;            // LED pin
const int ultrasonicTrigPin = D5;  // Ultrasonic sensor trigger pin
const int ultrasonicEchoPin = D6;  // Ultrasonic sensor echo pin
const int buzz = D7;
const int servoPin = D8;
boolean val_ir1 ;
boolean val_ir2 ;
ESP8266WebServer server(80);

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>Parking Management System</title>
  <!-- Include jQuery -->
  <script src="https://code.jquery.com/jquery-3.6.4.min.js"></script>
</head>
<body>

<h1>Parking Garage Status</h1>

<div id="parkingStatus">
  <p>Parking Area 1: <span id="status1"></span></p>
  <p>Parking Area 2: <span id="status2"></span></p>
  <p>Total Parking Area : <span id="status4"></span></p>
</div>

<button id="openDoorBtn">Open Door</button>
<button id="closeDoorBtn">Close Door</button>

<script>
  // Update parking status every second
  setInterval(function() {
    $.get("getStatus", function(data) {
      $("#status1").text(data.status1);
      $("#status2").text(data.status2);
      $("#status4").text(data.status4);
    });
  }, 1000);

  $("#openDoorBtn").click(function() {
    $.get("openDoor");
  });

  $("#closeDoorBtn").click(function() {
    $.get("closeDoor");
  });
</script>

</body>
</html>
)rawliteral";


void handleRoot() {
  server.send(200, "text/html", index_html);
}

void handleGetStatus() {
  // String status2;
  // String status1;
  // String status4;
  // if(val_ir1==1 && val_ir2==1)
  // {
  //    status4 = "there is no available slot" ;
  //    status1 ="isOccupied";
  //    status2 ="isOccupied";
  // }
  // else{
  //   status4 = "there is still available slot";
  //   if(val_ir1==1){
  //     status1 ="isOccupied";
  //   }else{
  //     status1 ="free";
  //   }
  //   if(val_ir2 == 1){
  //     status2 ="isOccupied";
  //   }else{
  //     status2 ="free";
  //   }
  // }
  // else if( )
  // {
  //   String status1 ="isOccupied";
  //   String status2 ="free";
  // }
  // else{
  //       String status2 ="isOccupied";
  //   String status1 ="free";
  // } 
    int totalOccupied = val_ir1 + val_ir2 ;
    String status1 = (val_ir1 == 0) ? "Free" : "isOccupied";
    String status2 = (val_ir2 == 0) ? "Free" : "isOccupied";
    String status4 = (totalOccupied == 2 ) ? "there is no available slot" : "there is still available slot";
  String status = "{\"status1\":\"" + status1 +
                  "\",\"status2\":\"" + status2 +
                  "\", \"status4\":\"" + status4 + "\"}";
  
  server.send(200, "application/json", status);
}

void handleOpenDoor() {
  openDoor();
  server.send(200, "text/plain", "Door opened");
}
void handleCloseDoor() {
  closeDoor();
  server.send(200, "text/plain", "Door closed");
}



void setup() {
  // put your setup code here, to run once:
  pinMode(irPin1, INPUT);
  pinMode(irPin2, INPUT);
  pinMode(irPin3, INPUT);
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ultrasonicTrigPin, OUTPUT);
  pinMode(ultrasonicEchoPin, INPUT);
  pinMode(buzz, OUTPUT);
  gateServo.attach(servoPin);
  gateServo.write(0);  // Initial position of the servo
  Serial.begin(115200);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  server.on("/", HTTP_GET, handleRoot);
  server.on("/getStatus", HTTP_GET, handleGetStatus);
  server.on("/openDoor", HTTP_GET, handleOpenDoor);
  server.on("/closeDoor", HTTP_GET, handleCloseDoor);
}

void loop() {
  // put your main code here, to run repeatedly:
  val_ir1 = !digitalRead(irPin1);
  val_ir2 = !digitalRead(irPin2);
  int val_ir3 = !digitalRead(irPin3);

  
  Serial.print(val_ir1);
  Serial.print('\t');
  Serial.print(val_ir2);
  Serial.print('\t');
  int distance = measureDistance();
  Serial.print("Distance: ");
  Serial.println(distance);

  if (val_ir1 == 1) {
    digitalWrite(ledPin1, HIGH);  // Turn on LED1
  } else {
    digitalWrite(ledPin1, LOW);  // Turn off LED1
  }

  if (val_ir2 == 1) {
    digitalWrite(ledPin2, HIGH);  // Turn on LED2
  } else {
    digitalWrite(ledPin2, LOW);  // Turn off LED2
  }
  if (distance < 20) {
    if (val_ir1 == 1 && val_ir2 == 1) {
      // Check ultrasonic distance
      
    } else  {
      openGate();
    }
  }
    if(val_ir3 == 1)
 {
  openGate();
}
server.handleClient();
}

int measureDistance() {
  float duration;
  int cm;
  digitalWrite(ultrasonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(ultrasonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(ultrasonicTrigPin, LOW);

  duration = pulseIn(ultrasonicEchoPin, HIGH);
  cm = duration / 29 / 2;  // Convert pulse duration to distance in cm
  return cm;
}
void openGate() {
  gateServo.write(0);
  digitalWrite(buzz, HIGH);  // Rotate the servo to 160 degrees (adjust as needed)
  delay(3000);               // Wait for the gate to open (adjust as needed)
  gateServo.write(160);        // Reset the servo to the initial position
  digitalWrite(buzz, LOW);
}



