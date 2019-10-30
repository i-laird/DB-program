# DB-program
Experiment Manager for CSI 3335

Manages a MySQL database used to store experiments. Features extensive error checking in order to ensure that all data entered into the DB is valid.

## Features
- Experiment Entry
  - Enter the Metadata for an experiment
    - each field type
    - if field is required for each instance of the experiment
    - the name of the field
- Experiment Run
  - enter the results of an instance of the experiment
  - enforces all METADATA restraints for the data
- Experiment Info
  - prints all METADATA saved for an experiment
- Experiment Run
  - print information about saved instances of an experiment
  - by default only prints general information, but more can be displayed at will
- Experiment Report
  - generate HTML doc
- display aggregate
  - displays aggregate values for an experiment
    

## Built with
- MS Vidual Studio
- MySQL
- MySQL connector
