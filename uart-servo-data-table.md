# UART Servo Data Table
Due to space limitations, the following tables use some [abbreviations](#Abbreviation-List).
## Basic Data (Read Only)
| Data ID | Caption          | Data Length | Data type | Description                   |
|---------|------------------|-------------|-----------|-------------------------------|
| 1       | voltage          | 2           | uint      | Unit: mV                      |
| 2       | current          | 2           | uint      | Unit: mA                      |
| 3       | power            | 2           | uint      | Unit: mW                      |
| 4       | temperature      | 2           | uint      | Unit: °C                      |
| 5       | status           | 1           | uint      | See [status details](#Status) |
| 6       | model            | 2           | uint      |                               |
| 7       | firmware version | 2           | uint      |                               |
| 8       | serial number    | 4           | uint      |                               |
### Details
#### Status
* **bit 0** : The bit will be set to 1 during command execution, and be set to 0 after the command is completed.
* **bit 1** : The bit will be set to 1 when a command execution error is occured, and be set to 0 after the next correct execution.
* **bit 2** : The bit will be set to 1 when a high voltage error is occured, and be set to 0 after the voltage is normal.
* **bit 3** : The bit will be set to 1 when a low voltage error is occured, and be set to 0 after the voltage is normal.
* **bit 4** : The bit will be set to 1 when an overcurrent error is occured, and be set to 0 after the current is normal.
* **bit 5** : The bit will be set to 1 when power ouput of the servo is higher than the setting value, and be set to 0 as the power ouput is within the allowable range.
* **bit 6** : The bit will be set to 1 when temperature of the servo is higher than the setting value, and be set to 0 as the temperature is within the allowable range.
* **bit 7** : The bit will be set to 1 when a stall error is occured, and be set to 0 after the stall is released.
## User Setting Data
| Data ID | Caption      | Data Length | Data type | Description                                                    |
|---------|--------------|-------------|-----------|----------------------------------------------------------------|
| 32      | reserved     | 1           | uint      | Reserved field, must be set to 1 when write data to the servo. |
| 33      | responsive   | 1           | uint      | See [responsive details](#Responsive)                          |
| 34      | id           | 1           | uint      | Servo ID.</br>Default value: 0x00                              |
| 35      | control mode | 1           | uint      |                                                                |
|36| baud index|1 | uint| |
|37| stall protection| 1| uint| |
|38| | 2| | uint|
|39| | 2| | uint|
|40| | 2| | uint|
|41| | 2| | uint|
|42| | 2| | uint|
|43| | 2| | uint|
|44| | | | |
|45| | | | |
|46| | | | |
|47| | | | |
|48| | | | |
|49| | | | |
|50| | | | |
|51| | | | |
|52| | | | |
|53| | | | |

### Details
#### Responsive
In the spin or rotate mode, whether the servo has responded after completing the command.</br>If the value is set to non-zero(true), the old command cannot be interrupted by the new command. The waiting commands will be added to a limited queue, if the queue is full, the newest one is discarded.</br>If the value is set to zero(false), the past command can be interrupted by the new command and the response packet will not be sent back.</br>The default value is zero (false).
## Abbreviation List
| Name | Description      |
|------|------------------|
| int  | signed integer   |
| uint | unsigned integer |
| mV   | millivolts       |
| mA   | milliamps        |
| mW   | milliwatts       |