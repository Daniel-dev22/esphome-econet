# esphome-econet

## Archived Repo
- This repo is being archived in favor of consolidating future development in a single repo located [here](https://github.com/esphome-econet/esphome-econet).

This ESPHome package creates a local network connection to econet-based devices, such as an HVAC system or water heaters such as the Rheem Heat Pump Water Heater (HPWH), and creates entities in Home Assistant to control and monitor the devices. This package provides more detailed sensors and control capability than the Rheem econet cloud-based integration available in Home Assistant. However, both can coexist.

## Get Started
 - The component is relatively easy to get setup. The first part and arguably the most difficult is simply getting the required hardware.
 - For a detailed guide see the [wiki](https://github.com/Daniel-dev22/esphome-econet/wiki/Get-Started).

## Discord Server
 - A discord server is available for troubleshooting and general collaboration
 - [Discord Server Invite link](https://discord.gg/MtEsC77xRg).

## Protocol Documentation ##

Example commands to change a heat pump water heater settings [here](https://github.com/daniel-dev22/esphome-econet/blob/main/m5atom-rs485-econet.yaml)

Decode this into Charcode [here](https://gchq.github.io/CyberChef/#recipe=From_Charcode('Space',16)Strings('Single%20byte',4,'Alphanumeric%20%2B%20punctuation%20(A)',false,false,false/disabled)&input=MHg4MCwgMHgwMCwgMHgxMiwgMHg4MCwgMHgwMCwgMHg4MCwgMHgwMCwgMHgwMywgMHg0MCwgMHgwMCwgMHgxMiwgMHgwMCwgMHgwMCwgMHgxRiwgMHgwMSwgMHgwMSwgMHgwMCwgMHgwNywgMHgwMCwgMHgwMCwgMHg1NywgMHg0OCwgMHg1NCwgMHg1MiwgMHg1MywgMHg0NSwgMHg1NCwgMHg1MCwgMHg0MiwgMHhGOCwgMHgwMCwgMHgwMCwgMHhFNCwgMHhFRQ)

Credits:

- This project is a fork of [Stockmopar's Repo](https://github.com/stockmopar/esphome-econet) It has been modified with additional features that the original project does not have. I also want to thank Stockmopar for his work on laying the foundation for this project and decoding the protocol as none of this would have existed without his help.

- https://github.com/syssi/esphome-solax-x1-mini

- https://github.com/glmnet/esphome_devices
