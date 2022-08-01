////
/// This is an implementation of a graphics editing program that allows the user to
/// edit an image using 4 filters:
/// a. Scatter - blurs the image
/// b. Edge detection - creates a black & white image
/// c. 'Green screen' a sticker image onto a background image
/// c. Compare an image with another image
/// After the filter has been applied the user has the option of saving the image
/// and repeating the filters application on onto the same image or another image
////

//Using Stanford libraries
#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
#include "simpio.h"
#include "random.h"
#include "strlib.h"
#include "vector.h"
//Using the c++ standard libraries
#include <iostream>
using namespace std;

//Function prototypes
bool nameImageFileToOpen(GBufferedImage &img, const string& prompt, bool exitWhenUserEntersBlank = true);
void openImage(GBufferedImage &img, GWindow &gw);
int selectFilterOptions();
int getValidInteger(const string& promptMessage, int lowerBound, int upperBound);
void getRandomPixelColor(const Grid<int> &original, Grid<int> &newImage, int row, int col, int scatterRadius);
void applyScatterFilter(GBufferedImage &img);
void applyEdgeDetectionFilter(GBufferedImage &img);
void applyGreenScreenFilter(GBufferedImage &img);
void compareImages(const GBufferedImage& img, GWindow &gw);
void saveImage(GBufferedImage &img);
int calculateRBGColourDifference(int pixel1, int pixel2);
void validateLocationInput(int& row, int& col);

int main() {
    //1. Greet the user
    cout << "Welcome to Fauxtoshop!" << endl;
    //Basic setup of Graphics Window
    GWindow gw;
    gw.setTitle("Fauxtoshop");
    gw.setExitOnClose(true);
    gw.setVisible(true);
    //Enable the same random numbers on each run
    fakeRandomNumberGenerator();
    //Set up for the main image
    GBufferedImage img;

    //Ask the user to open an image file
    while(nameImageFileToOpen(img, "Enter name of image file to open ('?' to open file selector or blank to quit): ")){
        openImage(img, gw);
        //User confirms which filter options to use: scatter, edge detection, 'green screen', compare image
        int optionNum = selectFilterOptions();

        //Complete filtering effect based on filter choice:
        //1.scatter, 2. edge detection, 3. 'green screen', 4. compare image
        switch (optionNum){
            case 1:
                applyScatterFilter(img);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img);
                break;
            case 2:
                applyEdgeDetectionFilter(img);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img);
                break;
            case 3:
                applyGreenScreenFilter(img);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img);
                break;
            case 4:
                compareImages(img,gw);
                break;
            default:
                cout << "Not a valid option" << endl;
                break;
        }
        //Print a blank line after the programme is run once and repeat sequence from the beginning
        cout << endl;
        gw.clear();
     }
    return 0;
}

//Supporting functions
/*
 * Function: nameImageFileToOpen
 * Usage: Prompts the user to open an image based on the file name. Adds the image to the window if
 * the image is found. If the user enters blank then no image is opened.
 * Params: GBufferedImage, GWindow
 * -----------------------------------------------------------------
 * Returns: True if an image was opened, false otherwise.
*/
bool nameImageFileToOpen(
        GBufferedImage &img, const string& prompt, bool exitWhenUserEntersBlank){
    while (true){
        string imageName = getLine(prompt);
        //Quit the programme when the user enters blank as a file name
        if (exitWhenUserEntersBlank && imageName.empty()){
            cout << "Exiting... see you later - bye!" << endl;
            return false;
        }
        //If a valid file name is given, load the image and return true, otherwise keep promting the user
        if (openImageFromFilename(img, imageName)){
            cout << "Opening image, please wait..." << endl;
            pause(1000);
            return true;
        }
        cout << "Sorry I can't find this file. Please try again" << endl;
    }
}

/*
 * Function: openImage
 * Usage: Opens an image in the library and add to the window
 * Params: GBufferedImage object, GWindow object
 * ------------------------------------------------------------
 * Returns: None. Void function
*/
void openImage(GBufferedImage &img, GWindow &gw){
    // adding a BufferedImage to GWindow
    gw.setCanvasSize(img.getWidth(), img.getHeight());
    gw.add(&img, 0, 0);
}

/*
 * Function: selectFilterOptions
 * Usage: Prompts the user to select 1 of 4 filter options as an integer
 * Params: None
 * -----------------------------------------------------------------
 * Returns: optionNum - the option as an Integer
*/

int selectFilterOptions(){
    //Give user 4 filter options: scatter, edge detection, green screen, compare with other image
    cout << "Which image filter would you like to apply?" << endl;
    cout << "        1. Scatter" << endl;
    cout << "        2. Edge detection" << endl;
    cout << "        3. 'Green screen' with another image" << endl;
    cout << "        4. Compare image with another image " << endl;
    //Validate that the user is entering an integer data type between 1 and 4
    return getValidInteger("Enter option value between 1 and 4:", 1, 4);
}

/*
 * Function: getValidInteger
 * Usage: Ensure user integer input is valid, other reprompt the user to re-enter.
 * Params:
 * ------------------------------------------------------------
 * Returns:
*/

int getValidInteger(const string& promptMessage, int lowerBound, int upperBound = INT_MAX){
    while (true){
        int userInput = getInteger(promptMessage);
        if (userInput >= lowerBound && userInput <= upperBound ){
            return userInput;
        }
        cout << "Invalid input! Please try again" << endl;
    }
}

/*
 * Function: validateLoctionInput
 * Usage: Ensure user input for specifying a location on the canvas/ window is valid,
 * otherwise reprompt the user to re-enter
 * The user can enter the location in '(<row>, <col>)' format or hit enter
 * to do this with a mouse click
 * Params: row - where the sticker is place, col - where the sticker is place
 * ------------------------------------------------------------
 * Returns: nothing. Void function
*/
void validateLocationInput(int& row, int& col){
    while (true){
        string locationInput = getLine("Enter sticker location in '(<row>, <col>)' format or hit enter to do this with a mouse click: ");
        //Validate that the input starts and ends with "("")" and contains ","
        if (locationInput.empty()){
            cout << "Click on the canvas to place your sticker :-)" << endl;
            getMouseClickLocation(row, col);
            break;
        } else if (startsWith(locationInput,"(")
                && endsWith(locationInput,")")
                && stringContains(locationInput, ",")
                ){
            //Remove the '('')' for delimitting
            //Note: this method causes a bug where if the user enters more than 1 set of brackets e.g. '((2,3)), this would
            //incorrectly pass validation. This is too annoying to fix so left for future improvement
            locationInput = stringReplace(locationInput, "(", "");
            locationInput = stringReplace(locationInput, ")", "");
            //Delimit the input string using "," and convert it into vector
            Vector<string> inputVector = stringSplit(locationInput, ",");
            //Validate that the vector has 2 elements and both elements can be converted into an integer
            if (inputVector.size() == 2
                    && stringIsInteger(inputVector[0])
                    && stringIsInteger(inputVector[1])
                    ){
                //Convert both elements into integers
                row = stringToInteger(inputVector[0]);
                col = stringToInteger(inputVector[1]);
                if(row > 0 && col > 0){
                    break;
                }
            }
        }
        cout << "Invalid input! Please try again" << endl;
    }
}

/*
 * Function: calculateRBGColourDifference
 * Usage: Calculates the difference between 2 colours for edge detection and green screen effects
 * Function:
 * a. Separate the pixels into their RGB components
 * b. Take difference between each pair of RGB values (e.g. 2 reds, 2 blues, 2 greens)
 * c. Take absolute value between for each difference (use nteger abs function in <math> standard)
 * d. Take a max of the three differences (max function for 2 values at a time)
 * scatter radius to provide colour for the pixel in new Grid. Select again, if the random selection is out of bounds
 * of orignal Grid
 * Hint: use int data type
 * Extension: Alternate calcalatons
 * Params: pixel1_row - integer, pixel1_col - integer, pixel2_row - integer, pixel2_col - integer
 * ----------------------------------------------------------------------------------------------------------------------
 * Returns: maxDifference - integer
*/

int calculateRBGColourDifference(int pixel1, int pixel2){
    //Instatiate variables for red, green, blue for pixel 1
    int red1, green1, blue1;
    //Instatiate variables for red, green, blue for pixel 2
    int red2, green2, blue2;
    //Get the individual red green blue elements for both pixel 1 and 2 so they can be compared
    GBufferedImage::getRedGreenBlue(pixel1, red1, green1, blue1);
    GBufferedImage::getRedGreenBlue(pixel2, red2, green2, blue2);
    //Calculate the difference between the red, green, blue components and take the absolute (positive) value
    int redDiff = abs(red1 - red2);
    int greenDiff = abs(green1 - green2);
    int blueDiff = abs(blue1 - blue2);
    int maxDifference = max(redDiff, greenDiff);
    maxDifference = max(maxDifference, blueDiff);
    return maxDifference;
}

/*
 * Function: getRandomPixelColor
 * Usage: For a specific pixel, gets the colour of a nearby pixel within the scatter radius
 * Function:
 * a. Calculate the random row and random column value for the random pixel based on the
 * scatter radius
 * b. Calculate whether the random pixel location is in bounds of the original image grid
 * c. If in bounds, assign the colour of the random pixel to the specified pixel
 * Params: original - grid of integers related to the original image,
 * newImage - grid of integers related to the new image,
 * row - integer, col - integer, scatterRadius - integer
 * -------------------------------------------------------------------------------------------
 * Returns: None. Void function.
*/

void getRandomPixelColor(Grid<int> &original
                         , int row, int col
                         , int scatterRadius){
    while (true){
        //For a specified pixel, calculate the random pixel location based on selecting
        //a random row and column within the scatter radius
        //The negative and positive radius value forms the upper and lower bounds
        int randomRow = row + randomInteger(-scatterRadius, scatterRadius);
        int randomCol = col + randomInteger(-scatterRadius, scatterRadius);
        //If the location of the random pixel is in bounds of original image grid
        //assign the colour of the random pixel to the original pixel
        //If the condition is not met, keep searching for an in bounds random pixel
        if (original.inBounds(randomRow, randomCol)){
            original[row][col] = original[randomRow][randomCol];
            break;
        }
    }
}

// getting the red/green/blue components of a pixel
//    int pixel = original[row][col];
//    int red, green, blue;
//    GBufferedImage::getRedGreenBlue(pixel, red, green, blue);
//    Tip: Make all images changes to a Grid<int> object by calling the img.fromGrid,
//which takes a Grid<int> as its argument. Reasons: 1. efficiency 2. practice using the Grid collection by doing looping and operations

/*
 * Function: applyScatterFilter
 * Usage: Make the image look like a shaken sand drawing
 * Function:
 * a. Prompt user to provide a scatter radius of how far the pixels should be scattered integer (1 - 100 inclusive),
 * if input is invalid reprompt
 * b. For each pixel in new Grid, randomly select a pixel from a nearby row and col in the original Grid in the
 * scatter radius to provide colour for the pixel in new Grid. Select again, if the random selection is out of bounds
 * of orignal Grid
 * Hint: to match expected outputs, use fakeRandomNumberGenerator() at the start of main function to return the same
 * sequence, and use row-major order to generate 'dr' and 'dc' values
 * Params: original - image Grid of integers
 * ------------------------------------------------------------
 * Returns: nothing. Void function
*/

void applyScatterFilter(GBufferedImage &img){
    //Convert your GBufferedImage object into a Grid<int>
    Grid<int> original = img.toGrid();
    //Prompt user for scatter radius input between 1 and 100 (inclusive)
    int scatterRadius = getValidInteger("Enter the scatter radius as number between 1 and 100: ", 1, 100);
    //Iterate through each pixel in the image grid
    for (int i = 0; i < original.numRows(); i++){
        for (int j = 0; j < original.numCols(); j++) {
        //Randamly select nearby pixel (row and col) to provide new colour
        getRandomPixelColor(original, i, j, scatterRadius);
        }
    }
    img.fromGrid(original);
}

/*
 * Function: applyEdgeDetectionFilter
 * Usage: Create a new black and white image the same size as the original. A pixel is black if it is an edge or
 * white if it is not an edge
 * Function:
 * a. Prompt user to provide an edge threshold that specifies the difference between 2 pixels (positive integer)
 * before they make an 'edge'. If input is invalid reprompt
 * b. Loop over each pixel to determine whether it is an edge or not
 * c. Check all 9 pixels (including self at 0) to check if at least 1 neighbour has a difference greater than
 * the threshold e.g. the pixel is an edge for [9, 8, 5, 3, 3, 0 (self), 4, 7, 8, 7] if threshold < 9
 * Hint: pixels near edges and corners may not have all 9 neighbors! Use Grid class inBounds member function to
 * cater for this
 * Params: original - image Grid of integers, blackWhiteGrid - image Grid of integers
 * ------------------------------------------------------------
 * Returns: nothing. Void function
*/

void applyEdgeDetectionFilter(GBufferedImage &img){
    //Prompt user for scatter radius input as a positive integer
    int edgeThreshold = getValidInteger("Enter the edge threshold as a positive number: ", 0, INT_MAX);
    //Set up a new image grid for the black and white output for edge detection
    Grid<int> original = img.toGrid();
    Grid<int> blackWhiteGrid(original.numRows(), original.numCols());
    //Iterate through each pixel
    for (int i = 0; i < original.numRows(); i++){
        for (int j = 0; j < original.numCols(); j++) {
            //Define boolean to set whether each pixel is an edge or not
            bool isEdge = false;
            //Check each pixel bordering the pixel at relative location (0,0) to detect the edge
            for(int rowOffset = -1; rowOffset < 2; rowOffset++){
                for(int colOffset = -1; colOffset <2; colOffset++){
                    if (!isEdge && original.inBounds(i + rowOffset, j + colOffset)){
                        //Calculate the RBG difference which defines whether this is an edge
                        int pixel1 = original[i][j];
                        int pixel2 = original[i + rowOffset][j + colOffset];
                        int edge = calculateRBGColourDifference(pixel1, pixel2);
                        //If this is an edge, set as black, else set as white
                        //If the edge is larger than the edgeThreshold, this is an edge
                        if (edge > edgeThreshold){
                            isEdge = true;
                            blackWhiteGrid[i][j] = BLACK;
                        } else {
                            blackWhiteGrid[i][j] = WHITE;
                        }
                    }
                }
            }
        }
    }
    img.fromGrid(blackWhiteGrid);
}

/*
 * Function: applyGreenScreenFilter
 * Usage: Paste a "sticker" image on top of a "background" image, but ignore any part of the sticker that
 * is close to pure green in color.
 * Function:
 * Image in the main menu is the background image.
 * a. Prompt user to specify a new image file name to be the sticker image. Re-prompt if open fails
 * b. Prompt user to specify tolerance for green (integer between 0 and 100 (inclusive) that isn't
 * pure green (0x00FF00)
 * c. Prompt user to specify location as (row,col) for where to place the sticker image. Reprompt if the
 * input is invalid. If the user hits enter, they can specify the location with a mouse click
 * d. Place the image in the location specified. Loop over each pixel, any pixel on the sticker image with
 * difference greater than threshold from pure green will be copied onto the background, otherwise use
 * the background image
 * Sticker imagecan be cut off on the bottom or right edge(s) if it cannot completely fit on the background.
 * Params: original - the background GBufferedImage, sticker - GBufferedImage, gw - GWindow,
 * row - where the sticker is place, col - where the sticker is place
 * ----------------------------------------------------------------------------------------------------------
 * Returns: nothing. Void function
*/

//Grid<int> applyGreenScreenFilter(GBufferedImage &sticker, GWindow &gw, int& row, int& col){
void applyGreenScreenFilter(GBufferedImage& img){
    //Prompt user to specify a new image file name to be the sticker image
    GBufferedImage sticker;
    nameImageFileToOpen(sticker, "Enter name of image file to open ('?' to open file selector): ", false);
    //Prompt user to specify tolerance for green that isn't pure green
    int greenTolerance = getValidInteger("Enter the green tolerance as a between 0 and 100 (inclusive): "
                                           , 0, 100);
    //Prompt user to specify location as a vector for where to place the sticker image
    int row;
    int col;
    validateLocationInput(row, col);
    Grid<int> original = img.toGrid();
    //Create a new Grid with the same dimensions as the sticker
    Grid<int> stickerImage = sticker.toGrid();
    //Iterate through each pixel
    for (int i = 0; i < stickerImage.numRows(); i++){
        for (int j = 0; j < stickerImage.numCols(); j++) {
            //Identify pixels that are not pure green for green screen effect
            int nonGreenScreen = calculateRBGColourDifference(stickerImage[i][j], GREEN);
            //Pixels with difference greater than the green tolerance will be copied onto the background
            //otherwise ignore
            if (nonGreenScreen > greenTolerance && original.inBounds(row + i, col + j)){
                original[row + i][col + j] = stickerImage[i][j];
            }
        }
    }
    img.fromGrid(original);
}

/*
 * Function: compareImages
 * Usage: Count how many pixels differ between two images
 * Function:
 * Image in the main menu is the background image.
 * a. Prompt user to specify a new image file name for comparison
 * b. Count how many pixels differ between the background and new images
 * Hint:Use countDiffPixels member function in the GBufferedImage class
 * c. Print results to user
 * i. For > 0 counts, print 'These images differ in _ pixel locations!'
 * ii Or 'These images are the same!'
 * d. If the images are different, pop up a graphical window to display the differences
 * Params: original - image Grid of integers, newImage - image Grid of integers
 * ------------------------------------------------------------
 * Returns: nothing. Void function.
*/

void compareImages(const GBufferedImage &img, GWindow &gw){
    GBufferedImage newImg;
    //Prompt user to specify a new image file name for comparison
    nameImageFileToOpen(newImg, "Enter name of image file to open ('?' to open file selector): ", false);
    //Count how many pixels differ between the background and new images
    int numDiffPixels = img.countDiffPixels(newImg);
    if (numDiffPixels > 0 ){
        cout << "These images differ in " << numDiffPixels << " pixel locations!" << endl;
        // pop up a window to display differences between the two images
        showDiffWindow(gw, newImg.getFilename());
    } else {
        cout << "These images are the same!" << endl;
    }
}

/*
 * Function: saveImage
 * Usage: Saves an image after the filter is applied on the image grid.
 * Function:
 * 1. Ask the user whether they'd like to save the image
 * 2. The User enters a file name in order to save the image
 * 3. The user is reprompted if the saving is unsuccesful
 * 4. After saving, clear the screen and print 1 blank line
 * Tip: If the user types "?" as the file name, a graphical file chooser window will pop up
 * to minimise typing for long file names
 * Params: img - GBuggeredImage, imageGrid - Grid<int>
 * ------------------------------------------------------------------------------------------
 * Returns: Nothing. Void function
*/

void saveImage(GBufferedImage &img){
    while (true){
        //Prompt the user to ask if they'd like to save the image
        string newImageName = getLine("Please enter file name to save image or blank to skip: ");
        if (newImageName.empty()){
            //If the user enters blank the image will not be saved
            cout << "OK, no saving this time. That's cool!" << endl;
            pause(2000);
            break;
        //Save the image if the user enters a valid file name
        } else if (saveImageToFilename(img, newImageName)){
            cout << "Saving image as '" << newImageName << "' successful!" << endl;
            break;
        }
        //Reprompt the user if there is an issue with the saving
        cout << "Oops! Something went wrong... Please try again" << endl;
    }
}
