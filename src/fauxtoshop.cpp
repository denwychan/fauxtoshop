//// This is the CPP file you will edit and turn in.
//// Also remove these comments here and add your own.
//// TODO: rewrite this comment


/*
 * Instructions for the programme
 * 1. Greet the user
 * 2. Ask them to open an image file
 * 3. Give user 4 filter options
 *      a. Scatter
 *      b. Edge detection
 *      c. 'Green screen' with another image
 *      d. Compare image with another image
 * 4. User confirms choice of options
 * 5. Complete filtering effect
 * 6. Ask user whether they'd like to save the image
 * 7. After saving, clear the screen and print 1 blank line
 * 8. Repeat the sequence from the beginning
 *
 *
 * Techniques utilised: 1. console interaction and 2. the Grid collection
 * Extension: one easy mouse click interaction
 * Tips: declare and use one GWindow for the duration of the programme to avoid the programme crashing
 * Images files are in the res directory (same level as src and lib) - in this location, Qt creator
 * will allow these to be used without additional path information
 * Main function:
 * To open image, declare a GBufferedImage object, and then call the openImageFromFilename function,
 * returning true or false
 * Tip: If you type "?" as the file name, openImageFromFilename will pop up a graphical file chooser window
 * to minimise typing
 * After the image is opened, resize the GWindow to be the same size as the image, and add the image to the
 * GWindow by calling add. Example code for adding a BufferedImage to GWindow
//gw.setCanvasSize(img.getWidth(), img.getHeight());
//gw.add(&img, 0, 0);
* Make sure GBufferedImage object you add to the GWindow not cease to exist (go out of scope) after you
* add it to the GWindow. Tip: use GBufferedImage in main and call gw.clear(); to remove images
* Tip: If it takes too long to open a file, use a smaller image
* To save the image, call saveImageToFilename, returning true or false.Tip: If you type "?" as the file name,
* saveImageToFilename will pop up a graphical file chooser window to minimise typing
* Images will be saved in build directory (sibling directory of the "Fauxtoshop" project directory) with a
* name like build-Fauxtoshop-Desktop_Qt_5_2_0_MinGW_32bit-Debug
*/

#include "console.h"
#include "gbufferedimage.h"
#include "grid.h"
#include "gwindow.h"
#include "fauxtoshop-provided.h"   // instructor-provided code
#include "simpio.h"
#include "random.h"
#include "strlib.h"
#include "vector.h"

#include <cstdlib>
#include <iostream>
#include <limits>
using namespace std;


//Function prototypes
void nameImageFileToOpen(GBufferedImage &img, GWindow &gw, bool openFile = true);
void openImage(GBufferedImage &img, GWindow &gw);
int selectFilterOptions();
Grid<int> convertImageToGrid(GBufferedImage img);
int validateUserInput(string promptMessage, int lowerBound, int upperBound);
void getRandomPixelColor(const Grid<int> &original, Grid<int> &newImage, int row, int col, int scatterRadius);
void applyScatterFilter(Grid<int> &original);
void applyEdgeDetectionFilter(const Grid<int> &original, Grid<int> &blackWhiteGrid);
void applyGreenScreenFilter(Grid<int> &original, GBufferedImage &sticker, GWindow &gw, int& row, int& col);
void compareImages(const GBufferedImage img, GWindow gw);
void saveImage(GBufferedImage &img, Grid<int> &imageGrid);
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
    //Set up for the sticker for the green screen filter
    GBufferedImage sticker;
    int stickerRow;
    int stickerCol;

    while(true){
        //Ask the user to open an image file
        nameImageFileToOpen(img, gw);
        //User confirms which filter options to use: scatter, edge detection, 'green screen', compare image
        int optionNum = selectFilterOptions();

        //Convert your GBufferedImage object into a Grid<int>
        Grid<int> imageGrid = convertImageToGrid(img);
        //Set up a new image grid for the black and white output for edge detection
        Grid<int> blackWhiteGrid(imageGrid.numRows(), imageGrid.numCols());

        //Complete filtering effect based on filter choice:
        //1.scatter, 2. edge detection, 3. 'green screen', 4. compare image
        switch (optionNum){
            case 1:
                applyScatterFilter(imageGrid);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img, imageGrid);
                break;
            case 2:
                applyEdgeDetectionFilter(imageGrid, blackWhiteGrid);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img, blackWhiteGrid);
                break;
            case 3:
                applyGreenScreenFilter(imageGrid, sticker, gw, stickerRow, stickerCol);
                //Prompt the save to ask if they would like to save the new image
                saveImage(img, imageGrid);
                break;
            case 4:
                compareImages(img,gw);
                break;
            default:
                cout << "Not a valid option" << endl;
                break;
        }
        //Print a blank line after the programme is run once
        cout << endl;
        gw.clear();
     }

    /*
     * Repeat the sequence from the beginning
     * Execution Log Hint: the Console window has a File → Compare Output feature for checking your output,
     * and a Load Input Script feature to auto-type
     *
     * Image Creation: create your own art using multiple effects and your own pic. Name them art.png, art2.png, art3.png
    */
    cout << "Exiting." << endl;
    return 0;
}

//Supporting functions
/*
 * Function: nameImageFileToOpen
 * Usage: Prompts the user to open an image based on the file name. Adds the
 * image to the window if the image is found
 * Params: GBufferedImage, GWindow
 * -----------------------------------------------------------------
 * Returns: None. Void function
*/
void nameImageFileToOpen(GBufferedImage &img, GWindow &gw, bool openFile){
    while (true){
        string imageName;
        if (openFile){
                    imageName = getLine("Enter name of image file to open ('?' to open file selector or blank to quit): ");
        } else {
            imageName = getLine("Enter name of image file to open ('?' to open file selector): ");
        }
        //TO DO: blank to quit condition
        if (openFile && imageName.empty()){
            gw.close();
        }
        //If the correct file name is given, load the image and break, otherwise keep promting the user
        if (openImageFromFilename(img, imageName)){
            if (openFile){
                cout << "Opening image, please wait..." << endl;
                openImage(img, gw);
            }
            break;
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
    while (true){
        int optionNum;
        //Give user 4 filter options: scatter, edge detection, green screen, compare with other image
        cout << "Which image filter would you like to apply?" << endl;
        cout << "        1. Scatter" << endl;
        cout << "        2. Edge detection" << endl;
        cout << "        3. 'Green screen' with another image" << endl;
        cout << "        4. Compare image with another image " << endl;
        //Validate that the user is entering an integer data type between 1 and 4
        optionNum = validateUserInput("Enter option value between 1 and 4:", 1, 4);
        return optionNum;
    }
}

/*
 * Function: validateUserInput
 * Usage: Ensure user input is valid, other reprompt the user to re-enter
 * Params:
 * ------------------------------------------------------------
 * Returns:
*/

int validateUserInput(string promptMessage, int lowerBound, int upperBound = INT_MAX){
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
 * Function: convertImageToGrid
 * Usage: Converts an image into a grid of integers representing the pixels and
 * their RGB (Red, Green, Blue) values (0 - 255)
 * Params: GBufferedImage object, GWindow object
 * ------------------------------------------------------------
 * Returns: Grid of integers
*/
Grid<int> convertImageToGrid(GBufferedImage img){
    // converting a BufferedImage to a Grid of ints
    Grid<int> original = img.toGrid();
    return original;
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
 * a. Prompt user to provide a scatter radius of how far the pixels should be scattered, if input is invalid reprompt
 * b. Create a new Grid with the same dimensions as original Grid
 * c. For each pixel in new Grid, randomly select a pixel from a nearby row and col in the original Grid in the
 * scatter radius to provide colour for the pixel in new Grid. Select again, if the random selection is out of bounds
 * of orignal Grid
 * Hint: to match expected outputs, use fakeRandomNumberGenerator() at the start of main function to return the same
 * sequence, and use row-major order to generate 'dr' and 'dc' values
 * Params: original - image Grid of integers, scatterRadius - integer (1 - 100 inclusive),
 * ------------------------------------------------------------
 * Returns: newImage - Grid of integers
*/

void applyScatterFilter(Grid<int> &original){
    //Prompt user for scatter radius input between 1 and 100 (inclusive)
    int scatterRadius = validateUserInput("Enter the scatter radius as number between 1 and 100: ", 1, 100);
    //Iterate through each pixel in the image grid
    for (int i = 0; i < original.numRows(); i++){
        for (int j = 0; j < original.numCols(); j++) {
        //Randamly select nearby pixel (row and col) to provide new colour
        getRandomPixelColor(original, i, j, scatterRadius);
        }
    }
}

/*
 * Function: applyEdgeDetectionFilter
 * Usage: Create a new black and white image the same size as the original. A pixel is black if it is an edge or
 * white if it is not an edge
 * Function:
 * a. Prompt user to provide an edge threshold that specifies the difference between 2 pixels before they make
 * an 'edge'. If input is invalid reprompt
 * b. Create a new Grid with the same dimensions as original Grid
 * c. Loop over each pixel to determine whether it is an edge or not
 * d. Check all 9 pixels (including self at 0) to check if at least 1 neighbour has a difference greater than
 * the threshold e.g. the pixel is an edge for [9, 8, 5, 3, 3, 0 (self), 4, 7, 8, 7] if threshold < 9
 * Hint: pixels near edges and corners may not have all 9 neighbors! Use Grid class inBounds member function to
 * cater for this
 * Params: original - image Grid of integers, edgeThreshold - positive integer
 * ------------------------------------------------------------
 * Returns: newImage - Grid of integers
*/

void applyEdgeDetectionFilter(const Grid<int> &original, Grid<int> &blackWhiteGrid){
    //Prompt user for scatter radius input as a positive integer
    int edgeThreshold = validateUserInput("Enter the edge threshold as a positive number: ", 0, INT_MAX);
    //Iterate through each pixel
    for (int i = 0; i < original.numRows(); i++){
        for (int j = 0; j < original.numCols(); j++) {
            //Define boolean to set whether each pixel is an edge or not
            bool isEdge = false;
            //Check each pixel bordering the pixel at relative location (0,0) to detect the edge
            for(int relative_i = -1; relative_i < 2; relative_i++){
                for(int relative_j = -1; relative_j <2; relative_j++){
                    if (!isEdge && original.inBounds(i + relative_i, j + relative_j)){
                        //Calculate the RBG difference which defines whether this is an edge
                        int pixel1 = original[i][j];
                        int pixel2 = original[i + relative_i][j + relative_j];
                        int edge = calculateRBGColourDifference(pixel1
                                                                , pixel2);
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
}

/*
 * Function: applyGreenScreenFilter
 * Usage: Paste a "sticker" image on top of a "background" image, but ignore any part of the sticker that
 * is close to pure green in color.
 * Function:
 * Image in the main menu is the background image.
 * a. Prompt user to specify a new image file name to be the sticker image. Re-prompt if open fails
 * b. Create a new Grid with the same dimensions as original Grid (for background and sticker)
 * c. Prompt user to specify tolerance for green that isn't pure green (0x00FF00)
 * d. Prompt user to specify location as (row,col) for where to place the sticker image. Reprompt if the
 * input is invalid. If the user hits enter, they can specify the location with a mouse click (see starter
 * code)
 * d. Place the image in the location specified. Loop over each pixel, any pixel on the sticker image with
 * difference greater than threshold from pure green will be copied onto the background, otherwise ignore.
 * Sticker imagecan be cut off on the bottom or right edge(s) if it cannot completely fit on the background.
 * Params: sticker - GBufferedImage, gw - GWindow, row - where the sticker is place, col -
 * where the sticker is place
 * ----------------------------------------------------------------------------------------------------------
 * Returns: nothing. Void function
*/

//Grid<int> applyGreenScreenFilter(GBufferedImage &sticker, GWindow &gw, int& row, int& col){
void applyGreenScreenFilter(Grid<int> &original, GBufferedImage &sticker, GWindow &gw, int& row, int& col){
    //Prompt user to specify a new image file name to be the sticker image
    nameImageFileToOpen(sticker, gw, false);
    //Prompt user to specify tolerance for green that isn't pure green
    int greenTolerance = validateUserInput("Enter the green tolerance as a between 0 and 100 (inclusive): "
                                           , 0, 100);
    //Prompt user to specify location as a vector for where to place the sticker image
    validateLocationInput(row, col);
    //Create a new Grid with the same dimensions as the sticker
    Grid<int> stickerImage = convertImageToGrid(sticker);
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
 * Call showDiffWindow function, passing it your GWindow and the second image's file name as parameters.
 * Ask a BufferedImage for its filename by calling getFilename() on it as shown below.
 * // pop up a window to display differences between the two images
 * showDiffWindow(gw, otherImg)
 * Params: original - image Grid of integers, newImage - image Grid of integers
 * ------------------------------------------------------------
 * Returns: nothing. Void function.
*/

void compareImages(const GBufferedImage img, GWindow gw){
    GBufferedImage newImg;
    //Prompt user to specify a new image file name for comparison
    nameImageFileToOpen(newImg, gw, false);
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

void saveImage(GBufferedImage &img, Grid<int> &imageGrid){
    while (true){
        //Convert the image grid back into an image for saving
        img.fromGrid(imageGrid);
        //Prompt the user to ask if they'd like to save the image
        string newImageName = getLine("Please enter file name to save image or blank to skip: ");
        if (newImageName.empty()){
            //If the user enters blank the image will not be saved
            cout << "OK, no saving this time. That's cool!" << endl;
            cout << endl;
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
