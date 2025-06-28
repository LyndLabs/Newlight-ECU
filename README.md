# NewLight ECU State Machine
Current code can be found under `main.cpp`.

## Operating Plan:
- Use one core for monitoring (**CORE-M**) and other for executing (**CORE-E**).
- **CORE-E** handles state machine logic
- **CORE-M** handles interrupts, logging, 
- Use STM32 mailbox to pass LOGLEVEL from **CORE-E** to **CORE-M**
    - `critical`: restart
    - `error`:    shutdown

## ToDo:
- [ ] MAP IGS / Valves to GPIO
- [ ] Implement Supply 

## Timesheet
#### Jun 27
*3 hrs*
- Implemented Purging State Machine
- Set up STM32H7 IDE
