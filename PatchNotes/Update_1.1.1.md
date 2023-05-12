Stop the player (displaying an error message in chat) if they're traveling over 1500 m/s to mitigate the "bounced off an enemy maxing upwards velocity" base game(?) bug




This update brought to you by Robb#6731.

Be sure to read the 1.0.0 changelog - lots of stuff changed!

## New Stuff

- Cancel out the player's velocity, displaying a message in chat and the logs, if they're traveling over 1500 m/s
  - Only applies to players wearing suits
  - Grants 30 seconds of damage immunity as long as the suit remains equipped
  - Implemented to mitigate the "bounced off an enemy" bug that I'm still not sure if PowerSuit or the base game is to blame for
  - Thanks Avexuzz, Switch_ZA, [Sg/Xtra](https://www.youtube.com/watch?v=auAtvrU6w8I), Liselotte Dzemael, Dorora, and other people whose names I don't have recorded.
- New easter egg
- New config option: **Enable Ctrl+Jump to start flight**
  - Previously, pressing Ctrl+Jump would start flying immediately, without requiring holding the key down. I don't think this was ever documented.
  - This got in the way of people holding ctrl to deconstruct stuff
  - Now it must be enabled in a mod option; it's off by default
  - Thanks Mathg44 for bringing this to my attention

## Known Bugs

- Multiplayer clients that logged in wearing a suit may need to re-equip it upon joining
- Entering a vehicle while flying can very rarely and inconsistently kill your player without leaving a death crate
- Multiplayer clients don't get fuel fuse recovery sounds
