/*
 * Description: Takes raw string from API call and cleans up all the stuff we dont want
 * 
 * Arguments:
 *  (IN/OUT) apiString -- pointer to string from api call
 *  
 * Returns:
 *  void
 */
void cleanUpString(String * apiString)
{
  int indxBegin = 0;
  int indxEnd = 0;
  
  //Remove the header
  apiString->remove(0,apiString->indexOf("<resultSet ")-1); 
  
  apiString->remove(apiString->indexOf('\n'),1);
  
  
  //remove the newlines and the hexcodes
  while(apiString->indexOf('\n') != -1)
  {
    indxBegin = apiString->indexOf('\n');
    indxEnd = apiString->indexOf('\n', indxBegin+1);
  
    if(indxBegin == -1 || indxEnd == -1)
    {
      break;
    }   
    apiString->remove(indxBegin,(indxEnd-indxBegin)+1);
  }
}



 /*
  * Description: Initalizes the train sruct to zero
  * 
  * Arguments: 
  *   (IN/OUT) trainLine -- pointer to trains struct
  * 
  * Returns:
  *  void
  */
void initTrainStruct(struct trains *trainLine)
{
  trainLine->numOfTrains = 0;
  
  for(uint8_t indx = 0; indx < 49; indx++)
  {
    trainLine->lat[indx] = 0;
  }
  
  for(uint8_t indx = 0; indx < 49; indx++)
  {
    trainLine->lon[indx] = 0;
  }
}



/*
 * Description: Takes string from the api call, cleans it, and
 *  extracts the train lat and lon info and stores it in the trains struct
 *  
 * Arguments:
 *  (IN) apiString -- pointer to string from api call
 *  (OUT) trainLine -- pointer to trains stuct
 *  
 * Returns:
 *  void
 */
void parseApiString(String * apiString, struct trains * trainLine)
{
  uint8_t numOfTrains = 0;
  String numHolder;
  int indxBegin = 0;
  int indxEnd = 0;
  
  cleanUpString(apiString);
  initTrainStruct(trainLine);

  /*Serial.print(*apiString);
  Serial.println();*/

  //Get total number of trains in api data string while also getting the lon and lat data per train
  while(apiString->indexOf("<vehicle") != -1 && numOfTrains < 49)//apiString->indexOf(" longitude=") != -1 && apiString->indexOf(" latitude=") != -1
  {
    //Get the longitude
    indxBegin = apiString->indexOf(" longitude=")+12;//Get the index of the begining of the number
    indxEnd = indxBegin+12;//Get the index just outside char just after number as substring is exclusive for the last char

    numHolder = apiString->substring(indxBegin, indxEnd);
    trainLine->lon[numOfTrains] = numHolder.toDouble();


    //Get the latitude
    indxBegin = apiString->indexOf(" latitude=")+11;
    indxEnd = indxBegin+10;

    numHolder = apiString->substring(indxBegin, indxEnd);
    trainLine->lat[numOfTrains] = numHolder.toDouble();


    //Now remove the vehicle element from the string as we dont need it anymore
    indxBegin = apiString->indexOf("<vehicle ");
    indxEnd = apiString->indexOf(" />")+3;


    if(indxEnd-indxBegin >= apiString->length())
    {
      apiString->remove(indxBegin,apiString->length());
    }
    else
    {
      apiString->remove(indxBegin,abs(indxEnd-indxBegin));
    }


    //Serial.print(*apiString);
    
    if(numOfTrains < 49)
    {
      numOfTrains++;
    }
  }
  trainLine->numOfTrains = numOfTrains;
}


/*
 * Description: Makes a call to the API and stores that data in a string
 * 
 * Arguments:
 *  (IN) routeID -- route number that is being called
 *  (OUT) callResult -- pointer to string that will hold the call result
 *  
 * Return:
 *  false -- call failed
 *  true -- call went through fine
 */
bool callTriMetApi(const uint16_t ROUTE_ID, String * callResult)
{
  Serial.print("connecting to ");
  Serial.println("developer.trimet.org");

  //Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect("developer.trimet.org", httpPort)) {
      Serial.println("connection failed");
      return false;
  }

  //We now create a URI for the request
  String url = "/ws/V2/vehicles/onRouteOnly/true/showStale/false/xml/true/routes/";
  url += ROUTE_ID;
  url += "/appID/";
  url += API_KEY;
  
  Serial.print("Requesting URL: ");
  Serial.println(url);

  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + "developer.trimet.org" + "\r\n" +
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while(client.available() == 0) 
  {
    if(millis() - timeout > 5000) 
    {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return false;
    }
  }

  *callResult = client.readString();
  client.stop();
  client.flush();
  Serial.println("closing connection");
  return true;
}



/*
 * Description: Calcuates what stations have trains at them
 * 
 * Arguments:
 *  none
 *  
 * Returns:
 *  void
 */
void findStationOccupancy()
{
  double latDiff = 0;
  double lonDiff = 0;

  struct stationOccupancy stationFilledOffline;

  //Reset the offline copy of the station occupancy struct
  for(uint8_t indx = 0; indx < 49; indx++)
  {
    if(indx < NUM_RED_STOPS)
    {
      stationFilledOffline.redStation[indx] = false;
    }
    
    if(indx < NUM_BLUE_STOPS)
    {
      stationFilledOffline.blueStation[indx] = false;
    }
    
    if(indx < NUM_GREEN_STOPS)
    {
      stationFilledOffline.greenStation[indx] = false;
    }

    if(indx < NUM_YELLOW_STOPS)
    {
      stationFilledOffline.yellowStation[indx] = false;
    }

    if(indx < NUM_ORANGE_STOPS)
    {
      stationFilledOffline.orangeStation[indx] = false;
    }
  }
  
  //Find occupancy for red line stations
  for(uint8_t trainIndx = 0; trainIndx < redLineTrains.numOfTrains; trainIndx++)
  {
    for(uint8_t stopIndx = 0; stopIndx < NUM_RED_STOPS; stopIndx++)
    {
      latDiff = RED_STATION_BOUNDS.LAT[stopIndx] - redLineTrains.lat[trainIndx];
      latDiff = abs(latDiff);
      
      lonDiff = redLineTrains.lon[trainIndx] - RED_STATION_BOUNDS.LON[stopIndx];
      lonDiff = abs(lonDiff);
      
      if(latDiff <= RED_STATION_BOUNDS.LAT_DIFF_MAX[stopIndx] && lonDiff <= RED_STATION_BOUNDS.LON_DIFF_MAX[stopIndx])
      {
        stationFilledOffline.redStation[stopIndx] = true;
        break;
      }
    }
  }

  //Find occupancy for blue line stations
  for(uint8_t trainIndx = 0; trainIndx < blueLineTrains.numOfTrains; trainIndx++)
  {
    for(uint8_t stopIndx = 0; stopIndx < NUM_BLUE_STOPS; stopIndx++)
    {
      latDiff = BLUE_STATION_BOUNDS.LAT[stopIndx] - blueLineTrains.lat[trainIndx];
      latDiff = abs(latDiff);
      
      lonDiff = blueLineTrains.lon[trainIndx] - BLUE_STATION_BOUNDS.LON[stopIndx];
      lonDiff = abs(lonDiff);
      
      if(latDiff <= BLUE_STATION_BOUNDS.LAT_DIFF_MAX[stopIndx] && lonDiff <= BLUE_STATION_BOUNDS.LON_DIFF_MAX[stopIndx])
      {
        stationFilledOffline.blueStation[stopIndx] = true;
        break;
      }
    }
  }

  //Find occupancy for green line stations
  for(uint8_t trainIndx = 0; trainIndx < greenLineTrains.numOfTrains; trainIndx++)
  {
    for(uint8_t stopIndx = 0; stopIndx < NUM_GREEN_STOPS; stopIndx++)
    {
      latDiff = GREEN_STATION_BOUNDS.LAT[stopIndx] - greenLineTrains.lat[trainIndx];
      latDiff = abs(latDiff);
      
      lonDiff = greenLineTrains.lon[trainIndx] - GREEN_STATION_BOUNDS.LON[stopIndx];
      lonDiff = abs(lonDiff);
      
      if(latDiff <= GREEN_STATION_BOUNDS.LAT_DIFF_MAX[stopIndx] && lonDiff <= GREEN_STATION_BOUNDS.LON_DIFF_MAX[stopIndx])
      {
        stationFilledOffline.greenStation[stopIndx] = true;
        break;
      }
    }
  }

  //Find occupancy for yellow line stations
  for(uint8_t trainIndx = 0; trainIndx < yellowLineTrains.numOfTrains; trainIndx++)
  {
    for(uint8_t stopIndx = 0; stopIndx < NUM_YELLOW_STOPS; stopIndx++)
    {
      latDiff = YELLOW_STATION_BOUNDS.LAT[stopIndx] - yellowLineTrains.lat[trainIndx];
      latDiff = abs(latDiff);
      
      lonDiff = yellowLineTrains.lon[trainIndx] - YELLOW_STATION_BOUNDS.LON[stopIndx];
      lonDiff = abs(lonDiff);
      
      if(latDiff <= YELLOW_STATION_BOUNDS.LAT_DIFF_MAX[stopIndx] && lonDiff <= YELLOW_STATION_BOUNDS.LON_DIFF_MAX[stopIndx])
      {
        stationFilledOffline.yellowStation[stopIndx] = true;
        break;
      }
    }
  }

  //Find occupancy for orange line stations
  for(uint8_t trainIndx = 0; trainIndx < orangeLineTrains.numOfTrains; trainIndx++)
  {
    for(uint8_t stopIndx = 0; stopIndx < NUM_ORANGE_STOPS; stopIndx++)
    {
      latDiff = ORANGE_STATION_BOUNDS.LAT[stopIndx] - orangeLineTrains.lat[trainIndx];
      latDiff = abs(latDiff);
      
      lonDiff = orangeLineTrains.lon[trainIndx] - ORANGE_STATION_BOUNDS.LON[stopIndx];
      lonDiff = abs(lonDiff);
      
      if(latDiff <= ORANGE_STATION_BOUNDS.LAT_DIFF_MAX[stopIndx] && lonDiff <= ORANGE_STATION_BOUNDS.LON_DIFF_MAX[stopIndx])
      {
        stationFilledOffline.orangeStation[stopIndx] = true;
        break;
      }
    }
  }

  //Update live struct
  canReadOccupancy = false;
  for(uint8_t indx = 0; indx < 49; indx++)
  {
    if(indx < NUM_RED_STOPS)
    {
      stationFilledLive.redStation[indx] = stationFilledOffline.redStation[indx];
    }
    
    if(indx < NUM_BLUE_STOPS)
    {
      stationFilledLive.blueStation[indx] = stationFilledOffline.blueStation[indx];
    }
    
    if(indx < NUM_GREEN_STOPS)
    {
      stationFilledLive.greenStation[indx] = stationFilledOffline.greenStation[indx];
    }

    if(indx < NUM_YELLOW_STOPS)
    {
      stationFilledLive.yellowStation[indx] = stationFilledOffline.yellowStation[indx];
    }

    if(indx < NUM_ORANGE_STOPS)
    {
      stationFilledLive.orangeStation[indx] = stationFilledOffline.orangeStation[indx];
    }
  }
  canReadOccupancy = true;
}


/*
 * Description: Code that runs on core 1 that controls LED timing
 * 
 * Arguments:
 *  none
 * 
 * Return:
 *  void
 */
 void ledControl(void* parameter)
 {
  uint8_t activeLedsNum = 0;
  uint8_t globalIndx = 0;
  uint8_t globalIndxSnapShot = 0;
  
  bool isContinuing = false;
  bool hasFinishedRed = false;
  bool hasFinishedBlue = false;
  bool hasFinishedGreen = false;
  bool hasFinishedYellow = false;
  bool hasFinishedOrange = false;
  const uint8_t TASK_DELAY = 0; 
  
  /*Serial.print("LED Code Running on core:");
  Serial.println(xPortGetCoreID());*/
  
  while(true)
  {
    while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to

      if(isContinuing)
      {
        globalIndx = globalIndxSnapShot;
        isContinuing = false;
      }
  
      //Activate Red LEDs 
      if(activeLedsNum < 18)
      {
        while(globalIndx < NUM_RED_STOPS && activeLedsNum < 18 && !hasFinishedRed)
        {
          while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to
          if(stationFilledLive.redStation[globalIndx] == true)
          {
            ledsRedLine.setPixelColor(globalIndx, ledsRedLine.Color(150, 0, 0));
            activeLedsNum++;
          }
          globalIndx++;
        }
        
        //If we have reached the end of the array, we have completed that train line  
        if(globalIndx >= NUM_RED_STOPS)
        {
          globalIndx = 0;
          hasFinishedRed = true;
        }
  
        //Only set the flag to start where we left off if we have reached our limit of on leds and we havent finished all the train lines
        if(activeLedsNum >= 18 && !(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange))
        {
          globalIndxSnapShot = globalIndx;
          isContinuing = true;
        }
      }
  
      //Activate Blue LEDs 
      if(activeLedsNum < 18)
      {    
        while(globalIndx < NUM_BLUE_STOPS && activeLedsNum < 18 && !hasFinishedBlue)
        {
          while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to
          if(stationFilledLive.blueStation[globalIndx] == true)
          {
            ledsBlueLine.setPixelColor(globalIndx, ledsBlueLine.Color(0, 0, 150));
            activeLedsNum++;
          }
          globalIndx++;
        }
        
        //If we have reached the end of the array, we have completed that train line  
        if(globalIndx >= NUM_BLUE_STOPS)
        {
          globalIndx = 0;
          hasFinishedBlue = true;
        }
    
        //Only set the flag to start where we left off if we have reached our limit of on leds and we havent finished all the train lines
        if(activeLedsNum >= 18 && !(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange))
        {
          globalIndxSnapShot = globalIndx;
          isContinuing = true;
        }
      }
      
      //Activate Green LEDs
      if(activeLedsNum < 18)
      {     
        while(globalIndx < NUM_GREEN_STOPS && activeLedsNum < 18 && !hasFinishedGreen)
        {
          while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to
          if(stationFilledLive.greenStation[globalIndx] == true)
          {
            ledsGreenLine.setPixelColor(globalIndx, ledsGreenLine.Color(0, 150, 0));
            activeLedsNum++;
          }
          globalIndx++;
        }
        
        //If we have reached the end of the array, we have completed that train line  
        if(globalIndx >= NUM_GREEN_STOPS)
        {
          globalIndx = 0;
          hasFinishedGreen = true;
        }
    
        //Only set the flag to start where we left off if we have reached our limit of on leds and we havent finished all the train lines
        if(activeLedsNum >= 18 && !(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange))
        {
          globalIndxSnapShot = globalIndx;
          isContinuing = true;
        }
      }
            
      //Activate Yellow LEDs
      if(activeLedsNum < 18)
      {     
        while(globalIndx < NUM_YELLOW_STOPS && activeLedsNum < 18 && !hasFinishedYellow)
        {
          while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to
          if(stationFilledLive.yellowStation[globalIndx] == true)
          {
            ledsYellowLine.setPixelColor(globalIndx, ledsYellowLine.Color(255, 255, 0));
            activeLedsNum++;
          }
          globalIndx++;
        }
        
        //If we have reached the end of the array, we have completed that train line
        if(globalIndx >= NUM_YELLOW_STOPS)
        {
          globalIndx = 0;
          hasFinishedYellow = true;
        }
    
        //Only set the flag to start where we left off if we have reached our limit of on leds and we havent finished all the train lines
        if(activeLedsNum >= 18 && !(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange))
        {
          globalIndxSnapShot = globalIndx;
          isContinuing = true;
        }
      }
                  
      //Activate Orange LEDs  
      if(activeLedsNum < 18)
      {   
        while(globalIndx < NUM_ORANGE_STOPS && activeLedsNum < 18 && !hasFinishedOrange)
        {
          while(!canReadOccupancy){vTaskDelay(TASK_DELAY);} //Only continue if the occupancy array isnt being written to
          if(stationFilledLive.orangeStation[globalIndx] == true)
          {
            ledsOrangeLine.setPixelColor(globalIndx, ledsOrangeLine.Color(255, 128, 0));
            activeLedsNum++;
          }
          globalIndx++;
        }

        //If we have reached the end of the array, we have completed that train line
        if(globalIndx >= NUM_ORANGE_STOPS)
        {
          globalIndx = 0;
          hasFinishedOrange = true;
        }
    
        //Only set the flag to start where we left off if we have reached our limit of on leds and we havent finished all the train lines
        if(activeLedsNum >= 18 && !(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange))
        {
          globalIndxSnapShot = globalIndx;
          isContinuing = true;
        }
      }
  
      //If we have finished all the lines, time to reset everying to start from the begining 
      if(hasFinishedRed && hasFinishedBlue && hasFinishedGreen && hasFinishedYellow && hasFinishedOrange)
      {
        globalIndx = 0;
        isContinuing = false;
        hasFinishedRed = false;
        hasFinishedBlue = false;
        hasFinishedGreen = false;
        hasFinishedYellow = false;
        hasFinishedOrange = false;
      }
  
      //Push the changes to the LEDs we set
      ledsRedLine.show();
      ledsBlueLine.show();
      ledsGreenLine.show();
      ledsYellowLine.show();
      ledsOrangeLine.show();
      
      delay(LED_DELAY_ON_MS);
      
      ledsRedLine.clear();
      ledsBlueLine.clear();
      ledsGreenLine.clear();
      ledsYellowLine.clear();
      ledsOrangeLine.clear();
      ledsRedLine.show();
      ledsBlueLine.show();
      ledsGreenLine.show();
      ledsYellowLine.show();
      ledsOrangeLine.show();
  
      activeLedsNum = 0;
      
      delay(LED_DELAY_OFF_MS);
     
    
    
    vTaskDelay(TASK_DELAY);//Delay in cycles to allow the cpu to do other stuff
  }
 }
