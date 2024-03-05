#  Overview
Detecting and localizing objects in an image is a challenging computational task. The overall problem space is complex and robust solutions in practical scenarios need many computational descriptors and vectors. In this project we explore using color as one such data point to accomplish this.

# Working
While color representation is only one vector (among others) to help address this problem, we are hopeful to get a good understanding of color representation and color spaces. You will be given a small library of object images where each image represents an object. All these images will be in a standard format representing the picture of one colorful object
placed against a green background. You may process these images to make color representations of objects. You will be given an input image which contains a combination of objects some of which will be in your library. You need to display your processed output
where you need to outline using a bounding box around the area of pixels that match any
object detected from the library. You are free to extend the display code sample given of
assignment 1 to implement this project. No external libraries or scripting environments are
allowed.
The program takes a few parameters.
- InputImage.rgb : This is the input image that contains a one of more of colorful objects some of which (or none of which) will be in your library.
- A list of objects object1.rgb object2.rgb ...objectn.rgb which will represent your library of objects.



All the images will be of the same size 640x480.
To invoke the program, we will compile it and run it at the command line as 

```
YourProgram.exe InputImage.rgb object1.rgb object2.rgb .... objectn.rgb
```


where the number of objects (objectn.rgb) may vary between 1 to n. Here is an example data set of the input image and library images with an expected output.
