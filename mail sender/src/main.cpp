#include <WiFi.h>
#include <ESP_Mail_Client.h>

#define WIFI_SSID "your WIFI_SSID"
#define WIFI_PASSWORD "your WIFI_PASSWORD"

//your credential
#define SMTP_SERVER "smtp.gmail.com"
#define SMTP_PORT 465
#define SENDER_EMAIL "the sender"
#define SENDER_PASSWORD "password of the app you generate"
#define RECIPIENT_EMAIL "the recipient"
#define RECIPIENT_NAME ""

SMTPSession smtp;

 // the function for connecting to wifi
void connectWiFi() {                                
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

// the function for sending email
void sendEmail(const String& messageText) {           
  ESP_Mail_Session session;
  session.server.host_name = SMTP_SERVER;
  session.server.port = SMTP_PORT;
  session.login.email = SENDER_EMAIL;
  session.login.password = SENDER_PASSWORD;
  session.login.user_domain = "";

  SMTP_Message message;
  message.sender.name = "ESP32";
  message.sender.email = SENDER_EMAIL;
  message.subject = "ESP32 test";
  message.addRecipient(RECIPIENT_NAME, RECIPIENT_EMAIL);
  message.text.content = messageText.c_str();
  message.text.charSet = "us-ascii";
  message.text.transfer_encoding = Content_Transfer_Encoding::enc_7bit;

  if (!smtp.connect(&session)) {
    Serial.println("Email Sent");
    return;
  }

  if (!MailClient.sendMail(&smtp, &message)) {
    Serial.print("Error sending Email, ");
    Serial.println(smtp.errorReason());
  }
}

void setup() {
  Serial.begin(115200);
  connectWiFi();
}

void loop() {
  String msg = "hello from esp:";//the message to be sent
  sendEmail(msg);
  delay(60000);}




