Smart Traffic Management System Simulator
Introduction
The Smart Traffic Management System Simulator is a dynamic and interactive simulation designed to replicate real-world traffic management in an urban environment. Built using data structures such as graphs, heaps, and priority queues, this simulator aims to optimize urban traffic flow with features like real-time vehicle routing, congestion visualization, and emergency handling. It showcases the practical application of algorithms and data structures to solve complex traffic problems efficiently.

Features
1. Graphical Visualization
The city’s road network is represented as a weighted, directed graph.
Nodes represent intersections, while edges represent roads, with weights denoting travel time or congestion levels.
Provides both text-based and graphical visualizations for better clarity.
2. Dynamic Vehicle Addition
Vehicles can be added to the network dynamically during simulation.
Supports real-time updates for vehicle positions, start, and destination points.
3. Dynamic Congestion Visualization
Monitors the number of vehicles on each road segment using hash tables.
Displays real-time congestion levels visually, highlighting congested routes for analysis.
Alerts the system to reroute vehicles from congested roads to less busy alternatives.
4. Road Blocking
Simulates disruptions like road closures or accidents.
Roads or intersections can be blocked dynamically during the simulation.
Automatically updates the graph structure and recalculates routes for all affected vehicles.
5. Dynamic Path Finding
Uses Dijkstra's Algorithm for optimal route calculation.
Recalculates paths dynamically in response to changes in traffic conditions or road availability.
Supports A Algorithm* for faster emergency vehicle routing.
6. Traffic Signal Management
Dynamically adjusts signal timings based on vehicle density at intersections using priority queues.
Implements an emergency override system for clearing traffic during critical situations.
Ensures smooth traffic flow and reduces wait times through real-time signal management.
7. Emergency Vehicle Handling
Prioritizes routing for emergency vehicles to ensure minimal delays.
Temporarily overrides normal traffic operations to clear a path for emergency vehicles.
Restores normal traffic flow once the emergency situation is resolved.
8. Accident and Road Closure Simulation
Handles disruptions in real-time by blocking affected roads or intersections.
Automatically recalculates routes for vehicles and updates the network visualization.
Analyzes the impact of disruptions on overall traffic flow.
9. Simulation Dashboard
Offers an interactive console-based interface for visualization and control.


**Displays:**


**Road Network**
![image](https://github.com/user-attachments/assets/614b19bd-06fe-4590-ba9f-36009f441e69)
Road Netwrok Visualization

**Congestion levels and affected zones.**
![image](https://github.com/user-attachments/assets/dcfb131c-7950-44b1-87f1-ecb347660215)
Red Lines: Heavy congestion
Yello Line: Moderate Congestion
Green Line: Light Congestion


**Signal statuses at intersections.**
![image](https://github.com/user-attachments/assets/e90d9cb8-5446-4d64-a7f0-35cbcb7f5baa)
