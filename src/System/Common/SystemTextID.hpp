#pragma once


enum SYSTEXT
{
    SYSTEXT_EMPTY = 0,          // ""
    SYSTEXT_YES,                // "Yes"
    SYSTEXT_NO,                 // "No"
    SYSTEXT_OK,                 // "OK"
    SYSTEXT_LANG_EN,            // "English"
    SYSTEXT_LANG_DE,            // "Deutsch"
    SYSTEXT_LANG_IT,            // "Italiano"
    SYSTEXT_LANG_FR,            // "Franais"
    SYSTEXT_LANG_ES,            // "Espaol"
#ifdef TMNT2_BUILD_EU
    SYSTEXT_ATTENTION,          //  "Attention!""
    SYSTEXT_ARCADE_TVMODE,      //  "You are currently running Teenage Mutant Ninja Turtles2 in 50Hz Mode. 
                                //   Therefore, the original Teenage Mutant Hero Turtles(TM) arcade game will not 
                                //   run with optimum performance.For best performance, please choose 60Hz Mode when 
                                //   starting up Teenage Mutant Ninja Turtles2."
#endif /* TMNT2_BUILD_EU */
    SYSTEXT_TIT_CHECK,          //  "GAME_DATA_CHECK"
    SYSTEXT_TIT_SAVE,           //  "SAVE"
    SYSTEXT_TIT_LOAD,           //  "LOAD"
    SYSTEXT_SAVE_SURE,          //  "Do you want to save game data?"
    SYSTEXT_SAVE_SURE_OVERW,    //  "Do you want to overwrite the existing game data?"
    SYSTEXT_SAVE_NOW,           //  "Now saving game data."
    SYSTEXT_SAVE_OK,            //  "Game data saved successfully."
    SYSTEXT_SAVE_FAIL,          //  "Failed in saving game data."
    SYSTEXT_LOAD_SURE,          //  "Do you want to load game data?"
    SYSTEXT_LOAD_CHECK,         //  "Checking for game data."
    SYSTEXT_LOAD_EMPTY,         //  "No game data exists."
    SYSTEXT_LOAD_NOW,           //  "Now loading game data."
    SYSTEXT_LOAD_OK,            //  "Completed loading game data."
    SYSTEXT_LOAD_FAIL,          //  "Failed in loading game data."
    SYSTEXT_LOAD_INVALID,       //  "There is invalid game data."
    SYSTEXT_WAIT,               //  "\nPlease wait."
    SYSTEXT_FAIL_VIDEO,         //  "Failure initializing video."
    SYSTEXT_FAIL_AUDIO,         //  "Failure initializing audio."
    SYSTEXT_UNK_ERR,            //  "Undefined Error. The program will be shut down."
#ifdef TMNT2_BUILD_EU
    SYSTEXT_GAME_DISC    = 111, //  "Please insert the Game Disc %d."
    SYSTEXT_WARN_NEWGAME = 128, //  "Your current progress will be lost. Are you sure?"
#else /* TMNT2_BUILD_EU */
    SYSTEXT_GAME_DISC    = 107, //  "Please insert the Game Disc %d."
    SYSTEXT_WARN_NEWGAME = 121, //  "Your current progress will be lost. Are you sure?"
#endif /* TMNT2_BUILD_EU */

    // more systext codes relate to consoles and not used in PC version so currently missed
};
