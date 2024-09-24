#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET     -1 // Reset pin gui -1 de bao cho 8266 biet 1306 khong co chan reset
#define SCREEN_ADDRESS 0x3C // dia chi I2P cua OLED
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int   GM65_RX_PIN = 1;
const int   GM65_TX_PIN = 3;
String      qrcode;
SoftwareSerial gm65Serial(GM65_RX_PIN, GM65_TX_PIN);


int D3 = 0;
int count = 0;
const char* ssid = "K_System";
const char* password = "Ksystem!00";
const char* host = "10.0.0.178";  // Địa chỉ IP của ESP1 (Server)
const uint16_t port = 80;

WebSocketsClient webSocket;

void setup() {
  Serial.begin(115200);
  gm65Serial.begin(9600);
  WiFi.begin(ssid, password);
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
  }
  display.clearDisplay();  
  display.setTextColor(WHITE, BLACK);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(ssid, password);
  }
  Serial.println("Wifi Connected");

  pinMode(D3, INPUT_PULLUP);

  webSocket.begin(host, port, "/");
  webSocket.onEvent(webSocketEvent);
  Serial.println("WebSocket client started");
}

void connectToWebSocket() {
  webSocket.begin(host, port, "/");  // Thay thế serverAddress và serverPort bằng thông tin của máy chủ WebSocket
  webSocket.onEvent(webSocketEvent);
}

void loop() {
  webSocket.loop();
  // display.setRotation(0);
  while (gm65Serial.available() > 0) {
    Serial.println(0);
    Serial.println("0");
    String qrcode = gm65Serial.readStringUntil('\n');
    qrcode.trim();
    Serial.println(qrcode);
    delay(300);
    if (qrcode == "Reconnect_Server"){
      Serial.println(qrcode);
      connectToWebSocket();
      // display.fillScreen(SSD1306_WHITE);
      // display.display();
      display.clearDisplay();
    }
    else{
      display.clearDisplay();
      String message = "From Client: " + String(qrcode);
      webSocket.sendTXT(message);
      String Send_to_Server = "Send to Server: " + String(qrcode);
      int16_t xAPName = getCenteredX(Send_to_Server.c_str());
      display.setCursor(xAPName, 10);
      display.println(Send_to_Server);
      Serial.println("Send to Server: " + String(qrcode));
      // Gửi dữ liệu nhị phân
      // uint8_t data[] = {0x01, 0x02, 0x03};
      // webSocket.sendBIN(data, sizeof(data));
      // Serial.println("Send to Server 1");
      display.display();
      delay(1000);
    } 
    qrcode = "";
    display.display();
  }
  int16_t xAPName = getCenteredX("Client");
  display.setCursor(xAPName, 0);
  display.println("Client");
  display.display();
}

int16_t getCenteredX(const char* text) {
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(text, 0, 0, &x1, &y1, &w, &h);
  return (SCREEN_WIDTH - w) / 2;
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch(type) {
    case WStype_DISCONNECTED:
      // display.clearDisplay();
      display.fillScreen(SSD1306_WHITE);
      Serial.println("Disconnected");
      // display.setCursor(30, 50);
      // display.println("Disconnected");
      // display.display();
      // delay(1000);
      // connectToWebSocket();
      break;
    case WStype_CONNECTED:
      display.clearDisplay();
      Serial.println("Connected to server");
      display.setCursor(7, 50);
      display.println("Connected to server");
      display.display(); 
      break;
    case WStype_TEXT:
      display.clearDisplay();
      Serial.println((char*)payload);
      String Data_From_Server = (char*) payload;
      int16_t xAPName = getCenteredX(Data_From_Server.c_str());
      display.setCursor(xAPName, 20);
      display.println(Data_From_Server);
      display.display();
      break;
  }
}


// #include <ESP8266WiFi.h>
// #include <WebSocketsClient.h>

// const char* ssid = "K_System";
// const char* password = "Ksystem!00";
// const char* host = "10.0.0.203";
// const uint16_t port = 81;

// WebSocketsClient webSocket;
// WebSocketsClient webSocketClient;

// void setup() {
//   // ... (tương tự như trên)
//   webSocket.begin(host, port, "/");
//   webSocket.onEvent(onWebSocketEvent);
//   webSocket.setReconnectInterval(5000);
//   webSocketClient.onEvent(webSocketClientEvent);
//   Serial.println("WebSocket CLIENT started.");
// }

// void loop() {
//   webSocket.loop();
//   // Gửi tin nhắn đến server
//   // if (millis() % 5000 == 0) {
//   //   webSocket.sendTXT("Hello from client!");
//   // }
// }

// void onWebSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
//   switch(type) {
    // case WStype_DISCONNECTED:
    //   Serial.println("Disconnected!");
    //   break;
    // case WStype_CONNECTED:
    //   Serial.println("Connected to server!");
    //   break;
//     case WStype_TEXT:
//       Serial.println("Got text: " + String((char*)payload));
//       break;
//   }
// }

// void tx_esp(char* payload) {
//   // server address, port and URL
//   webSocketClient.loop();// constantly check for websocket CLIENT events
//   webSocketClient.begin("10.0.0.203", 81, "/");
//   Serial.println("WebSocket Client started ..sending message");
//   Serial.printf("[WSc] get text: %s\n", payload);
//   webSocketClient.sendTXT("^");
  
//   webSocketClient.disconnect();
//   };
// Hàm onWebSocketEvent tương tự như trên, nhưng chỉ xử lý các sự kiện kết nối, ngắt kết nối và nhận tin nhắn