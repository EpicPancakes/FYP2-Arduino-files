int TURBIDITY_PIN = A1;


double avergearray2(int* arr, int number){
  int i;
  int max,min;
  double avg;
  long amount=0;
  if(number<=0){
    //Serial.println("Error number for the array to avraging!/n");
    return 0;
  }
  if(number<5){   //less than 5, calculated directly statistics
    for(i=0;i<number;i++){
      amount+=arr[i];
    }
    avg = amount/number;
    return avg;
  }else{
    if(arr[0]<arr[1]){
      min = arr[0];max=arr[1];
    }
    else{
      min=arr[1];max=arr[0];
    }
    for(i=2;i<number;i++){
      if(arr[i]<min){
        amount+=min;        //arr<min
        min=arr[i];
      }else {
        if(arr[i]>max){
          amount+=max;    //arr>max
          max=arr[i];
        }else{
          amount+=arr[i]; //min<=arr<=max
        }
      }//if
    }//for
    avg = (double)amount/(number-2);
  }//if
  return avg;
}

#define samplingInterval_t 20
#define printInterval_t 800
#define ArrayLenth_t  100    //times of collection
int TurbidArray[ArrayLenth_t];   //Store the average value of the sensor feedback
int TurbidArrayIndex=0;

float get_turbidity_volt()
{
  //int sensorValue = analogRead(TURBIDITY_PIN);// read the input on analog pin 0:
  static float voltage;// = sensorValue * (5.0 / 1024.0); // Convert the analog reading (which goes from 0 - 1023) to a voltage (0 - 5V):
  static unsigned long samplingTime = millis();
  static unsigned long printTime = millis();
    if(millis()-samplingTime > samplingInterval_t)
  {
      TurbidArray[TurbidArrayIndex++]=analogRead(TURBIDITY_PIN);
      if(TurbidArrayIndex==ArrayLenth_t)TurbidArrayIndex=0;
      voltage = avergearray2(TurbidArray, ArrayLenth_t)*5.0/1024;
      samplingTime=millis();
  }
  return voltage;





//  static float pHValue,voltage;

//  return pHValue;
}

float get_turbidity_NTU()
{
  float turbidity = 0;
  float voltage = 0;
  int counter=0;
  //cnt add this. Need non blocking function
//  for(counter = 0; counter < 2000; counter++)
//  {
  voltage = get_turbidity_volt();
  Serial.print(F("TURB Voltage:"));
  Serial.println(voltage,2);
//  turbidity += (-1120.4*(voltage)*(voltage) + 5742.3*(voltage) -4352.9 + 480.0);
//  }
//turbidity = -7.97*voltage + 33.17; //temp palok 28 apr
turbidity = voltage;
if(turbidity < 0) turbidity = 0;
  return turbidity; //turbidity/2000.0;
}
