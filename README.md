# online-info-olympiad
This is a server/client project which can be used to set up a online informatics olympiad. 

## Dependencies(shared libs):

1. **libpq++** - c++ connector library for accessing PostgreSQL databases.
2. **boost** - a collection of many useful libraries. Regex and filesystem modules are used here. 

## It supports the following commands:  

**login**               -command to log in  
**logout**              -command to log out  
**exit**                -exit program  
**register**            -register new user  
**lang_add**            -add support for a programming language to server  
**lang_remove**         -remove programming language support from server  
**new_olympic**         -create new olympic  
**list_olympic**        -list all olympics  
**del_olympic**         -remove an olympic  
**apply**               -choose to participate to an olympic  
**undo_apply**          -choose to not participate to an olympic  
**exercise_add**        -create an exercise and add it to an olympic  
**exercise_rem**        -delete an exercise from an olympic  
**list_ex**           -view all exercises from an olympic  
**lang_olympic_add**    -add programming language support to olympic  
**lang_olympic_rem**  -remove programming language support to olympic  
**lang_olympic_list**   -view all programming languages available on an olympic  
**compile_exercise**   -check if your solution is right  
**submit_exercise**     -submit your exercise solution  
**current_exercises**   -view exercises from olympics that are current running and you are a participant of  
**view_exercise**       -view exercise description  
**view_leaderboard**    -show leaderboard of an olympic  
**help**                -view help  
