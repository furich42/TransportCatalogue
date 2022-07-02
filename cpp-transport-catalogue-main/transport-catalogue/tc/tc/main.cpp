
#include "transport_catalogue.h"
#include "json_reader.h"
#include "request_handler.h"
#include <cassert>
#include <iostream>


using namespace catalogue;


int main() {
    
    catalogue::TransportCatalogue catalogue;
    input::JSONReader reader;
    
    reader.ReadRequests(std::cin, std::cout, catalogue);
    
    //stat::ReadAndProcessQueries( catalogue, std::cin, std::cout);
    
    return 0;
}


//Bus 256: 6 stops on route, 5 unique stops, 5950 route length, 1.36124 curvature
//Bus 750: 7 stops on route, 3 unique stops, 27400 route length, 1.30853 curvature
//Bus 751: not found
//Stop Samara: not found
//Stop Prazhskaya: no buses
//Stop Biryulyovo Zapadnoye: buses 256 828


/*
 
 
 [
     {
         "buses": [
             "114"
         ],
         "request_id": 1
     },
     {
         "curvature": 1.23199,
         "request_id": 2,
         "route_length": 1700,
         "stop_count": 3,
         "unique_stop_count": 2
     }
 ] 
 ------------------------------------------------------
 {
   "base_requests": [
     {
       "type": "Bus",
       "stops": ["Morskoy vokzal", "Riviersky most"],
       "is_roundtrip": false,
       "name": "114"
     },
     {
       "type": "Stop",
       "name": "Riviersky most",
       "latitude": 43.587795,
       "longitude": 39.716901,
       "road_distances": {"Morskoy vokzal": 850}
     },
     {
       "type": "Stop",
       "name": "Morskoy vokzal",
       "latitude": 43.581969,
       "longitude": 39.719848,
       "road_distances": {"Riviersky most": 850}
     }
   ],
   "render_settings": {
  "width": 1200.0,
  "height": 1200.0,

  "padding": 50.0,

  "line_width": 14.0,
  "stop_radius": 5.0,

  "bus_label_font_size": 20,
  "bus_label_offset": [7.0, 15.0],

  "stop_label_font_size": 20,
  "stop_label_offset": [7.0, -3.0],

  "underlayer_color": [255, 255, 255, 0.85],
  "underlayer_width": 3.0,

  "color_palette": [
    "green",
    [255, 160, 0],
    "red"
  ]
},
   "stat_requests": [
     { "id": 1, "type": "Stop", "name": "Riviersky most" },
     { "id": 2, "type": "Bus", "name": "114" }
   ]
 }
 
  
 
 
 */
