/*
  Visualizing VL53L5CX Output As A 3D Mesh
  By: Nick Poole
  SparkFun Electronics
  Date: November 3, 2021
  License: MIT. See license file for more information but you can
  basically do whatever you want with this code.

  This example is a companion to the Arduino Library Example 4 "MaxOutput"
  Which you need to be running on an attached device. You'll need to 
  change "COM13" below to the name of your serial port. 
  
  Once the sketch is running, it may take a few seconds for the sensor to respond,
  during which you'll see a red square in the window. If it takes more than a few
  seconds for this to change, check your serial port and run the sketch again. 
  
  Once data is incoming, you can rotate the mesh by dragging within the display window.
  You can zoom the mesh in and out using the scroll wheel on your mouse.

  Feel like supporting our work? Buy a board from SparkFun!
  https://www.sparkfun.com/products/18642
*/

import processing.serial.*;
import org.gicentre.utils.stat.*;    // For chart classes.

// Serial Port Variables
Serial port; // Initialize Serial object
String buff = ""; // Create a serial buffer
float[] tofValues = new float[3]; // Create a list to parse serial into 
float[] soilValues = new float[2]; // Create a list to parse serial into 

// Declare Header Font
PFont h1;

//Declare Label Font
PFont l1;

BarChart tofBarChart;
BarChart soilBarChart;

void setup(){
  size(800, 600);
  smooth();
  
  //textFont(createFont("Serif",10),10);
  
  tofBarChart = new BarChart(this);  
  tofBarChart.setData(new float[] {0., 0.});
  tofBarChart.setMinValue(0);
  tofBarChart.setMaxValue(4000);
  tofBarChart.showValueAxis(true);
  tofBarChart.setBarLabels(new String[] {"ToF Maximum","ToF Minimum"});  
  tofBarChart.showCategoryAxis(true);
  
  port = new Serial(this, "/dev/ttyACM0", 9600); // CHANGE COM13 TO YOUR SERIAL PORT
  port.bufferUntil(10); // ASCII LineFeed Character 
  
  String[] args = {"TwoFrameTest"};
  PercentageNoToFReading sa = new PercentageNoToFReading();
  PApplet.runSketch(args, sa);
  //SoilSensor sa2 = new SoilSensor();  
  //PApplet.runSketch(args, sa2);
}

void draw(){
  //Set Background to white.
  background(255);
  
  tofBarChart.setData(subset(tofValues,0,2));
  tofBarChart.draw(15,15,width-30,height-30); 

  println("Soil temperature: " + soilValues[0] + "  |  Soil moisture: " + soilValues[1]);
}

public class PercentageNoToFReading extends PApplet {

  public void settings() {
    size(640, 360);
  }
  public void draw() { //<>//
    background(255);
    int percNoReadings = int((tofValues[2]/64)*365);
    pieChart(300, new int[] {365-percNoReadings,percNoReadings});
  }
  void pieChart(float diameter, int[] data) {
    float lastAngle = 0;
    for (int i = 0; i < data.length; i++) {
      float gray = map(i, 0, data.length, 255, 0);
      fill(gray);
      arc(width/2, height/2, diameter, diameter, lastAngle, lastAngle+radians(data[i]));
      lastAngle += radians(data[i]);
    }
  }
}

public class SoilSensor extends PApplet {

  SoilSensor() {
   super();
  }
  
  public void settings() {
    size(800, 600);
    smooth();
    
    tofBarChart = new BarChart(this);
    tofBarChart.setData(new float[] {0., 0.});
    tofBarChart.setMinValue(0);
    tofBarChart.setMaxValue(300);
    tofBarChart.showValueAxis(true);
    tofBarChart.setBarLabels(new String[] {"Temperature","Moisture"});  
    tofBarChart.showCategoryAxis(true);
  }
  public void draw() {
    background(255);
  
    tofBarChart.setData(soilValues);
    tofBarChart.draw(15,15,width-30,height-30); 
  }
}

// Handle incoming serial data
void serialEvent(Serial p){ 
  buff = (port.readString()); // read the whole line
  buff = buff.substring(0, buff.length()-1); // remove the Carriage Return
  if (buff != "") {
    String messageParts[] = split(buff, ';');
    String tofMessage = split(messageParts[0], 't')[1];
    String soilMessage = split(messageParts[1], 's')[1];
    if (tofMessage != "") {
      tofValues = float(split(tofMessage, ','));
    }
    if (soilMessage != "") {
      soilValues = float(split(soilMessage, ','));
    }
  }
}
