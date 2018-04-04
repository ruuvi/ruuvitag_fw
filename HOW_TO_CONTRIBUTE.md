# How to contribute

## Contents
- Setting up development environment
- Getting in touch with the community
- Contributing code for RuuviTag
- Contributing other services, examples, integrations

## Setting up development environment
To develop for RuuviTags you essentially need only your computer, a device with Bluetooth Low-Energy (BLE) support 
and a single RuuviTag.

On a modern computer you can try out our development virtual machine image [here](http://46.101.140.152/RuuviBuilder_public_01.ova).
User name is "ruuvi-user" and password is also "ruuvi-user".

If you want to setup your own environment, you need:

- git
- [ARMGCC](https://launchpad.net/gcc-arm-embedded) compiler
- [nrfutil](https://github.com/NordicSemiconductor/pc-nrfutil) program for generating DFU packets to upload to tag. 

You might additionally benefit from [JLink](https://www.segger.com/jlink-debug-probes.html) used to 
connect to tags with SWD, GDB debugger, and more advanced tools.

## Getting in touch with community
Join [Ruuvi Slack team](http://slack.ruuvi.com) to chat with the developers and other community members.

Take a look at [Ruuvi's Trello board](https://trello.com/b/kz1llpvK/ruuvitag-firmware) to see what people are working on.

Please check if any work is being done on the task you're interested in and either join or announce that you've started to work in the area
(for example by commenting on an appropriate Trello card).

## Contributing code to RuuviTag
Contributing is done by forking Ruuvi's main repository and making a branch which contains your work.
Once you're happy with your branch, please submit a pull request to Ruuvi's master branch through GitHub's web interface.

We do not have any strict coding guidelines or conventions to be followed.
However, please make sure any pull request you make follows the directory structure shown in the README
and ask for advice if you're unsure to where your code should go.

Once you're made a pull request, Ruuvi Team will run an automated Jenkins check to see that the main makefile compiles.
Please be sure that the main makefile compiles the code you've made.
If the Jenkins tests pass, your work is manually reviewed and tested.

Once we've reviewed your code, it will be merged into Ruuvi's main branch.
Once we've merged your code, we'll do our best to maintain the compatibility in future work of Ruuvi's software.
So you do not have to worry about some library change obsoleting your application.

## Contributing other services, examples, integrations
Aside from code that runs in RuuviTag, there's a lot of applications which use something that RuuviTag produces, for example weather logging services, tracking services etc.
Contact us through email at [info@ruuvi.com](mailto:info@ruuvi.com) or through [Slack](https://ruuvi.slack.com) and we'll see how we can support your work.
