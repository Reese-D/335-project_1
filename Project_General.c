#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#define DEFAULT_ROW_LENGTH 256
#define MAX_SUBSTRING_LENGTH 50
#define MAX_WORD_LENGTH 20
#define FILE_DELIMITER ","
#define MAX_IGNORED_COLUMNS 1
#define MEMORY_INCREMENTS 500
#define PI 3.14159
#define E 2.71828
/*
* defines a commonly used loop for code reuse purposes
* firstOrder: whatever code will be executed during the first but not second loop
* secondOrder & secondOrder2: code that will be executed within first&second loop
*
*NOTE: only applicable after main has instantiated numColumns and numItems
*/
#define iterThroughColumnsItems(firstOrder, secondOrder, secondOrder2)    for(int x = 0; x < numColumns; x++){ \
      firstOrder; \
      for(int y = 0; y < numItems; y++){ \
        secondOrder; \
        secondOrder2; \
      } \
    }


//Will be the basis for holding all classifier data
typedef struct{
  double classY;
  double classN;
} data;
data **list, *numericAttributes; //pointer to pointer to struct (multidimensional array 2 deep)
//Will record how each item did in regard to the classifier
typedef struct{
  double trueTrue; //for example the % of the time 'item' was true while classifier was also true
  double trueFalse;
  double falseTrue;
  double falseFalse;
} discriminators;
discriminators **discriminationList; //one for every substring

double meanY = 0, meanN = 0, thetaY = 0, thetaN = 0;
int numColumns ,classifier, numItems, rowLength, fileLength, numericAttributesLength, classifierTrue,fileLengthY = 0, fileLengthN = 0;
int *ignoredColumns; //ignored columns for our confusion matrix
FILE *input;
char *currentLine;  //keeps track of input lines
char ***substrings; // point to pointer to char (multidimension 2 deep)


/*
* fills the multidimensional array 'substrings' with each substring value *
* obtained from the user by querying for input
*/
void findParsingValues(){
  char *valueHolder = malloc(sizeof(char) * MAX_SUBSTRING_LENGTH);
  printf("Please indicate what number is being compared or the substring(s) being compared; delimited by spaces\n");
  substrings = malloc(sizeof(char**) * numColumns);
  //fill in substrings
  for(int x = 0; x < numColumns; x++){
    char *string, *token, *replacement;
    substrings[x] = malloc(sizeof(char*) * numItems);
    printf("For column %i:",x);
    fgets(valueHolder, MAX_SUBSTRING_LENGTH, stdin);
    //allocate space for each substring
    for(int y = 0; y < numItems; y++){
      substrings[x][y] = malloc(sizeof(char) * MAX_WORD_LENGTH);
    }

    //parse the user inputs into multiple categories
    int count = 0;
    string = valueHolder;
    while((token = (strsep(&string, " "))) != NULL){
      strncpy(substrings[x][count], token, MAX_WORD_LENGTH);
      replacement = strchr(substrings[x][count], '\n');
      if(replacement != NULL ) *replacement = '\0'; //replace newline character
      count++;
    }
  }
  free(valueHolder);
}

/*
* used to compare user defined substrings with some specific token *
*   taken from a file
*
* NOTE: cannot be called before substrings has been malloc'd
*/
double compareValues(int *swapSpace, char *token, int x, int y, int doubleUp){
  //keep track of numeric data occurences (ex:170) for each column
  if(atoi(substrings[x][y]) > 0 && atoi(token) > 0){
    (atoi(token) > atoi(substrings[x][y])) ? (*swapSpace = 1) : (*swapSpace = 0); //if the number is less than user input
    //if we've read more file lines than we've allocated memory allocate some more
    if(numericAttributesLength <= fileLength){
      numericAttributes = realloc(numericAttributes, numericAttributesLength * sizeof(data) + MEMORY_INCREMENTS);
    }

    //TODO: out of scope of project, this only works with 1 or less numeric columns
    if(!doubleUp){
      if(classifierTrue){
        numericAttributes[fileLength].classY = atof(token);

      }else{
        numericAttributes[fileLength].classN = atof(token);
      }
    }

  }
  //track substring occurences for each column
  if(!atoi(substrings[x][y]))
    strcasestr(token, substrings[x][y]) != NULL ? (*swapSpace = 1) : (*swapSpace = 0);

  return atof(token);
}

// /*
// * reads the input files and checks for each row whether the column data matches *
// * the user input. EX: if row 1 was 1245,female,a+,y and user input *
// * was 2000,female,+,y  with a classifier in row 5 (the y) it would count how many
// * times the first column was below 2000, the second column had the word "female"
// * as a substring, and how many bloodtypes were positive for both classifiers of y
// * and classifiers of not y
// *
// * NOTE: cannot be called before findParsingValues
// */
void addValues(){
  //define variables
  classifierTrue = 0;
  char *string, *token;
  fileLength = 0;

  //acquire needed memory for computations
  currentLine = malloc(sizeof(char) * rowLength);
  char *currentLine2 = malloc(sizeof(char) * rowLength);
  numericAttributes = malloc(sizeof(data) * MEMORY_INCREMENTS);
  numericAttributesLength = 500;
  int swapSpace = 0;


  //iterate over input file
  while(fgets(currentLine, rowLength, input) != NULL){
    fileLength++; //keep track of total lines

    //find the classifier
    strcpy(currentLine2, currentLine);
    string = currentLine2;
    for(int y = 0; y < classifier; y++){
      token = strsep(&string, FILE_DELIMITER);
    }

    //check whether classifier matches our given substring
    (strcasestr(token, substrings[classifier-1][0]) != NULL) ? (classifierTrue = 1) : (classifierTrue = 0);
    string = currentLine;

    iterThroughColumnsItems(
        (   token = strsep(&string, FILE_DELIMITER)   ),
        (   compareValues(&swapSpace, token, x, y, 0)     ),
        (   (classifierTrue) ? (list[x][y].classY += swapSpace) : (list[x][y].classN += swapSpace)      )
        );
    classifierTrue ? (fileLengthY++) : (fileLengthN++); //added for the final 10% of project to keep track

    // //iterate through our substring list
    // for(int x = 0; x < numColumns; x++){
    //   token = strsep(&string, FILE_DELIMITER);
    //   for(int y = 0; y < numItems; y++){
    //     /*TODO: at some point change numeric classification to handle more than just 1 number
    //     * at the moment if two number are given ex: 170 450 everything that is less than 170 will also be true for 450
    //     * it doesn't check between both values, it only compares less than for each. Fix after project turned in for
    //     * future self use. at the moment there can only be 1 classifier as well ex: y and everything not y.
    //     */
    //     compareValues(swapSpace, token, x, y);
    //     (classifierTrue) ? (list[x][y].classY += swapSpace[y]) : (list[x][y].classN += swapSpace[y]);
    //   }
    // }
  }

  //the N classifier will never be tallied so add it in
  list[classifier-1][0].classN = (fileLength - list[classifier-1][0].classY);

  //free up the space we allocated
  free(currentLine);
  free(currentLine2);
  printf("\nFileLength: %i\n\n", fileLength);
}



/*
*reads in the given input file and stores data into arrays
*/
void findNumColumns(){
  currentLine = malloc(sizeof(char) * rowLength);

  //figure out how many columns there are
  fgets(currentLine, rowLength, input);
  strtok(currentLine, FILE_DELIMITER);
  int count = 1;
  while(strtok(NULL, FILE_DELIMITER) != NULL){
    count++;
  }
  fseek(input, 0, SEEK_SET); //go back to the start of the file
  numColumns = count;
  free(currentLine);
}

/*
* prints out useful information
*
* NOTE: cannot be called before addValues
*/
void printPriorsAndClassifiers(){
  printf("Priors:\n");

  printf("\tclassifier = %s: %f\n", substrings[classifier-1][0],
    (list[classifier-1][0].classY / (double) fileLength));

  printf("\tNOT classifier = %s: %f\n\n", substrings[classifier-1][0],
    (list[classifier-1][0].classN / (double) fileLength));

  printf("likelihoods for column values, if numeric data likelihood given > #:\n");

  discriminationList = malloc(sizeof(discriminators*)*numColumns);
  for(int x = 0; x < numColumns; x++){
    discriminationList[x] = malloc(sizeof(discriminators)*numItems);
    for(int y = 0; y < numItems; y++){

      if(x != classifier -1){

        //find discrimintors based on our classifier
        discriminationList[x][y].trueTrue = (list[x][y].classY / (double) list[classifier -1][y].classY);
        discriminationList[x][y].falseTrue = 1 - discriminationList[x][y].trueTrue;
        discriminationList[x][y].trueFalse = (list[x][y].classN / (double) list[classifier -1][y].classN);
        discriminationList[x][y].falseFalse = 1 - discriminationList[x][y].trueFalse;


        printf("\t\t--%s--\n", substrings[x][y]);
        printf("\tlikelihood for %s given %s: %f\n", substrings[x][y], substrings[classifier-1][y],
          discriminationList[x][y].trueTrue);
        printf("\tlikelihood for NOT %s given %s: %f\n", substrings[x][y], substrings[classifier-1][y],
          discriminationList[x][y].falseTrue);
        printf("\tlikelihood for %s given NOT %s: %f\n", substrings[x][y], substrings[classifier-1][y],
          discriminationList[x][y].trueFalse);
        printf("\tlikelihood for NOT %s given NOT %s: %f\n\n", substrings[x][y], substrings[classifier-1][y],
          discriminationList[x][y].falseFalse);
      }
    }
  }
}

/*
* initiates the prediction portion of the statistical analysis on a given test
* set by querying the user for a filename.
*/
void predictTestSets(){
  printf("\nplease input test set data via filename:\t");
  char testFile[MAX_SUBSTRING_LENGTH];
  FILE *testInput;
  short boolean = 1;
  char *string, *token, *replace;
  int swapSpace = 0, classifierTrue, cMx, cMy;
  int confusionMatrix[2][2] ={{0,0},
                              {0,0}};//initialized to omit random junk (could have made it global or static also, but this was more clear)
  int confusionMatrix2[2][2] ={{0,0},
                              {0,0}};
  //keep checking till the user provides a valid file
  while(boolean){
    fgets(testFile, MAX_SUBSTRING_LENGTH, stdin);
    replace = strchr(testFile, '\n');
    *replace = '\0';
    if((testInput = fopen(testFile, "r")) != NULL){
      boolean = 0;
    }else{
      printf("Invalid filename. Please provide a valid filename: ");
    }
  }

  // totalN = (1 / (sqrt(2 * PI * pow(thetaN,2)))) * pow(E, (pow((meanN - x),2))/(2*pow(thetaN,2)));
  for(int doubleUp = 0; doubleUp < 2; doubleUp++){
    currentLine = malloc(sizeof(char) * rowLength);
    char *currentLine2 = malloc(sizeof(char) * rowLength);
    if(doubleUp) fseek(testInput, 0, SEEK_SET);
    while(fgets(currentLine, rowLength, testInput) != NULL){
      double countYes = 1, countNo = 1;

      //find the classifier
      strcpy(currentLine2, currentLine);
      string = currentLine2;
      for(int y = 0; y < classifier; y++){
        token = strsep(&string, FILE_DELIMITER);
      }

      //check whether classifier matches our given substring
      (strcasestr(token, substrings[classifier-1][0]) != NULL) ? (classifierTrue = 1) : (classifierTrue = 0);

      string = strdup(currentLine);
      for(int x = 0; x < numColumns; x++){
        for(int y = 0; y < numItems; y++){

          token = strsep(&string, FILE_DELIMITER);
          double tempX = compareValues(&swapSpace, token, x, y, doubleUp);
          int temp = 1;
          for(int z = 0; z < MAX_IGNORED_COLUMNS; z++){
            if(ignoredColumns[z] == x || classifier-1 == x) temp = 0;
          }
          if(temp){
            if(swapSpace){
              if(doubleUp && x == 3){
                countYes *= (1 / (sqrt(2 * PI * pow(thetaY,2)))) * pow(E, (pow((meanY - tempX),2))/(2*pow(thetaY,2)));
              }else{
                countYes *= discriminationList[x][y].trueTrue;
                countNo *= discriminationList[x][y].trueFalse;
              }
            }else{
              if(doubleUp && x == 3){
                countNo *= (1 / (sqrt(2 * PI * pow(thetaN,2)))) * pow(E, (pow((meanN - tempX),2))/(2*pow(thetaN,2)));
              }else{
                countYes *= discriminationList[x][y].falseTrue;
                countNo *= discriminationList[x][y].falseFalse;
              }
            }
          }
          // printf("countYes: %f\t", countYes);
          // printf("countNo: %f\t", countNo);
        }
      }
      classifierTrue ? (cMx = 1) : (cMx = 0);
      (countYes > countNo) ? (cMy = 1) : (cMy = 0);
      if(doubleUp) printf("\t\t\t");
      cMx ? printf("Y ") : printf("N ");
      cMy ? printf("Y\n") : printf("N\n");

      if(!doubleUp){
        confusionMatrix[cMx][cMy] ++;
      }else{
        confusionMatrix2[cMx][cMy] ++;
      }
      //printf("\n");
      free(string);
    }
    printf("\n\n\n");
    if(!doubleUp) printf("\t\t\tWith weight likelihoods replaced with the function given\n");
    free(currentLine);
    free(currentLine2);
  }


  printf("\n\tConfusion Matrix\n\tNo\tYes\nNo\t%i\t%i\nYes\t%i\t%i\n\n", confusionMatrix[0][0], confusionMatrix[0][1], confusionMatrix[1][0], confusionMatrix[1][1]);
  printf("\n\t\t\tConfusion Matrix with weight likelihoods replaced\n\t\t\tNo\tYes\n\t\t\tNo\t%i\t%i\n\t\t\tYes\t%i\t%i\n\n", confusionMatrix2[0][0], confusionMatrix2[0][1], confusionMatrix2[1][0], confusionMatrix2[1][1]);
}


void mathsHurrah(){
  double *stdDeviationY = malloc(sizeof(double) * fileLengthY);
  double *stdDeviationN = malloc(sizeof(double) * fileLengthN);
  for(int x = 0; x < fileLength; x++){

    meanY += numericAttributes[x].classY;
    meanN += numericAttributes[x].classN;
  }
  // printf("FileLength y:%i, N%i\n",fileLengthY, fileLengthN);
  meanY /= fileLengthY;
  meanN /= fileLengthN;
  // printf("\nMEANY %f\n", meanY);
  // printf("\nMEANN %f\n", meanN);
  //find standard deviation using formula sqrt((summation(x - mean)^2 / n))
  for(int x = 0; x < fileLength; x++){
    stdDeviationY[x] = pow((numericAttributes[x].classY - meanY), 2);
    stdDeviationN[x] = pow((numericAttributes[x].classN - meanN), 2);
    thetaY += stdDeviationY[x];
    thetaN += stdDeviationN[x];
  }
  thetaY /= fileLengthY;
  thetaN /= fileLengthN;
  thetaY = sqrt(thetaY);
  thetaN = sqrt(thetaN);
  // printf("\nTHETAY: %f\n", thetaY);
  // printf("\nTHETAN: %f\n", thetaN);


}

int main(int argc, char *argv[]){

    //when the minimum argument requirement is not met
    if(argc < 4){
      printf("\nPlease specify the following delimited by spaces:\n");
      printf("\tdocument/input file name name\n");
      printf("\twhich column is the classifier\n");
      printf("\tnumber of unique values supplied for largest column\n");
      printf("\tan optional row size  or  a -1 if not supplied\n");
      printf("\twhich column(s) will be ignored\n");
      return 1;
    }

    //set up variables
    classifier = atoi(argv[2]);
    numItems = atoi(argv[3]);
    rowLength = DEFAULT_ROW_LENGTH;
    if(argc > 5 && (atoi(argv[4]) != -1)) rowLength = atoi(argv[4]);
    ignoredColumns = malloc(sizeof(int) * (argc - 4));
    for(int x = 5; x < argc - 4; x++){
      ignoredColumns[x] = atoi(argv[x]);
    }

    //try and open the file (file name is first parameter passed in)
    if ((input = fopen(argv[1], "r")) == NULL){
      printf("Couldn't open file");
      exit(1);
    }
    findNumColumns();
    list = malloc(sizeof(data*) * numColumns); //allocate number of columns

    // initialize each column
    for(int x = 0; x < numColumns; x++){
      list[x] = calloc(numItems,sizeof(data) * numItems);
    }
    findParsingValues();
    addValues();
    printPriorsAndClassifiers();
    predictTestSets();
    mathsHurrah();

    //unecessary as program ends: reminder if program altered in future
    free(numericAttributes);
    free(ignoredColumns);
    for(int x = 0; x < numColumns; x++){
      for(int y = 0; y < numItems; y++){
        free(substrings[x][y]);
      }
      free(substrings[x]);
      free(discriminationList[x]);
      free(list[x]);
    }
    free(list);
    free(substrings);
    free(discriminationList);

    return 0;
}
