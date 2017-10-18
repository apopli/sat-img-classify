# Resemblance

* Author    : Anmol Popli <anmol.ap020@gmail.com> , Nitesh Arora <aranitesh101@gmail.com>

# OVERVIEW

This source code provides an implementation of an SVM based classifier to classify
satellite imagery.

# UNIX/LINUX

The code is compilable on Unix/Linux.

- Compilation. <br />
Automated compilation requires the make program.

- Library. <br />
This code requires the libpng, libjpeg and libtiff library.

- Image format. <br />
Formats supported are jpeg , png and tiff. 

-------------------------------------------------------------------------
Usage:
1. Download the code package and extract it. Go to the directory source_code. 

2. Five programs can be executed in this package. The first 3 are part of training the
classifier. The fourth tests the classifier on the traing data. The fifth applies the
classifier to any test images. <br />
imageio.c is the image I/O library and svm.cpp is the SVM library.

3. Compile the source code of IMG2TRAIN (img_traindata.c) by running <br />
make IMG2TRAIN <br />
Run the executable IMG2TRAIN. <br />
./IMG2TRAIN <br />
This converts the training windows from 4 bands to training data and stores it in the
file traindata.

4. Compile the source code of CROSSVAL (train_cv.c) by running <br />
make CROSSVAL <br />
The values of C,gamma and n are to be input in the function train_params inside the MAIN
of train_cv.c. <br />
Run the executable CROSSVAL. <br />
./CROSSVAL <br />
This performs n-fold cross-validation on traing data with inputs C and gamma and gives
the Cross Validation Accuracy in result.

5. Compile the source code of TRAIN (sat_train.c) by running <br />
make TRAIN <br />
The values of C and gamma are to be input in the function train_params inside the MAIN
of sat_train.c. <br />
Run the executable TRAIN. <br />
./TRAIN <br />
This trains the classifier with training data using the values of inputs C and gamma.
The classifier model is stored in the file traindata.model

6. Compile the source code of CLASSIFY_TRAIN (sat_classify.c) by running <br />
make CLASSIFY_TRAIN <br />
Run the executable CLASSIFY_TRAIN. <br />
./CLASSIFY_TRAIN <br />
This classifies the training data in traindata using the model in traindata.model and
stores the classified result in the file traindata.classified. Computes accuracy parametrs
and stores Confusion Matrix in the file confusion, Accuracy and Kappa Coefficient are is
output onto the terminal.

7. Compile the source code of CLASSIFY_TEST (test_classify.c) by running <br />
make CLASSIFY_TEST <br />
Run the executable CLASSIFY_TEST. <br />
./CLASSIFY_TEST RED_BAND.tif GREEN_BAND.tif BLUE_BAND.tif INFRARED_BAND.tif <br />
with :
* RED_BAND.tif is the red band image;
* GREEN_BAND.tif is the green band image;
* BLUE_BAND.tif is the blue band image;
* INFRARED_BAND.tif is the infrared band image. <br />
This classifies the test images of 4 bands into the 7 classes. Stores converted test data
in file testdata. Stores classified test data in file testdata.classified. Generates the
classified output image classified.jpg
where colors represent :
* blue - class 1
* green - class 2
* red - class 3
* cyan - class 4
* yellow - class 5
* magenta - class 6
* black - class 7

8. To delete redundant files, <br />
run make clean 

----Usage of demo file: <br />
Since we have already generated the file traindata.model and stored in the package directory,
CLASSIFY_TEST can be directly used to classify any input test images using this demo file.

In classify_demo.sh , change the parameters as: <br />
red as "full name of red band image"; <br />
green as "full name of green band image"; <br />
blue as "full name of blue band image"; <br />
infrared as "full name of infrared band image"

Run sh classify_demo.sh

Thanks
------
