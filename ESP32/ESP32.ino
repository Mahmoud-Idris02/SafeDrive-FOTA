/*******************************************************************
@file: ESP.ino
@breif: main esp32 program
@author: FOTA Team
*******************************************************************/


#include "BL_SR.h"


void handleRoot();
void handleNotFound();
void handleFileUpload();
void SendFileRecivedACK();
void jumpHandle();


const char* ssid = "Username";
const char* password = "password";


//====Decisions Var==== 
bool decision = false; 
bool isReport = false;
bool critical = false;

//====File Buffer Var====
uint8_t* fileBuffer = nullptr;
size_t bufferIndex = 0;
size_t bufferSize = 0;

//====Server URL====
String serverURL = "http://192.168.1.5:8000/";


WebServer server(80);  // Object of WebServer(HTTP port, 80 is defult)

void setup() {
  //setting the custom uart
  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected");
  
  
  server.on("/", handleRoot);
  server.on("/jump",jumpHandle);
  server.on("/check",checkJumpHandle);
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/plain", "File Uploaded Successfully");
  }, handleFileUpload);
  server.onNotFound(handleNotFound);
  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();
  Serial.println("HTTP server started");
}
void loop() {
  server.handleClient();
}



void handleRoot() {
  
  Serial.println("Comming req");
  

  if (server.args() > 0 ) { 
    if (server.hasArg("critical")) {
      Serial.println("Setting the critical var to TRUE");
      critical = (server.arg(0) == "True");  
    }
    else if (server.hasArg("decision"))
    {
      Serial.println("Setting the Fearure var to TRUE");
      decision = (server.arg(0) == "Accept");  
      Serial.print("The decision is :- ");
      Serial.println(server.arg(0));

    }

  }
  server.send(200, "text/plain", "done");
  
}
void handleFileUpload() 
{
  Serial.println("I'm in handleFileUpload");

  set_jump(false);
  if (critical || decision)
  {
    Serial.println("I'm in if");
    HTTPUpload& upload = server.upload();
    Serial.println(upload.status);
    if (upload.status == UPLOAD_FILE_START) {
      String filename = upload.filename;
      if (!filename.startsWith("/")) {
        filename = "/" + filename;
      }
      Serial.print("Handle file upload name: ");
      Serial.println(filename);
      bufferIndex = 0; // Reset buffer index for new file upload
      bufferSize = 0; // Reset buffer size
      if (fileBuffer) {
        delete[] fileBuffer; // Free any previously allocated buffer
        fileBuffer = nullptr;
      }
    } 
    else if (upload.status == UPLOAD_FILE_WRITE) {
      if (bufferIndex + upload.currentSize > bufferSize) {
        // Reallocate buffer
        size_t newBufferSize = bufferIndex + upload.currentSize;
        uint8_t* newBuffer = new uint8_t[newBufferSize];
        if (fileBuffer) 
        {
          memcpy(newBuffer, fileBuffer, bufferIndex);
          delete[] fileBuffer;
        }
        fileBuffer = newBuffer;
        bufferSize = newBufferSize;
      }
      memcpy(fileBuffer + bufferIndex, upload.buf, upload.currentSize);
      bufferIndex += upload.currentSize;
      Serial.println("File Out");
    } 
    else if (upload.status == UPLOAD_FILE_END) {
      Serial.println("File upload completed");
      Serial.print("File size =");
      Serial.println(bufferSize);
      server.send(200, "text/plain", "File Uploaded Successfully");
      if (decision) {SendFileRecivedACK();}
      printFileContent();
      Send_CMD_Upload_Application(fileBuffer,bufferSize);
      Send_CMD_Jump_To_Application();
    }
  }  
  else
  {
    server.send(200, "text/plain", "No File to upload");

  }
}

void handleNotFound() 
{
  server.send(404, "text/plain", "404: Not found"); 
}
void jumpHandle()
{
  Send_CMD_Jump_To_Application();
}
void SendFileRecivedACK() 
{
    if (WiFi.status() == WL_CONNECTED) {
      WiFiClient client;
      HTTPClient http;
      String serverURLPath = serverURL + "index/files/receive-success/";
      http.begin(client,serverURLPath);  

      http.addHeader("Content-Type", "application/x-www-form-urlencoded"); 

      
      String postData = "accept=1";

      int httpResponseCode = http.POST(postData);

      if (httpResponseCode > 0) {
        String response = http.getString();  
        Serial.println(httpResponseCode); 
        Serial.println(response);  
      } 
      else {
        Serial.print("Error on sending POST: ");
        Serial.println(httpResponseCode);
      }

      http.end();  
  } 
  else {
    Serial.println("WiFi not connected");
  }
}

void printFileContent() 
{
  for (size_t i = 0; i < bufferSize; i++) {
    Serial.print(fileBuffer[i], HEX);
    Serial.print(" ");
  }
  Serial.println();
}

void checkJumpHandle()
{
  if (check_if_jumped())
  {
    server.send(200, "text/plain", "jumped");
  }
  else
  {
    server.send(400, "text/plain", "still");
  }
}
