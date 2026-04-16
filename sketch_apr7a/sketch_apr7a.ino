#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <ArduinoJson.h>

// --- CẤU HÌNH WIFI (KẾT NỐI TRỰC TIẾP) ---
const char* ssid = "703 - 704 - 719";      // Điền tên WiFi nhà bạn vào đây
const char* password = "23456789"; // Điền mật khẩu WiFi vào đây

// --- CẤU HÌNH HIVEMQ CLOUD ---
const char* mqtt_server = "b47231f3e05d4693806682d3f94b7e9b.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

// ĐIỀN TÀI KHOẢN/MẬT KHẨU HIVEMQ CỦA BẠN VÀO ĐÂY
const char* mqtt_username = "esp32"; 
const char* mqtt_password = "ESP32pass"; 

// --- CÁC TOPIC MQTT ĐỂ NHẬN DỮ LIỆU ĐIỀU KHIỂN ---
const char* topic_light = "home/livingroom/light";
const char* topic_water = "home/garden/irrigation";
const char* topic_auto = "home/garden/auto";

// --- CẤU HÌNH CHÂN (PINS) ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define I2C_SDA 22    
#define I2C_SCL 23    
#define LED1_PIN 26   // LED Vàng
#define LED2_PIN 25   // Relay

// --- KHỞI TẠO ĐỐI TƯỢNG ---
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
float previousLux = 100.0;
bool isAutoMode = false;
unsigned long autoPauseEndTime = 0;
unsigned long pumpEndTime = 0;
unsigned long lampEndTime = 0;
float currentTemp = 0, currentHum = 0;

// --- HÀM XỬ LÝ KHI NHẬN ĐƯỢC DỮ LIỆU MQTT ---
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Đã nhận tin nhắn từ Topic [");
  Serial.print(topic);
  Serial.print("]: ");
  
  String message = "";
  for (int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.println(message);

  StaticJsonDocument<200> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("Lỗi phân tích JSON: ");
    Serial.println(error.c_str());
    return;
  }

  // Xử lý điều khiển thủ công từ MQTT
  if (String(topic) == topic_light) {
    if (doc.containsKey("led")) {
      bool ledState = doc["led"];
      digitalWrite(LED1_PIN, ledState ? HIGH : LOW);
      Serial.print("=> (MQTT) LED 1: "); Serial.println(ledState ? "BẬT" : "TẮT");
      // Ghi đè thủ công: Tạm dừng Auto 10s
      if (isAutoMode) {
        autoPauseEndTime = millis() + 10000;
        Serial.println("=> (AUTO) Phát hiện điều khiển thủ công: Tạm dừng Auto 10s");
      }
    }
  }
  else if (String(topic) == topic_water) {
    if (doc.containsKey("water")) {
      bool waterState = doc["water"];
      digitalWrite(LED2_PIN, waterState ? HIGH : LOW);
      Serial.print("=> (MQTT) BƠM: "); Serial.println(waterState ? "BẬT" : "TẮT");
      // Ghi đè thủ công: Tạm dừng Auto 10s
      if (isAutoMode) {
        autoPauseEndTime = millis() + 10000;
        Serial.println("=> (AUTO) Phát hiện điều khiển thủ công: Tạm dừng Auto 10s");
      }
    }
  }
  else if (String(topic) == topic_auto) {
    if (doc.containsKey("auto")) {
      isAutoMode = doc["auto"];
      Serial.print("=> (MQTT) Chế độ AUTO: "); Serial.println(isAutoMode ? "BẬT" : "TẮT");
    }
  }
}

// Hàm kết nối lại MQTT (Tối ưu chống treo mạch)
void reconnect() {
  while (!client.connected()) {
    Serial.print("Đang kết nối MQTT... ");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    yield(); // Nhường CPU để tránh lỗi Watchdog Timer

    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Thành công!");
      client.subscribe(topic_light);
      client.subscribe(topic_water);
      client.subscribe(topic_auto);
    } else {
      Serial.print("Thất bại, mã lỗi: ");
      Serial.print(client.state());
      Serial.println(" Thử lại sau 5 giây");
      
      // Chia nhỏ delay
      for(int i = 0; i < 50; i++) {
        delay(100);
        yield();
      }
    }
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println("\n--- BẮT ĐẦU KHỞI TẠO ---");

  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  
  digitalWrite(LED1_PIN, LOW);
  digitalWrite(LED2_PIN, LOW);

  dht.begin();
  Wire.begin(I2C_SDA, I2C_SCL);

  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("BH1750: OK");
  } else {
    Serial.println("BH1750: Lỗi!");
  }

  // --- KẾT NỐI WIFI CỨNG (HARDCODE) ---
  Serial.println();
  Serial.print("Đang kết nối vào WiFi: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  // Đợi cho đến khi kết nối thành công
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nĐã kết nối WiFi thành công!");
  Serial.print("Địa chỉ IP: ");
  Serial.println(WiFi.localIP());
  // ------------------------------------

  espClient.setInsecure();
  client.setBufferSize(1024); // Mở rộng buffer tránh văng MQTT
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  unsigned long now = millis();

  // --- QUẢN LÝ THỜI GIAN TẮT THIẾT BỊ (AUTO) ---
  if (pumpEndTime > 0 && now > pumpEndTime) {
    digitalWrite(LED2_PIN, LOW);
    pumpEndTime = 0;
    Serial.println("=> (AUTO) Hết 10s: Đã tắt BƠM");
  }
  if (lampEndTime > 0 && now > lampEndTime) {
    digitalWrite(LED1_PIN, LOW);
    lampEndTime = 0;
    Serial.println("=> (AUTO) Hết 10s: Đã tắt ĐÈN SƯỞI");
  }

  // --- ĐỌC CẢM BIẾN & GỬI MQTT (MỖI 10 GIÂY) ---
  if (now - lastMsg > 10000) {
    lastMsg = now;

    currentHum = dht.readHumidity();
    currentTemp = dht.readTemperature();
    float lux = lightMeter.readLightLevel();

    if (!isnan(currentHum) && !isnan(currentTemp)) {
      client.publish("esp32/nhietdo", String(currentTemp).c_str());
      client.publish("esp32/doam", String(currentHum).c_str());
      client.publish("esp32/anhsang", String(lux).c_str());
      
      Serial.printf("T: %.1f C | H: %.1f %% | L: %.1f lx\n", currentTemp, currentHum, lux);
    }

    // --- LOGIC TỰ ĐỘNG THÔNG MINH ---
    if (isAutoMode) {
      if (now < autoPauseEndTime) {
        Serial.println("=> (AUTO) Đang tạm dừng do điều khiển thủ công...");
      } else {
        // Kiểm tra độ ẩm và nhiệt độ
        if (currentHum < 40.0 && pumpEndTime == 0) {
          digitalWrite(LED2_PIN, HIGH);
          pumpEndTime = now + 10000;
          Serial.println("=> (AUTO) Độ ẩm < 40%: Bật BƠM 10s");
        }
        
        if (currentTemp < 35.0 && lampEndTime == 0) {
          digitalWrite(LED1_PIN, HIGH);
          lampEndTime = now + 10000;
          Serial.println("=> (AUTO) Nhiệt độ < 35C: Bật ĐÈN SƯỞI 10s");
        }
      }
    }
  }
}