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
import processing.pdf.*;

// Serial Port Variables
Serial port; // Initialize Serial object
String buff = ""; // Create a serial buffer
String[] depths = new String[64]; // Create a list to parse serial into 

// Mesh Generation Variables

final int WIDTH = 500;
final int HEIGHT = 500;
final int BLOCKX = WIDTH / 8;
final int BLOCKY = HEIGHT / 8;
int cols = 8; // Sensor is 8x8
int rows = 8;
int scale = 100; // Scale value for drawing our mesh
float[][] terrain = new float[8][8]; // Create a table of distance values by pixel location
float[][] status = new float[8][8]; // Create a table of distance values by pixel location
float[][] signal = new float[8][8]; // Create a table of distance values by pixel location
float[][] ambient = new float[8][8]; // Create a table of distance values by pixel location

// Cursor Tracking Variables
float xPress = 0.0; // Store the cursor x position on mouse press 
float yPress = 0.0; // Store the cursor y position on mouse press

// Mesh Rotation Variables
float xRotOffset = 0; // Temporary x rotational offset (relevant during mouse drag)
float xRotPos = 0; // X rotational position
float zRotOffset = 0; // Temporary z rotational offset (relevant during mouse drag)
float zRotPos = 0; // Z rotational position
float scaleOffset = .5; // Scale factor from mouse wheel

boolean saveVid = false;
int vidNumber = 0;
int[] vid = new int[64];

void setup(){
  size(500, 500);
   
  port = new Serial(this, "/dev/ttyACM2", 9600); // CHANGE COM13 TO YOUR SERIAL PORT
  port.bufferUntil(10); // ASCII LineFeed Character
   
  // Fill our list with 0s to avoid a null pointer exception
  // in case the sensor data is not immediately available
  for(int idx = 0; idx < 64; idx++){
    depths[idx] = "0,0,0,0"; 
  }
 
      textSize(25);
}

void draw(){
  
  colorMode(HSB); // HSB color space make+s it easy to map hue
  //lights(); // Add ambient light to scene
  //noStroke(); // Draw without stroke
  //smooth(); // Draw with anti-aliasing
  background(0); // Fill background with black
  
  // This stuff is all basically to scale and rotate the mesh
  // while keeping it in the center of the scene
  //translate(width/2,height/2);
  //rotateX(PI/3-(xRotOffset+xRotPos));
  //rotateZ(0-zRotOffset-zRotPos);
  scale(scaleOffset);
  //translate(-width/2, -height/2);

  // Load our list into our matrix for easy addressing
  // This could be optimized out but I find this more readable
  // as you could potentially print the intermediate step 
  // between csv parsing and matrix-stuffing. Also, it's easier
  // to check length on a 1-dimensional array.
  for(int y=0; y<rows; y++){
    for(int x=0; x<cols; x++){
      if(depths.length >= 64){
        int[] values = int(split(depths[x+y*cols], ','));
        terrain[x][y] = values[0]/10;
        status[x][y] = values[1];
        signal[x][y] = values[2];
        ambient[x][y] = values[3];
        print(terrain[x][y] + ",");
      }
    }
  }
  
  // For each row, draw a triangle strip with the z-height of
  // each vertex corresponding to the distance reading at that 
  // location. While we're at it, set the fill color to a hue
  // corresponding to the data as well.
  for(int y=0; y<rows; y++){
    //beginShape(TRIANGLE_STRIP);
    for(int x=0; x<cols; x++){ 
      if(terrain[cols-x-1][y] == 500) {
        fill(255);
      } else {
      fill(map(terrain[cols-x-1][y],0,300,255,0),255,255);
      }
      rect(x * BLOCKX*2, y * BLOCKY*2, (x + 1) * BLOCKX*2, (y + 1) * BLOCKY*2);
      fill(0, 0, 0);
      text((int)terrain[cols-x-1][y], x * BLOCKX*2+5, y * BLOCKY*2+30);
      text((int)status[cols-x-1][y], x * BLOCKX*2+5, y * BLOCKY*2+60);
      text((int)signal[cols-x-1][y], x * BLOCKX*2+5, y * BLOCKY*2+90);
      text((int)ambient[cols-x-1][y], x * BLOCKX*2+5, y * BLOCKY*2+120);
    }
  endShape();
  }
  if(saveVid) {
    saveFrame(vidNumber+"line-######.png");
  }

}

// Handle incoming serial data
void serialEvent(Serial p){ 
  
  buff = (port.readString()); // read the whole line
  buff = buff.substring(0, buff.length()-1); // remove the Carriage Return
  if (buff != "") {
    depths = (split(buff, ';')); // split at commas into our array
  }
  
    if(saveVid) {
      for(int y=0; y<rows; y++){
        for(int x=0; x<cols; x++){
          if(depths.length >= 64){
            print(split(depths[x+y*cols], ",")[0] + ";");
          }
        }
      print("\n");
      }
      print("\n");
    }
}

void keyPressed() {
  saveVid = !saveVid;  
  vidNumber++;
  frameCount = 0;
}
