import processing.javafx.*;

import processing.serial.*;

// constants

//Button Stuff
int rect1X, rect2X, rect1Y, rect2Y;
int rectHeight = 60;
int rectWidth = 150;
color rectColor, currentColor, clickedColor, hoverColor;

float scale = 1;

int countdown = 5;
int countdownStart;
boolean isCountingDown = false;

int recordingTime = 10;
int recordingStart;
boolean isRecording = false;

boolean showMovement = false;
String valuesBuff = "";
Serial port; // Initialize Serial object
String buff = "0,0,0,;"; // Create a serial buffer
float prevAngle = 0;

void setup() {
  size(1000, 1000, FX2D);
  rect1X=20;
  rect2X=20;
  rect1Y=100;
  rect2Y=200;
  rectColor= color(100);
  clickedColor = color(200);
  hoverColor = color(150);
  port = new Serial(this, "/dev/ttyACM0", 9600);
  port.bufferUntil(10); // ASCII LineFeed Character
}

void draw()
{
  background(55);

  // draw distance circles
  stroke(110, 110, 110);
  noFill();
  for (int i = 200; i <= 3000; i += 200) {
    if (i % 1000 == 0) { // each 5m should be a bold stroke
      strokeWeight(4);
    } else {
      strokeWeight(1);
    }
    ellipse(width / 2, height / 2, i * scale, i * scale);
  }

  // sweep dot
  fill(0, 200, 0);
  noStroke();
  ellipse(width / 2, height / 2, 20, 20);
  String[] values = (split(valuesBuff, ';'));
  for (int i = 0; i<values.length; i++) {
    String[] valuesSplit = new String[4];
    valuesSplit = (split(values[i], ','));
    if (valuesSplit.length > 1) {
    fill(255, 0, 100);
    noStroke();
    float x =  (width / 2) + (cos(radians(float(valuesSplit[1]))) * float(valuesSplit[2])) * scale;
    float y = (height / 2) - (sin(radians(float(valuesSplit[1]))) * float(valuesSplit[2])) * scale;
    ellipse(x, y, 10, 10);
    }
  }
}
void serialEvent(Serial p){ 
  
  buff = (port.readString()); // read the whole line
  buff = buff.substring(0, buff.length()-1); // remove the Carriage Return
  if (buff != "") {
    valuesBuff = buff;
  }
}

void exit() {
  super.exit();
}
