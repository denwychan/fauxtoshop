# Fauxtoshop
Implementation of a graphics editting program which allows the user to to edit images using the following filtering effects:

- Scatter
- Edge detection
- 'Green screen' with a background image
- Compare an image with another image

This is Assignment #1 of CS106B Programming Abstractions module by Stanford University

## How it works

Fauxtoshop runs on Qt Creator 5.9.0 for macOS
Fauxtoshop allows the user to edit images in the following formats: bmp, gif, ppm, jpg,png. On running the program will:

1. Ask them to open an image file using the file name or '?' to open the file browser (the graphics window automatically resizes with the image)
2. Ask the user to select 1 of 4 filters for image editting
3. Complete the editing using filtering effect
4. Ask the user whether they'd like to save the image by specifying the 'save as' file name
5. Clear the screen and allow the sequence to be repeated from the beginning

The programming techniques used include: 1. console interaction with the user, and 2. the Grid collection for image pixels manipulation

Refer to fauxtoshop.cpp in the src/ directory for the source code for implementation

## How the Filters Work

### 1. Scatter

Blurs the image by making it look like a shaken sand drawing
- Takes a scatter radius of how far the pixels should be scattered from the user
- For each pixel in the image, randomly select a pixel from a nearby row and col in the original within the scatter radius to provide colour for the pixel in new Grid. Select again, if the random selection is out of bounds
 * of orignal Grid

### 2. Edge Detection

Creates a new black and white image the same size as the original. A pixel is black if it is an edge or white if it isn't
- Takes an edge threshold from the user that specifies the difference between 2 pixels before they make 'edge'
- Loops over each pixel to determine whether it is an edge or not
- Check all 9 pixels (including the starting pixel at 0) to check if at least 1 neighbour has a difference greater than the threshold e.g. the pixel is an edge for [9, 8, 5, 3, 3, 0 (self), 4, 7, 8, 7] if the threshold < 9

### 3. 'Green screen' a sticker image onto a background image

Pastes a 'sticker' image on top of a 'background' image and ignores any part of the sticker that is close to pure green in color.
- Takes a new sticker image from the user
- Takes a tolerance for green that isn't pure green from the user
- Takes a pixel location from the user for where to place the sticker image on the background image
- Loops over each pixel, any pixel on the sticker image with difference greater than the green threshold will be copied onto the background, otherwise it will be ignored

### 4. Compare an image with another image

Count how many pixels differ between two images
- Takes a new image file name for comparison
- Counts how many pixels differ between the background and new images
- If the images are different, pops up a graphical window to display the differences

## Testing

Testing is performed via Qt Creators's 'Compare Output' feature for checking the output and 'Load Input Script' feature for running the input

The input and output scripts are stored in the output/ directory
