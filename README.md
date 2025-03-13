# Abyss Engine
A dynamic game engine simulator developed in Visual Studio using C++ and integrates Lua for Scripting with the aid of Sol2 library. It uses ImGui and SFML for both its user interface and graphics rendering. 

# Demo
https://github.com/user-attachments/assets/cbc82624-5ddd-4aab-b1b8-d37192962d32

## Dynamic Script Preview
https://github.com/user-attachments/assets/4908c79c-3eff-4152-a86b-6242aa85a627

## Prefab Preview
https://github.com/user-attachments/assets/50d5dfa5-6cc3-4ba1-80dd-eb824c428ab6

## Editor-Embedded Game Preview
https://github.com/user-attachments/assets/efa3636a-df75-45a6-a668-0fa3bce944d4

## Project Setup
- Have Visual Studio Installed
- Run the solution file 'AbyssEngine.sln'
- Run the local windows debugger to compile
- ### Application Setup
  - Make some folder(e.g AbyssEngine) anywhere in library, then add an **Assets**(must be exact) to the folder
  - Copy **imgui** file configuration settings from project folder
  - Copy the necessary dill files from project folder Assets\Game\SFML\lib

## Libraries Used
- SFML: Simple and Fast Multimedia Library
- ImGui: Immediate Mode Graphical User Interface
- Sol2: Simple Object Layer
- NFD: Native File Dialog
- nlohmann json
