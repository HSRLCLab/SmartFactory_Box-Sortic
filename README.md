# Smart Box
The Code includes comments to give a better understanding of what is happening in the Code.

If you with to see some graphs and get more explanation, I'll refer to my documentation (ask [here](https://www.hsr.ch/) for permission).

The Code is written in C++ on the Framework Platformio (IDE: Visual Studio Code).






## Communication Model
```mermaid
sequenceDiagram
SmartBox -->> Vehicles: I'm full
Vehicles -->> SmartBox: here are my Parameters
SmartBox-->> Vehicles : ok, I'm asking for VehicleX
Vehicles -->> SmartBox: VehicleX is coming
Vehicles -->> SmartBox: VehicleX is transporting Box
Vehicles -->> SmartBox: VehicleX has transported and returned Box
Note right of Vehicles : Asynchronous <br/> MQTT communication



```

