# TrueBeaconSimulator

Question requirement: All requirements have been coded.


<h3>Pre-requisite</h3>

install boost lib, cmake  if not already. Below command is for ubuntu system
sudo apt-get update
sudo apt-get install build-essential
sudo apt-get install libboost-all-dev libssl-dev cmake

Code is built using vscode so each application has (dot)vscode folder which should be replaced with .vscode name if you want to compile from vscode UI. Also adjust path of g++ in tasks.json 


<h3>There are 3 applicatopns:</h3>

<h5>Simulator</h5><br></br>
  &nbsp;&nbsp;&nbsp;compile: /usr/bin/g++ -fdiagnostics-color=always -g -DBOOST_BIND_GLOBAL_PLACEHOLDERS -lboost_system -lboost_serialization -pthread /home/student/c++/TrueBeacon/simulator/*.cpp -o /home/student/c++/TrueBeacon/simulator/output<br></br>
  
  &nbsp;&nbsp;&nbsp;This application opens two ports on same machine 1234, 6153. 1234 is for publishing ticks and 6153 is for taking market and limit orders and downloading order history. This will be first application to start and we need to wait till it says "reading ticks completed" before starting other two application.<br></br>
  
  &nbsp;&nbsp;&nbsp;To run this an addition parameter is required of csv file, similar to what was shared on question link.
  eg: ./output ~/Downloads/hft-assignment-100<br></br>
  
  &nbsp;&nbsp;&nbsp;In this application we can adjust packet rate from stdin once all three applications are up. 

<h5>Client</h5><br></br>
  compile: /usr/bin/g++ -fdiagnostics-color=always -g -DBOOST_BIND_GLOBAL_PLACEHOLDERS -lboost_system -lboost_thread -lboost_coroutine -lboost_context -lboost_serialization -pthread /home/student/c++/TrueBeacon/client/*.cpp -o /home/student/c++/TrueBeacon/client/output<br></br>
  
  &nbsp;&nbsp;&nbsp;connects simulator on 1234. This will be second application to start. this will be receiving Ticking at the rate set in simulator.

<h5>OrderPlacer</h5><br></br>
  compile: /usr/bin/g++ -fdiagnostics-color=always -g -DBOOST_BIND_GLOBAL_PLACEHOLDERS -lboost_system -lboost_thread -lboost_coroutine -lboost_context -lboost_serialization -pthread /home/student/c++/TrueBeacon/orderPlacer/*.cpp -o /home/student/c++/TrueBeacon/orderPlacer/output<br></br>
  
  &nbsp;&nbsp;&nbsp;connects simulator on 6153. This will be last application to start. This will be placing order and downloading order History. Sample messaegs are present in it. we can extend it as required.
  
