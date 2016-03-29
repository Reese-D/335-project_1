#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
/*
  Usage: First argument is filename, second argument should be the max size of
  a single line in bytes, defaults to 100 if not specified
*/
#define POSITIVE 1
#define NEGATIVE 0
#define FEMALE 1
#define MALE 0

struct patient{
  int gender;
  int bloodType;
  int weight;
  int hasVirus;
};
static int currentPatient;
static int totalPatients;
struct patient *myList;

void addPatient(struct patient *p){
  if(currentPatient == totalPatients){
    //list is full so lets double the size
    totalPatients *= 2;
    myList = realloc(myList, sizeof(struct patient) * totalPatients);

  }
  //add the new patient to the list and increment which patient we're at
  myList[currentPatient] = *p;
  currentPatient++;
}

int main(int argc, char *argv[]){
  FILE *doc;
  //if argv2 isn't a number it will return 0 and lineSize defaults to 100
  //int lineSize = atoi(argv[2]) ? atoi(argv[2]) : 100;
  int lineSize = 100;
  currentPatient = 0;
  totalPatients = 1;
  char currentLine[lineSize];
  //array of all the patients
  myList = malloc(sizeof(struct patient) * totalPatients);

  //try and open the file (file name is first parameter passed in)
  if ((doc = fopen(argv[1], "r")) == NULL){
    printf("Couldn't open file");
    exit(1);
  }

  //read in file and save to arrays
  while(fgets(currentLine, lineSize, doc) != NULL){
    struct patient tempPatient;
    strtok(currentLine, ","); //don't care about the ID token, throw it out
    //set gender to male or female
    tempPatient.gender = strcmp("female", strtok(NULL, ",")) ? MALE : FEMALE;

    //if we can't find a + sign in the token set blood to negative otherwise positive
    tempPatient.bloodType = (strrchr(strtok(NULL, ","), '+') == NULL) ? NEGATIVE : POSITIVE;
    tempPatient.weight = atoi(strtok(NULL, ","));
    char *temp = strtok(NULL, ",");
    tempPatient.hasVirus = strchr(temp, 'Y') ? POSITIVE : NEGATIVE;
    addPatient(&tempPatient);
  }
  int genderCount = 0;  //female given NOT virus
  int bloodCount = 0;   //blood + given NOT virus
  int weightCount = 0;  //weight > 170 given NOT virus
  int genderCount2 = 0; //these next three are the same except given virus
  int bloodCount2 = 0;
  int weightCount2 = 0;
  float virusCount = 0; //really should be called notVirusCount
  float totalCount = 0; //total number of patients
  for(int i = 0; i < currentPatient; i++){
    if(myList[i].hasVirus != 1){
      if(myList[i].gender == 1){
        genderCount++;
      }
      if(myList[i].bloodType == 1){
        bloodCount++;
      }
      if(myList[i].weight > 170){
        weightCount++;
      }
      virusCount++;
    }else{
      if(myList[i].gender == 1){
        genderCount2++;
      }
      if(myList[i].bloodType == 1){
        bloodCount2++;
      }
      if(myList[i].weight > 170){
        weightCount2++;
      }
    }
    totalCount++;

    printf("gender: %i, blood: %i, weight: %i, hasVirus: %i\n",myList[i].gender,myList[i].bloodType,myList[i].weight,myList[i].hasVirus);
  }
  printf("female given not Virus: %f\nblood positive given not virus: %f\nweight > 170 given not virus: %f\n", genderCount/virusCount, bloodCount/virusCount, weightCount/virusCount);
  printf("female given Virus: %f\nblood positive given virus: %f\nweight > 170 given virus: %f\n", genderCount2/(totalCount - virusCount), bloodCount2/(totalCount - virusCount), weightCount2/(totalCount - virusCount));
  printf("male given not Virus: %f\nblood negative given not virus: %f\nweight < 170 given not virus: %f\n", (virusCount - genderCount)/virusCount, (virusCount - bloodCount)/virusCount, (virusCount - weightCount)/virusCount);
  printf("male given Virus: %f\nblood negative given virus: %f\nweight < 170 given virus: %f\n", (totalCount - virusCount - genderCount2)/(totalCount - virusCount), (totalCount - virusCount - bloodCount2)/(totalCount - virusCount), (totalCount - virusCount - weightCount2)/(totalCount - virusCount));
  printf("virusCount: %f\ntotal: %f\n", virusCount, totalCount);
  //free(myList);
  return 0;
}
