#include "OTAUpdate.h"
#ifdef ESP32
  WebServer wserver(80);
  
  // OTA Update Pull
  WiFiClient client;
  String hostURL = "192.168.4.1"; // hostURL => bucket-name.s3.region.amazonaws.com
  int port = 3000; // Non https. For HTTPS 443. As of today, HTTPS doesn't work.
  String bin = "/BSides2019CPT-Firmware.ino.esp32.bin"; // bin file name with a slash in front.
  long contentLength = 0;
  bool isValidContentType = false;
  char *passwordAdmin = nullptr;

#endif

#ifdef ESP32
// Utility to extract header value from headers
String getHeaderValue(String header, String headerName) {
  return header.substring(strlen(headerName.c_str()));
}

// OTA Logic 
bool execOTA() {
  Serial.println("Connecting to: " + String(hostURL));
  // Connect to S3
  if (client.connect(hostURL.c_str(), port)) {
    // Connection Succeed.
    // Fecthing the bin
    Serial.println("Fetching Bin: " + String(bin));

    // Get the contents of the bin file
    client.print(String("GET ") + bin + " HTTP/1.1\r\n" +
                 "Host: " + hostURL + "\r\n" +
                 "Cache-Control: no-cache\r\n" +
                 "Connection: close\r\n\r\n");

    // Check what is being sent
    //    Serial.print(String("GET ") + bin + " HTTP/1.1\r\n" +
    //                 "Host: " + hostURL + "\r\n" +
    //                 "Cache-Control: no-cache\r\n" +
    //                 "Connection: close\r\n\r\n");

    unsigned long timeout = millis();
    while (client.available() == 0) {
      if (millis() - timeout > 5000) {
        Serial.println("Client Timeout !");
        client.stop();
        return false;
      }
    }
    // Once the response is available,
    // check stuff

    /*
       Response Structure
        HTTP/1.1 200 OK
        x-amz-id-2: NVKxnU1aIQMmpGKhSwpCBh8y2JPbak18QLIfE+OiUDOos+7UftZKjtCFqrwsGOZRN5Zee0jpTd0=
        x-amz-request-id: 2D56B47560B764EC
        Date: Wed, 14 Jun 2017 03:33:59 GMT
        Last-Modified: Fri, 02 Jun 2017 14:50:11 GMT
        ETag: "d2afebbaaebc38cd669ce36727152af9"
        Accept-Ranges: bytes
        Content-Type: application/octet-stream
        Content-Length: 357280
        Server: AmazonS3
                                   
        {{BIN FILE CONTENTS}}
    */
    while (client.available()) {
      // read line till /n
      String line = client.readStringUntil('\n');
      // remove space, to check if the line is end of headers
      line.trim();

      // if the the line is empty,
      // this is end of headers
      // break the while and feed the
      // remaining `client` to the
      // Update.writeStream();
      if (!line.length()) {
        //headers ended
        break; // and get the OTA started
      }

      // Check if the HTTP Response is 200
      // else break and Exit Update
      if (line.startsWith("HTTP/1.1")) {
        if (line.indexOf("200") < 0) {
          Serial.println("Got a non 200 status code from server. Exiting OTA Update.");
          break;
        }
      }

      // extract headers here
      // Start with content length
      if (line.startsWith("Content-Length: ")) {
        contentLength = atol((getHeaderValue(line, "Content-Length: ")).c_str());
        Serial.println("Got " + String(contentLength) + " bytes from server");
      }

      // Next, the content type
      if (line.startsWith("Content-Type: ")) {
        String contentType = getHeaderValue(line, "Content-Type: ");
        Serial.println("Got " + contentType + " payload.");
        if (contentType == "application/octet-stream") {
          isValidContentType = true;
        }
      }
    }
  } else {
    // Connect to S3 failed
    // May be try?
    // Probably a choppy network?
    Serial.println("Connection to " + String(hostURL) + " failed. Please check your setup");
    // retry??
    // execOTA();
  }

  // Check what is the contentLength and if content type is `application/octet-stream`
  Serial.println("contentLength : " + String(contentLength) + ", isValidContentType : " + String(isValidContentType));

  // check contentLength and content type
  if (contentLength && isValidContentType) {
    // Check if there is enough to OTA Update
    bool canBegin = Update.begin(contentLength);

    // If yes, begin
    if (canBegin) {
      Serial.println("Begin OTA. This may take 2 - 5 mins to complete. Things might be quite for a while.. Patience!");
      // No activity would appear on the Serial monitor
      // So be patient. This may take 2 - 5mins to complete
      size_t written = Update.writeStream(client);

      if (written == contentLength) {
        Serial.println("Written : " + String(written) + " successfully");
      } else {
        Serial.println("Written only : " + String(written) + "/" + String(contentLength) + ". Retry?" );
        // retry??
        // execOTA();
      }

      if (Update.end()) {
        Serial.println("OTA done!");
        if (Update.isFinished()) {
          Serial.println("Update successfully completed. Rebooting.");
          return true;
        } else {
          Serial.println("Update not finished? Something went wrong!");
        }
      } else {
        Serial.println("Error Occurred. Error #: " + String(Update.getError()));
      }
    } else {
      // not enough space to begin OTA
      // Understand the partitions and
      // space availability
      Serial.println("Not enough space to begin OTA");
      client.flush();
    }
  } else {
    Serial.println("There was no content in the response");
    client.flush();
  }

  return false;
}

#endif

void ArdunioOTASetup(GameBuff *gameBuff) {
#ifdef ESP32
  passwordAdmin = gameBuff->badgeState->bt_addr;
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS
        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void OTASetup(GameBuff *gameBuff) {
  int i = 0;
	displayClear(gameBuff, 0x00);
  drawString2x(gameBuff,(char*)"Starting Wifi",0,16*i++,0xFF,0);
#ifdef ESP32
  // Connect to WiFi network
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  drawString2x(gameBuff,(char*)"Connected to",0,16*i++,0xFF,0);
  drawString2x(gameBuff,(char *)ssid,0,16*i++,0xFF,0);
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP().toString());
  Serial.print("PW : ");
  Serial.println(passwordAdmin);
  char line[30];
  IPAddress ip = WiFi.localIP();
  sprintf(line, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  drawString2x(gameBuff,line,0,16*i++,0xFF,0);

  /*use mdns for host name resolution*/
  if (!MDNS.begin(host)) { //http://esp32.local
    Serial.println("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }
  Serial.println("mDNS responder started");
  /*return index page which is stored in serverIndex */
  wserver.on("/", HTTP_GET, []() {
    wserver.sendHeader("Connection", "close");
    
    unsigned int const sz1  = strlen(loginIndex);
    unsigned int const sz2  = strlen(passwordAdmin);
    unsigned int const sz3  = strlen(loginIndex2);    
    
    char *concat            = (char*)malloc(sz1+sz2+sz3+1);
    memcpy( concat         , loginIndex  , sz1 );
    memcpy( concat+sz1     , passwordAdmin , sz2 );
    memcpy( concat+sz1+sz2 , loginIndex2 , sz3 );
    concat[sz1+sz2+sz3] = '\0';

    wserver.send(200, "text/html", concat);
    free(concat);

  });
  wserver.on("/serverIndex", HTTP_GET, []() {
    wserver.sendHeader("Connection", "close");
    wserver.send(200, "text/html", serverIndex);
  });
  /*handling uploading firmware file */
  wserver.on("/update", HTTP_POST, []() {
    wserver.sendHeader("Connection", "close");
    wserver.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
    ESP.restart();
  }, []() {
    HTTPUpload& upload = wserver.upload();
    if (upload.status == UPLOAD_FILE_START) {
      Serial.printf("Update: %s\n", upload.filename.c_str());
      if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_WRITE) {
      /* flashing firmware to ESP*/
      if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
        Update.printError(Serial);
      }
    } else if (upload.status == UPLOAD_FILE_END) {
      if (Update.end(true)) { //true to set the size to the current progress
        Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
      } else {
        Update.printError(Serial);
      }
    }
  });

  Serial.println("Server - Begin");
  wserver.begin();
  Serial.println("Server - Started");
 #endif
  drawString2x(gameBuff,(char*)"Wifi started",0,16*i++,0xFF,0);
  drawString2x(gameBuff,(char*)"2019-09-03",0,16*i++,0xFF,0);

  drawString2x(gameBuff,(char*)"",0,16*i++,0xFF,0);
  drawString2x(gameBuff,(char*)"Press Start ",0,16*i++,0xFF,0);
  drawString2x(gameBuff,(char*)"and Select ",0,16*i++,0xFF,0);
  drawString2x(gameBuff,(char*)"To Update ",0,16*i++,0xFF,0);
}

static bool OTAFirstRun = true;
bool OTAUpdateloop(GameBuff *gameBuff) {
  if (OTAFirstRun) {
    OTASetup(gameBuff);
    ArdunioOTASetup(gameBuff);
    OTAFirstRun = false;
  } else {}

  #ifdef ESP32
  wserver.handleClient();
  ArduinoOTA.handle();
  #endif
  
  if (gameBuff->playerKeys.a && gameBuff->playerKeys.b) {
    #ifdef ESP32
    // Exit screen
    wserver.stop();
    ESP.restart();
    #endif
   
    return true;
  }

  if (gameBuff->playerKeys.start && gameBuff->playerKeys.select || gameBuff->playerKeys.up && gameBuff->playerKeys.down) {
    // Auto update
    #ifdef ESP32
      if (execOTA()) {
      // Success
        ESP.restart();
      } else {
        //Failure
        ESP.restart();
      }
    #endif
    }

  return false;
}
