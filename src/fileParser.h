/**
 * @file fileParser.h
 * @brief Header file for the FileParser class.
 */

 #ifndef FILEPARSER_H
 #define FILEPARSER_H
 
 #include <QString>
 #include <QList>
 #include "MooreMachine.h"
 
 // Define JsonState, JsonTransition, and JsonAutomaton structs
 struct JsonState {
     QString name;
     bool isInitial = false;
 };
 
 struct JsonTransition {
     QString name;
     QString fromName;
     QString toName;
 };
 
 struct JsonAutomaton {
     QString name;
     QString description;
     QList<JsonState> stateList;
     QList<JsonTransition> transitionList;
 };
 
 /**
  * @class FileParser
  * @brief A class for parsing JSON files and converting automaton data.
  */
 class FileParser {
 public:
     /**
      * @brief Load a MooreMachine from a JSON file and convert it to a JsonAutomaton structure.
      * @param filename Path to the JSON file.
      * @param machine Reference to the MooreMachine object to load.
      * @return JsonAutomaton structure containing the automaton data.
      */
     static JsonAutomaton loadAutomatonFromMooreMachine(const QString& filename, MooreMachine& machine);
 };
 
 #endif // FILEPARSER_H