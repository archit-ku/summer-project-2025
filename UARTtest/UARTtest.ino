void setup() {
  // Begin USB Serial communication (to Serial Monitor)
  Serial.begin(9600);
  while (!Serial); // Wait for Serial Monitor to open

  // Begin Serial1 (UART on pins 0 and 1)
  Serial1.begin(600); // Change baud rate if your UART device uses a different one
}


//try to read name a single time
String readName(){
  String reading;
  String names[20];
  int counts[20];
  int totalNames = 0;
  char VA0;
  for(int i = 0; i < 80; i++){
    if(Serial1.available()){
      VA0 = Serial1.read();
      reading += VA0;
    }
    delay(2);
  }
  
  int i = 0;
  while (i < reading.length()) {
    
    if (reading.charAt(i) == '#') {
      int start = i + 1;
      int end = start + 3;
      if (end <= reading.length()) {
        String name = reading.substring(start, end);
        
        //check all valid chars
        bool valid = true;
        for (int j = 0; j < 3; ++j) {
          if (!isAlpha(name.charAt(j))) {
            valid = false;
            break;
          }
        }

        if (valid) {
          bool found = false;
          for (int j = 0; j < totalNames; ++j) {
            if (names[j] == name) {
              counts[j]++;
              found = true;
              break;
            }
          }
          if (!found && totalNames < 20) {
            names[totalNames] = name;
            counts[totalNames] = 1;
            totalNames++;
          }
        }
      }
    }
    i++;
  }

  if (totalNames == 0) {
    return "unknown";
  }

  //find modal name
  int maxCount = counts[0];
  String mostCommon = names[0];
  for (int j = 1; j < totalNames; ++j) {
    if (counts[j] > maxCount) {
      maxCount = counts[j];
      mostCommon = names[j];
    }
  }

  return mostCommon;
}


void loop() {

  if (Serial.available()) {
    char key = Serial.read();
    if (key == 'r') {
      String name = readName();
      Serial.println("name guess: " + name);
      Serial.println("finished read");
    }
  }
}