String[] lines; // Array to store lines from the CSV file
int currentRow = 1; // Variable to keep track of the current row being drawn
String[] depths = new String[1282]; // Create a list to parse serial into
boolean found = false; 
boolean paused = false; // Variable to control the pause between rows


// Mesh Generation Variables

final int WIDTH = 1500;
final int HEIGHT = 1000;
final int BLOCKX = WIDTH / (8*5);
final int BLOCKY = HEIGHT / (8*4);
int cols = 8; // Sensor is 8x8
int rows = 8;
int scale = 100; // Scale value for drawing our mesh
float[][][] terrain = new float[20][9][9]; // Create a table of distance values by pixel location

void setup() {
    size(1650,1100);
    background(255);

    // Fill our list with 0s to avoid a null pointer exception
    // in case the sensor data is not immediately available
    for(int idx = 0; idx < 64; idx++){
        depths[idx] = "0,0,0,0"; 
    }

    textSize(35);

    // Load the CSV file
    lines = loadStrings("../../trainingsdaten/fahrraeder.csv");
}

void draw() {
  // If not paused, draw the current row
  if (!paused) {
    drawRow();
    currentRow++; // Move to the next row
    if (currentRow >= lines.length) {
      noLoop(); // Stop drawing when all rows are drawn
    } else {
      // Pause for 5 milliseconds before drawing the next row
      redraw();
      delay(2);
    }
  }
}

void drawRow() {
    // Split the current row into values
    depths = split(lines[currentRow], ",");
    colorMode(HSB); // HSB color space make+s it easy to map hue
    background(0); // Fill background with black
    
    scale(.5);

    for(int i=0; i<20; i++) {
      for(int y=0; y<rows; y++){
          for(int x=0; x<cols; x++){
          if(depths.length >= 64){
              terrain[i][x][y] = float(depths[i*64+x+y*cols]);
          }
          }
      }
    }
    int count = 0;
    for(int i=0; i<4; i++) {
      for(int j=0; j<5; j++) {
        for(int y=0; y<rows; y++){
            //beginShape(TRIANGLE_STRIP);
            for(int x=0; x<cols; x++){ 
              if(terrain[0][cols-x-1][y] == 500) {
                  fill(255);
              } else {
                  fill(map(terrain[count][cols-x-1][y],0,2600,255,0) ,255,255);
              }
              rect((x+j*9)*BLOCKX*2, (y+i*9) * BLOCKY*2, (x + 1+j*9) * BLOCKX*2, (y + 1+i*9) * BLOCKY*2);
            }
             fill(0);
             rect((8+j*9)*BLOCKX*2, (y+i*9) * BLOCKY*2, (8 + 1+j*9) * BLOCKX*2, (y + 1+i*9) * BLOCKY*2);
          endShape();
        };
        
             fill(0);
             rect((0+j*9)*BLOCKX*2, (8+i*9) * BLOCKY*2, (8 + 1+j*9) * BLOCKX*2, (8 + 1+i*9) * BLOCKY*2);
          count = count + 1;
      }
    }
    fill(0, 0, 0);
    text(depths[1280], 50, 50,200);
    if(!found){
        if(isInArray(depths[1280])) {
            found = true;
            paused=true;
            textSize(120);
            text("takeover!", BLOCKX*2, 3*BLOCKY*2,200);
            textSize(35);
        }
    }
    else {
        textSize(120);
        text("takeover!", BLOCKX*2, 3*BLOCKY*2,200);
        textSize(35);
        if(isInArray(depths[1280])) {
            found = false;
        }
        delay(100);
    }
}    

boolean isInArray(String valueToCheck) {
  String[] timepairs = {
        "16:05:27.427", "16:05:28.351",
        "16:05:33.330", "16:05:34.103",
        "16:05:40.475", "16:05:41.304",
        "16:05:42.524", "16:05:43.310",
        "16:05:46.325", "16:05:47.252",
        "16:05:49.302", "16:05:50.134",
        "16:05:52.908", "16:05:53.687",
        "16:05:55.346", "16:05:56.671",
        "16:06:02.030", "16:06:03.006",
        "16:06:04.562", "16:06:05.392",
        "16:06:10.118", "16:06:10.950",
        "16:06:16.699", "16:06:17.520",
        "16:06:25.122", "16:06:25.991",
        "16:06:55.390", "16:06:56.214",
        "16:06:58.504", "16:06:59.383",
        "16:07:05.825", "16:07:06.660",
        "16:07:12.938", "16:07:13.715",
        "16:07:21.698", "16:07:22.575",
        "16:07:26.908", "16:07:27.841",
        "16:07:28.234", "16:07:29.057",
        "16:07:33.060", "16:07:33.927",
        "16:07:45.330", "16:07:46.260",
        "16:07:47.332", "16:07:48.211",
        "16:08:00.776", "16:08:01.640",
        "16:08:28.459", "16:08:29.379",
        "16:08:33.775", "16:08:34.549",
        "16:08:40.588", "16:08:41.494",
        "16:08:41.908", "16:08:42.845",
        "16:08:48.536", "16:08:49.424",
        "16:08:51.609", "16:08:52.540",
        "16:08:53.279", "16:08:54.945",
        "16:08:56.311", "16:08:57.390",
        "16:08:58.476", "16:08:59.318",
        "16:09:00.629", "16:09:01.505",
        "16:09:03.548", "16:09:04.481",
        "16:09:05.019", "16:09:05.798",
        "16:09:09.011", "16:09:09.885",
        "16:09:43.428", "16:09:44.368",
        "16:09:57.119", "16:09:58.051",
        "16:10:12.363", "16:10:13.284",
        "16:10:14.560", "16:10:15.838",
        "16:10:18.323", "16:10:19.111",
        "16:10:21.927", "16:10:22.789",
        "16:10:26.162", "16:10:27.034",
        "16:10:51.219", "16:10:52.162",
        "16:11:32.988", "16:11:33.917",
        "16:11:37.669", "16:11:38.654",
        "16:11:39.757", "16:11:40.634",
        "16:11:44.795", "16:11:45.712",
        "16:11:46.359", "16:11:47.236",
        "16:11:56.332", "16:11:57.267",
        "16:11:58.383", "16:11:59.656",
        "16:12:00.156", "16:12:00.971",
        "16:12:01.758", "16:12:02.701",
        "16:12:05.569", "16:12:06.600",
        "16:12:32.541", "16:12:33.474",
        "16:12:56.672", "16:12:57.558",
        "16:13:03.006", "16:13:03.931",
        "16:13:05.997", "16:13:08.112",
        "16:13:08.338", "16:13:09.022",
        "16:13:10.868", "16:13:11.755",
        "16:13:12.092", "16:13:13.023",
        "16:13:17.596", "16:13:18.432",
        "16:13:19.026", "16:13:20.162",
        "16:13:23.868", "16:13:24.776",
        "16:13:25.027", "16:13:26.440",
        "16:54:32.105","16:54:33.889",
        "16:54:35.068","16:54:37.002",
        "16:54:38.378","16:54:40.211",
        "16:54:42.082","16:54:43.769",
        "16:54:45.002","16:54:46.835",
        "16:54:49.099","16:54:50.786",
        "16:54:52.801","16:54:54.587",
        "16:54:55.868","16:54:57.698",
        "16:54:59.618","16:55:01.604",
        "16:55:03.080","16:55:04.864",
        "16:55:06.686","16:55:08.520",
        "16:55:10.043","16:55:11.827",
        "16:55:14.189","16:55:16.073",
        "16:55:17.697","16:55:19.482",
        "16:55:21.201","16:55:23.086",
        "16:55:24.856","16:55:26.890",
        "16:55:29.104","16:55:31.134",
        "16:55:32.415","16:55:34.198",
        "16:55:36.362","16:55:38.247",
        "16:55:39.870","16:55:41.705",
        "16:55:43.574","16:55:45.360",
        "16:55:47.821","16:55:49.755",
        "16:55:50.495","16:55:52.131",
        "16:55:55.575","16:55:57.361",
        "16:55:59.232","16:56:00.967",
        "16:56:03.377","16:56:05.314",
        "16:56:07.626","16:56:09.462",
        "16:56:11.675","16:56:13.413",
        "16:56:15.724","16:56:17.560",
        "16:56:19.727","16:56:21.463",
        "16:56:23.723","16:56:25.360",
        "16:56:27.527","16:56:29.211",
        "16:56:30.343","16:56:31.928",
        "16:56:33.552","16:56:35.138",
        "16:56:37.208","16:56:38.840",
        "16:56:39.576","16:56:41.262",
        "16:56:42.936","16:56:44.621",
        "16:56:45.213","16:56:46.799",
        "16:56:48.666","16:56:50.202",
        "16:56:50.892","16:56:52.426",
        "16:56:54.246","16:56:55.882",
        "16:56:56.619","16:56:58.203",
        "16:57:00.123","16:57:01.707",
        "16:57:02.643","16:57:04.229",
        "16:57:05.902","16:57:07.485",
        "16:57:08.815","16:57:10.602",
        "16:57:11.980","16:57:13.566",
        "16:57:16.126","16:57:17.911",
        "16:57:18.746","16:57:20.532",
        "16:57:23.137","16:57:24.728",
        "16:57:26.300","16:57:27.785",
        "16:57:29.015","16:57:30.554",
        "16:57:32.423","16:57:33.861",
        "16:57:34.990","16:57:36.475",
        "16:57:38.050","16:57:39.687",
        "16:57:40.719","16:57:42.204",
        "16:57:44.176","16:57:45.762",
        "16:57:46.355","16:57:47.892",
        "16:57:50.053","16:57:51.690",
        "16:57:52.328","16:57:53.865",
        "16:57:56.079","16:57:57.666",
        "16:57:58.602","16:58:00.136",
        "16:58:02.551","16:58:04.033",
        "16:58:04.869","16:58:06.307"
    };
  
    // Iterate through the array
  for (int i = 0; i < timepairs.length; i++) {
    // Check if the current element is equal to the value we are checking
    if (timepairs[i].equals(valueToCheck)) {
      // If found, return true
      print("\n\nfound!\n\n");
      return true;
    }
  }
  return false;
}

void keyPressed() {
  //println("Key pressed: " + key + ", ASCII value: " + int(key));
  // Press any arrow key to pause and move back or forward one frame
  if (key == 'a') {
    if (currentRow > 0) {
      currentRow--;
      drawRow();
      redraw();
      delay(3);
    }
  } else if (key == 'd') {
    if (currentRow < lines.length - 2) {
      currentRow++;
      drawRow();
      redraw();
      delay(3);
    }
  } else if (key == ' ') {
    // Press space to pause or resume the animation
    paused = !paused;
    if (!paused) {
      redraw();
    }
  }
}
