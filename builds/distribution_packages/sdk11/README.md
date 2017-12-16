# SDK11 based Ruuvitag example programs

The `Eddystone` and `Weather-Station` example programs were used to demonstrate
the potential of RuuviTag during development and later during the Kickstarter campaign.

These packages are compatible with softdevice 2, mostly found on Ruuvitag B1 and B2s.

Eddystone demo broadcasts configurable data using Eddystone protocol. This demo will not
be ported over to SDK12 demonstrations, as [Nordic has confirmed they will make Eddystone part of their future SDK](https://github.com/NordicSemiconductor/nrf5-sdk-for-eddystone).
Once Nordic has released the official Eddystone SDK application, it will be supported on RuuviTags.

Weather station demo reads the BME280 sensor and broadcasts the sensor data in BASE91-encoded URL.
User can view the data by visiting the URL, data is decoded and displayed by client-side JavaScript
on the website.
