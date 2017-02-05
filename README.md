# Stringless

Stringless is a real-time, markerless, single-camera facial motion capture system. By using [dlib](http://dlib.net/) facial landmark detection, Stringless captures facial motion data and sends it to an Autodesk Maya plug-in, where a user can map the motion capture data to a rigged face model.

Currently Stringless is still a work in progress. I am unable to accept pull requests at this time but issues for bugs + feature requests are welcome.

## Installation

Stringless requires [OpenCV](http://opencv.org/) to compile.

1. Clone a copy of Stringless:

    `$ git clone https://github.com/octop1/stringless.git`

2. Make a build directory and compile:

    ```
    $ cd stringless

    $ mkdir build

    $ cd build

    $ cmake .. -DUSE_AVX_INSTRUCTIONS=ON

    $ make
    ```

## Running Stringless

`$ src/Stringless -iw -flp ../ext/dlib/shape_predictor_68_face_landmarks.dat`