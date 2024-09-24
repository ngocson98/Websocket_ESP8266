#include <ESP8266WiFi.h>
#include <WebSocketsServer.h>
#include <iostream>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin gui -1 de bao cho 8266 biet 1306 khong co chan reset
#define SCREEN_ADDRESS 0x3C // dia chi I2P cua OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

IPAddress local_IP(10, 0, 0, 178);
IPAddress gateway(10, 0, 0, 2);
IPAddress subnet(255, 255, 255, 0);
IPAddress primaryDNS(1, 1, 1, 1); //optional
IPAddress secondaryDNS(8, 8, 4, 4); //optional

int D3 = 0;
int count = 0;
const char* ssid = "your_id";
const char* password = "your_pass";


WebSocketsServer WebSocketsServer(80);
// webSocketServer.broadcastTXT(payload, length); để gửi cho tất cả các client.
void setup() {
  if (WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS) == false){
    Serial.println("Faild");
  }
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  }
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  delay(500);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(1000);
    WiFi.begin(ssid, password);
  }
  Serial.println("Wifi_Connected");
  pinMode(D3, INPUT_PULLUP); 
  WebSocketsServer.begin();
  Serial.println("WebSocket server started");
  Serial.println(WiFi.localIP());
  WebSocketsServer.onEvent(onWebSocketEvent);

}

void loop() {
  WebSocketsServer.loop();
  if (digitalRead(D3) == 0){ 
    display.clearDisplay();
    count += 1;
    // Serial.println(count);
    String message = "From Server: " + String(count);
    WebSocketsServer.broadcastTXT(message);
    String Send_to_Client = "Send to Client: " + String(count);
    int16_t xAPName = getCenteredX(Send_to_Client.c_str());
    display.setCursor(xAPName, 10);
    display.print(Send_to_Client);
    // display.println(count);
    Serial.println("Send to Client: " + String(count));
    delay(300);
    display.display();
  }
  int16_t xAPName = getCenteredX("Server");
  display.setCursor(xAPName, 0);
  display.println("Server");
  display.display();
}

int16_t getCenteredX(const char* text) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (SCREEN_WIDTH - w) / 2;
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      Serial.println("Disconnected");
      display.fillScreen(SSD1306_WHITE);
      // display.setCursor(30,50);
      // display.println("Disconnected");
      display.display();
      break;
    case WStype_CONNECTED:
      display.clearDisplay();
      Serial.println("Connected to client");
      display.setCursor(7,50);
      display.println("Connected to client");
      display.display(); 
      break;
    case WStype_TEXT:
      display.clearDisplay();
      Serial.println((char*) payload);
      String Data_From_Client = (char*) payload;
      int16_t xAPName = getCenteredX(Data_From_Client.c_str());
      display.setCursor(xAPName, 20);
      display.println(Data_From_Client);
      display.display();
      WebSocketsServer.broadcastTXT("OK");
      break;
  }
}
// void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
//   switch(type) {
//     case WStype_DISCONNECTED:
//       Serial.println("Disconnected!");
//       break;
//     case WStype_CONNECTED:
//       Serial.println("Connected to server!");
//       break;
//     case WStype_TEXT:
//       Serial.println("Got text: " + String((char*)payload));
//       // Xử lý tin nhắn nhận được từ server
//       break;
//   }
// }


// #include <ESP8266WiFi.h>
// #include <WebSocketsServer.h>
// const char* ssid = "K_System";
// const char* password = "Ksystem!00";
// // ... (các khai báo khác)

// WebSocketsServer webSocketServer(80); // Khởi tạo server trên cổng 81

// void setup() {
//   Serial.begin(115200);
//   WiFi.begin(ssid, password);
//   if (WiFi.waitForConnectResult() != WL_CONNECTED) {
//     Serial.printf("STA: Failed!\n");
//     WiFi.disconnect(false);
//     delay(1000);
//     WiFi.begin(ssid, password);
//   }
//   Serial.println("Wifi_Connected");

//   WebSocketsServer.begin();
//   Serial.println("WebSocket server started");
//   Serial.println(WiFi.localIP());
//   WebSocketsServer.onEvent(onWebSocketEvent);
// }

// void loop() {
//   WebSocketsServer.loop();
// }

// void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {
//   switch(type) {
//     case WStype_TEXT:
//       Serial.printf("[%u] Received TEXT: %s\n", num, payload);
//       // Xử lý dữ liệu nhận được ở đây
//       String message = (char*)payload;
//       break;
//   }
// }
