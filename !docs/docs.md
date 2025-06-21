
| Mnemonic                                            |                         |                        |     |
| --------------------------------------------------- | ----------------------- | ---------------------- | --- |
| SYS - Jump to a machine code routine                | addr                    |                        |     |
|                                                     | 12 bit                  |                        |     |
|                                                     |                         |                        |     |
| CLS - Clear the display.                            |                         |                        |     |
|                                                     |                         |                        |     |
| RET - Return from a subroutine.                     |                         |                        |     |
|                                                     |                         |                        |     |
| JP - Jump to location                               | addr                    |                        |     |
|                                                     | 12 bit                  |                        |     |
|                                                     | V0                      | addr                   |     |
|                                                     | 8 bit register          | 12 bit                 |     |
|                                                     |                         |                        |     |
| CALL - Call subroutine                              | addr                    |                        |     |
|                                                     | 12 bit                  |                        |     |
|                                                     |                         |                        |     |
| SE - Skip next instruction if 1-st operand = 2-nd   | Vx                      | byte                   |     |
|                                                     | 8 bit register          | 8 bit                  |     |
|                                                     | Vx                      | Vy                     |     |
|                                                     | 8 bit register          | 8 bit register         |     |
|                                                     |                         |                        |     |
| SNE - Skip next instruction if 1-st operand != 2-nd | Vx                      | byte                   |     |
|                                                     | 8 bit register          | 8 bit                  |     |
|                                                     | Vx                      | Vy                     |     |
|                                                     | 8 bit register          | 8 bit register         |     |
|                                                     |                         |                        |     |
| LD - set 1-st operand to 2-nd                       | Vx                      | byte                   |     |
|                                                     | 8 bit register          | 8 bit                  |     |
|                                                     | Vx                      | Vy                     |     |
|                                                     | 8 bit register          | 8 bit register         |     |
|                                                     | I                       | addr                   |     |
|                                                     | special memory register | 12 bit                 |     |
|                                                     | Vx                      | DT                     |     |
|                                                     | 8 bit register          | special delay register |     |
|                                                     | DT                      | Vx                     |     |
|                                                     | special delay register  | 8 bit register         |     |
|                                                     | ST                      | Vx                     |     |
|                                                     | special sound register  | 8 bit register         |     |
|                                                     | F                       | Vx                     |     |
|                                                     | special operand         | 8 bit register         |     |
|                                                     | B                       | Vx                     |     |
|                                                     | special operand         | 8 bit register         |     |
|                                                     | \[I]                    | Vx                     |     |
|                                                     | special operand         | 8 bit register         |     |
|                                                     | Vx                      | \[I]                   |     |
|                                                     | 8 bit register          | special operand        |     |
|                                                     | Vx                      | K                      |     |
|                                                     | 8 bit register          | special operand        |     |

