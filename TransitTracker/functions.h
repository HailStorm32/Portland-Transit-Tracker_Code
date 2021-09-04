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
 *  (IN/OUT) trainLine -- pointer to trains stuct
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

  //Get total number of trains in api data string while also getting the lon and lat data per train
  while(apiString->indexOf(" longitude=") != -1 && apiString->indexOf(" latitude=") != -1)
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

    apiString->remove(indxBegin,(indxEnd-indxBegin));

    //Serial.print(*apiString);
    numOfTrains++;
  }
  trainLine->numOfTrains = numOfTrains;
}
