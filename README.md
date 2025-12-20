# GeneSim project
The purpose of this program is to facilitate the development of story-based game designs.
* genesim - A comprehensive kinship and family relationship simulator. (in development)
* namegen - NameGen a comprehensive lineage and family relationship simulator. (in development)
* namegen - NameGen (demo) a comprehensive lineage and family relationship simulator demo. (released)

<h1>Namegen – Historical Name Generator - DEMO</h1>
Namegen is a command-line based application designed to generate authentic Finnish names from specific historical periods.

* working demo versio 0.4.18 found releases namegen (demo).

Stop using generic placeholder names. This tool is the ultimate solution for Game Developers, TTRPG creators, and narrative designers who need historically accurate, culturally authentic, and uniquely structured Finnish names from the late 19th and early 20th centuries.

✨ New: Couple Generation & Name Complexity! 

👩‍❤️‍👨 Build Instant, Believable Relationships
* Couple Generation Mode: Instantly create married couples who automatically share the same, newly generated surname. Perfect for populating towns or establishing family lines with minimal effort.
* New: Couple generation is now available via interactive prompt!
* Layered Middle Names: Design names with depth and tradition. Set a maximum of three (3) middle names to generate complex, authentic-sounding Finnish identities (e.g., Jaakko Johannes Eemil Virtanen).

🛡️ Reliability for Production Pipelines
We provide more than just names; we provide reliable production assets.
* Period-Specific Realism: Select the exact historical era (from 1860s to 1920s) for your game setting. Only era-appropriate names are used, ensuring your world is always believable.
* Validated Data (CRITICAL): Thanks to strict data validation, you receive 100% clean name strings. The program automatically rejects all non-name artifacts, numbers, and data errors during loading, ensuring smooth integration into your engine or database.

🎯 Ultimate Control for Content Creators
Designed for Bulk and Detail:
* Command Line: Use flags for mass asset generation and scripting (e.g., -p 5 -n 100 to create 100 names from the 1900s).
* Interactive Mode: Run the program without flags and answer simple prompts—perfect for quick, ad-hoc character creation.

Refined Control Features:
* Mandatory Surname: Set a specific surname override (e.g., -S Nieminen) for all generated characters, ideal for designating members of a particular in-game clan or faction.
* Fine-Tune Probability: Control the middle name chance (0–100%) to match the complexity level needed for your characters.
* Clean Input/Output: Technical fixes ensure input buffer cleaning, eliminating common interactive errors and ensuring a smooth, fast workflow.

This is more than a name generator; it's a character asset creation tool engineered for narrative designers.

<h2>Technical Specifications</h2>
<li>Language: Pure C.</li>
<li>Compatibility: Designed to work on both Windows and Linux environments (using #ifdef constructs).</li>
<li>Version: 0.4.18 (Development stage).</li>
<li>Data is based on https://www.tuomas.salste.net/suku/kela-etunimet.html#M1 research. There are a lot of ready-made names.</li>
