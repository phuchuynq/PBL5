#include <SPI.h>
#include <MFRC522.h>
#include <ESP32Servo.h>  // Thư viện Servo

#define SS_PIN 16
#define RST_PIN 17
MFRC522 rfid(SS_PIN, RST_PIN);
String uidString;

#include "WiFi.h"
#include <HTTPClient.h>
const char* ssid = "ARMY";  //--> Your wifi name
const char* password = "06202901@@";

// Google script Web_App_URL.
String Web_App_URL = "https://script.google.com/macros/s/AKfycbyT9cMn9fAPYvC9HbLiobc99RVDBhU_K0Yp8fjtJDzXKyqX_oiWb_lMtF1ErA4dF0YG7g/exec";

#define On_Board_LED_PIN 2
#define MAX_STUDENTS 10 // Số lượng nhân viên tối đa
struct Student {
  String id;
  char code[10];
  char name[30];
};
Student students[MAX_STUDENTS];
int studentCount = 0;
int runMode = 2;
const int btnIO = 15;
const int buttonPin = 14;  // Chân GPIO cho nút bấm mới
bool buttonState = HIGH;   // Trạng thái ban đầu của nút bấm
bool btnIOState = HIGH;
unsigned long timeDelay = millis();
unsigned long timeDelay2 = millis();
bool InOutState = 0; // 0: vào, 1: ra
const int ledIO = 4;
const int buzzer = 5;

// Khai báo và khởi tạo đối tượng Servo
Servo doorServo;
const int servoPin = 33; // Chân GPIO điều khiển động cơ servo
const int openAngle = 90; // Góc mở cửa
const int closeAngle = 0; // Góc đóng cửa

// * Thêm phần khai báo cảm biến hồng ngoại *
#define IR_SENSOR_PIN 32  // Chân GPIO cho cảm biến hồng ngoại

void setup() {
  Serial.begin(115200);
  pinMode(buzzer, OUTPUT);
  digitalWrite(buzzer, LOW);
  pinMode(btnIO, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);  // Thiết lập nút bấm với điện trở kéo lên
  pinMode(ledIO, OUTPUT);
  pinMode(On_Board_LED_PIN, OUTPUT);
  
  // * Thiết lập chân cho cảm biến hồng ngoại *
  pinMode(IR_SENSOR_PIN, INPUT);

  // Khởi tạo động cơ servo
  doorServo.attach(servoPin);
  doorServo.write(closeAngle); // Đảm bảo cửa đóng khi khởi động
  
  Serial.println();
  Serial.println("-------------");
  Serial.println("WIFI mode : STA");
  Serial.println("-------------");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }
  if (!readDataSheet()) Serial.println("Can't read data from Google Sheet!");

  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522
}

void loop() {
  if (digitalRead(buttonPin) == LOW) { // Nếu nút bấm được nhấn
    if (buttonState == HIGH) { // Trạng thái nút bấm chuyển từ chưa nhấn sang nhấn
      openDoor();  // Mở cửa
      buttonState = LOW;  // Cập nhật trạng thái nút bấm
    }
  } else {
    buttonState = HIGH; // Cập nhật lại trạng thái khi nút bấm không được nhấn
  }

  // Phần còn lại của hàm loop() không thay đổi
  if (millis() - timeDelay2 > 500) {
    readUID();
    timeDelay2 = millis();
  }
  
  // Kiểm tra trạng thái của nút bấm hiện có
  if (digitalRead(btnIO) == LOW) {
    if (btnIOState == HIGH) {
      if (millis() - timeDelay > 500) {
        // Thực hiện lệnh
        InOutState = !InOutState;
        digitalWrite(ledIO, InOutState);
        timeDelay = millis();
      }
      btnIOState = LOW;
    }
  } else {
    btnIOState = HIGH;
  }
}

// Hàm báo hiệu bằng buzzer
void beep(int n, int d) {
  for (int i = 0; i < n; i++) {
    digitalWrite(buzzer, HIGH);
    delay(d);
    digitalWrite(buzzer, LOW);
    delay(d);
  }
}

void readUID() {
  // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
  MFRC522::MIFARE_Key key;
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
  }
  // Look for new cards
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  // Now a card is selected. The UID and SAK is in mfrc522.uid.
  uidString = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    uidString.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    uidString.concat(String(rfid.uid.uidByte[i], HEX));
  }
  uidString.toUpperCase();
  Serial.println("Card UID: " + uidString);
  beep(1, 200);
  
  // Kiểm tra tính hợp lệ của UID và điều khiển động cơ servo
  if (isUIDValid()) {
    // Chỉ mở cửa nếu đang ở trạng thái VÀO
    if (InOutState == 0) {
      openDoor();
    } else {
      Serial.println("Trạng thái RA, không mở cửa.");
    }

    // Ghi log
    if (runMode == 1) writeUIDSheet();
    else if (runMode == 2) writeLogSheet();
  } else {
    Serial.println("UID không hợp lệ.");
    beep(3, 500);
  }

  // Dump PICC type
  byte piccType = rfid.PICC_GetType(rfid.uid.sak);
  if (piccType != MFRC522::PICC_TYPE_MIFARE_MINI
      && piccType != MFRC522::PICC_TYPE_MIFARE_1K
      && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
    return;
  }
}

bool isUIDValid() {
  for (int i = 0; i < studentCount; i++) {
    if (strcmp(students[i].code, uidString.c_str()) == 0) { // Chuyển đổi uidString thành chuỗi ký tự
      return true;
    }
  }
  return false;
}

bool readDataSheet() {
  if (WiFi.status() == WL_CONNECTED) {
    digitalWrite(On_Board_LED_PIN, HIGH);

    // Create a URL for reading or getting data from Google Sheets.
    String Read_Data_URL = Web_App_URL + "?sts=read";

    Serial.println();
    Serial.println("-------------");
    Serial.println("Read data from Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Read_Data_URL);

    //::::::::::::::::::The process of reading or getting data from Google Sheets.
    HTTPClient http;

    http.begin(Read_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    String payload;
    studentCount = 0;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload); 

      char charArray[payload.length() + 1];
      payload.toCharArray(charArray, payload.length() + 1);
      int numberOfElements = countElements(charArray, ',');
      Serial.println("Number of elements: " + String(numberOfElements));
      char *token = strtok(charArray, ",");
      while (token != NULL && studentCount < numberOfElements / 3) {
        students[studentCount].id = atoi(token); // Chuyển đổi ID từ chuỗi sang số nguyên
        token = strtok(NULL, ",");
        strcpy(students[studentCount].code, token); // Sao chép mã nhân viên
        token = strtok(NULL, ",");
        strcpy(students[studentCount].name, token); // Sao chép tên nhân viên
        
        studentCount++;
        token = strtok(NULL, ",");
      }
      
      for (int i = 0; i < studentCount; i++) {
        Serial.print("ID: ");
        Serial.println(students[i].id);
        Serial.print("Code: ");
        Serial.println(students[i].code);
        Serial.print("Name: ");
        Serial.println(students[i].name);
      }
    }

    http.end();
    digitalWrite(On_Board_LED_PIN, LOW);
    Serial.println("-------------");
    return studentCount > 0;
  }
  return false;
}

void writeUIDSheet() {
  String Send_Data_URL = Web_App_URL + "?sts=writeuid";
  Send_Data_URL += "&uid=" + uidString;
  Serial.println();
  Serial.println("-------------");
  Serial.println("Send data to Google Spreadsheet...");
  Serial.print("URL : ");
  Serial.println(Send_Data_URL);
  HTTPClient http;

  http.begin(Send_Data_URL.c_str());
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

  int httpCode = http.GET(); 
  Serial.print("HTTP Status Code : ");
  Serial.println(httpCode);

  String payload;
  if (httpCode > 0) {
    payload = http.getString();
    Serial.println("Payload : " + payload);    
  }
  
  http.end();
}

void writeLogSheet() {
  char charArray[uidString.length() + 1];
  uidString.toCharArray(charArray, uidString.length() + 1);
  char* studentName = getStudentNameById(charArray);
  if (studentName != nullptr) {
    Serial.print("Tên nhân viên với ID ");
    Serial.print(uidString);
    Serial.print(" là: ");
    Serial.println(studentName);

    String Send_Data_URL = Web_App_URL + "?sts=writelog";
    Send_Data_URL += "&uid=" + uidString;
    Send_Data_URL += "&name=" + urlencode(String(studentName));
    if (InOutState == 0) {
      Send_Data_URL += "&inout=" + urlencode("VÀO");
    } else {
      Send_Data_URL += "&inout=" + urlencode("RA");
    }

    Serial.println();
    Serial.println("-------------");
    Serial.println("Send data to Google Spreadsheet...");
    Serial.print("URL : ");
    Serial.println(Send_Data_URL);
    HTTPClient http;

    http.begin(Send_Data_URL.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET(); 
    Serial.print("HTTP Status Code : ");
    Serial.println(httpCode);

    String payload;
    if (httpCode > 0) {
      payload = http.getString();
      Serial.println("Payload : " + payload);    
    }
    
    http.end();
  } else {
    Serial.print("Không tìm thấy nhân viên với ID ");
    Serial.println(uidString);
    beep(3, 500);
  }
}

String urlencode(String str) {
  String encodedString = "";
  char c;
  char code0;
  char code1;
  char code2;
  for (int i = 0; i < str.length(); i++) {
    c = str.charAt(i);
    if (c == ' ') {
      encodedString += '+';
    } else if (isalnum(c)) {
      encodedString += c;
    } else {
      code1 = (c & 0xf) + '0';
      if ((c & 0xf) > 9) {
        code1 = (c & 0xf) - 10 + 'A';
      }
      c = (c >> 4) & 0xf;
      code0 = c + '0';
      if (c > 9) {
        code0 = c - 10 + 'A';
      }
      code2 = '\0';
      encodedString += '%';
      encodedString += code0;
      encodedString += code1;
    }
    yield();
  }
  return encodedString;
}

char* getStudentNameById(char* uid) {
  for (int i = 0; i < studentCount; i++) {
    if (strcmp(students[i].code, uid) == 0) {
      return students[i].name;
    }
  }
  return nullptr; // Trả về nullptr nếu không tìm thấy
}

int countElements(const char* data, char delimiter) {
  // Tạo một bản sao của chuỗi dữ liệu để tránh thay đổi chuỗi gốc
  char dataCopy[strlen(data) + 1];
  strcpy(dataCopy, data);
  
  int count = 0;
  char* token = strtok(dataCopy, &delimiter);
  while (token != NULL) {
    count++;
    token = strtok(NULL, &delimiter);
  }
  return count;
}

void openDoor() {
  doorServo.write(openAngle);  // Mở cửa
  Serial.println("Cửa mở");
  delay(5000);  // Giữ cửa mở trong 5 giây
  closeDoor();  // Đóng cửa sau 5 giây
}

void closeDoor() {
  // Kiểm tra cảm biến hồng ngoại trước khi đóng cửa
  while (digitalRead(IR_SENSOR_PIN) == LOW) {  // Nếu có vật cản (thường LOW khi phát hiện vật)
    Serial.println("Phát hiện vật cản, dừng cửa.");
    beep(3, 200);  // Báo hiệu có vật cản
    delay(1000);  // Đợi 1 giây trước khi kiểm tra lại
  }
  
  // Khi không còn vật cản, cửa sẽ đóng
  doorServo.write(closeAngle);  // Đóng cửa
  Serial.println("Cửa đóng");
}
