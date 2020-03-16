# GooberEats
A route navigation project developed for Winter Quarter 2020 CS 32 at UCLA. Implements an A* algorithm and a file of map data to navigate through the streets of Los Angeles, CA and print food delivery instructions to the user.
Traffic rules are followed as if the user is a pedestrian or food delivery robot, not as an automobile.

This program is split into 5 main files. Navigate to the Project4 file to access these files.
ExpandableHashMap.h provides a template hash map data structure which can assign keys of any class type to values of any class type.
StreetMap.cpp uses an ExpandableHashMap object to provide loading and storage functionality for map and coordinate data throughout any location. In this project, Los Angeles is used as the location.
PointToPointRouter.cpp provides route calculation between two geographic locations by using streets stored in a StreetMap object to connect the dots with the shortest path possible.
DeliveryOptimizer.cpp optimizes a set of delivery instructions by approximating the best order in which to deliver multiple meals after starting from a centralized food depot. This class attempts to solve the Traveling Salesman Problem without calculating all N! routes which can be taken.
Finally, DeliverPlanner.cpp plans a delivery route by using the other 5 classes to calculate the shortest path in which to deliver all the user's meals. The calculated instructions are then printed out to the user.

To run this project, compile all the files together and run main.cpp. This will provide an executable line which when entered into your command line will print out delivery instructions including all streets to be turned to and proceeded along.
