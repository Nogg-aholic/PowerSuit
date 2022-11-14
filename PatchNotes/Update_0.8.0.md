Update 5 support and some crash fixes. This update brought to you by Robb#6731




## "What took so long!?"

Read the PowerSuit Modules v3.5.0 changelog for more info.
## New Stuff

- Shield Cooldown progress is now visible in the suit HUD as a gray bar above the shield bar.
  - When it reaches the right side, the cooldown is over and the shield starts to recharge again
  - This cooldown functionality was always happening behind the scenes, but now it's visible how much time remains.

![Shield Cooldown](https://i.imgur.com/pXgF7ot.png)

- (For developers) BP Lib function to eject items to the ground from an inventory
- (For developers) BP Lib function to get info on suit shield regen cooldown

## Bugfixes

- Temporary fix for swapping modules in-place in the suit crashing your game
  - Surprisingly few people ran into (or at least, reported) this issue.
  - My current fix causes the stats of the modules to be double-counted until you re-equip the suit. This is not good, but I think it's better than hard crashing for now.
- Probably more I forgot to write down

## Known Bugs

- **The handheld suit controllers are not visible**. Something changed in U5 that interferes with how they used to attach to the player. I haven't been able to fix this yet.
- In-place swapping of modules inside the suit can cause the stats of the modules to be double-counted until you re-equip the suit. This was a result of partially fixing a crash, but I don't have time to fully fix this right now.

