# Leader Mod
Try [AMX Version](https://github.com/ShingekiNoRex/CS1.6-LeaderMode) before you decide to download this. The AMX version is utterly playable with almost the same contents.

## What is this?
A gameplay mod originally wrote in Pawn(AmxModX).<br/>
Exclusive roles will be assigned to each player in both teams. The roles mean unique skills, weapon limitations, and playstyles.<br/>
The ultimate goal is to eliminate the other team. Use whatever strategies you want. Tips: kill their leader will stop their redeployment.

## How to install?
1. You have to own copies of all these three games: [Half-Life](https://store.steampowered.com/app/70/HalfLife/), [Condition Zero](https://store.steampowered.com/app/80/CounterStrike_Condition_Zero/) and [Counter-Strike](https://store.steampowered.com/app/10/CounterStrike/).
1. Rename the RESOURCE/ folder to "leadermode" or something like that, then place it along with GoldSrc engine like other mods.
1. Install fonts located in EXTERNAL/FONTS/ folder into Windows.
1. Download the released version or build the game DLLs by yourself. Place the game DLLs in their corresponding locations.
1. Build [metahook](https://github.com/nagist/metahook). Rename the .exe file to "leadermode" as well, and place it along with the GoldSrc engine. Alternatively, you can use the one located in EXTERNAL/LAUNCHER/.
1. Move the fmod.dll in EXTERNAL/LAUNCHER/ to where leadermode.exe is currently located.
1. Start leadermode.exe and enjoy the game.

## Known Issues
* Please report in the _ISSUE_ section.

## Commands
| Command                             | Description                                     |
| :---------------------------------- | :---------------------------------------------- |
| +qtg                                | Throw grenade. It works just like R6:S. (no cooking support... yet.) |
| lastinv                             | Cancel grenade throw. It also works as original quick weapon switch. |
| eqpnext                             | Choose the next equipment for +qtg usage from your inventory. |
| eqpprev                             | Choose previous equipment for +qtg usage from your inventory. |
| melee                               | Swing the default knife for an emergency attack. This won't cancel reload but pause it. |
| executeskill                        | Execute skills designed for your role. |
| declarerole                         | Open a menu with which you may choose your role. |
| votescheme                          | Vote the tactical scheme for your team. |
| changemode                          | Use the special function of the current weapon. |

## Configuration (CVars)
| CVar                               | Default | Min | Max | Type   | Description                                    |
| :--------------------------------- | :-----: | :-: | :-: | :----: | :--------------------------------------------- |
| mp_client_cvar_query_inv           | 10      | 0   | inf.| Server | The time interval of server querying cvar from clients. |
| lm_commander_maxhealth             | 1000    | 0   | inf.| Server | Start HP of commander. |
| lm_godfather_maxhealth             | 1000    | 0   | inf.| Server | Start HP of godfather. |
| lm_human_role_priority             | 1       | 0   | 1   | Server | [0 - BOTs have equal chances of becoming important roles.]<br/>[1 - Humans first.] |
| lm_menpower_per_player             | 1       | 0   | inf.| Server | Starting manpower per player in his corresponding team. |
| lm_ballot_boxes_opening_interval   | 0       | 0   | inf.| Server | The time interval of team scheme renewal. |
| cl_holdtoaim                       | 0       | 0   | 1   | Client | [0 - Click to switch aiming.]<br/>[1 - Press & hold to stay aiming.] |

## Credits
* [ReGameDLL_CS](https://github.com/s1lentq/ReGameDLL_CS) (This mod was created on the basis of ReGameDLL_CS)
* [Metahook](https://github.com/nagist/metahook) (A total game-changer. Literally and metaphorically.)
* [FMOD](https://www.fmod.com/) (Via whom we get rid of the original obsolete sound system of GoldSrc.)

## How can I help the project?
Just play it and contact me if you have anything to say.
