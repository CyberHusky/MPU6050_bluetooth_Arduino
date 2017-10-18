import processing.serial.*;

Serial mySerial;
PrintWriter output; // for saving data to txt file
PShape MPU_sensor;
PImage MPU_img;

String value;
int value_int;
float [] value_split;

int key_count = 0;



void setup() {
// ========= Draw MPU =========  
size(800, 600, P3D);  // make a 3D GUI


MPU_img = loadImage("MPU-6050_2.jpg"); // load MPU image
  
 
background(204);
rectMode(CENTER); // mode for draw rectangle in center of coordinate
//rectMode(RADIUS); 
//rectMode(CORNERS); 
MPU_sensor = createShape(RECT, 0, 0, 40, 50); // draw rectangle of MPU
//MPU_sensor = createShape(RECT, -25, -25, 40, 50);

   
//MPU_sensor.setFill(color(0)); 
MPU_sensor.setTexture(MPU_img); // draw picture on rectangle
// ============================   
  
  
  
// ========= Read Data =========  
  
   mySerial = new Serial( this, Serial.list()[0], 38400 ); // new serial obj
   output = createWriter( "/home/artem/data.txt" ); // way for saving file
   int lf = 10; // symbol of new line in ASCII code
   mySerial.bufferUntil(lf); // wait for new line in input data

 
      // --- Serial connect ---- 
/* We need to inicializate serail port and wait for couple sec 
or we have save not valid input data */
if (mySerial.available() > 0 ) { 
  String value = mySerial.readString();
     if ( value != null ) {
              
     }
}
delay(2000); // Wait for end of serial connecting
       // -----------------------

/* Make ficsation of start recording time in file and other info */ 
/*
String Start_Data ="Data recording start at: " + str(hour())+ ":" + str(minute()) + ":" + str(second());
output.println(Start_Data); // write to file start recording time
output.println("Z gyro  " + "X gyro  " + "Y gyro  " + "Y acc  " + "Y acc  " + "Y acc  "); 
*/
// ========================== 

}







void draw() {

    
    // ----- Data processing --------
if (value != null){
  value_split = float(split(value, ' ')); // separate input data from bluetooth
  println(value_split);
              if (key_count==1){
                output.print(value); // saving data to file  
              }
// value_split[0] - Z , value_split[1] - X , value_split[2] - Y
     
} else { // in normal work else doesn't appear
  value_split[0] = 0;
  value_split[1] = 0;
  value_split[2] = 0;
  
  println(value_split);
}
    
    // -------------------------------------    
    
    
    // ----- Draw MPU -------- 
    
// camera make right side visualization of MPU
camera(width/2.0, height/2.0, (height/2.0) / tan(PI*30.0 / 180.0), width/2.0, height/2.0, 0.0, 0.0, 1.0, 0.0);

translate(400, 400, 0); // First location of all building    

background(204); // 

//float value_split_map_X = map(value_split[2], 180, -180, 3*HALF_PI, -HALF_PI);//  
float value_split_map_X = map(value_split[2], 180, -180, -HALF_PI, 3*HALF_PI);// 
float value_split_map_Y = map(value_split[1], 180, -180, PI, -PI);//   
float value_split_map_Z = map(value_split[0], 180, -180, PI, -PI);//   


//float value_split_map_X = map(value_split[2], -180, 180, 3*HALF_PI, -HALF_PI);// 
//float value_split_map_Y = map(value_split[1], 180, -180, PI, -PI);//   
//float value_split_map_Z = map(value_split[0], 180, -180, PI, -PI);//  




// value_split[0] - Z , value_split[1] - X , value_split[2] - Y
/*float value_split_map_X, value_split_map_Y, value_split_map_Z;
value_split_map_X = map(value_split[2], -180, 180, 3*HALF_PI, -HALF_PI);//  
if (value_split[2] <= 0){
  value_split_map_Y = map(value_split[1], -180, 180, PI, -PI);
} else {
  value_split_map_Y = map(value_split[1], -180, 180, -PI, PI);
}
//value_split_map_Y = map(value_split[1], -180, 180, PI, -PI);//   
value_split_map_Z = map(value_split[0], -180, 180, -PI, PI);// */


/*
float value_split_map_X, value_split_map_Y, value_split_map_Z;

value_split_map_X = map(value_split[2], -180, 180, 3*HALF_PI, -HALF_PI);//  
if (value_split[1]>=0 && value_split[1]<=90){
  value_split_map_Y = map(value_split[1], 0, 90, 0, -HALF_PI );//  
} else {
  value_split_map_Y = map(value_split[1], 0, -90, 0, HALF_PI);//  
}
value_split_map_Z = map(value_split[0], -90, 90, -PI, PI);//   
*/

textSize(14);

//fill(50);

if (key_count==0){fill(255,0,0); text("For start saving tap any key!", -250, -20, 0);}
fill(0, 102, 153); // texture for text
text("Z_gyro: "+value_split[0], -200, 0, 0); // draw gyro input data
text("X_gyro: "+value_split[1], -200, 20, 0);
text("Y_gyro: "+value_split[2], -200, 40, 0);
text("X_acce: "+value_split[3], -200, 60, 0);  // draw accel input data
text("Y_acce: "+value_split[4], -200, 80, 0);
text("Z_acce: "+value_split[5], -200, 100, 0);




rotateX(value_split_map_X); // rotate the picture
rotateY(value_split_map_Y); // rotate the picture
rotateZ(value_split_map_Z); // rotate the picture
// rotateZ(-PI); // rotate the picture


// ---- Build rectangle
texture(MPU_img);
shape(MPU_sensor); // Draw the picture

// ---- Build cuboit
translate(0, 0, -3); // Shift location of all building
box(40, 50, 5); // Draw the cuboit under picture
 

 
// ---- text OS ----
strokeWeight(2);
line(0,0,0,0,0,50);
line(0,0,0,-50,0,0);
line(0,-50,0,0,0,0); //X

rotateX(-HALF_PI); // rotate the picture for drawing a X, Y, Z text
text("Z", 5, -50, 0);
text("X", 0, 15, -50);
text("Y", -60, 5, 0);
    // -------------------------------------  

}


/* Need for starting Serial reading only after first line changing. 
For more information watch void setup (line 44) */
void serialEvent(Serial p) { 
  value = p.readString();
}


// --- if any key on keyboard pressed
void keyPressed() {

key_count++;
      
if(key_count==1){
      
      /* Make ficsation of start recording time in file and other info */   
String Start_Data ="Data recording start at: " + str(day())+ "." + str(month()) + "." + str(year()) + "_" + str(hour())+ ":" + str(minute()) + ":" + str(second());
output.println(Start_Data); // write to file start recording time
output.println("Zgyro  " + "Xgyro " + "Ygyro  " + "X acc   " + "Y acc   " + "Z acc  "); 
      
      
}else if(key_count==2){
      
      String End_Data = "Data recording end at: " + str(hour())+ ":" + str(minute()) + ":" + str(second());
      output.println("\n" + End_Data); // write to file end recording time
  
      output.flush();  // Writes the remaining data to the file
      output.close();  // Finishes the file
      exit();  // Stops the program     
}


    
    
}