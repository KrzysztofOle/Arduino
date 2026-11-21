# Arduino — archiwum szkiców i eksperymentów

Repozytorium gromadzi **niezależne szkice Arduino** i testy elektroniki. Nie jest jedną aplikacją ani jednolitym pakietem do instalacji.

## Zawartość
- `EchoServer/`, `Kanapka_EchoServer/`, `Kanapka_WebServer/`, `RelayTcpServer/` — eksperymenty komunikacji sieciowej i serwerów.
- `StepperControlerLCD/`, `Kanapka_graphicstest/`, `Server_TcpIP_TFT/` — sterowanie i interfejsy.
- `four_channel_relay_control/`, `change_i2c_address/`, `4hr_read_firmware_version/` — narzędzia i testy urządzeń.
- `Lib/`, `libraries/` — lokalne biblioteki i pliki pomocnicze.

## Uruchamianie
Otwórz **konkretny szkic** w Arduino IDE i dobierz płytkę, port oraz biblioteki na podstawie jego kodu i używanego sprzętu. Brak wspólnego pliku zależności lub uniwersalnej procedury flashowania. Weryfikuj przypisanie pinów i napięcia przed podłączeniem do rzeczywistych przekaźników, silników i urządzeń.

## Status
Kolekcja historycznych eksperymentów. Nie potwierdzono wspólnej wersji produkcyjnej ani automatycznych testów.
