# Транспортный справочник
Транспортный справочник - формирует базу данных автобусных остановок и автобусных маршрутов.
По запросу может выводить информацию о маршруте, или об остановке.
По запросу может формировать карту автобусных маршрутов в формате SVG.
Взаимодействие со справочником осуществляется путем отправки JSON запросов.

## Сборка

```
mkdir build
cd build
cmake ../
cmake --build .
```

Для работы необходима библиотека protobuf 3.0

## Пример использования:

```
transport_catalogue [make_base|process_requests]
```
``` make_base``` - команда для создания базы данных
```process_requests``` - команда для чтения из JSON документа запросов на вывод данных

Далее следует ввести JSON запрос, где:

```
base_requests - данные об остановках и маршрутах
render_settings - данные для настроек для отрисовки карты
stat_requests - запросы к базе данных на отрисовку карты, вывод информации о маршруте и остановке
```

Выходные данные так же представляются в формате JSON

Пример запроса к справочнику:

``` 
{
    "base_requests": [
      {
        "type": "Bus",
        "name": "114",
        "stops": ["Морской вокзал", "Ривьерский мост"],
        "is_roundtrip": false
      },
      {
        "type": "Stop",
        "name": "Ривьерский мост",
        "latitude": 43.587795,
        "longitude": 39.716901,
        "road_distances": {"Морской вокзал": 850}
      },
      {
        "type": "Stop",
        "name": "Морской вокзал",
        "latitude": 43.581969,
        "longitude": 39.719848,
        "road_distances": {"Ривьерский мост": 850}
      }
    ],
    "render_settings": {
      "width": 200,
      "height": 200,
      "padding": 30,
      "stop_radius": 5,
      "line_width": 14,
      "bus_label_font_size": 20,
      "bus_label_offset": [7, 15],
      "stop_label_font_size": 20,
      "stop_label_offset": [7, -3],
      "underlayer_color": [255,255,255,0.85],
      "underlayer_width": 3,
      "color_palette": ["green", [255,160,0],"red"]
    },
    "stat_requests": [
      { "id": 1, "type": "Map" },
      { "id": 2, "type": "Stop", "name": "Ривьерский мост" },
      { "id": 3, "type": "Bus", "name": "114" }
    ]
  }
  ```
  
  Версия языка C++ 17
