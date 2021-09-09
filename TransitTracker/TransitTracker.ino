//----------------Begin Includes------------------------------//
#include <Adafruit_NeoPixel.h>
#include <WiFi.h>
#include "config.h"


//----------------Begin Globals------------------------------//
struct trains
{
  uint8_t numOfTrains;
  double lat[49];
  double lon[49];
}redLineTrains,blueLineTrains,greenLineTrains,yellowLineTrains,orangeLineTrains;

const uint16_t RED_ROUTE_ID = 90;
const uint16_t BLUE_ROUTE_ID = 100;
const uint16_t GREEN_ROUTE_ID = 200;
const uint16_t YELLOW_ROUTE_ID = 190;
const uint16_t ORANGE_ROUTE_ID = 290;

const uint8_t NUM_RED_STOPS = 27;
const uint8_t NUM_BLUE_STOPS = 49;
const uint8_t NUM_GREEN_STOPS = 30;
const uint8_t NUM_YELLOW_STOPS = 17;
const uint8_t NUM_ORANGE_STOPS = 17;

const int LED_DELAY_ON_MS = .5;
const int LED_DELAY_OFF_MS = 0;

bool canReadOccupancy = true;
bool firstRun = true;

struct redStationBounds
{
  const double LAT[NUM_RED_STOPS] = {45.491518,45.510231,45.510643,45.518083,45.521791,45.519858,45.519190,45.518120,45.521346,45.519190,45.518526,45.517167,45.520109,45.523146,45.524912,45.530129,45.530103,45.530110,45.530091,45.532852,45.528740,45.533710,45.530733,45.559038,
                                      45.572240,45.577100,45.587450};
  const double LON[NUM_RED_STOPS] = {-122.801489,-122.781002,-122.716818,-122.693753,-122.689808,-122.681582,-122.679153,-122.675068,-122.689874,-122.681937,-122.679445,-122.674467,-122.672304,-122.671355,-122.671404,-122.665214,-122.662237,-122.658060,-122.654048,-122.622923,
                                      -122.602810,-122.578208,-122.563680,-122.565560,-122.558030,-122.567150,-122.592980};
  const double LAT_DIFF_MAX[NUM_RED_STOPS] = {0.000295,0.000196,0.000276,0.000203,0.000125,0.000125,0.000200,0.000200,0.000125,0.000125,0.000200,0.000200,0.000350,0.000550,0.000550,0.000200,0.000200,0.000200,0.000200,0.000355,0.000455,0.000670,0.000600,0.000430,0.000290,0.000380,0.000320};
  const double LON_DIFF_MAX[NUM_RED_STOPS] = {0.000316,0.000648,0.000713,0.000795,0.000470,0.000470,0.000470,0.000600,0.000470,0.000470,0.000600,0.000600,0.000300,0.000200,0.000200,0.000648,0.000520,0.000520,0.000570,0.002879,0.000900,0.000900,0.000400,0.000590,0.000530,0.000330,0.000440};
}RED_STATION_BOUNDS;

struct blueStationBounds
{
  const double LAT[NUM_BLUE_STOPS] = {45.522248,45.521410,45.521305,45.521175,45.527015,45.529985,45.530277,45.523186,45.517242,45.509961,45.505262,45.500420,45.495279,45.490484,45.491518,45.510231,45.510643,45.518083,45.521791,
                                        45.521346,45.519190,45.518120,45.517167,45.518526,45.519190,45.521346,45.520109,45.523146,45.524912,45.530129,45.530103,45.530110,45.530091,45.532852,45.528740,45.533710,45.530733,45.522669,
                                          45.522500,45.522300,45.522174,45.522095,45.522016,45.520542,45.516109,45.508128,45.505825,45.502599,45.501739};
  const double LON[NUM_BLUE_STOPS] = {-122.991065,-122.985280,-122.978333,-122.969505,-122.946069,-122.930690,-122.915874,-122.888806,-122.870175,-122.851473,-122.842209,-122.833050,-122.821574,-122.807056,-122.801489,-122.781002,-122.716818,-122.693753,-122.689808,
                                        -122.689874,-122.679153,-122.675068,-122.674467,-122.679445,-122.681937,-122.689874,-122.672304,-122.671355,-122.671404,-122.665214,-122.662237,-122.658060,-122.654048,-122.622923,-122.602810,-122.578208,-122.563680,-122.55894,
                                          -122.538440,-122.511330,-122.496469,-122.486156,-122.477125,-122.470798,-122.460323,-122.441589,-122.436077,-122.427054,-122.418355};
  const double LAT_DIFF_MAX[NUM_BLUE_STOPS] = {0.000380,0.000200,0.000150,0.000300,0.000250,0.000250,0.000250,0.000250,0.000250,0.000280,0.000300,0.000368,0.000236,0.000250,0.000295,0.000196,0.000276,0.000203,0.000125,0.000125,0.000200,0.000200,0.000200,0.000200,0.000125,
                                                0.000125,0.000350,0.000550,0.000550,0.000200,0.000200,0.000200,0.000200,0.000355,0.000455,0.000670,0.000600,0.000125,0.000125,0.000125,0.000125,0.000125,0.000125,0.000280,0.000590,0.000220,0.000240,0.000245,0.000245};
  const double LON_DIFF_MAX[NUM_BLUE_STOPS] = {0.000260,0.000540,0.000560,0.009400,0.000630,0.000530,0.000500,0.000535,0.000550,0.000480,0.000510,0.000650,0.000530,0.000530,0.000316,0.000648,0.000713,0.000795,0.000470,0.000470,0.000470,0.000600,0.000600,0.000600,0.000470,
                                                0.000470,0.000300,0.000200,0.000200,0.000648,0.000520,0.000520,0.000570,0.002879,0.000900,0.000900,0.000400,0.000610,0.000428,0.000550,0.001150,0.001180,0.001200,0.000475,0.001215,0.000425,0.000495,0.000500,0.000735};
}BLUE_STATION_BOUNDS;

struct greenStationBounds
{
  const double LAT[NUM_GREEN_STOPS] = {45.509471,45.509183,45.511410,45.511890,45.515480,45.515130,45.518783,45.518492,45.522120,45.521749,45.524251,45.524147,45.527024,45.527026,45.530129,45.530103,45.530110,45.530091,45.532852,45.528740,45.533710,45.530733,45.514037,
                                        45.503392,45.495736,45.491097,45.480500,45.467898,45.453426,45.435763};
  const double LON[NUM_GREEN_STOPS] = {-122.683692,-122.682885,-122.682700,-122.681400,-122.680460,-122.679630,-122.678722,-122.677779,-122.67690,-122.676042,-122.676409,-122.675499,-122.676517,-122.675637,-122.665214,-122.662237,-122.658060,-122.654048,-122.622923,
                                        -122.602810,-122.578208,-122.563680,-122.564438,-122.566631,-122.566207,-122.566270,-122.566810,-122.566617,-122.573612,-122.567768};
  const double LAT_DIFF_MAX[NUM_GREEN_STOPS] = {0.000430,0.000340,0.000320,0.000450,0.000395,0.000370,0.000420,0.000390,0.000390,0.000480,0.000420,0.000345,0.000290,0.000340,0.000200,0.000200,0.000200,0.000200,0.000355,0.000455,0.000670,0.000600,0.000670,0.000400,
                                                  0.000400,0.000440,0.000970,0.000469,0.000470,0.000365};
  const double LON_DIFF_MAX[NUM_GREEN_STOPS] = {0.000250,0.000300,0.000270,0.000270,0.000350,0.000300,0.000390,0.000330,0.000300,0.000370,0.000370,0.000290,0.000250,0.000250,0.000648,0.000520,0.000520,0.000570,0.002879,0.000900,0.000900,0.000400,0.000250,0.000250,
                                                  0.000250,0.000250,0.000250,0.000375,0.000375,0.000260};
}GREEN_STATION_BOUNDS;

struct yellowStationBounds
{
  const double LAT[NUM_YELLOW_STOPS] = {45.509471,45.511890,45.515480,45.518783,45.522120,45.524251,45.527024,45.530170,45.539558,45.548708,45.555180,45.562655,45.569854,45.577117,45.583850,45.596135,45.605440};
  const double LON[NUM_YELLOW_STOPS] = {-122.683692,-122.681400,-122.680460,-122.678722,-122.676900,-122.676409,-122.676517,-122.667810,-122.675524,-122.680996,-122.682312,-122.682244,-122.682139,-122.682115,-122.685931,-122.685660,-122.685542};
  const double LAT_DIFF_MAX[NUM_YELLOW_STOPS] = {0.000430,0.000450,0.000395,0.000420,0.000390,0.000420,0.000290,0.000289,0.000289,0.000850,0.000300,0.000715,0.000815,0.000785,0.000250,0.000335,0.000355};
  const double LON_DIFF_MAX[NUM_YELLOW_STOPS] = {0.000250,0.000270,0.000350,0.000390,0.000300,0.000370,0.000250,0.000550,0.000420,0.000250,0.000150,0.000150,0.000200,0.000150,0.000450,0.000200,0.000375};
}YELLOW_STATION_BOUNDS;

struct orangeStationBounds
{
  const double LAT[NUM_ORANGE_STOPS] = {45.430771,45.441215,45.462789,45.474817,45.490902,45.497904,45.502779,45.506565,45.502757,45.507911,45.509183,45.511890,45.515130,45.518492,45.521749,45.524147,45.527026};
  const double LON[NUM_ORANGE_STOPS] = {-122.635129,-122.639947,-122.637634,-122.640033,-122.648508,-122.64817,-122.653267,-122.662755,-122.671252,-122.680519,-122.682885,-122.681400,-122.679630,-122.677779,-122.676042,-122.675499,-122.675637};
  const double LAT_DIFF_MAX[NUM_ORANGE_STOPS] = {0.000368,0.000375,0.000375,0.000570,0.000400,0.000400,0.000320,0.000150,0.000340,0.000150,0.000340,0.000450,0.000370,0.000390,0.000480,0.000345,0.000340};
  const double LON_DIFF_MAX[NUM_ORANGE_STOPS] = {0.000250,0.000300,0.000540,0.000256,0.000150,0.000150,0.000500,0.000675,0.000770,0.000515,0.000300,0.000270,0.000300,0.000330,0.000370,0.000290,0.000250};
}ORANGE_STATION_BOUNDS;

struct stationOccupancy
{
  bool redStation[NUM_RED_STOPS] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  bool blueStation[NUM_BLUE_STOPS] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,
                                              false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  bool greenStation[NUM_GREEN_STOPS] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  bool yellowStation[NUM_YELLOW_STOPS] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
  bool orangeStation[NUM_ORANGE_STOPS] = {false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false,false};
}stationFilledLive;

Adafruit_NeoPixel ledsRedLine(NUM_RED_STOPS, 14, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsBlueLine(NUM_BLUE_STOPS, 12, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsGreenLine(NUM_GREEN_STOPS, 26, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsYellowLine(NUM_YELLOW_STOPS, 27, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledsOrangeLine(NUM_ORANGE_STOPS, 32, NEO_GRB + NEO_KHZ800);

#include "functions.h"





void setup() {
  Serial.begin(115200);
  
  ledsRedLine.begin();
  ledsBlueLine.begin();
  ledsGreenLine.begin();
  ledsYellowLine.begin();
  ledsOrangeLine.begin();

  //Turn off all the LEDs
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
 
  WiFi.begin(WIFI_SSID, WIFI_PSWD);

  uint8_t indx = 0;
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    if(indx < 19)
    {
      ledsGreenLine.setPixelColor(indx, ledsGreenLine.Color(138,43,226));
      ledsGreenLine.show();
      indx++;
    }
    else if(indx >= 19)
    {
      indx = 0;
      ledsGreenLine.clear();
      ledsGreenLine.show();
    }
  }

  ledsGreenLine.clear();
  ledsGreenLine.show();

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  xTaskCreatePinnedToCore(apiControl, "apiControl", 20000, NULL, 3, NULL,  0); 

}

void loop() {

}

void apiControl(void* parameter)
{
  String apiCallResult;


  while(true)
  { 
    if(firstRun)
    {
      ledsRedLine.setPixelColor(0, ledsRedLine.Color(150, 0, 0));
      ledsRedLine.show();
      callTriMetApi(RED_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &redLineTrains);
      apiCallResult = " ";
      delay(250);
      
      ledsBlueLine.setPixelColor(0, ledsBlueLine.Color(0, 0, 150));
      ledsBlueLine.show();
      callTriMetApi(BLUE_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &blueLineTrains);
      apiCallResult = " ";
      delay(250);

      ledsGreenLine.setPixelColor(0, ledsGreenLine.Color(0, 150, 0));
      ledsGreenLine.show();
      callTriMetApi(GREEN_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &greenLineTrains);
      apiCallResult = " ";
      delay(250);

      ledsYellowLine.setPixelColor(0, ledsYellowLine.Color(255, 255, 0));
      ledsYellowLine.show();
      callTriMetApi(YELLOW_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &yellowLineTrains);
      apiCallResult = " ";
      delay(250);
      
      ledsOrangeLine.setPixelColor(0, ledsOrangeLine.Color(255,69,0));
      ledsOrangeLine.show();
      callTriMetApi(ORANGE_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &orangeLineTrains);
      apiCallResult = " ";

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

      xTaskCreatePinnedToCore(ledControl, "ledControl", 10000, NULL, 3, NULL,  1); 

      firstRun = false;
    }
    else
    {
      callTriMetApi(RED_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &redLineTrains);
      apiCallResult = " ";
      delay(250);
      callTriMetApi(BLUE_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &blueLineTrains);
      apiCallResult = " ";
      delay(250);
      callTriMetApi(GREEN_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &greenLineTrains);
      apiCallResult = " ";
      delay(250);
      callTriMetApi(YELLOW_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &yellowLineTrains);
      apiCallResult = " ";
      delay(250);
      callTriMetApi(ORANGE_ROUTE_ID, &apiCallResult);
      parseApiString(&apiCallResult, &orangeLineTrains);
      apiCallResult = " ";

      findStationOccupancy();

    }
     
    
    /*
    //DEBUG CODE
    Serial.println();
    Serial.println("--------------");
    Serial.println(blueLineTrains.numOfTrains);
    Serial.println("--------------");
    
    for(int i = 0; i < 49; i++)
    {
      Serial.print(blueLineTrains.lat[i],7);
      Serial.print(" , ");
      Serial.println(blueLineTrains.lon[i],6);
    }*/

    findStationOccupancy();
    
    

    
    /*
    //DEBUG CODE
    Serial.println("\n\n");
    Serial.println("---RED LINE---");

    for(int i = 0; i < 27; i++)
    {
      if(stationFilledLive.redStation[i] == true)
      {
        Serial.print("Train at: ");
        Serial.print(RED_STATION_BOUNDS.LAT[i],7);
        Serial.print(" , ");
        Serial.println(RED_STATION_BOUNDS.LON[i],6);
        Serial.println(i);
      }
    }


    Serial.println("\n\n");
    Serial.println("---BLUE LINE---");
    for(int i = 0; i < 49; i++)
    {
      if(stationFilledLive.blueStation[i] == true)
      {
        Serial.print("Train at: ");
        Serial.print(BLUE_STATION_BOUNDS.LAT[i],7);
        Serial.print(" , ");
        Serial.println(BLUE_STATION_BOUNDS.LON[i],6);
        Serial.println(i);
      }
    }

    Serial.println("\n\n");
    Serial.println("---GREEN LINE---");
    for(int i = 0; i < 30; i++)
    {
      if(stationFilledLive.greenStation[i] == true)
      {
        Serial.print("Train at: ");
        Serial.print(GREEN_STATION_BOUNDS.LAT[i],7);
        Serial.print(" , ");
        Serial.println(GREEN_STATION_BOUNDS.LON[i],6);
        Serial.println(i);
      }
    }
    
    Serial.println("\n\n");
    Serial.println("---YELLOW LINE---");
    for(int i = 0; i < 17; i++)
    {
      if(stationFilledLive.yellowStation[i] == true)
      {
        Serial.print("Train at: ");
        Serial.print(YELLOW_STATION_BOUNDS.LAT[i],7);
        Serial.print(" , ");
        Serial.println(YELLOW_STATION_BOUNDS.LON[i],6);
        Serial.println(i);
      }
    }
    
    Serial.println("\n\n");
    Serial.println("---ORANGE LINE---");
    for(int i = 0; i < 17; i++)
    {
      if(stationFilledLive.orangeStation[i] == true)
      {
        Serial.print("Train at: ");
        Serial.print(ORANGE_STATION_BOUNDS.LAT[i],7);
        Serial.print(" , ");
        Serial.println(ORANGE_STATION_BOUNDS.LON[i],6);
        Serial.println(i);
      }
    }*/

   delay(55000);
  }
}
