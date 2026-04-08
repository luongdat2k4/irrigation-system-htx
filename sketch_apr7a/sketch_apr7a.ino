#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <BH1750.h>
#include <DHT.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>

// --- CẤU HÌNH HIVEMQ CLOUD ---
const char* mqtt_server = "2bc93b5857b249d3988054d99e413b9a.s1.eu.hivemq.cloud";
const int mqtt_port = 8883;

// ĐIỀN TÀI KHOẢN/MẬT KHẨU HIVEMQ CỦA BẠN VÀO ĐÂY
const char* mqtt_username = "esp32"; 
const char* mqtt_password = "Ltdat2004@"; 

// --- CÁC TOPIC MQTT ĐỂ NHẬN DỮ LIỆU ---
const char* topic_light = "home/livingroom/light";
const char* topic_water = "home/garden/irrigation";

// --- CẤU HÌNH CHÂN (PINS) ---
#define DHTPIN 4
#define DHTTYPE DHT11
#define I2C_SDA 22
#define I2C_SCL 23
#define LED1_PIN 26
#define LED2_PIN 25

// --- KHỞI TẠO ĐỐI TƯỢNG ---
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
WiFiClientSecure espClient;
PubSubClient client(espClient);

unsigned long lastMsg = 0;
float previousLux = 100.0; // Thêm biến lưu trữ độ sáng của lần đọc trước đó (Khởi tạo > 20 để có thể kích hoạt lần đầu)

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
      if (ledState == false) {
        digitalWrite(LED1_PIN, LOW);
        Serial.println("=> (MQTT) Đã TẮT LED 1");
      } else {
        digitalWrite(LED1_PIN, HIGH);
        Serial.println("=> (MQTT) Đã BẬT LED 1");
      }
    }
  }
  else if (String(topic) == topic_water) {
    if (doc.containsKey("water")) {
      bool waterState = doc["water"];
      if (waterState == false) {
        digitalWrite(LED2_PIN, LOW);
        Serial.println("=> (MQTT) Đã TẮT BƠM/LED 2");
      } else {
        digitalWrite(LED2_PIN, HIGH);
        Serial.println("=> (MQTT) Đã BẬT BƠM/LED 2");
      }
    }
  }
}

// Hàm kết nối lại MQTT
void reconnect() {
  while (!client.connected()) {
    Serial.print("Đang kết nối MQTT... ");
    String clientId = "ESP32Client-";
    clientId += String(random(0xffff), HEX);
    
    if (client.connect(clientId.c_str(), mqtt_username, mqtt_password)) {
      Serial.println("Thành công!");
      client.subscribe(topic_light);
      client.subscribe(topic_water);
    } else {
      Serial.print("Thất bại, mã lỗi: ");
      Serial.print(client.state());
      Serial.println(" Thử lại sau 5 giây");
      delay(5000);
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

  WiFiManager wm;
  Serial.println("Đang kiểm tra kết nối WiFi...");
  
  bool res = wm.autoConnect("ESP32_ThietLap", "12345678"); 

  if(!res) {
      Serial.println("Không thể kết nối WiFi. Hãy khởi động lại mạch.");
  } else {
      Serial.println("Đã kết nối WiFi thành công!");
      Serial.print("Địa chỉ IP: ");
      Serial.println(WiFi.localIP());
  }

  espClient.setInsecure();
  client.setServer(mqtt_server, mqtt_port);
  client.setCallback(callback); 
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop(); 

  unsigned long now = millis();
  if (now - lastMsg > 10000) {
    lastMsg = now;

    float h = dht.readHumidity();
    float t = dht.readTemperature();
    float lux = lightMeter.readLightLevel();

    if (!isnan(h) && !isnan(t)) {
      Serial.print("Nhiệt độ: "); Serial.print(t); Serial.print(" °C | ");
      Serial.print("Độ ẩm: "); Serial.print(h); Serial.print(" % | ");
      Serial.print("Ánh sáng: "); Serial.print(lux); Serial.println(" lx");

      client.publish("esp32/nhietdo", String(t).c_str());
      client.publish("esp32/doam", String(h).c_str());
      client.publish("esp32/anhsang", String(lux).c_str());
    }

    // --- CƠ CHẾ TỰ ĐỘNG THÔNG MINH (CHỈ KÍCH HOẠT 1 LẦN KHI ĐIỀU KIỆN XẢY RA) ---
    // Chỉ tự động tắt nếu lúc trước ánh sáng > 20lx và bây giờ <= 20lx
    if (previousLux > 20.0 && lux <= 20.0) {
      digitalWrite(LED1_PIN, LOW);
      digitalWrite(LED2_PIN, LOW);
      Serial.println("=> (AUTO) Ánh sáng giảm <= 20lx: Đã tự động TẮT cả 2 LED.");
    }
    
    // Lưu lại giá trị ánh sáng hiện tại để so sánh cho chu kỳ 10 giây tiếp theo
    previousLux = lux; 
  }
}