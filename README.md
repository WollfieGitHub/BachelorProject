# BachelorProject - MDF/Stepper OpenSource Robotic Arm

![Image of the robotic arm in fusion 360](/Resources/Project_Documentation/Arm_Picture.png "Robotic Arm")

## Table of Content

1. [Building Prerequisites](#building-prerequisites) : Material and electronic components needed to build the arm
   1. [Electronics](#electronics)
   2. [Mechanical Pieces](#mechanical-pieces)
   3. [Raw Material](#raw-material)
2. [Assembly Instructions](#assembly-instructions) : Step-by-step instructions to build the arm
3. [Control Software](#control-software--cottus-arm-controller) : Download instructions for a basic software to control the arm
   1. [Download Instructions](#download-instructions) : Download instructions
   2. [Code Documentation](#developer-instructions): Instructions to code extensions fo the software
      1. [Server](#server) : The server side of the software
      2. [Client](#client) : The client side of the software
4. [Scholar Papers](#scholar-papers): The scholar papers that were useful to this project

## Building Prerequisites

### Electronics


### Mechanical Pieces

#### Screws
- [x16] : M4*12mm > Nema 17 Stepper Upper connection
- [x12] : M3*20mm > Nema 17 to Gearbox Adapter Connection
  - You might not need these
- 

### Raw Material

## Assembly Instructions



## Control Software : Cottus Arm Controller

### Download Instructions

### Developer Instructions

#### General Architecture
The arm's controller hosts a server which the software's web client
connects to in order to control it.

#### Server

##### Framework used :
The framework chosen to code the backend of the application is Quarkus. It is a recent
framework based on Spring Boot. It has a well documented API that I invite you to [check
here](https://quarkus.io/). 

#### Architecture :
```
src/java/packagename/
   ⎿ dto
   ⎿ exception
   ⎿ models
   ⎿ repositories
   ⎿ resources
   ⎿ security
   ⎿ services
   ⎿ utils
```
##### dto
Objects sent by Quarkus as a response to an HTTP request are serialized by the 
Jackson library. They are decomposed into basic types to the JSON format. 
In order to facilitate this process and agree with the frontend to a common representation
of the objects travelling over the network, we use the DTO (Data Transfer Objects) layer.

As such, each object is re-declared in this layer and made serializable for the Jackson
library. The exact same definition of the DTOs can be found on the frontend's side. 
Only fields interesting to the other side of the network should be specified on this layer,
as the rest of them are not relevant and may often constitute abstraction leaks.

##### exception
This directory holds all the custom exceptions for the application

##### models
This is the only directory (along with `utils` to some extent) that should hold the implementation
details of the model behind your application (also referred as "business logic"). 

In our case, it holds all the computation for the control of the arm. 

##### repositories

TODO

##### resources
The `resources` directory declares all the endpoints reachable by the frontend. Handling
of all types of HTTP requests (`GET`, `POST`, ...) is implemented here even though it can
be mostly summed up to forwarding the calls to the `services` layer.

##### security
This directory handles the security logic of the application, which currently isn't much.

For now, it simply checks that a client trying to establish a connection to websocket does
so with a valid token that it previously got from an HTTP request to the server.

##### utils
Everything that isn't really proper to the business logic goes here. That includes
things like implementation of mathematical concepts like Vector3D...

#### Client
##### Framework used :
The framework chosen to code the frontend of the application
is [React](https://reactjs.org/) with a layer of [TypeScript](https://www.typescriptlang.org/)
on top to enforce types and have a cleaner code with improved auto-completion from the IDE.

The React code also uses the [Material UI](https://mui.com/material-ui/getting-started/overview/)
library to have nice-looking components out of the box.

## Scholar Papers:
Papers that I read fully or just in diagonal
- [Design, Analysis and Implementation of a Robotic Arm](Resources/Papers/4DesignAnalysisandImplementationofaRoboticArm-TheAnimator.pdf)
- [Design and Implementation of a 6-DOF Intelligent Single-arm](Resources/Papers/25883917.pdf)
- [Modeling and Analysis of a 6 DOF robotic arm manipulator](Resources/Papers/CJEEE_2012.pdf)
- [Solving Kinematics Problems of a 6-DOF Robot Manipulator](Resources/Papers/CSC2593.pdf)
- [Design and Development of a Robotic Arm](Resources/Papers/Design_and_development_of_a_robotic_arm.pdf)
- [ANYpulator: Design and Control of a Safe Robotic Arm](Resources/Papers/eth-49493-01.pdf)
- [Design and Analysis of six DOF Robotic Manipulator](Resources/Papers/Pratheep_2021_IOP_Conf._Ser.__Mater._Sci._Eng._1057_012034.pdf)
- [Design, analysis and fabrication of robotic arm for sorting of multi-materials](Resources/Papers/Roboticarmforsortingofmulti-materialsZolBahri-Khoo.pdf)

