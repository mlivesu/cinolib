This folder contains a number of sample programs that illustrate the functionalities of CinoLib. 

At the moment there are just a few sample programs. Unfortunately maintaining a library alone is very time consuming, and the amount of time I can spend on CinoLib is limited.

I am currently working on a wide number of code samples that showcase the core functionalities of CinoLib. They should be available in the following weeks. All of these topics will be covered:

* Copy/paste points of view from the GL canvas for accurate visual comparisons and fast snapshot reproduction
* Customize data structures with user defined mesh/vertex/edge/polygon/polyhedron attributes
* Generate, process and visualize scalar fields
* Generate, process and visualize vector fields
* Overview of the topology editing operators implemented in CinoLib
* Evaluation of mapping distortions (angle/areas distortion popular metrics)
* Evaluation of 2D polygon metrics (kernels, incircles, circumcircles...)
* Computation of conformal maps for surface meshes
* Parameterization based hexahedral remeshing
* Surface and volumetric format converters
* Solve multi-labeling problems on surfaces and volumes using the [GraphCuts](http://vision.csd.uwo.ca/code/) wrapper
* Integral curve computation (and tessellation)
* Iso curve computation (and tessellation)
* Subdivision schemas for hexahedral meshes
* Uniform sphere sampling using spherical Fibonacci

and much more!

### How to compile the example projects
To compile the examples you'll need to download CinoLib and [Eigen](http://eigen.tuxfamily.org) on your machine, and to install [Qt](https://www.qt.io) and [libQGLViewer](http://libqglviewer.com/index.html) for the GUI. Each sample project comes with its own configuration file (.pro), in which details on the aforementioned libraries are specified. Alternatively, you can compile them all by considering the file `build_all_examples.pro`. Configuration fles require minimal editing prior compilation. Specifically, you need to:

* change the path `/usr/local/include/eigen3` with the local path to your Eigen installation
* change the path `/Library/Frameworks/QGLViewer.framework/Versions/2/Headers` with the local path to your QGLViewer installation
* change the `.` in `-L. -lQGLViewer` with your local path to the QGLViewer library

Once you do it, you can open your terminal on the selected examples folder, and type
```
qmake .
make -j4
```

### Additional dependencies
Notice that some of the projects may also depend from external libraries which are wrapped in CinoLib (e.g. Triangle, Tetgen or VTK). In this case, such libraries should be installed separately, and the configuration files updated with correct paths for compiler and linker. If you are not interested in these examples, you can safely comment them (adding `#` at the beginning of the corresponding lines in `build_all_examples.pro`, and they will be ignored by qmake).
