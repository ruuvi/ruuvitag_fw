Readme for Nordic Semiconductor plugin for Bluetooth Developer Studio (BDS)
-------------------------------------------------------------------------------------------------------------

The contents of this file are applicable to the following software versions:
* Bluetooth Developer Studio:       v1.0.2095.0 or newer
* Nordic Semiconductor BDS Plugin:  v1.2.1
* Nordic Semiconductor SDK:         v10.0.0


Header file generation
----------------------------
Data structures like structs and enums are generated in the header file by iterating through all levels of
Profile members, specifically Services -> Characteristics -> Fields -> Bits/Enumerations.

Header file code generation for services is highly dependent upon the properties of each Characteristic. 
If for instance the characteristic has the Read property set to "Mandatory", the service and service_init 
struct will have a member called "is_read_supported". The same applies to other properties, such as 
Notify and Indicate.

Set and Send functions are also generated for each characteristic depending on the properties mentioned above.
If a characteristic's Read property is set to anything but Excluded, a Set function definition will be 
generated for that characteristic. If a characteristic's Notify or Indicate properties are set to anything 
but Excluded, a Send function definition will be generated for the characteristic.


Code file generation
----------------------------
Code file code generation for services is equally dependent upon the properties of each Characteristic.

Encode and decode functions are created as necessary for each characteristic and its fields (structs and enums).
If a characteristic's Read, Notify or Indicate properties are set to anything but Excluded, an Encode function
will be generated for that characteristic and appropriate fields. If a characteristic's Write or WriteWithoutResponse 
properties are set to anything but Excluded, a Decode function will be generated for that characteristic and
appropriate fields.

The code will also have an on_write function, the contents of which are dependent upon the properties of each of 
the service's characteristics. If Notify or Indicate is supported by the characteristic, the on_write function will
have a code block for handling CCCD events. If Write or WriteWithoutResponse is supported by the characteristic, 
the on_write function will have a code block for handling value handles events. For Write and WriteWithoutResponse
a code block for handling value handle write events will be generated in the on_rw_authorize_request function.

In the service_init function, if a characteristic's Read, Notify or Indicate properties are supported, code for
initializing and adding the characteristic will be generated.

Matching the code generated in the header file, if a characteristic's Read property is supported, its Set function
implementation will be generated. If a characteristic's Notify or Indicate properties are supported, its Send
function implementation will be generated.


service_if header file and code file
---------------------------------------
The service_if.h and service_if.c files should be copied into the SDK experimental BDS template project folder,
replacing the placeholder files that are already there. All other generated files (one header/code file per service)
should also be copied to the template project folder, and the code files must be added to the project in the IDE.