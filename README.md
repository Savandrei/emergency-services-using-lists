# Emergency Services Management Using Linked Lists

## Overview

This project implements a command-driven simulation of an emergency services dispatch system in C. It manages emergency incidents, available response units, and interventions using linked data structures.

Incidents are assigned one of three priority levels: `high`, `medium`, or `low`. When a dispatch is requested, the system selects an incident from the highest-priority non-empty queue and assigns the first available unit. The program also keeps an intervention history so that the most recent ongoing dispatch can be undone.

The implementation was developed for a data structures and algorithms assignment focused on linked lists, queues, and stacks.

## Features

- Register incidents with an ID, priority, description, and initial status.
- Maintain separate queues for high-, medium-, and low-priority incidents.
- Track available response units in a queue.
- Dispatch available units to incidents according to priority.
- Undo the most recent dispatch that is still in progress.
- Mark an intervention as solved and return its unit to the available queue.
- Display unit details, incident details, and the intervention history.
- Release dynamically allocated memory during program cleanup.

## Data Structures

The implementation uses several linked structures, each with a specific role.

| Structure | Purpose |
| --- | --- |
| Circular linked queue of units | Stores pointers to currently available response units. |
| Three circular linked priority queues | Store pointers to incidents awaiting dispatch, separated by priority. |
| Circular doubly linked incident list | Keeps a record of all incidents. A sentinel node simplifies list operations. |
| Circular doubly linked intervention list | Stores interventions in the order they were initiated. |
| Linked stack of interventions | Keeps pointers to interventions so the latest ongoing dispatch can be undone. |

The queues store pointers to existing units or incident nodes rather than copying the full structures. This allows status changes to be reflected in the central records.

## Supported Commands

### `ADD_INCIDENT <ID> <priority> "<description>"`

Creates an incident with the specified ID, priority, and description. The incident starts with status `queued`, is added to the incident list, and is inserted into the corresponding priority queue.

Valid priorities are `high`, `medium`, and `low`.

### `CHECK_UNITS_AVAILABILITY`

Prints the number of currently available units:

```text
Number of available units: <count>
```

### `DISPATCH`

If at least one unit and one queued incident are available, assigns the first available unit to the highest-priority incident waiting for service. The incident status becomes `intervened`, the unit becomes unavailable, and the intervention is recorded in both the intervention list and the history stack.

### `UNDO_LAST_DISPATCH`

Searches backward through the dispatch history until it finds an intervention that has not been solved. That intervention is removed from the intervention list, its incident returns to status `queued` and is placed at the front of its priority queue, and its unit is returned to the available queue.

Solved interventions in the history are skipped.

### `SOLVED_INCIDENT <ID>`

Marks the specified incident as `solved` if it currently has an ongoing intervention. Its assigned unit becomes available again. The intervention remains in the intervention list as part of the record.

### `SHOW_UNIT <ID>`

Displays the unit's ID, type, and availability:

```text
Unit <ID> is type <A/B/C> and is <available/unavailable>
```

### `SHOW_INCIDENT <ID>`

Displays the incident's ID, priority, description, and current status.

### `SHOW_INTERVENTIONS`

Lists all interventions in the order they were recorded. If no intervention has been initiated, it prints:

```text
No intervention has been initiated
```

### `list`

An additional debugging command that prints the incident list, the three priority queues, and the intervention stack to the console. This command is intended for inspecting internal state and is separate from the normal output written to `tema1.out`.

## Error Handling

Invalid operations print the following message to the output file:

```text
INVALID OPERATION! ERROR 404
```

This applies when dispatching without an available unit or queued incident, undoing a dispatch when no ongoing intervention remains in the history, solving an unknown incident or one that is not currently being handled, or requesting details for an unknown unit or incident.

## Input and Output

The program reads input from `tema1.in` and writes command results to `tema1.out`.

The input begins with the number of response units, followed by one line per unit containing its ID and type. Next comes the number of commands, followed by the commands and any required arguments.

Example input:

```text
3
1 A
2 B
3 C
7
CHECK_UNITS_AVAILABILITY
ADD_INCIDENT 101 high "Road accident"
ADD_INCIDENT 102 low "Blocked road"
DISPATCH
SHOW_INCIDENT 101
SHOW_INTERVENTIONS
CHECK_UNITS_AVAILABILITY
```

The program expects the input file to be available in the working directory when it runs.

## Build and Run

A `Makefile` is included in the project directory. From that directory, compile using:

```bash
make
```

Run the compiled program from the directory containing `tema1.in`:

```bash
./tema1
```

The executable name depends on the target defined in the provided `Makefile`. If necessary, inspect the `Makefile` for the exact target and compiler flags.

To remove generated build files, use:

```bash
make clean
```

if the `Makefile` defines a `clean` target.

## Implementation Notes

- The main function is organized into initialization, command processing, and deallocation.
- Incident descriptions are dynamically allocated.
- The incident and intervention records use circular doubly linked lists with sentinel nodes.
- The priority queues and available-unit queue preserve FIFO order during normal processing.
- `UNDO_LAST_DISPATCH` puts the postponed incident at the front of its priority queue so it can be considered again before other incidents at the same priority.
- The program frees the lists, queues, stack, unit array, and command buffer before exiting.

## Project Structure

```text
emergency-services-using-lists/
└── emergency services using lists/
    ├── Makefile
    ├── README
    ├── SDA___Tema_1_2026.pdf
    └── tema1.c
```

## Technologies

- C
- Standard C library
- Dynamic memory allocation
- Linked lists, queues, and stacks
